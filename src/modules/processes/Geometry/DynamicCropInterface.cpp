// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Geometry Process Module Version 01.01.00.0245
// ****************************************************************************
// DynamicCropInterface.cpp - Released 2014/10/29 07:34:55 UTC
// ****************************************************************************
// This file is part of the standard Geometry PixInsight module.
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

#include "DynamicCropInterface.h"
#include "DynamicCropPreferencesDialog.h"
#include "DynamicCropProcess.h"

#include <pcl/Graphics.h>
#include <pcl/ImageWindow.h>
#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

// Dynamic cursors
#include "cursors/move.xpm"
#include "cursors/move_drag.xpm"
#include "cursors/move_point.xpm"
#include "cursors/move_point_drag.xpm"
#include "cursors/resize_bottom.xpm"
#include "cursors/resize_left.xpm"
#include "cursors/resize_left_bottom.xpm"
#include "cursors/resize_left_top.xpm"
#include "cursors/resize_right.xpm"
#include "cursors/resize_right_bottom.xpm"
#include "cursors/resize_right_top.xpm"
#include "cursors/resize_top.xpm"
#include "cursors/rotate.xpm"

// ----------------------------------------------------------------------------

// Half size of center mark in viewport coordinates
const int centerSz = 5;

// ----------------------------------------------------------------------------

DynamicCropInterface* TheDynamicCropInterface = 0;

// ----------------------------------------------------------------------------

#include "DynamicCropIcon.xpm"

// ----------------------------------------------------------------------------

DynamicCropInterface::DynamicCropInterface() :
ProcessInterface(),
instance( TheDynamicCropProcess ),
view( 0 ),
width( 0 ),
height( 0 ),
center( 0 ),
rotationCenter( 0 ),
rotationFixed( false ),
anchorPoint( 4 ), // center
anchor( 0 ),
flags(),
dragging( false ),
dragOrigin( 0 ),
initializing( false ),
rect( 0 ),
selectionColor( 0xFFFFFFFF ),
centerColor( 0xFFFFFFFF ),
fillColor( 0x28FFFFFF ),
GUI( 0 )
{
   TheDynamicCropInterface = this;
}

// ----------------------------------------------------------------------------

DynamicCropInterface::~DynamicCropInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

// ----------------------------------------------------------------------------

IsoString DynamicCropInterface::Id() const
{
   return "DynamicCrop";
}

// ----------------------------------------------------------------------------

MetaProcess* DynamicCropInterface::Process() const
{
   return TheDynamicCropProcess;
}

// ----------------------------------------------------------------------------

const char** DynamicCropInterface::IconImageXPM() const
{
   return DynamicCropIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures DynamicCropInterface::Features() const
{
   return InterfaceFeature::DefaultDynamic | InterfaceFeature::PreferencesButton;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::Execute()
{
   if ( view != 0 )
   {
      /*
       * Obtain local working references to the target view and window.
       */
      View v = *view;
      ImageWindow w = v.Window();

      /*
       * Reset reference to the target view in the dynamic interface. This
       * prevents inconsistent behavior during execution.
       */
      delete view, view = 0;

      /*
       * Since active dynamic targets cannot be modified, we have to remove our
       * target view from the dynamic targets set before attempting to process.
       */
      v.RemoveFromDynamicTargets();

      /*
       * Ensure that our target view is selected as the current view.
       */
      w.BringToFront();
      w.SelectView( v );

      /*
       * Execute the instance on the target window.
       */
      instance.LaunchOn( w );

      /*
       * Reset instance and interface to default states.
       */
      instance.Assign( DynamicCropInstance( TheDynamicCropProcess ) );
      InitControls();
      UpdateControls();
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::EditPreferences()
{
   DynamicCropPreferencesDialog dlg;
   dlg.Execute();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::ResetInstance()
{
   DynamicCropInstance defaultInstance( TheDynamicCropProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "DynamicCrop" );
      InitControls();
      UpdateControls();
   }

   dynamic = true;
   return &P == TheDynamicCropProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* DynamicCropInterface::NewProcess() const
{
   return new DynamicCropInstance( instance );
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const DynamicCropInstance* r = dynamic_cast<const DynamicCropInstance*>( &p );

   if ( r == 0 )
   {
      whyNot = "Not a DynamicCrop instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::ImportProcess( const ProcessImplementation& p )
{
   const DynamicCropInstance* i = dynamic_cast<const DynamicCropInstance*>( &p );

   if ( i == 0 )
      throw Error( "Not a DynamicCrop instance." );

   if ( view == 0 )
   {
      ImageWindow w = ImageWindow::ActiveWindow();
      if ( w.IsNull() )
      {
         throw Error( "DynamicCrop: No active image window" );
         return false;
      }

      view = new View( w.MainView() );

      view->AddToDynamicTargets();

      //Console().WriteLn( "<end><cbr>DynamicCrop: Selected target view: " + view->Id() );
   }
   else
      UpdateView();

   instance.Assign( *i );

   int w0 = view->Width();
   int h0 = view->Height();

   width = Max( 1.0, Round( instance.p_width*w0, 2 ) );
   height = Max( 1.0, Round( instance.p_height*h0, 2 ) );

   anchor.x = center.x = rotationCenter.x = Round( instance.p_center.x*w0, 2 );
   anchor.y = center.y = rotationCenter.y = Round( instance.p_center.y*h0, 2 );
   anchorPoint = 4; // center

   rotationFixed = false;

   if ( view->Window().CurrentView() != *view )
      view->Window().SelectView( *view );
   else
   {
      UpdateView();
      //view->Window().CommitPendingUpdates();
   }

   InitControls();
   UpdateControls();

   return true;
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::IsDynamicInterface() const
{
   return true;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::ExitDynamicMode()
{
   /*
    * Forget the current target view.
    */
   if ( view != 0 )
      delete view, view = 0;

   /*
    * Reset the instance. This ensures default GUI control values.
    */
   instance.Assign( DynamicCropInstance( TheDynamicCropProcess ) );

   /*
    * Update GUI
    */
   InitControls();
   UpdateControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::DynamicMouseEnter( View& v )
{
   if ( view == 0 || v != *view )
      return;

   // Force a dynamic cursor update upon subsequent mouse move event.
   flags = Flags();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::DynamicMouseMove( View& v, const DPoint& p, unsigned buttons, unsigned modifiers )
{
   if ( view == 0 || v != *view )
      return;

   if ( dragging )
   {
      if ( initializing )
      {
         ImageWindow w = view->Window();

         w.ModifySelection( RoundI( p.x ), RoundI( p.y ) );
         //w.CommitPendingUpdates();

         rect = w.SelectionRect();

         width = Max( 1, rect.Width() );
         height = Max( 1, rect.Height() );

         rotationCenter = center = rect.Center();

         UpdateSizePosControls();
         UpdateRotationControls();
         UpdateScaleControls();
      }
      else
         UpdateOperation( p, modifiers );
   }
   else
   {
      Flags f = OperationInfo( p );

      if ( f != flags )
      {
         flags = f;
         UpdateDynamicCursor();
      }
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::DynamicMousePress( View& v, const DPoint& p, int button, unsigned buttons, unsigned modifiers )
{
   if ( button != MouseButton::Left )
      return;

   dragging = true;

   if ( view != 0 )
   {
      if ( v != *view )
         return;

      BeginOperation( p, modifiers );
      UpdateDynamicCursor();
   }
   else
   {
      if ( !v.IsMainView() )
         throw Error( "DynamicCrop cannot run on previews. Please select a main view." );

      initializing = true;

      view = new View( v );

      ImageWindow w = view->Window();

      w.BeginSelection( RoundI( p.x ), RoundI( p.y ) );

      rect = w.SelectionRect();

      width = height = 1;
      rotationCenter = center = rect.Center();

      UpdateControls();
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::DynamicMouseRelease( View& v, const DPoint& p, int button, unsigned buttons, unsigned modifiers )
{
   if ( dragging )
   {
      dragging = false;

      if ( initializing )
      {
         ImageWindow w = view->Window();

         Rect r = w.SelectionRect();

         w.EndSelection();

         view->AddToDynamicTargets();

         initializing = false;
         Initialize( r );

         flags = OperationInfo( p );
         UpdateDynamicCursor();
      }
      else
      {
         if ( view == 0 || v != *view )
            return;

         UpdateDynamicCursor();
         EndOperation();
      }
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::DynamicMouseDoubleClick( View& v, const DPoint& p, unsigned buttons, unsigned modifiers )
{
   if ( view == 0 || v != *view )
      return;

   if ( IsPointOnRotationCenter( p ) )
   {
      SetRotationCenter( center );
      rotationFixed = false;
      flags = OperationInfo( p );
      UpdateDynamicCursor();
   }
   else if ( IsPointInsideRect( p ) )
      Execute();
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::DynamicKeyPress( View& v, int key, unsigned modifiers )
{
   if ( view == 0 || v != *view )
      return false;

   switch ( key )
   {
   case KeyCode::Enter:
      Execute();
      break;

   case KeyCode::Escape:
      ImageWindow::TerminateDynamicSession();
      break;

   default:
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::RequiresDynamicUpdate( const View& v, const DRect& updateRect ) const
{
   if ( view == 0 || v != *view || initializing )
      return false;

   // Check intersection with current cropping rectangle

   DRect r;

   if ( instance.p_angle == 0 )
      GetUnrotatedRect( r );
   else
      GetRotatedBounds( r );

   if ( r.Intersects( updateRect ) )
      return true;

   // Obtain the half-size of center marks in image coordinates
   double dr = v.Window().ViewportScalarToImage( centerSz+1 ); // add 1 pixel to guard against roundoff errors

   // Check intersection with the cropping rectangle's center mark
   if ( DRect( center.x-dr, center.y-dr,
               center.x+dr, center.y+dr ).Intersects( updateRect ) )
      return true;

   // Check intersection with the rotation center mark
   if ( DRect( rotationCenter.x-dr, rotationCenter.y-dr,
               rotationCenter.x+dr, rotationCenter.y+dr ).Intersects( updateRect ) )
      return true;

   return false;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::PaintRect( Graphics& G, ImageWindow& w, const Point& p0 ) const
{
   // Auxiliary routine to paint the cropping rectangle

   if ( instance.p_angle == 0 )
   {
      // Optimize for zero rotation
      DRect r;
      GetUnrotatedRect( r );
      w.ImageToViewport( r.x0, r.y0 );
      w.ImageToViewport( r.x1, r.y1 );

      G.DrawRect( RoundI( r.x0 )-p0.x, RoundI( r.y0 )-p0.y,
                  RoundI( r.x1 )-p0.x, RoundI( r.y1 )-p0.y );
   }
   else
   {
      DPoint tl, tr, bl, br;
      GetRotatedRect( tl, tr, bl, br );
      w.ImageToViewport( tl.x, tl.y );
      w.ImageToViewport( tr.x, tr.y );
      w.ImageToViewport( bl.x, bl.y );
      w.ImageToViewport( br.x, br.y );

      Array<Point> p( 4 );
      p[0].x = RoundI( tl.x ); p[0].y = RoundI( tl.y );
      p[1].x = RoundI( tr.x ); p[1].y = RoundI( tr.y );
      p[2].x = RoundI( br.x ); p[2].y = RoundI( br.y );
      p[3].x = RoundI( bl.x ); p[3].y = RoundI( bl.y );

      for ( int i = 0; i < 4; ++i )
         p[i] -= p0;

      G.DrawPolygon( p );
   }
}

void DynamicCropInterface::DynamicPaint( const View& v, Graphics& g, const DRect& ur ) const
{
   if ( view == 0 || v != *view )
      return;

   ImageWindow w = view->Window();

   // Update rectangle in real viewport coordinates.
   DRect vr = w.ImageToViewport( ur );

   // Update rectangle, integer viewport coordinates.
   // N.B.: The bottom-right corner (x1,y1) of a Rect object is *excluded*
   //       from the defined rectangle. For this reason we must extend the
   //       corresponding x1 and y1 coordinates by one unit.
   Rect r0( TruncI( vr.x0 ), TruncI( vr.y0 ), TruncI( vr.x1 )+1, TruncI( vr.y1 )+1 );

   // Working bitmap
   Bitmap bmp1( r0.Width(), r0.Height() );

   // Origin of the local bitmap coordinate system in viewport coordinates.
   Point p0 = r0.LeftTop();

   // Initialize the working bitmap with a solid black color. Use a low-level,
   // high-speed routine instead of a high-level drawing routine.
   bmp1.Fill( 0xff000000 );

   // Create a graphics context associated to our working bitmap.
   Graphics G;
   G.BeginPaint( bmp1 );

   // Draw the cropping rectangle
   G.SetPen( selectionColor );
   G.SetBrush( Brush::Null() );
   PaintRect( G, w, p0 );

   // Draw the center of the cropping rectangle
   DPoint ci = center;
   w.ImageToViewport( ci.x, ci.y );
   Point c0( RoundI( ci.x ), RoundI( ci.y ) );
   c0 -= p0;
   Point c1( c0.x-centerSz, c0.y-centerSz );
   Point c2( c0.x+centerSz, c0.y+centerSz );
   G.SetPen( centerColor );
   G.DrawLine( c1.x, c1.y, c2.x, c2.y );
   G.DrawLine( c2.x, c1.y, c1.x, c2.y );

   // Draw the center of rotation

   ci = rotationCenter;
   w.ImageToViewport( ci.x, ci.y );
   Point c3( RoundI( ci.x ), RoundI( ci.y ) );
   c3 -= p0;

   if ( c3 != c0 )
   {
      Point c4( c3.x-centerSz, c3.y-centerSz );
      Point c5( c3.x+centerSz, c3.y+centerSz );
      G.DrawLine( c3.x, c4.y, c3.x, c5.y );
      G.DrawLine( c4.x, c3.y, c5.x, c3.y );
   }

   G.DrawCircle( c3, centerSz );

   G.EndPaint();

   // Viewport bitmap
   Bitmap bmp2 = w.ViewportBitmap( r0 );

   // Draw the translucent cropping rectangle
   if ( Alpha( fillColor ) != 0 )
   {
      G.BeginPaint( bmp2 );
      G.SetPen( Pen::Null() );
      G.SetBrush( fillColor );
      PaintRect( G, w, p0 );
      G.EndPaint();
   }

   // Xor-combine our working bitmap with the corresponding viewport bitmap
   bmp2.Xor( bmp1 );

   // Render our working bitmap on the viewport
   g.DrawBitmap( p0, bmp2 );
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::WantsReadoutNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateReadout( const View& v, const DPoint& p, double R, double G, double B, double /*A*/ )
{
   if ( GUI != 0 && IsVisible() && GUI->FillColor_SectionBar.Section().IsVisible() )
   {
      instance.p_fillColor[0] = R;
      instance.p_fillColor[1] = G;
      instance.p_fillColor[2] = B;
      UpdateFillColorControls();
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::SaveSettings() const
{
   Settings::Write( SettingsKey() + "SelectionColor", selectionColor );
   Settings::Write( SettingsKey() + "CenterColor", centerColor );
   Settings::Write( SettingsKey() + "FillColor", fillColor );
}

void DynamicCropInterface::LoadSettings()
{
   Settings::Read( SettingsKey() + "SelectionColor", selectionColor );
   Settings::Read( SettingsKey() + "CenterColor", centerColor );
   Settings::Read( SettingsKey() + "FillColor", fillColor );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::Initialize( const Rect& r )
{
   if ( view == 0 )
      return;

   width = Max( 1, r.Width() );
   height = Max( 1, r.Height() );

   rect = Rect( TruncI( width ), TruncI( height ) ) + r.Ordered().LeftTop();

   anchor = center = rotationCenter = DRect( rect ).Center();
   anchorPoint = 4; // center

   rotationFixed = false;

   int w0 = view->Width();
   int h0 = view->Height();

   instance.p_center.x = center.x/w0;
   instance.p_center.y = center.y/h0;
   instance.p_width = width/w0;
   instance.p_height = height/h0;
   instance.p_angle = 0;
   instance.p_scaleX = instance.p_scaleY = 1;

   UpdateView();
   //view->Window().CommitPendingUpdates();

   InitControls();
   UpdateControls();
}

void DynamicCropInterface::GetRotatedRect(
   DPoint& topLeft, DPoint& topRight, DPoint& bottomLeft, DPoint& bottomRight ) const
{
   double w2 = 0.5*width;
   double h2 = 0.5*height;
   bottomLeft.x   = topLeft.x    = center.x - w2;
   topRight.y     = topLeft.y    = center.y - h2;
   bottomRight.x  = topRight.x   = center.x + w2;
   bottomRight.y  = bottomLeft.y = center.y + h2;

   if ( instance.p_angle != 0 )
   {
      double sa, ca;
      SinCos( instance.p_angle, sa, ca );
      Rotate( topLeft, sa, ca, center );
      Rotate( topRight, sa, ca, center );
      Rotate( bottomLeft, sa, ca, center );
      Rotate( bottomRight, sa, ca, center );
   }
}

void DynamicCropInterface::GetRotatedBounds( DRect& r ) const
{
   DPoint tl, tr, bl, br;
   GetRotatedRect( tl, tr, bl, br );
   r.x0 = Min( Min( Min( tl.x, tr.x ), bl.x ), br.x );
   r.y0 = Min( Min( Min( tl.y, tr.y ), bl.y ), br.y );
   r.x1 = Max( Max( Max( tl.x, tr.x ), bl.x ), br.x );
   r.y1 = Max( Max( Max( tl.y, tr.y ), bl.y ), br.y );
}

void DynamicCropInterface::GetUnrotatedRect( DRect& r ) const
{
   double w2 = 0.5*width;
   double h2 = 0.5*height;
   r.x0 = center.x - w2;
   r.y0 = center.y - h2;
   r.x1 = center.x + w2;
   r.y1 = center.y + h2;
}

DynamicCropInterface::Flags DynamicCropInterface::OperationInfo( const DPoint& p ) const
{
   Flags f;

   if ( IsPointOnRotationCenter( p ) )
   {
      f.bits.movingCenter = true;
   }
   else if ( IsPointNearRect( p ) )
   {
      bool L, T, R, B;

      if ( IsPointOnRectEdges( p, L, T, R, B ) )
      {
         f.bits.resizing = true;
         f.bits.resizeLeft = L;
         f.bits.resizeTop = T;
         f.bits.resizeRight = R;
         f.bits.resizeBottom = B;
      }
      else // implies IsInsideRect( p )
         f.bits.moving = true;
   }
   else // outside rect+tolerance
      f.bits.rotating = true;

   return f;
}

void DynamicCropInterface::UpdateDynamicCursor() const
{
   const char** csr = 0;

   if ( flags.bits.movingCenter )
      csr = dragging ? move_point_drag_XPM : move_point_XPM;
   else if ( flags.bits.resizing )
   {
      if ( flags.bits.resizeLeft )
      {
         if ( flags.bits.resizeTop )
            csr = resize_left_top_XPM;
         else if ( flags.bits.resizeBottom )
            csr = resize_left_bottom_XPM;
         else
            csr = resize_left_XPM;
      }
      else if ( flags.bits.resizeRight )
      {
         if ( flags.bits.resizeTop )
            csr = resize_right_top_XPM;
         else if ( flags.bits.resizeBottom )
            csr = resize_right_bottom_XPM;
         else
            csr = resize_right_XPM;
      }
      else if ( flags.bits.resizeTop )
         csr = resize_top_XPM;
      else if ( flags.bits.resizeBottom )
         csr = resize_bottom_XPM;
   }
   else if ( flags.bits.moving )
      csr = dragging ? move_drag_XPM : move_XPM;
   else if ( flags.bits.rotating )
      csr = rotate_XPM;

   if ( csr != 0 )
      view->Window().SetDynamicCursor( csr, 10, 10 );
   else
      view->Window().ResetDynamicCursor();
}

void DynamicCropInterface::BeginOperation( const DPoint& p, unsigned modifiers )
{
   flags = OperationInfo( p );
   dragOrigin = (modifiers & KeyModifier::Shift) ? p.Rounded() : p;
}

void DynamicCropInterface::UpdateOperation( const DPoint& p, unsigned modifiers )
{
   DPoint q = p;

   if ( flags.bits.movingCenter )
      UpdateCenterMove( q, modifiers );
   else if ( flags.bits.resizing )
      UpdateResize( q, modifiers );
   else if ( flags.bits.moving )
      UpdateMove( q, modifiers );
   else if ( flags.bits.rotating )
      UpdateRotation( q, modifiers );

   dragOrigin = q;
}

#define CONSTRAINED_MOVE   (!(modifiers & KeyModifier::Shift) && instance.p_angle == 0)

void DynamicCropInterface::UpdateRotation( DPoint& p, unsigned /*modifiers*/ )
{
   UpdateView();

   double da = RotationAngle( p ) - RotationAngle( dragOrigin );

   Rotate( center, da, rotationCenter );

   double a = instance.p_angle + da;
   instance.p_angle = ArcTan( Sin( a ), Cos( a ) );

   UpdateAnchorPosition();

   UpdateInstance();

   UpdateView();
   //view->Window().CommitPendingUpdates();

   UpdateSizePosControls();
   UpdateRotationControls();
}

void DynamicCropInterface::UpdateCenterMove( DPoint& p, unsigned modifiers )
{
   if ( CONSTRAINED_MOVE )
      p = (2.0*p).Rounded()/2.0;

   SetRotationCenter( p );
}

void DynamicCropInterface::UpdateMove( DPoint& p, unsigned modifiers )
{
   if ( CONSTRAINED_MOVE )
   {
      UpdateView();
      width = Round( width );
      height = Round( height );
      center.x = (int( width ) & 1) ? int( center.x ) + 0.5 : Round( center.x );
      center.y = (int( height ) & 1) ? int( center.y ) + 0.5 : Round( center.y );
      p = p.Rounded();
   }

   MoveTo( center + p - dragOrigin );
}

void DynamicCropInterface::UpdateResize( DPoint& p, unsigned modifiers )
{
   UpdateView();

   if ( CONSTRAINED_MOVE )
      p = p.Rounded();

   DPoint p0 = dragOrigin;
   DPoint p1 = p;
   double sa, ca;
   SinCos( -instance.p_angle, sa, ca );
   Rotate( p0, sa, ca, center );
   Rotate( p1, sa, ca, center );

   DPoint d = p1 - p0;
   DPoint d2 = 0.5*d;

   DPoint c = center;
   double w = width;
   double h = height;

   if ( flags.bits.resizeLeft )   { center.x += d2.x; w -= d.x; }
   if ( flags.bits.resizeTop )    { center.y += d2.y; h -= d.y; }
   if ( flags.bits.resizeRight )  { center.x += d2.x; w += d.x; }
   if ( flags.bits.resizeBottom ) { center.y += d2.y; h += d.y; }

   width  = Max( 1.0, Abs( w ) );
   height = Max( 1.0, Abs( h ) );

   if ( CONSTRAINED_MOVE )
   {
      width = Round( width );
      height = Round( height );
      center.x = (int( width ) & 1) ? int( center.x ) + 0.5 : Round( center.x );
      center.y = (int( height ) & 1) ? int( center.y ) + 0.5 : Round( center.y );
   }

   Rotate( center, instance.p_angle, c );

   if ( !rotationFixed )
      rotationCenter = center;

   UpdateAnchorPosition();

   UpdateInstance();

   UpdateView();
   //view->Window().CommitPendingUpdates();

   UpdateSizePosControls();
   UpdateRotationControls();
   UpdateScaleControls();
}

void DynamicCropInterface::EndOperation()
{
   // defined for completeness
}

void DynamicCropInterface::SetRotationAngle( double a )
{
   UpdateView();

   instance.p_angle = ArcTan( Sin( a ), Cos( a ) );

   UpdateAnchorPosition();

   UpdateInstance();

   UpdateView();
   //view->Window().CommitPendingUpdates();

   UpdateSizePosControls();
   UpdateRotationControls();

}

void DynamicCropInterface::SetRotationCenter( const DPoint& p )
{
   UpdateView();

   rotationCenter = p;
   rotationFixed = p != center;

   UpdateInstance();

   UpdateView();
   //view->Window().CommitPendingUpdates();

   UpdateRotationControls();
}

void DynamicCropInterface::MoveTo( const DPoint& p )
{
   UpdateView();

   center = p;

   if ( !rotationFixed )
      rotationCenter = p;

   UpdateAnchorPosition();

   UpdateInstance();

   UpdateView();
   //view->Window().CommitPendingUpdates();

   UpdateSizePosControls();
   UpdateRotationControls();
}

void DynamicCropInterface::ResizeBy( double dL, double dT, double dR, double dB )
{
   UpdateView();

   double w2 = 0.5*width;
   double h2 = 0.5*height;
   DRect r( center.x - w2 - dL, center.y - h2 - dT,
            center.x + w2 + dR, center.y + h2 + dB );

   width = Max( 1.0, r.Width() );
   height = Max( 1.0, r.Height() );

   DPoint c = center;
   center = r.Center();
   Rotate( center, instance.p_angle, c );

   if ( !rotationFixed )
      rotationCenter = center;

   UpdateAnchorPosition();

   UpdateInstance();

   UpdateView();
   //view->Window().CommitPendingUpdates();

   UpdateSizePosControls();
   UpdateRotationControls();
   UpdateScaleControls();
}

void DynamicCropInterface::UpdateAnchorPosition()
{
   DPoint tl, tr, bl, br;
   GetRotatedRect( tl, tr, bl, br );

   switch ( anchorPoint )
   {
   case 0 : // top left
      anchor = tl;
      break;
   case 1 : // top middle
      anchor.x = 0.5*(tl.x + tr.x);
      anchor.y = 0.5*(tl.y + tr.y);
      break;
   case 2 : // top right
      anchor = tr;
      break;
   case 3 : // middle left
      anchor.x = 0.5*(tl.x + bl.x);
      anchor.y = 0.5*(tl.y + bl.y);
      break;
   case 4 : // center
      anchor.x = 0.5*(tl.x + br.x);
      anchor.y = 0.5*(tl.y + br.y);
      break;
   case 5 : // middle right
      anchor.x = 0.5*(tr.x + br.x);
      anchor.y = 0.5*(tr.y + br.y);
      break;
   case 6 : // bottom left
      anchor = bl;
      break;
   case 7 : // bottom middle
      anchor.x = 0.5*(bl.x + br.x);
      anchor.y = 0.5*(bl.y + br.y);
      break;
   case 8 : // bottom right
      anchor = br;
      break;
   }
}

void DynamicCropInterface::UpdateInstance()
{
   if ( view != 0 )
   {
      int w0 = view->Width();
      int h0 = view->Height();
      instance.p_width = width/w0;
      instance.p_height = height/h0;
      instance.p_center.x = center.x/w0;
      instance.p_center.y = center.y/h0;
   }
}

bool DynamicCropInterface::IsPointInsideRect( const DPoint& p ) const
{
   DPoint d = p;
   Rotate( d, -instance.p_angle, center );
   d -= center;
   return Abs( d.x ) <= 0.5*width && Abs( d.y ) <= 0.5*height;
}

bool DynamicCropInterface::IsPointNearRect( const DPoint& p ) const
{
   DPoint d = p;
   Rotate( d, -instance.p_angle, center );
   d -= center;

   double t = view->Window().ViewportScalarToImage( double( ImageWindow::CursorTolerance() ) );
   return Abs( d.x ) <= 0.5*width + t && Abs( d.y ) <= 0.5*height + t;
}

bool DynamicCropInterface::IsPointOnRectEdges( const DPoint& p,
                     bool& left, bool& top, bool& right, bool& bottom ) const
{
   DPoint d = p;
   Rotate( d, -instance.p_angle, center );
   d -= center;

   double t = view->Window().ViewportScalarToImage( double( ImageWindow::CursorTolerance() ) );
   double w2 = 0.5*width;
   double h2 = 0.5*height;
   left   = Abs( d.x + w2 ) <= t;
   top    = Abs( d.y + h2 ) <= t;
   right  = Abs( d.x - w2 ) <= t;
   bottom = Abs( d.y - h2 ) <= t;

   return left || top || right || bottom;
}

bool DynamicCropInterface::IsPointOnRectCenter( const DPoint& p ) const
{
   DPoint d = p - center;

   double t = view->Window().ViewportScalarToImage( double( ImageWindow::CursorTolerance() ) );
   return Abs( d.x ) <= t &&
          Abs( d.y ) <= t;
}

bool DynamicCropInterface::IsPointOnRotationCenter( const DPoint& p ) const
{
   DPoint d = p - rotationCenter;

   double t = view->Window().ViewportScalarToImage( double( ImageWindow::CursorTolerance() ) );
   return Abs( d.x ) <= t &&
          Abs( d.y ) <= t;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::InitControls()
{
   bool isView = view != 0;
   bool isColor = isView && view->IsColor();
   bool hasAlpha = isView && view->Image().HasAlphaChannels();

   GUI->Width_NumericEdit.Enable( isView );
   GUI->Height_NumericEdit.Enable( isView );
   GUI->PosX_NumericEdit.Enable( isView );
   GUI->PosY_NumericEdit.Enable( isView );
   GUI->AnchorSelectors_Control.Enable( isView );

   GUI->Angle_NumericEdit.Enable( isView );
   GUI->Clockwise_Label.Enable( isView );
   GUI->Clockwise_CheckBox.Enable( isView );
   GUI->CenterX_NumericEdit.Enable( isView );
   GUI->CenterY_NumericEdit.Enable( isView );
   GUI->Dial_Control.Enable( isView );
   GUI->OptimizeFast_CheckBox.Enable( isView );

   GUI->ScaleX_NumericEdit.Enable( isView );
   GUI->ScaledWidth_NumericEdit.Enable( isView );
   GUI->ScaleY_NumericEdit.Enable( isView );
   GUI->ScaledHeight_NumericEdit.Enable( isView );

   GUI->Algorithm_Label.Enable( isView );
   GUI->Algorithm_ComboBox.Enable( isView );
   GUI->ClampingThreshold_NumericEdit.Enable( InterpolationAlgorithm::IsClampedInterpolation( instance.p_interpolation ) );
   GUI->Smoothness_NumericEdit.Enable( InterpolationAlgorithm::IsCubicFilterInterpolation( instance.p_interpolation ) );

   GUI->Red_NumericControl.Enable( isView );
   GUI->Green_NumericControl.Enable( isColor );
   GUI->Blue_NumericControl.Enable( isColor );
   GUI->Alpha_NumericControl.Enable( hasAlpha );
   GUI->ColorSample_Control.Enable( isView );
}

void DynamicCropInterface::UpdateControls()
{
   UpdateSizePosControls();
   UpdateRotationControls();
   UpdateScaleControls();
   UpdateInterpolationControls();
   UpdateFillColorControls();
}

void DynamicCropInterface::UpdateSizePosControls()
{
   if ( !initializing )
      GUI->AnchorSelectors_Control.Update();

   GUI->Width_NumericEdit.SetValue( width );
   GUI->Height_NumericEdit.SetValue( height );
   GUI->PosX_NumericEdit.SetValue( anchor.x );
   GUI->PosY_NumericEdit.SetValue( anchor.y );
}

void DynamicCropInterface::UpdateRotationControls()
{
   if ( !initializing )
   {
      GUI->Angle_NumericEdit.SetValue( Abs( Deg( instance.p_angle ) ) );
      GUI->Clockwise_CheckBox.SetChecked( instance.p_angle < 0 );
      GUI->Dial_Control.Update();
      GUI->OptimizeFast_CheckBox.SetChecked( instance.p_optimizeFast );
   }

   GUI->CenterX_NumericEdit.SetValue( rotationCenter.x );
   GUI->CenterY_NumericEdit.SetValue( rotationCenter.y );
}

void DynamicCropInterface::UpdateScaleControls()
{
   GUI->ScaleX_NumericEdit.SetValue( instance.p_scaleX );
   GUI->ScaledWidth_NumericEdit.SetValue( RoundI( width*instance.p_scaleX ) );
   GUI->ScaleY_NumericEdit.SetValue( instance.p_scaleY );
   GUI->ScaledHeight_NumericEdit.SetValue( RoundI( height*instance.p_scaleY ) );
}

void DynamicCropInterface::UpdateInterpolationControls()
{
   GUI->Algorithm_ComboBox.SetCurrentItem( instance.p_interpolation );

   GUI->ClampingThreshold_NumericEdit.SetValue( instance.p_clampingThreshold );
   GUI->ClampingThreshold_NumericEdit.Enable( InterpolationAlgorithm::IsClampedInterpolation( instance.p_interpolation ) );

   GUI->Smoothness_NumericEdit.SetValue( instance.p_smoothness );
   GUI->Smoothness_NumericEdit.Enable( InterpolationAlgorithm::IsCubicFilterInterpolation( instance.p_interpolation ) );
}

void DynamicCropInterface::UpdateFillColorControls()
{
   GUI->Red_NumericControl.SetValue( instance.p_fillColor[0] );
   GUI->Green_NumericControl.SetValue( instance.p_fillColor[1] );
   GUI->Blue_NumericControl.SetValue( instance.p_fillColor[2] );
   GUI->Alpha_NumericControl.SetValue( instance.p_fillColor[3] );
   GUI->ColorSample_Control.Update();
}

void DynamicCropInterface::UpdateView()
{
   ImageWindow w = view->Window();

   DPoint tl, tr, bl, br;
   GetRotatedRect( tl, tr, bl, br );
   w.UpdateImageRect( Min( Min( Min( tl.x, tr.x ), bl.x ), br.x ),
                      Min( Min( Min( tl.y, tr.y ), bl.y ), br.y ),
                      Max( Max( Max( tl.x, tr.x ), bl.x ), br.x ),
                      Max( Max( Max( tl.y, tr.y ), bl.y ), br.y ) );

   double x = center.x;
   double y = center.y;
   w.ImageToViewport( x, y );
   int cx = RoundI( x );
   int cy = RoundI( y );
   w.UpdateViewportRect( cx-centerSz, cy-centerSz, cx+centerSz, cy+centerSz );

   if ( rotationCenter != center )
   {
      x = rotationCenter.x;
      y = rotationCenter.y;
      w.ImageToViewport( x, y );
      cx = RoundI( x );
      cy = RoundI( y );
      w.UpdateViewportRect( cx-centerSz, cy-centerSz, cx+centerSz, cy+centerSz );
   }
}

// ----------------------------------------------------------------------------

#define ISCOLOR   (view != 0 && view->IsColor())
#define ISGRAY    (view != 0 && !view->IsColor())

void DynamicCropInterface::__Size_ValueUpdated( NumericEdit& sender, double value )
{
   if ( view == 0 )
      return;

   if ( sender == GUI->Width_NumericEdit )
   {
      if ( value != width )
      {
         double dw = value - width;
         double dL, dR;

         switch ( anchorPoint )
         {
         default:
         case 0 : // top left
         case 3 : // middle left
         case 6 : // bottom left
            dL = 0;
            dR = dw;
            break;
         case 1 : // top middle
         case 4 : // center
         case 7 : // bottom middle
            dL = dR = 0.5*dw;
            break;
         case 2 : // top right
         case 5 : // middle right
         case 8 : // bottom right
            dL = dw;
            dR = 0;
            break;
         }

         ResizeBy( dL, 0, dR, 0 );
      }
   }
   else if ( sender == GUI->Height_NumericEdit )
   {
      if ( value != height )
      {
         double dh = value - height;
         double dT, dB;

         switch ( anchorPoint )
         {
         default:
         case 0 : // top left
         case 1 : // top middle
         case 2 : // top right
            dT = 0;
            dB = dh;
            break;
         case 3 : // middle left
         case 4 : // center
         case 5 : // middle right
            dT = dB = 0.5*dh;
            break;
         case 6 : // bottom left
         case 7 : // bottom middle
         case 8 : // bottom right
            dT = dh;
            dB = 0;
            break;
         }

         ResizeBy( 0, dT, 0, dB );
      }
   }
}

void DynamicCropInterface::__Pos_ValueUpdated( NumericEdit& sender, double value )
{
   if ( view == 0 )
      return;

   if ( sender == GUI->PosX_NumericEdit )
   {
      if ( value != anchor.x )
         MoveTo( center + DPoint( value - anchor.x, 0.0 ) );
   }
   else if ( sender == GUI->PosY_NumericEdit )
   {
      if ( value != anchor.y )
         MoveTo( center + DPoint( 0.0, value - anchor.y ) );
   }
}

void DynamicCropInterface::__AnchorSelector_Paint( Control& sender, const Rect& updateRect )
{
   Graphics g( sender );

   Rect r( sender.BoundsRect() );

   float w3 = r.Width()/3.0F;
   float h3 = r.Height()/3.0F;

   int x3 = RoundI( w3 );
   int x6 = RoundI( w3+w3 );
   int y3 = RoundI( h3 );
   int y6 = RoundI( h3+h3 );

   g.FillRect( r, RGBAColor( 0, 0, 0 ) );

   g.SetBrush( Brush::Null() );
   g.SetPen( RGBAColor( 127, 127, 127 ) );

   g.DrawLine( x3, 0, x3, r.y1 );
   g.DrawLine( x6, 0, x6, r.y1 );
   g.DrawLine( 0, y3, r.x1, y3 );
   g.DrawLine( 0, y6, r.x1, y6 );

   int x0, y0;

   if ( anchorPoint < 3 )
   {
      y0 = 0;
      x0 = (anchorPoint == 0) ? 0 : ((anchorPoint == 1) ? x3 : x6);
   }
   else if ( anchorPoint < 6 )
   {
      y0 = y3;
      x0 = (anchorPoint == 3) ? 0 : ((anchorPoint == 4) ? x3 : x6);
   }
   else
   {
      y0 = y6;
      x0 = (anchorPoint == 6) ? 0 : ((anchorPoint == 7) ? x3 : x6);
   }

   g.SetPen( RGBAColor( 255, 255, 255 ) );
   g.DrawLine( x0+3, y0+3, x0+x3-3, y0+y3-3 );
   g.DrawLine( x0+3, y0+y3-3, x0+x3-3, y0+3 );
}

void DynamicCropInterface::__AnchorSelector_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( button != MouseButton::Left )
      return;

   Rect r( sender.BoundsRect() );

   float w3 = r.Width()/3.0F;
   float h3 = r.Height()/3.0F;

   int x3 = RoundI( w3 );
   int x6 = RoundI( w3+w3 );
   int y3 = RoundI( h3 );
   int y6 = RoundI( h3+h3 );

   int row = (pos.y < y3) ? 0 : ((pos.y < y6) ? 1 : 2);
   int col = (pos.x < x3) ? 0 : ((pos.x < x6) ? 1 : 2);

   anchorPoint = 3*row + col;

   UpdateAnchorPosition();
   UpdateSizePosControls();
}

void DynamicCropInterface::__AnchorSelector_MouseRelease( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   dragging = false;
}

void DynamicCropInterface::__AnchorSelector_MouseDoubleClick( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( view == 0 )
      return;

   if ( modifiers & KeyModifier::Shift )
   {
      // Shift+DoubleClick: Move cropping rectangle to anchor position.

      int w = view->Width();
      int h = view->Height();

      DPoint p = center;

      switch ( anchorPoint )
      {
      case 0 : // top left
         p = center - anchor;
         break;
      case 1 : // top middle
         p = center + DPoint( 0.5*w - anchor.x, -anchor.y );
         break;
      case 2 : // top right
         p = center + DPoint( w-anchor.x, -anchor.y );
         break;
      case 3 : // middle left
         p = center + DPoint( -anchor.x, 0.5*h - anchor.y );
         break;
      case 4 : // center
         p = DPoint( 0.5*w, 0.5*h );
         break;
      case 5 : // middle right
         p = center + DPoint( w - anchor.x, 0.5*h - anchor.y );
         break;
      case 6 : // bottom left
         p = center + DPoint( -anchor.x, h - anchor.y );
         break;
      case 7 : // bottom middle
         p = center + DPoint( 0.5*w - anchor.x, h - anchor.y );
         break;
      case 8 : // bottom right
         p = center + DPoint( w - anchor.x, h - anchor.y );
         break;
      }

      MoveTo( p );
   }
   else if ( modifiers & KeyModifier::Control )
   {
      // Ctrl+DoubleClick: Set center of rotation to anchor.

      SetRotationCenter( anchor );
   }
   else
   {
      // DoubleClick: Center view on anchor position coordinates.

      view->Window().BringToFront();
      view->Window().SelectMainView();
      view->Window().SetViewport( anchor );
   }
}

void DynamicCropInterface::__Angle_ValueUpdated( NumericEdit& sender, double value )
{
   if ( view == 0 )
      return;

   double a = Rad( value );

   if ( GUI->Clockwise_CheckBox.IsChecked() )
      a = -a;

   SetRotationAngle( a );
}

void DynamicCropInterface::__Clockwise_Click( Button& sender, bool checked )
{
   if ( view == 0 )
      return;

   if ( Round( Abs( Deg( instance.p_angle ) ), 3 ) < 180 )
      SetRotationAngle( -instance.p_angle );
   else
      GUI->Clockwise_CheckBox.Uncheck();
}

void DynamicCropInterface::__Center_ValueUpdated( NumericEdit& sender, double value )
{
   if ( view == 0 )
      return;

   if ( sender == GUI->CenterX_NumericEdit )
      SetRotationCenter( DPoint( value, rotationCenter.y ) );
   else if ( sender == GUI->CenterY_NumericEdit )
      SetRotationCenter( DPoint( rotationCenter.x, value ) );
}

void DynamicCropInterface::__OptimizeFast_Click( Button& sender, bool checked )
{
   instance.p_optimizeFast = checked;
}

void DynamicCropInterface::__AngleDial_Paint( Control& sender, const Rect& updateRect )
{
   Graphics g( sender );

   Rect r( sender.BoundsRect() );

   int w = r.Width();
   int h = r.Height();
   int x0 = w >> 1;
   int y0 = h >> 1;

   g.FillRect( r, RGBAColor( 0, 0, 0 ) );

   g.SetBrush( Brush::Null() );
   g.SetPen( RGBAColor( 127, 127, 127 ) );
   g.DrawLine( x0, 0, x0, h );
   g.DrawLine( 0, y0, w, y0 );

   g.EnableAntialiasing();

   g.DrawEllipse( r );

   double sa, ca;
   SinCos( instance.p_angle, sa, ca );
   int x1 = RoundI( x0 + 0.5F*w*ca );
   int y1 = RoundI( y0 - 0.5F*h*sa );

   g.SetPen( RGBAColor( 255, 255, 255 ) );
   g.SetBrush( RGBAColor( 255, 255, 255 ) );
   g.DrawLine( x0, y0, x1, y1 );
   g.DrawRect( x1-2, y1-2, x1+3, y1+3 );
}

void DynamicCropInterface::__AngleDial_MouseMove( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( dragging )
   {
      double a = Round( Deg( ArcTan( double( (sender.ClientHeight() >> 1) - pos.y ),
                                     double( pos.x - (sender.ClientWidth() >> 1) ) ) ), 3 );
      SetRotationAngle( Rad( a ) );
      //sender.Update();
   }
}

void DynamicCropInterface::__AngleDial_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( button != MouseButton::Left )
      return;

   dragging = true;
   __AngleDial_MouseMove( sender, pos, buttons, modifiers );
}

void DynamicCropInterface::__AngleDial_MouseRelease( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   dragging = false;
}

void DynamicCropInterface::__Scale_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ScaleX_NumericEdit )
   {
      instance.p_scaleX = value;
      GUI->ScaledWidth_NumericEdit.SetValue( Max( 1, RoundI( value*width ) ) );
   }
   else if ( sender == GUI->ScaleY_NumericEdit )
   {
      instance.p_scaleY = value;
      GUI->ScaledHeight_NumericEdit.SetValue( Max( 1, RoundI( value*height ) ) );
   }
}

void DynamicCropInterface::__ScaledSize_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ScaledWidth_NumericEdit )
      GUI->ScaleX_NumericEdit.SetValue( instance.p_scaleX = value/width );
   else if ( sender == GUI->ScaledHeight_NumericEdit )
      GUI->ScaleY_NumericEdit.SetValue( instance.p_scaleY = value/height );
}

void DynamicCropInterface::__Algorithm_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Algorithm_ComboBox )
   {
      instance.p_interpolation = itemIndex;
      UpdateInterpolationControls();
   }
}

void DynamicCropInterface::__Algorithm_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ClampingThreshold_NumericEdit )
      instance.p_clampingThreshold = value;
   else if ( sender == GUI->Smoothness_NumericEdit )
      instance.p_smoothness = value;
}

void DynamicCropInterface::__FilColor_ValueUpdated( NumericEdit& sender, double value )
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

void DynamicCropInterface::__ColorSample_Paint( Control& sender, const Rect& updateRect )
{
   Graphics g( sender );

   RGBA color;

   if ( view == 0 || view->IsColor() )
   {
      color = RGBAColor( float( instance.p_fillColor[0] ),
                         float( instance.p_fillColor[1] ),
                         float( instance.p_fillColor[2] ) );
   }
   else
   {
      RGBColorSystem rgb;
      view->Window().GetRGBWS( rgb );
      float L = rgb.Lightness( instance.p_fillColor[0],
                               instance.p_fillColor[1],
                               instance.p_fillColor[2] );
      color = RGBAColor( L, L, L );
   }

   SetAlpha( color, uint8( RoundI( 255*instance.p_fillColor[3] ) ) );

   if ( Alpha( color ) != 0 )
   {
      g.SetBrush( Bitmap( ":/image-window/transparent-small.png" ) );
      g.SetPen( Pen::Null() );
      g.DrawRect( sender.BoundsRect() );
   }

   g.SetBrush( color );
   g.SetPen( RGBAColor( 0, 0, 0 ) );
   g.DrawRect( sender.BoundsRect() );
}

// ----------------------------------------------------------------------------

DynamicCropInterface::GUIData::GUIData( DynamicCropInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Smoothness:" ) + 'T' );
   int labelWidth3 = fnt.Width( String( 'M',  2 ) );
   int editWidth   = fnt.Width( String( '0', 10 ) );
   int editWidth2  = fnt.Width( String( '0',  8 ) );

   //

   SizePos_SectionBar.SetTitle( "Size/Position" );
   SizePos_SectionBar.SetSection( SizePos_Control );

   Width_NumericEdit.SetReal();
   Width_NumericEdit.SetPrecision( 2 );
   Width_NumericEdit.SetRange( 1, int_max );
   Width_NumericEdit.label.SetText( "Width:" );
   Width_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Width_NumericEdit.edit.SetFixedWidth( editWidth );
   Width_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Size_ValueUpdated, w );

   Height_NumericEdit.SetReal();
   Height_NumericEdit.SetPrecision( 2 );
   Height_NumericEdit.SetRange( 1, int_max );
   Height_NumericEdit.label.SetText( "Height:" );
   Height_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Height_NumericEdit.edit.SetFixedWidth( editWidth );
   Height_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Size_ValueUpdated, w );

   PosX_NumericEdit.SetReal();
   PosX_NumericEdit.SetPrecision( 2 );
   PosX_NumericEdit.SetRange( int_min, int_max );
   PosX_NumericEdit.label.SetText( "Anchor X:" );
   PosX_NumericEdit.label.SetFixedWidth( labelWidth1 );
   PosX_NumericEdit.edit.SetFixedWidth( editWidth );
   PosX_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Pos_ValueUpdated, w );

   PosY_NumericEdit.SetReal();
   PosY_NumericEdit.SetPrecision( 2 );
   PosY_NumericEdit.SetRange( int_min, int_max );
   PosY_NumericEdit.label.SetText( "Anchor Y:" );
   PosY_NumericEdit.label.SetFixedWidth( labelWidth1 );
   PosY_NumericEdit.edit.SetFixedWidth( editWidth );
   PosY_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Pos_ValueUpdated, w );

   SizePosLeft_Sizer.SetSpacing( 4 );
   SizePosLeft_Sizer.Add( Width_NumericEdit );
   SizePosLeft_Sizer.Add( Height_NumericEdit );
   SizePosLeft_Sizer.Add( PosX_NumericEdit );
   SizePosLeft_Sizer.Add( PosY_NumericEdit );

   AnchorSelectors_Control.SetBackgroundColor( RGBAColor( "black" ) );
   AnchorSelectors_Control.OnPaint( (Control::paint_event_handler)&DynamicCropInterface::__AnchorSelector_Paint, w );
   AnchorSelectors_Control.OnMousePress( (Control::mouse_button_event_handler)&DynamicCropInterface::__AnchorSelector_MousePress, w );
   AnchorSelectors_Control.OnMouseDoubleClick( (Control::mouse_event_handler)&DynamicCropInterface::__AnchorSelector_MouseDoubleClick, w );

   SizePos_Sizer.SetSpacing( 6 );
   SizePos_Sizer.Add( SizePosLeft_Sizer );
   SizePos_Sizer.Add( AnchorSelectors_Control );

   SizePos_Control.SetSizer( SizePos_Sizer );
   SizePos_Control.AdjustToContents();

   int panelSize = SizePos_Control.Height();
   AnchorSelectors_Control.SetFixedSize( panelSize, panelSize );

   SizePos_Control.AdjustToContents();

   //

   Rotation_SectionBar.SetTitle( "Rotation" );
   Rotation_SectionBar.SetSection( Rotation_Control );

   Angle_NumericEdit.SetReal();
   Angle_NumericEdit.SetPrecision( 3 );
   Angle_NumericEdit.SetRange( 0, 180 );
   Angle_NumericEdit.label.SetText( "Angle (\xb0):" );
   Angle_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Angle_NumericEdit.edit.SetFixedWidth( editWidth );
   Angle_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Angle_ValueUpdated, w );

   Clockwise_Label.SetText( "Clockwise:" );
   Clockwise_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Clockwise_Label.SetFixedWidth( labelWidth1 );

   Clockwise_CheckBox.OnClick( (Button::click_event_handler)&DynamicCropInterface::__Clockwise_Click, w );

   Clockwise_Sizer.SetSpacing( 4 );
   Clockwise_Sizer.Add( Clockwise_Label );
   Clockwise_Sizer.Add( Clockwise_CheckBox );

   CenterX_NumericEdit.SetReal();
   CenterX_NumericEdit.SetPrecision( 2 );
   CenterX_NumericEdit.SetRange( int_min, int_max );
   CenterX_NumericEdit.label.SetText( "Center X:" );
   CenterX_NumericEdit.label.SetFixedWidth( labelWidth1 );
   CenterX_NumericEdit.edit.SetFixedWidth( editWidth );
   CenterX_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Center_ValueUpdated, w );

   CenterY_NumericEdit.SetReal();
   CenterY_NumericEdit.SetPrecision( 2 );
   CenterY_NumericEdit.SetRange( int_min, int_max );
   CenterY_NumericEdit.label.SetText( "Center Y:" );
   CenterY_NumericEdit.label.SetFixedWidth( labelWidth1 );
   CenterY_NumericEdit.edit.SetFixedWidth( editWidth );
   CenterY_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Center_ValueUpdated, w );

   RotationLeft_Sizer.SetSpacing( 4 );
   RotationLeft_Sizer.Add( Angle_NumericEdit );
   RotationLeft_Sizer.Add( Clockwise_Sizer );
   RotationLeft_Sizer.Add( CenterX_NumericEdit );
   RotationLeft_Sizer.Add( CenterY_NumericEdit );

   Dial_Control.SetBackgroundColor( StringToRGBAColor( "black" ) );
   Dial_Control.SetFixedSize( panelSize, panelSize );
   Dial_Control.OnPaint( (Control::paint_event_handler)&DynamicCropInterface::__AngleDial_Paint, w );
   Dial_Control.OnMousePress( (Control::mouse_button_event_handler)&DynamicCropInterface::__AngleDial_MousePress, w );
   Dial_Control.OnMouseRelease( (Control::mouse_button_event_handler)&DynamicCropInterface::__AngleDial_MouseRelease, w );
   Dial_Control.OnMouseMove( (Control::mouse_event_handler)&DynamicCropInterface::__AngleDial_MouseMove, w );

   RotationTop_Sizer.SetSpacing( 6 );
   RotationTop_Sizer.Add( RotationLeft_Sizer );
   RotationTop_Sizer.Add( Dial_Control );

   OptimizeFast_CheckBox.SetText( "Use fast rotations" );
   OptimizeFast_CheckBox.OnClick( (Button::click_event_handler)&DynamicCropInterface::__OptimizeFast_Click, w );

   RotationBottom_Sizer.AddSpacing( labelWidth1+4 );
   RotationBottom_Sizer.Add( OptimizeFast_CheckBox, 100 );

   Rotation_Sizer.SetSpacing( 4 );
   Rotation_Sizer.Add( RotationTop_Sizer );
   Rotation_Sizer.Add( RotationBottom_Sizer );

   Rotation_Control.SetSizer( Rotation_Sizer );

   //

   Scale_SectionBar.SetTitle( "Scale" );
   Scale_SectionBar.SetSection( Scale_Control );

   ScaleX_NumericEdit.SetReal();
   ScaleX_NumericEdit.SetPrecision( 5 );
   ScaleX_NumericEdit.SetRange( 0.00001, 100 );
   ScaleX_NumericEdit.label.SetText( "Scale X:" );
   ScaleX_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ScaleX_NumericEdit.edit.SetFixedWidth( editWidth );
   ScaleX_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Scale_ValueUpdated, w );

   ScaledWidth_NumericEdit.SetInteger();
   ScaledWidth_NumericEdit.SetRange( 1, uint16_max );
   ScaledWidth_NumericEdit.label.SetText( "Width:" );
   ScaledWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ScaledWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__ScaledSize_ValueUpdated, w );

   ScaleX_Sizer.SetSpacing( 4 );
   ScaleX_Sizer.Add( ScaleX_NumericEdit );
   ScaleX_Sizer.AddStretch();
   ScaleX_Sizer.Add( ScaledWidth_NumericEdit );

   ScaleY_NumericEdit.SetReal();
   ScaleY_NumericEdit.SetPrecision( 5 );
   ScaleY_NumericEdit.SetRange( 0.00001, 100 );
   ScaleY_NumericEdit.label.SetText( "Scale Y:" );
   ScaleY_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ScaleY_NumericEdit.edit.SetFixedWidth( editWidth );
   ScaleY_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Scale_ValueUpdated, w );

   ScaledHeight_NumericEdit.SetInteger();
   ScaledHeight_NumericEdit.SetRange( 1, uint16_max );
   ScaledHeight_NumericEdit.label.SetText( "Height:" );
   ScaledHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ScaledHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__ScaledSize_ValueUpdated, w );

   ScaleY_Sizer.SetSpacing( 4 );
   ScaleY_Sizer.Add( ScaleY_NumericEdit );
   ScaleY_Sizer.AddStretch();
   ScaleY_Sizer.Add( ScaledHeight_NumericEdit );

   Scale_Sizer.SetSpacing( 4 );
   Scale_Sizer.Add( ScaleX_Sizer );
   Scale_Sizer.Add( ScaleY_Sizer );

   Scale_Control.SetSizer( Scale_Sizer );

   //

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
   Algorithm_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DynamicCropInterface::__Algorithm_ItemSelected, w );

   Algorithm_Sizer.SetSpacing( 4 );
   Algorithm_Sizer.Add( Algorithm_Label );
   Algorithm_Sizer.Add( Algorithm_ComboBox, 100 );

   ClampingThreshold_NumericEdit.SetReal();
   ClampingThreshold_NumericEdit.SetPrecision( TheClampingThresholdDynamicCropParameter->Precision() );
   ClampingThreshold_NumericEdit.SetRange( TheClampingThresholdDynamicCropParameter->MinimumValue(),
                                           TheClampingThresholdDynamicCropParameter->MaximumValue() );
   ClampingThreshold_NumericEdit.label.SetText( "Clamping:" );
   ClampingThreshold_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ClampingThreshold_NumericEdit.SetToolTip( "<p>Deringing clamping threshold for bicubic spline and Lanczos interpolation algorithms.</p>" );
   ClampingThreshold_NumericEdit.sizer.AddStretch();
   ClampingThreshold_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Algorithm_ValueUpdated, w );

   Smoothness_NumericEdit.SetReal();
   Smoothness_NumericEdit.SetPrecision( TheSmoothnessDynamicCropParameter->Precision() );
   Smoothness_NumericEdit.SetRange( TheSmoothnessDynamicCropParameter->MinimumValue(),
                                    TheSmoothnessDynamicCropParameter->MaximumValue() );
   Smoothness_NumericEdit.label.SetText( "Smoothness:" );
   Smoothness_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Smoothness_NumericEdit.SetToolTip( "<p>Smoothness level for cubic filter interpolation algorithms.</p>" );
   Smoothness_NumericEdit.sizer.AddStretch();
   Smoothness_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Algorithm_ValueUpdated, w );

   Interpolation_Sizer.SetSpacing( 4 );
   Interpolation_Sizer.Add( Algorithm_Sizer );
   Interpolation_Sizer.Add( ClampingThreshold_NumericEdit );
   Interpolation_Sizer.Add( Smoothness_NumericEdit );

   Interpolation_Control.SetSizer( Interpolation_Sizer );

   //

   FillColor_SectionBar.SetTitle( "Fill Color" );
   FillColor_SectionBar.SetSection( FillColor_Control );

   Red_NumericControl.label.SetText( "R:" );
   Red_NumericControl.label.SetFixedWidth( labelWidth3 );
   Red_NumericControl.slider.SetRange( 0, 100 );
   Red_NumericControl.SetReal();
   Red_NumericControl.SetRange( 0, 1 );
   Red_NumericControl.SetPrecision( 6 );
   Red_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__FilColor_ValueUpdated, w );

   Green_NumericControl.label.SetText( "G:" );
   Green_NumericControl.label.SetFixedWidth( labelWidth3 );
   Green_NumericControl.slider.SetRange( 0, 100 );
   Green_NumericControl.SetReal();
   Green_NumericControl.SetRange( 0, 1 );
   Green_NumericControl.SetPrecision( 6 );
   Green_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__FilColor_ValueUpdated, w );

   Blue_NumericControl.label.SetText( "B:" );
   Blue_NumericControl.label.SetFixedWidth( labelWidth3 );
   Blue_NumericControl.slider.SetRange( 0, 100 );
   Blue_NumericControl.SetReal();
   Blue_NumericControl.SetRange( 0, 1 );
   Blue_NumericControl.SetPrecision( 6 );
   Blue_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__FilColor_ValueUpdated, w );

   Alpha_NumericControl.label.SetText( "A:" );
   Alpha_NumericControl.label.SetFixedWidth( labelWidth3 );
   Alpha_NumericControl.slider.SetRange( 0, 100 );
   Alpha_NumericControl.SetReal();
   Alpha_NumericControl.SetRange( 0, 1 );
   Alpha_NumericControl.SetPrecision( 6 );
   Alpha_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__FilColor_ValueUpdated, w );

   ColorSample_Control.SetFixedHeight( 20 );
   ColorSample_Control.OnPaint( (Control::paint_event_handler)&DynamicCropInterface::__ColorSample_Paint, w );

   FillColor_Sizer.SetSpacing( 4 );
   FillColor_Sizer.Add( Red_NumericControl );
   FillColor_Sizer.Add( Green_NumericControl );
   FillColor_Sizer.Add( Blue_NumericControl );
   FillColor_Sizer.Add( Alpha_NumericControl );
   FillColor_Sizer.Add( ColorSample_Control );

   FillColor_Control.SetSizer( FillColor_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( SizePos_SectionBar );
   Global_Sizer.Add( SizePos_Control );
   Global_Sizer.Add( Rotation_SectionBar );
   Global_Sizer.Add( Rotation_Control );
   Global_Sizer.Add( Scale_SectionBar );
   Global_Sizer.Add( Scale_Control );
   Global_Sizer.Add( Interpolation_SectionBar );
   Global_Sizer.Add( Interpolation_Control );
   Global_Sizer.Add( FillColor_SectionBar );
   Global_Sizer.Add( FillColor_Control );

   w.SetSizer( Global_Sizer );

   w.AdjustToContents();
   w.SetFixedWidth();

   Scale_Control.Hide();
   Interpolation_Control.Hide();
   FillColor_Control.Hide();

   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF DynamicCropInterface.cpp - Released 2014/10/29 07:34:55 UTC
