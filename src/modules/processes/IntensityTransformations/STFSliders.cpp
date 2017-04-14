//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0364
// ----------------------------------------------------------------------------
// STFSliders.cpp - Released 2017-04-14T23:07:12Z
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

#include "STFSliders.h"
#include "ScreenTransferFunctionInterface.h" // for ScreenTransferFunctionInterface::WorkingMode()

#include <pcl/GlobalSettings.h>
#include <pcl/Graphics.h>
#include <pcl/Bitmap.h>
#include <pcl/Vector.h>

#define WHEEL_STEP_ANGLE   PixInsightSettings::GlobalInteger( "ImageWindow/WheelStepAngle" )

namespace pcl
{

// ----------------------------------------------------------------------------

STFSliders::STFSliders() :
   channel( 0 ),
   rgb( true ),
   m( 0.5F ), c0( 0 ), c1( 1 ), v0( 0 ), v1( 1 ),
   gradient( Bitmap::Null() ),
   beingDragged( -1 ),
   scrolling( false ),
   scrollOrigin( 0 ),
   m_wheelSteps( 0 )
{
   OnPaint( (Control::paint_event_handler)&STFSliders::__Paint, *this );
   OnResize( (Control::resize_event_handler)&STFSliders::__Resize, *this );
   OnMousePress( (Control::mouse_button_event_handler)&STFSliders::__MousePress, *this );
   OnMouseMove( (Control::mouse_event_handler)&STFSliders::__MouseMove, *this );
   OnMouseRelease( (Control::mouse_button_event_handler)&STFSliders::__MouseRelease, *this );
   OnMouseWheel( (Control::mouse_wheel_event_handler)&STFSliders::__MouseWheel, *this );
}

STFSliders::~STFSliders()
{
}

// ----------------------------------------------------------------------------

void STFSliders::SetChannel( int c, bool color )
{
   if ( (c = Range( c, 0, 3 )) != channel || color != rgb )
   {
      channel = c;
      rgb = color;
      InvalidateGradient();
      Update();
   }
}

void STFSliders::SetValues( const STF& stf )
{
   m = stf.m;
   c0 = stf.c0;
   c1 = stf.c1;
   Update();
}

void STFSliders::SetVisibleRange( double a, double b )
{
   v0 = Range( a, 0.0, 1.0 );
   v1 = Range( b, 0.0, 1.0 );
   if ( v1 < v0 )
      Swap( v0, v1 );
   InvalidateGradient();
   Update();
}

// ----------------------------------------------------------------------------

void STFSliders::OnValueUpdated( value_event_handler f, Control& c )
{
   if ( f == nullptr || c.IsNull() )
   {
      onValueUpdated = nullptr;
      onValueUpdatedReceiver = nullptr;
   }
   else
   {
      onValueUpdated = f;
      onValueUpdatedReceiver = &c;
   }
}

void STFSliders::OnRangeUpdated( range_event_handler f, Control& c )
{
   if ( f == nullptr || c.IsNull() )
   {
      onRangeUpdated = nullptr;
      onRangeUpdatedReceiver = nullptr;
   }
   else
   {
      onRangeUpdated = f;
      onRangeUpdatedReceiver = &c;
   }
}

// ----------------------------------------------------------------------------

void STFSliders::InvalidateGradient()
{
   gradient = Bitmap::Null();
}

void STFSliders::RebuildGradient()
{
   Rect r = BoundsRect();
   int w = r.Width();
   int h = r.Height();

   gradient = Bitmap( w, h, BitmapFormat::RGB32 );

   Graphics g( gradient );

   bool c0 = channel == 0 || channel == 3 || !rgb && channel == 0;
   bool c1 = channel == 1 || channel == 3 || !rgb && channel == 0;
   bool c2 = channel == 2 || channel == 3 || !rgb && channel == 0;

   GradientBrush::stop_list stops;
   stops.Add( GradientBrush::Stop( 0.0, RGBAColor( float( c0 ? v0 : 0.0 ), float( c1 ? v0 : 0.0 ), float( c2 ? v0 : 0.0 ) ) ) );
   stops.Add( GradientBrush::Stop( 1.0, RGBAColor( float( c0 ? v1 : 0.0 ), float( c1 ? v1 : 0.0 ), float( c2 ? v1 : 0.0 ) ) ) );

   g.FillRect( r, LinearGradientBrush( 0, 0, w, 0, stops ) );

   g.SetPen( RGBAColor( 0, 0, 0 ) );
   g.SetBrush( Brush::Null() );
   g.DrawRect( r );

   if ( !rgb && channel > 0 )
   {
      Bitmap b( ScaledResource( ":/browser/disabled.png" ) );
      int d = (h - b.Height()) >> 1;
      g.DrawBitmap( d, d, b );
   }
}

// ----------------------------------------------------------------------------

RGBA STFSliders::HandlerColor( double v ) const
{
   // Ensure visibility of handlers on R, G, B and gray backgrounds.
   if ( (channel == 0 && !rgb || channel == 1 || channel == 3) && v > 0.5 )
      return RGBAColor( 0, 0, 0 );
   return RGBAColor( 255, 255, 255 );
}

void STFSliders::DrawHandler( Graphics& g, double v )
{
   RGBA color = HandlerColor( v );

   g.SetPen( color );
   //g.SetBrush( color );

   int x = STFToSlider( v );
   int h1 = Height() - 1;

   GenericVector<Point> notch( 4 );
   notch[0] = Point( x, h1-5 );
   notch[1] = Point( x-5, h1 );
   notch[2] = Point( x+5, h1 );
   notch[3] = Point( x, h1-5 );

   g.DrawLine( x, 0, x, h1-6 );
   g.DrawPolyline( notch );
}

void STFSliders::__Paint( Control& sender, const pcl::Rect& updateRect )
{
   Graphics g( sender );

   if ( gradient.IsNull() )
      RebuildGradient();

   g.DrawBitmapRect( updateRect.LeftTop(), gradient, updateRect );

   DrawHandler( g, c0 );
   DrawHandler( g, c0 + m*(c1 - c0) );
   DrawHandler( g, c1 );
}

// ----------------------------------------------------------------------------

void STFSliders::__Resize( Control& sender, int newWidth, int newHeight, int oldWidth, int oldHeight )
{
   InvalidateGradient();
}

// ----------------------------------------------------------------------------

int STFSliders::FindHandler( double v )
{
   double vm = c0 + m*(c1 - c0);

   double dvm = Abs( v - vm );
   double dc0 = Abs( v - c0 );
   double dc1 = Abs( v - c1 );

   if ( dvm < dc0 )
   {
      if ( dvm < dc1 )
         return 0;
   }

   if ( dc0 < dvm )
   {
      if ( dc0 < dc1 )
         return 1;
   }

   return 2;
}

double STFSliders::UpdateDragging( int x )
{
   double v = Range( SliderToSTF( x ), 0.0, 1.0 );

   if ( beingDragged == 0 )
      m = v = Round( (c0 != c1) ? (Range( v, c0, c1 ) - c0)/(c1 - c0) : c0, 10 );
   else
   {
      v = Round( v, 10 );

      if ( beingDragged == 1 )
      {
         if ( (c0 = v) > c1 )
            c1 = c0;
      }
      else
      {
         if ( (c1 = v) < c0 )
            c0 = c1;
      }
   }

   Update();

   return v;
}

bool STFSliders::UpdateScrolling( int x )
{
   bool update = false;
   double dv = (scrollOrigin - x)*(v1 - v0)/(Width() - 1);
   if ( dv < 0 )
   {
      double a = Max( 0.0, v0 + dv );
      if ( a != v0 )
      {
         update = true;
         v1 = a + v1 - v0;
         v0 = a;
      }
   }
   else
   {
      double b = Min( v1 + dv, 1.0 );
      if ( b != v1 )
      {
         update = true;
         v0 = b - v1 + v0;
         v1 = b;
      }
   }

   if ( update )
   {
      InvalidateGradient();
      Update();
   }

   scrollOrigin = x;

   return update;
}

#define mode   TheScreenTransferFunctionInterface->WorkingMode()

void STFSliders::__MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( button == MouseButton::Left )
   {
      switch ( mode )
      {
      case ScreenTransferFunctionInterface::EditMode:
         beingDragged = FindHandler( SliderToSTF( pos.x ) );
         __MouseMove( sender, pos, buttons, modifiers );
         break;
      case ScreenTransferFunctionInterface::ZoomInMode:
      case ScreenTransferFunctionInterface::ZoomOutMode:
         {
            FPoint vz = (mode == ScreenTransferFunctionInterface::ZoomInMode) ?
                                             ZoomInRange( pos.x ) : ZoomOutRange( pos.x );
            if ( vz.x != v0 || vz.y != v1 )
            {
               SetVisibleRange( vz.x, vz.y );
               if ( onRangeUpdated != nullptr )
                  (onRangeUpdatedReceiver->*onRangeUpdated)( *this, channel, v0, v1, modifiers );
            }
         }
         break;
      case ScreenTransferFunctionInterface::PanMode:
         SetCursor( StdCursor::ClosedHand );
         scrolling = true;
         scrollOrigin = pos.x;
         break;
      }
   }
   else if ( button == MouseButton::Middle )
   {
      TheScreenTransferFunctionInterface->SetWorkingMode( ScreenTransferFunctionInterface::PanMode );
      SetCursor( StdCursor::ClosedHand );
      scrolling = true;
      scrollOrigin = pos.x;
   }
}

#undef mode

void STFSliders::__MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( scrolling )
   {
      if ( UpdateScrolling( pos.x ) )
         if ( onRangeUpdated != nullptr )
            (onRangeUpdatedReceiver->*onRangeUpdated)( *this, channel, v0, v1, modifiers );
   }
   else if ( beingDragged >= 0 )
   {
      double v = UpdateDragging( pos.x );
      Control::ShowToolTip( Cursor::Position(), String().Format( "%.6g", v ) );
      if ( onValueUpdated != nullptr )
         (onValueUpdatedReceiver->*onValueUpdated)( *this, channel, beingDragged, v, modifiers, false );
   }
}

void STFSliders::__MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( scrolling )
   {
      if ( button == MouseButton::Middle )
         TheScreenTransferFunctionInterface->SetWorkingMode( ScreenTransferFunctionInterface::EditMode );
      else
         SetCursor( StdCursor::OpenHand );

      if ( UpdateScrolling( pos.x ) )
         if ( onRangeUpdated != nullptr )
            (onRangeUpdatedReceiver->*onRangeUpdated)( *this, channel, v0, v1, modifiers );

      scrolling = false;
   }
   else if ( beingDragged >= 0 )
   {
      Control::HideToolTip();
      double v = UpdateDragging( pos.x );
      if ( onValueUpdated != nullptr )
         (onValueUpdatedReceiver->*onValueUpdated)( *this, channel, beingDragged, v, modifiers, true );

      beingDragged = -1;
   }
}

void STFSliders::__MouseWheel( Control& sender, const pcl::Point& pos, int delta, unsigned buttons, unsigned modifiers )
{
   m_wheelSteps += delta; // delta is rotation angle in 1/8 degree steps
   if ( Abs( m_wheelSteps ) >= WHEEL_STEP_ANGLE*8 )
   {
      FPoint vz = (delta < 0) ? ZoomInRange( pos.x ) : ZoomOutRange( pos.x );
      if ( vz.x != v0 || vz.y != v1 )
      {
         SetVisibleRange( vz.x, vz.y );
         if ( onRangeUpdated != nullptr )
            (onRangeUpdatedReceiver->*onRangeUpdated)( *this, channel, v0, v1, modifiers );
      }
      m_wheelSteps = 0;
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF STFSliders.cpp - Released 2017-04-14T23:07:12Z
