// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard IntensityTransformations Process Module Version 01.07.00.0287
// ****************************************************************************
// AdaptiveStretchCurveGraphInterface.cpp - Released 2014/11/14 17:19:23 UTC
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

#include "AdaptiveStretchCurveGraphInterface.h"
#include "AdaptiveStretchProcess.h"
#include "CurvesTransformationInstance.h"
#include "CurvesTransformationProcess.h"

#include <pcl/Graphics.h>
#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "AdaptiveStretchIcon.xpm"
#include "CurvesTransformationIcon.xpm"

// ----------------------------------------------------------------------------

AdaptiveStretchCurveGraphInterface* TheAdaptiveStretchCurveGraphInterface = 0;

static int s_sizeItems[] = { 400, 500, 600, 800, 1000 };

// ----------------------------------------------------------------------------

AdaptiveStretchCurveGraphInterface::AdaptiveStretchCurveGraphInterface() :
ProcessInterface(),
GUI( 0 ),
m_width( 400 ),
m_height( 400 ),
m_backgroundColor( 0xFFFFFFFF ), // white
m_curveColor( 0xFFFF0000 ),      // red
m_gridColor( 0xFFD0D0D0 ),       // light gray
m_axisColor( 0xFF000000 ),       // black
m_fontFace( "Helvetica" ),
m_fontSize( 12 ),
m_tickSize( 5 ),
m_margin( 15 ),
m_curve(),
m_curveRect( 0 ),
m_gridBitmap( Bitmap::Null() ),
m_curveBitmap( Bitmap::Null() )
{
   TheAdaptiveStretchCurveGraphInterface = this;
}

AdaptiveStretchCurveGraphInterface::~AdaptiveStretchCurveGraphInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

// ----------------------------------------------------------------------------

IsoString AdaptiveStretchCurveGraphInterface::Id() const
{
   return "AdaptiveStretchCurveGraph";
}

// ----------------------------------------------------------------------------

MetaProcess* AdaptiveStretchCurveGraphInterface::Process() const
{
   return TheAdaptiveStretchProcess;
}

// ----------------------------------------------------------------------------

const char** AdaptiveStretchCurveGraphInterface::IconImageXPM() const
{
   return AdaptiveStretchIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures AdaptiveStretchCurveGraphInterface::Features() const
{
   return InterfaceFeature::None;
}

// ----------------------------------------------------------------------------

bool AdaptiveStretchCurveGraphInterface::IsInstanceGenerator() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool AdaptiveStretchCurveGraphInterface::CanImportInstances() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool AdaptiveStretchCurveGraphInterface::Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "AdaptiveStretch Curve Graph" );
      UpdateControls();
   }

   dynamic = false;
   return true;
}

// ----------------------------------------------------------------------------

void AdaptiveStretchCurveGraphInterface::SaveSettings() const
{
   Settings::Write( SettingsKey() + "_GraphWidth", m_width );
   Settings::Write( SettingsKey() + "_GraphHeight", m_height );
}

// ----------------------------------------------------------------------------

void AdaptiveStretchCurveGraphInterface::LoadSettings()
{
   int width = m_width;
   int height = m_height;
   Settings::Read( SettingsKey() + "_GraphWidth", width );
   Settings::Read( SettingsKey() + "_GraphHeight", height );
   if ( width != m_width || height != m_height )
      Resize( width, height );
}

// ----------------------------------------------------------------------------

void AdaptiveStretchCurveGraphInterface::UpdateGraph( const StretchCurve& curve )
{
   if ( GUI != 0 )
   {
      m_curve = curve;
      if ( m_gridBitmap.IsNull() )
         GenerateGraphGrid();
      GenerateGraphCurve();
      GUI->CurveGraph_Control.Update();
   }
}

void AdaptiveStretchCurveGraphInterface::UpdateControls()
{
   for ( int i = 0; i < int( ItemsInArray( s_sizeItems ) ); ++i )
      if ( s_sizeItems[i] == m_width )
      {
         GUI->Size_ComboBox.SetCurrentItem( i );
         break;
      }
   GenerateGraphGrid();
   GenerateGraphCurve();
   GUI->CurveGraph_Control.Update();
}

void AdaptiveStretchCurveGraphInterface::Resize( int width, int height )
{
   m_gridBitmap = Bitmap::Null();
   m_curveBitmap = Bitmap::Null();

   GUI->CurveGraph_Control.SetFixedSize( m_width = width, m_height = height );
   SetVariableSize();
   AdjustToContents();
   SetFixedSize();

   UpdateControls();
}

void AdaptiveStretchCurveGraphInterface::GenerateGraphGrid()
{
   pcl::Font font( m_fontFace );
   font.SetPixelSize( m_fontSize );

   int labelHeight = font.TightBoundingRect( "123" ).Height();
   int labelSeparation = font.Width( '-' );
   int xLabelHeight = labelHeight + labelSeparation;
   int yLabelWidth = font.Width( "1.0" ) + labelSeparation;

   m_curveRect = Rect( m_margin + yLabelWidth + m_tickSize,
                       m_margin + m_tickSize,
                       m_width  - m_margin - m_tickSize,
                       m_height - m_margin - xLabelHeight - m_tickSize );

   if ( m_gridBitmap.IsNull() )
      m_gridBitmap = Bitmap( m_width, m_height );

   m_gridBitmap.Fill( m_backgroundColor );

   Graphics G( m_gridBitmap );

   G.SetPen( m_axisColor );
   G.StrokeRect( m_curveRect );

   G.SetFont( font );

   // Plot horizontal axes + vertical grid
   for ( int w = 0; w <= 100; w += 10 )
   {
      double f = w/100.0;
      int x = RoundI( m_curveRect.x0 + f*(m_curveRect.Width() - 1) );
      G.DrawLine( x, m_curveRect.y0-m_tickSize, x, m_curveRect.y0 );
      G.DrawLine( x, m_curveRect.y1, x, m_curveRect.y1+m_tickSize );
      G.DrawText( x - labelHeight/2, m_height-m_margin, String().Format( "%.1f", f ) );
      if ( w > 0 && w < 100 )
      {
         G.SetPen( m_gridColor );
         G.DrawLine( x, m_curveRect.y0+1, x, m_curveRect.y1-2 );
         G.SetPen( m_axisColor );
      }
   }

   // Plot vertical axes + horizontal grid
   for ( int h = 0; h <= 100; h += 10 )
   {
      double f = h/100.0;
      int y = RoundI( m_curveRect.y1 - 1 - f*(m_curveRect.Height() - 1) );
      G.DrawLine( m_curveRect.x0-m_tickSize, y, m_curveRect.x0, y );
      G.DrawLine( m_curveRect.x1, y, m_curveRect.x1+m_tickSize, y );
      G.DrawText( m_curveRect.x0-m_tickSize-yLabelWidth, y+labelHeight/2, String().Format( "%.1f", f ) );
      if ( h > 0 && h < 100 )
      {
         G.SetPen( m_gridColor );
         G.DrawLine( m_curveRect.x0+1, y, m_curveRect.x1-2, y );
         G.SetPen( m_axisColor );
      }
   }

   G.EndPaint();
}

void AdaptiveStretchCurveGraphInterface::GenerateGraphCurve()
{
   if ( m_curveBitmap.IsNull() )
      m_curveBitmap = Bitmap( m_width, m_height );

   m_curveBitmap.Fill( 0 ); // transparent

   if ( !m_curve.IsEmpty() )
   {
      Array<Point> points( m_curveRect.Width() );
      for ( int i = 0; i < m_curveRect.Width(); ++i )
      {
         points[i].x = m_curveRect.x0 + i;
         points[i].y = RoundI( m_curveRect.y1 - 1 - (m_curveRect.Height() - 1)*m_curve( double( i )/(m_curveRect.Width() - 1) ) );
      }
      Graphics G( m_curveBitmap );
      G.EnableAntialiasing();
      G.SetPen( m_curveColor );
      G.DrawPolyline( points );
      G.EndPaint();
   }
}

// ----------------------------------------------------------------------------

void AdaptiveStretchCurveGraphInterface::__Paint( Control& sender, const Rect& updateRect )
{
   Graphics G( sender );
   if ( m_gridBitmap.IsNull() )
      GenerateGraphGrid();
   G.DrawBitmapRect( Point( 0 ), m_gridBitmap, updateRect );
   if ( !m_curveBitmap.IsNull() )
      G.DrawBitmapRect( Point( 0 ), m_curveBitmap, updateRect );
   G.EndPaint();
}

void AdaptiveStretchCurveGraphInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Size_ComboBox )
      Resize( s_sizeItems[itemIndex], s_sizeItems[itemIndex] );
}

void AdaptiveStretchCurveGraphInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->Render_ToolButton )
   {
      ImageWindow window( m_width, m_height,
                          3,      // numberOfChannels
                          8,      // bitsPerSample
                          false,  // floating point
                          true ); // color
      if ( !m_gridBitmap.IsNull() )
      {
         View mainView = window.MainView();
         ImageVariant v = mainView.Image();
         static_cast<UInt8Image&>( *v ).Blend( m_gridBitmap );
         if ( !m_curveBitmap.IsNull() )
            static_cast<UInt8Image&>( *v ).Blend( m_curveBitmap );
      }

      window.BringToFront();
      window.Show();
      window.ZoomToFit( false/*allowMagnification*/ );
   }
   else if ( sender == GUI->Edit_ToolButton )
   {
      CurvesTransformationInstance curves( TheCurvesTransformationProcess );
      float ux = 1.0/(m_curve.Length() - 1);
      float m0 = 0;
      for ( int i = 0, j = 1; j < m_curve.Length(); ++j )
      {
         float dy = Abs( m_curve[j] - m_curve[i] );
         if ( dy > 0.01 )
         {
            float dx = ux*(j - i);
            float m = dy/dx;
            if ( Abs( m - m0 )/m > 0.05 )
            {
               m0 = m;
               i = j;
               curves[CurveIndex::RGBK].Add( ux*i, m_curve[i] );
            }
         }
         else if ( 1 + dy == 1 )
         {
            for ( ; ++j < m_curve.Length(); ++j )
            {
               dy = Abs( m_curve[j] - m_curve[i] );
               if ( 1 + dy > 1 )
               {
                  m0 = 0;
                  i = j-1;
                  curves[CurveIndex::RGBK].Add( ux*i, m_curve[i] );
                  break;
               }
            }
         }
      }
      curves.LaunchInterface();
   }
}

void AdaptiveStretchCurveGraphInterface::__Hide( Control& sender )
{
   // Release memory when the window is hidden.
   m_curve = StretchCurve();
   m_gridBitmap = Bitmap::Null();
   m_curveBitmap = Bitmap::Null();
}

// ----------------------------------------------------------------------------

AdaptiveStretchCurveGraphInterface::GUIData::GUIData( AdaptiveStretchCurveGraphInterface& w )
{
   CurveGraph_Control.SetFixedSize( w.m_width, w.m_height );
   CurveGraph_Control.OnPaint( (Control::paint_event_handler)&AdaptiveStretchCurveGraphInterface::__Paint, w );

   for ( int i = 0; i < int( ItemsInArray( s_sizeItems ) ); ++i )
      Size_ComboBox.AddItem( String().Format( "%d", s_sizeItems[i] ) );
   Size_ComboBox.SetToolTip( "<p>Graph size in pixels.</p>" );
   Size_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&AdaptiveStretchCurveGraphInterface::__ItemSelected, w );

   Render_ToolButton.SetIcon( Bitmap( String( ":/icons/camera.png" ) ) );
   Render_ToolButton.SetFixedSize( 20, 20 );
   Render_ToolButton.SetToolTip( "<p>Render the current graph as a new image.</p>" );
   Render_ToolButton.OnClick( (Button::click_event_handler)&AdaptiveStretchCurveGraphInterface::__Click, w );

   Edit_ToolButton.SetIcon( Bitmap( CurvesTransformationIcon_XPM ) );
   Edit_ToolButton.SetFixedSize( 20, 20 );
   Edit_ToolButton.SetToolTip( "<p>Edit as a CurvesTransformation instance.</p>" );
   Edit_ToolButton.OnClick( (Button::click_event_handler)&AdaptiveStretchCurveGraphInterface::__Click, w );

   Options_Sizer.AddSpacing( 4 );
   Options_Sizer.Add( Size_ComboBox );
   Options_Sizer.AddSpacing( 12 );
   Options_Sizer.Add( Render_ToolButton );
   Options_Sizer.AddSpacing( 12 );
   Options_Sizer.Add( Edit_ToolButton );
   Options_Sizer.AddStretch();

   Global_Sizer.Add( CurveGraph_Control );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Options_Sizer );
   Global_Sizer.AddSpacing( 4 );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();

   w.OnHide( (Control::event_handler)&AdaptiveStretchCurveGraphInterface::__Hide, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF AdaptiveStretchCurveGraphInterface.cpp - Released 2014/11/14 17:19:23 UTC
