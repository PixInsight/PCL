//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.02.0398
// ----------------------------------------------------------------------------
// DynamicCropInterface.cpp - Released 2018-12-12T09:25:24Z
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

#include "DynamicCropInterface.h"
#include "DynamicCropPreferencesDialog.h"
#include "DynamicCropProcess.h"

#include <pcl/ImageWindow.h>
#include <pcl/MetaModule.h>
#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

// Half size of center mark in logical viewport coordinates
#define CENTER_RADIUS  5

// ----------------------------------------------------------------------------

DynamicCropInterface* TheDynamicCropInterface = nullptr;

// ----------------------------------------------------------------------------

#include "DynamicCropIcon.xpm"

// ----------------------------------------------------------------------------

DynamicCropInterface::DynamicCropInterface() :
   instance( TheDynamicCropProcess ),
   m_view(),
   m_width( 0 ),
   m_height( 0 ),
   m_center( 0 ),
   m_rotationCenter( 0 ),
   m_rotationFixed( false ),
   m_anchorPoint( 4 ), // center
   m_anchor( 0 ),
   m_flags(),
   m_dragging( false ),
   m_dragOrigin( 0 ),
   m_initializing( false ),
   m_rect( 0 ),
   m_selectionColor( 0xFFFFFFFF ),
   m_centerColor( 0xFFFFFFFF ),
   m_fillColor( 0x28FFFFFF )
{
   TheDynamicCropInterface = this;
}

// ----------------------------------------------------------------------------

DynamicCropInterface::~DynamicCropInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
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
   if ( !m_view.IsNull() )
   {
      /*
       * Obtain local working references to the target view and window, then
       * reset our reference to the target view now to prevent inconsistencies
       * in case an exception is thrown.
       */
      View view = m_view;
      m_view = View();
      ImageWindow window = view.Window();

      /*
       * Since active dynamic targets cannot be modified, we have to remove our
       * target view from the dynamic targets set before attempting to process.
       */
      view.RemoveFromDynamicTargets();

      /*
       * Ensure that our target view is selected as the current view.
       */
      window.BringToFront();
      window.SelectView( view );

      /*
       * Execute the instance on the target window.
       */
      instance.LaunchOn( window );

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

bool DynamicCropInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& flags )
{
   if ( GUI == nullptr )
   {
      Module->LoadResource( "@module_resource_dir/cursors.rcc" );
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
   if ( dynamic_cast<const DynamicCropInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a DynamicCrop instance.";
   return false;
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
   if ( i == nullptr )
      throw Error( "Not a DynamicCrop instance." );

   if ( m_view.IsNull() )
   {
      ImageWindow window = ImageWindow::ActiveWindow();
      if ( window.IsNull() )
      {
         throw Error( "DynamicCrop: No active image window" );
         return false;
      }

      m_view = window.MainView();
      m_view.AddToDynamicTargets();
      //Console().WriteLn( "<end><cbr>DynamicCrop: Selected target view: " + m_view.Id() );
   }
   else
      UpdateView();

   instance.Assign( *i );

   int w0 = m_view.Width();
   int h0 = m_view.Height();

   m_width = Max( 1.0, Round( instance.p_width*w0, 2 ) );
   m_height = Max( 1.0, Round( instance.p_height*h0, 2 ) );

   m_anchor.x = m_center.x = m_rotationCenter.x = Round( instance.p_center.x*w0, 2 );
   m_anchor.y = m_center.y = m_rotationCenter.y = Round( instance.p_center.y*h0, 2 );
   m_anchorPoint = 4; // center

   m_rotationFixed = false;

   if ( m_view.Window().CurrentView() != m_view )
      m_view.Window().SelectView( m_view );
   else
   {
      UpdateView();
      //m_view.Window().CommitPendingUpdates();
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
   m_view = View();

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

void DynamicCropInterface::DynamicMouseEnter( View& view )
{
   if ( m_view.IsNull() || view != m_view )
      return;

   // Force a dynamic cursor update upon a subsequent mouse move event.
   m_flags = Flags();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::DynamicMouseMove( View& view, const DPoint& p, unsigned buttons, unsigned modifiers )
{
   if ( m_view.IsNull() || view != m_view )
      return;

   if ( m_dragging )
   {
      if ( m_initializing )
      {
         ImageWindow window = m_view.Window();

         window.ModifySelection( RoundInt( p.x ), RoundInt( p.y ) );
         //window.CommitPendingUpdates();

         m_rect = window.SelectionRect();
         m_width = Max( 1, m_rect.Width() );
         m_height = Max( 1, m_rect.Height() );
         m_rotationCenter = m_center = m_rect.Center();

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
      if ( f != m_flags )
      {
         m_flags = f;
         UpdateDynamicCursor();
      }
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::DynamicMousePress( View& view, const DPoint& p, int button, unsigned buttons, unsigned modifiers )
{
   if ( button != MouseButton::Left )
      return;

   m_dragging = true;

   if ( !m_view.IsNull() )
   {
      if ( view != m_view )
         return;

      BeginOperation( p, modifiers );
      UpdateDynamicCursor();
   }
   else
   {
      if ( !view.IsMainView() )
         throw Error( "DynamicCrop cannot run on previews. Please select a main view." );

      m_initializing = true;

      m_view = view;

      ImageWindow window = m_view.Window();
      window.BeginSelection( RoundInt( p.x ), RoundInt( p.y ) );

      m_rect = window.SelectionRect();
      m_width = m_height = 1;
      m_rotationCenter = m_center = m_rect.Center();

      UpdateControls();
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::DynamicMouseRelease( View& view, const DPoint& p, int button, unsigned buttons, unsigned modifiers )
{
   if ( m_dragging )
   {
      m_dragging = false;

      if ( m_initializing )
      {
         ImageWindow window = m_view.Window();
         Rect r = window.SelectionRect();
         window.EndSelection();

         m_view.AddToDynamicTargets();

         m_initializing = false;
         Initialize( r );

         m_flags = OperationInfo( p );
         UpdateDynamicCursor();
      }
      else
      {
         if ( m_view.IsNull() || view != m_view )
            return;

         UpdateDynamicCursor();
         EndOperation();
      }
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::DynamicMouseDoubleClick( View& view, const DPoint& p, unsigned buttons, unsigned modifiers )
{
   if ( m_view.IsNull() || view != m_view )
      return;

   if ( IsPointOnRotationCenter( p ) )
   {
      SetRotationCenter( m_center );
      m_rotationFixed = false;
      m_flags = OperationInfo( p );
      UpdateDynamicCursor();
   }
   else if ( IsPointInsideRect( p ) )
      Execute();
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::DynamicKeyPress( View& view, int key, unsigned modifiers )
{
   if ( m_view.IsNull() || view != m_view )
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

bool DynamicCropInterface::RequiresDynamicUpdate( const View& view, const DRect& updateRect ) const
{
   if ( m_view.IsNull() || view != m_view || m_initializing )
      return false;

   ImageWindow window = view.Window();

   // Check intersection with current cropping rectangle
   DRect r;
   if ( instance.p_angle == 0 )
      GetUnrotatedRect( r );
   else
      GetRotatedBounds( r );
   if ( r.Intersects( updateRect ) )
      return true;

   // Obtain the half-size of center marks in image coordinates
   double dr = window.ViewportScalarToImage( window.DisplayPixelRatio()*CENTER_RADIUS + 1 ); // add 1 pixel to guard against roundoff errors

   // Check intersection with the cropping rectangle's center mark
   if ( DRect( m_center.x-dr, m_center.y-dr,
               m_center.x+dr, m_center.y+dr ).Intersects( updateRect ) )
      return true;

   // Check intersection with the rotation center mark
   if ( DRect( m_rotationCenter.x-dr, m_rotationCenter.y-dr,
               m_rotationCenter.x+dr, m_rotationCenter.y+dr ).Intersects( updateRect ) )
      return true;

   return false;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::PaintRect( VectorGraphics& g, ImageWindow& window ) const
{
   // Auxiliary routine to paint a cropping rectangle

   if ( instance.p_angle == 0 )
   {
      // Optimize for zero rotation
      DRect r;
      GetUnrotatedRect( r );
      g.DrawRect( window.ImageToViewport( r ) );
   }
   else
   {
      Array<DPoint> r( 4 );
      GetRotatedRect( r[0], r[1], r[3], r[2] );
      for ( DPoint& p : r )
         window.ImageToViewport( p.x, p.y );
      g.DrawPolygon( r );
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::DynamicPaint( const View& view, VectorGraphics& g, const DRect& ur ) const
{
   if ( m_view.IsNull() || view != m_view )
      return;

   ImageWindow window = m_view.Window();

   double f = window.DisplayPixelRatio();

   g.EnableAntialiasing();

   // Draw the translucent cropping rectangle
   if ( Alpha( m_fillColor ) != 0 )
   {
      g.SetPen( Pen::Null() );
      g.SetBrush( m_fillColor );
      PaintRect( g, window );
   }

   g.SetCompositionOperator( CompositionOp::Difference );

   // Draw the cropping rectangle
   g.SetPen( m_selectionColor, f );
   g.SetBrush( Brush::Null() );
   PaintRect( g, window );

   // Center mark radius in physical pixels
   double dr = f * CENTER_RADIUS;

   // Draw the center of the cropping rectangle
   DPoint c0 = window.ImageToViewport( m_center );
   DPoint c1( c0.x-dr, c0.y-dr );
   DPoint c2( c0.x+dr, c0.y+dr );
   g.SetPen( m_centerColor, f );
   g.DrawLine( c1.x, c1.y, c2.x, c2.y );
   g.DrawLine( c2.x, c1.y, c1.x, c2.y );

   // Draw the center of rotation
   DPoint c3 = window.ImageToViewport( m_rotationCenter );
   if ( c3.ManhattanDistanceTo( c0 ) > 0.5 )
   {
      DPoint c4( c3.x-dr, c3.y-dr );
      DPoint c5( c3.x+dr, c3.y+dr );
      g.DrawLine( c3.x, c4.y, c3.x, c5.y );
      g.DrawLine( c4.x, c3.y, c5.x, c3.y );
   }
   g.DrawCircle( c3, dr );
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::WantsReadoutNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateReadout( const View& view, const DPoint& p, double R, double G, double B, double /*A*/ )
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

void DynamicCropInterface::SaveSettings() const
{
   Settings::Write( SettingsKey() + "SelectionColor", m_selectionColor );
   Settings::Write( SettingsKey() + "CenterColor", m_centerColor );
   Settings::Write( SettingsKey() + "FillColor", m_fillColor );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::LoadSettings()
{
   Settings::Read( SettingsKey() + "SelectionColor", m_selectionColor );
   Settings::Read( SettingsKey() + "CenterColor", m_centerColor );
   Settings::Read( SettingsKey() + "FillColor", m_fillColor );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::Initialize( const Rect& r )
{
   if ( m_view.IsNull() )
      return;

   m_width = Max( 1, r.Width() );
   m_height = Max( 1, r.Height() );
   m_rect = Rect( TruncInt( m_width ), TruncInt( m_height ) ) + r.Ordered().LeftTop();
   m_anchor = m_center = m_rotationCenter = DRect( m_rect ).Center();
   m_anchorPoint = 4; // center
   m_rotationFixed = false;

   int w0 = m_view.Width();
   int h0 = m_view.Height();

   instance.p_center.x = m_center.x/w0;
   instance.p_center.y = m_center.y/h0;
   instance.p_width = m_width/w0;
   instance.p_height = m_height/h0;
   instance.p_angle = 0;
   instance.p_scaleX = instance.p_scaleY = 1;

   UpdateView();
   //m_view.Window().CommitPendingUpdates();

   InitControls();
   UpdateControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::GetRotatedRect( DPoint& topLeft, DPoint& topRight, DPoint& bottomLeft, DPoint& bottomRight ) const
{
   double w2 = 0.5*m_width;
   double h2 = 0.5*m_height;
   bottomLeft.x   = topLeft.x    = m_center.x - w2;
   topRight.y     = topLeft.y    = m_center.y - h2;
   bottomRight.x  = topRight.x   = m_center.x + w2;
   bottomRight.y  = bottomLeft.y = m_center.y + h2;

   if ( instance.p_angle != 0 )
   {
      double sa, ca;
      SinCos( instance.p_angle, sa, ca );
      Rotate( topLeft, sa, ca, m_center );
      Rotate( topRight, sa, ca, m_center );
      Rotate( bottomLeft, sa, ca, m_center );
      Rotate( bottomRight, sa, ca, m_center );
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::GetRotatedBounds( DRect& r ) const
{
   DPoint tl, tr, bl, br;
   GetRotatedRect( tl, tr, bl, br );
   r.x0 = Min( Min( Min( tl.x, tr.x ), bl.x ), br.x );
   r.y0 = Min( Min( Min( tl.y, tr.y ), bl.y ), br.y );
   r.x1 = Max( Max( Max( tl.x, tr.x ), bl.x ), br.x );
   r.y1 = Max( Max( Max( tl.y, tr.y ), bl.y ), br.y );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::GetUnrotatedRect( DRect& r ) const
{
   double w2 = 0.5*m_width;
   double h2 = 0.5*m_height;
   r.x0 = m_center.x - w2;
   r.y0 = m_center.y - h2;
   r.x1 = m_center.x + w2;
   r.y1 = m_center.y + h2;
}

// ----------------------------------------------------------------------------

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
      else // implies IsPointInsideRect( p )
         f.bits.moving = true;
   }
   else // outside rect+tolerance
      f.bits.rotating = true;

   return f;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateDynamicCursor() const
{
   String csr;

   if ( m_flags.bits.movingCenter )
      csr = m_dragging ? "move_point_drag.png" : "move_point.png";
   else if ( m_flags.bits.resizing )
   {
      if ( m_flags.bits.resizeLeft )
      {
         if ( m_flags.bits.resizeTop )
            csr = "resize_left_top.png";
         else if ( m_flags.bits.resizeBottom )
            csr = "resize_left_bottom.png";
         else
            csr = "resize_left.png";
      }
      else if ( m_flags.bits.resizeRight )
      {
         if ( m_flags.bits.resizeTop )
            csr = "resize_right_top.png";
         else if ( m_flags.bits.resizeBottom )
            csr = "resize_right_bottom.png";
         else
            csr = "resize_right.png";
      }
      else if ( m_flags.bits.resizeTop )
         csr = "resize_top.png";
      else if ( m_flags.bits.resizeBottom )
         csr = "resize_bottom.png";
   }
   else if ( m_flags.bits.moving )
      csr = m_dragging ? "move_drag.png" : "move.png";
   else if ( m_flags.bits.rotating )
      csr = "rotate.png";

   if ( !csr.IsEmpty() )
      m_view.Window().SetDynamicCursor( ScaledResource( ":/@module_root/" + csr ), ScaledCursorHotSpot( 10, 10 ) );
   else
      m_view.Window().ResetDynamicCursor();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::BeginOperation( const DPoint& p, unsigned modifiers )
{
   m_flags = OperationInfo( p );
   m_dragOrigin = (modifiers & KeyModifier::Shift) ? p.Rounded() : p;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateOperation( const DPoint& p, unsigned modifiers )
{
   DPoint q = p;

   if ( m_flags.bits.movingCenter )
      UpdateCenterMove( q, modifiers );
   else if ( m_flags.bits.resizing )
      UpdateResize( q, modifiers );
   else if ( m_flags.bits.moving )
      UpdateMove( q, modifiers );
   else if ( m_flags.bits.rotating )
      UpdateRotation( q, modifiers );

   m_dragOrigin = q;
}

// ----------------------------------------------------------------------------

#define CONSTRAINED_MOVE   (!(modifiers & KeyModifier::Shift) && instance.p_angle == 0)

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateRotation( DPoint& p, unsigned /*modifiers*/ )
{
   UpdateView();

   double da = RotationAngle( p ) - RotationAngle( m_dragOrigin );

   Rotate( m_center, da, m_rotationCenter );

   double a = instance.p_angle + da;
   instance.p_angle = ArcTan( Sin( a ), Cos( a ) );

   UpdateAnchorPosition();

   UpdateInstance();

   UpdateView();
   //m_view.Window().CommitPendingUpdates();

   UpdateSizePosControls();
   UpdateRotationControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateCenterMove( DPoint& p, unsigned modifiers )
{
   if ( CONSTRAINED_MOVE )
      p = (2.0*p).Rounded()/2.0;

   SetRotationCenter( p );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateMove( DPoint& p, unsigned modifiers )
{
   if ( CONSTRAINED_MOVE )
   {
      UpdateView();
      m_width = Round( m_width );
      m_height = Round( m_height );
      m_center.x = (int( m_width ) & 1) ? int( m_center.x ) + 0.5 : Round( m_center.x );
      m_center.y = (int( m_height ) & 1) ? int( m_center.y ) + 0.5 : Round( m_center.y );
      p = p.Rounded();
   }

   MoveTo( m_center + p - m_dragOrigin );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateResize( DPoint& p, unsigned modifiers )
{
   UpdateView();

   if ( CONSTRAINED_MOVE )
      p = p.Rounded();

   DPoint p0 = m_dragOrigin;
   DPoint p1 = p;
   double sa, ca;
   SinCos( -instance.p_angle, sa, ca );
   Rotate( p0, sa, ca, m_center );
   Rotate( p1, sa, ca, m_center );

   DPoint d = p1 - p0;
   DPoint d2 = 0.5*d;

   DPoint c = m_center;
   double w = m_width;
   double h = m_height;

   if ( modifiers & KeyModifier::Control )
   {
      double rw = w/h;
      double rh = h/w;

      bool resizeTop = m_flags.bits.resizeTop;
      bool resizeBottom = m_flags.bits.resizeBottom;

      if ( m_flags.bits.resizeLeft )
      {
         if ( resizeTop )
         {
            if ( Abs( d.x ) < Abs( d.y ) )
            {
               h -= d.y;
               m_center.y += d2.y;
               w = rw*h;
               m_center.x -= 0.5*(w - m_width);
            }
            else
            {
               w -= d.x;
               m_center.x += d2.x;
               h = rh*w;
               m_center.y -= 0.5*(h - m_height);
            }
            resizeTop = false;
         }
         else if ( resizeBottom )
         {
            if ( Abs( d.x ) < Abs( d.y ) )
            {
               h += d.y;
               m_center.y += d2.y;
               w = rw*h;
               m_center.x -= 0.5*(w - m_width);
            }
            else
            {
               w -= d.x;
               m_center.x += d2.x;
               h = rh*w;
               m_center.y += 0.5*(h - m_height);
            }
            resizeBottom = false;
         }
         else
         {
            m_center.x += d2.x;
            w -= d.x;
            h = rh*w;
         }
      }

      if ( m_flags.bits.resizeRight )
      {
         if ( resizeTop )
         {
            if ( Abs( d.x ) < Abs( d.y ) )
            {
               h -= d.y;
               m_center.y += d2.y;
               w = rw*h;
               m_center.x += 0.5*(w - m_width);
            }
            else
            {
               w += d.x;
               m_center.x += d2.x;
               h = rh*w;
               m_center.y -= 0.5*(h - m_height);
            }
            resizeTop = false;
         }
         else if ( resizeBottom )
         {
            if ( Abs( d.x ) < Abs( d.y ) )
            {
               h += d.y;
               m_center.y += d2.y;
               w = rw*h;
               m_center.x += 0.5*(w - m_width);
            }
            else
            {
               w += d.x;
               m_center.x += d2.x;
               h = rh*w;
               m_center.y += 0.5*(h - m_height);
            }
            resizeBottom = false;
         }
         else
         {
            m_center.x += d2.x;
            w += d.x;
            h = rh*w;
         }
      }

      if ( resizeTop )
      {
         m_center.y += d2.y;
         h -= d.y;
         w = rw*h;
      }

      if ( resizeBottom )
      {
         m_center.y += d2.y;
         h += d.y;
         w = rw*h;
      }
   }
   else
   {
      if ( m_flags.bits.resizeLeft )
      {
         m_center.x += d2.x;
         w -= d.x;
      }

      if ( m_flags.bits.resizeTop )
      {
         m_center.y += d2.y;
         h -= d.y;
      }

      if ( m_flags.bits.resizeRight )
      {
         m_center.x += d2.x;
         w += d.x;
      }

      if ( m_flags.bits.resizeBottom )
      {
         m_center.y += d2.y;
         h += d.y;
      }
   }

   m_width  = Max( 1.0, Abs( w ) );
   m_height = Max( 1.0, Abs( h ) );

   if ( CONSTRAINED_MOVE )
   {
      m_width = Round( m_width );
      m_height = Round( m_height );
      m_center.x = (int( m_width ) & 1) ? int( m_center.x ) + 0.5 : Round( m_center.x );
      m_center.y = (int( m_height ) & 1) ? int( m_center.y ) + 0.5 : Round( m_center.y );
   }

   Rotate( m_center, instance.p_angle, c );

   if ( !m_rotationFixed )
      m_rotationCenter = m_center;

   UpdateAnchorPosition();

   UpdateInstance();

   UpdateView();
   //m_view.Window().CommitPendingUpdates();

   UpdateSizePosControls();
   UpdateRotationControls();
   UpdateScaleControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::EndOperation()
{
   // defined for completeness
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::SetRotationAngle( double a )
{
   UpdateView();

   instance.p_angle = ArcTan( Sin( a ), Cos( a ) );

   UpdateAnchorPosition();

   UpdateInstance();

   UpdateView();
   //m_view.Window().CommitPendingUpdates();

   UpdateSizePosControls();
   UpdateRotationControls();

}

// ----------------------------------------------------------------------------

void DynamicCropInterface::SetRotationCenter( const DPoint& p )
{
   UpdateView();

   m_rotationCenter = p;
   m_rotationFixed = p != m_center;

   UpdateInstance();

   UpdateView();
   //m_view.Window().CommitPendingUpdates();

   UpdateRotationControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::MoveTo( const DPoint& p )
{
   UpdateView();

   m_center = p;

   if ( !m_rotationFixed )
      m_rotationCenter = p;

   UpdateAnchorPosition();

   UpdateInstance();

   UpdateView();
   //m_view.Window().CommitPendingUpdates();

   UpdateSizePosControls();
   UpdateRotationControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::ResizeBy( double dL, double dT, double dR, double dB )
{
   UpdateView();

   double w2 = 0.5*m_width;
   double h2 = 0.5*m_height;
   DRect r( m_center.x - w2 - dL, m_center.y - h2 - dT,
            m_center.x + w2 + dR, m_center.y + h2 + dB );

   m_width = Max( 1.0, r.Width() );
   m_height = Max( 1.0, r.Height() );

   DPoint c = m_center;
   m_center = r.Center();
   Rotate( m_center, instance.p_angle, c );

   if ( !m_rotationFixed )
      m_rotationCenter = m_center;

   UpdateAnchorPosition();

   UpdateInstance();

   UpdateView();
   //m_view.Window().CommitPendingUpdates();

   UpdateSizePosControls();
   UpdateRotationControls();
   UpdateScaleControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateAnchorPosition()
{
   DPoint tl, tr, bl, br;
   GetRotatedRect( tl, tr, bl, br );

   switch ( m_anchorPoint )
   {
   case 0 : // top left
      m_anchor = tl;
      break;
   case 1 : // top middle
      m_anchor.x = 0.5*(tl.x + tr.x);
      m_anchor.y = 0.5*(tl.y + tr.y);
      break;
   case 2 : // top right
      m_anchor = tr;
      break;
   case 3 : // middle left
      m_anchor.x = 0.5*(tl.x + bl.x);
      m_anchor.y = 0.5*(tl.y + bl.y);
      break;
   case 4 : // center
      m_anchor.x = 0.5*(tl.x + br.x);
      m_anchor.y = 0.5*(tl.y + br.y);
      break;
   case 5 : // middle right
      m_anchor.x = 0.5*(tr.x + br.x);
      m_anchor.y = 0.5*(tr.y + br.y);
      break;
   case 6 : // bottom left
      m_anchor = bl;
      break;
   case 7 : // bottom middle
      m_anchor.x = 0.5*(bl.x + br.x);
      m_anchor.y = 0.5*(bl.y + br.y);
      break;
   case 8 : // bottom right
      m_anchor = br;
      break;
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateInstance()
{
   if ( !m_view.IsNull() )
   {
      int w0 = m_view.Width();
      int h0 = m_view.Height();
      instance.p_width = m_width/w0;
      instance.p_height = m_height/h0;
      instance.p_center.x = m_center.x/w0;
      instance.p_center.y = m_center.y/h0;
   }
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::IsPointInsideRect( const DPoint& p ) const
{
   DPoint d = p;
   Rotate( d, -instance.p_angle, m_center );
   d -= m_center;
   return Abs( d.x ) <= 0.5*m_width && Abs( d.y ) <= 0.5*m_height;
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::IsPointNearRect( const DPoint& p ) const
{
   DPoint d = p;
   Rotate( d, -instance.p_angle, m_center );
   d -= m_center;
   double t = m_view.Window().ViewportScalarToImage( double( ImageWindow::CursorTolerance() ) );
   return Abs( d.x ) <= 0.5*m_width + t && Abs( d.y ) <= 0.5*m_height + t;
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::IsPointOnRectEdges( const DPoint& p, bool& left, bool& top, bool& right, bool& bottom ) const
{
   DPoint d = p;
   Rotate( d, -instance.p_angle, m_center );
   d -= m_center;
   double t = m_view.Window().ViewportScalarToImage( double( ImageWindow::CursorTolerance() ) );
   double w2 = 0.5*m_width;
   double h2 = 0.5*m_height;
   left   = Abs( d.x + w2 ) <= t;
   top    = Abs( d.y + h2 ) <= t;
   right  = Abs( d.x - w2 ) <= t;
   bottom = Abs( d.y - h2 ) <= t;
   return left || top || right || bottom;
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::IsPointOnRectCenter( const DPoint& p ) const
{
   DPoint d = p - m_center;
   double t = m_view.Window().ViewportScalarToImage( double( ImageWindow::CursorTolerance() ) );
   return Abs( d.x ) <= t && Abs( d.y ) <= t;
}

// ----------------------------------------------------------------------------

bool DynamicCropInterface::IsPointOnRotationCenter( const DPoint& p ) const
{
   DPoint d = p - m_rotationCenter;
   double t = m_view.Window().ViewportScalarToImage( double( ImageWindow::CursorTolerance() ) );
   return Abs( d.x ) <= t && Abs( d.y ) <= t;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DynamicCropInterface::InitControls()
{
   bool isView = !m_view.IsNull();
   bool isColor = isView && m_view.IsColor();
   bool hasAlpha = isView && m_view.Image().HasAlphaChannels();

   GUI->SizePos_Control.Enable( isView );
   GUI->Rotation_Control.Enable( isView );
   GUI->Scale_Control.Enable( isView );
   GUI->Interpolation_Control.Enable( isView );
   GUI->Resolution_Control.Enable( isView );
   GUI->FillColor_Control.Enable( isView );
   GUI->Green_NumericControl.Enable( isColor );
   GUI->Blue_NumericControl.Enable( isColor );
   GUI->Alpha_NumericControl.Enable( hasAlpha );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateControls()
{
   UpdateSizePosControls();
   UpdateRotationControls();
   UpdateScaleControls();
   UpdateInterpolationControls();
   UpdateResolutionControls();
   UpdateFillColorControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateSizePosControls()
{
   if ( !m_initializing )
      GUI->AnchorSelectors_Control.Update();

   GUI->Width_NumericEdit.SetValue( m_width );
   GUI->Height_NumericEdit.SetValue( m_height );
   GUI->PosX_NumericEdit.SetValue( m_anchor.x );
   GUI->PosY_NumericEdit.SetValue( m_anchor.y );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateRotationControls()
{
   if ( !m_initializing )
   {
      GUI->Angle_NumericEdit.SetValue( Abs( Deg( instance.p_angle ) ) );
      GUI->Clockwise_CheckBox.SetChecked( instance.p_angle < 0 );
      GUI->Dial_Control.Update();
      GUI->OptimizeFast_CheckBox.SetChecked( instance.p_optimizeFast );
   }

   GUI->CenterX_NumericEdit.SetValue( m_rotationCenter.x );
   GUI->CenterY_NumericEdit.SetValue( m_rotationCenter.y );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateScaleControls()
{
   double wPx = m_width*instance.p_scaleX;
   double hPx = m_height*instance.p_scaleY;
   double wCm, hCm, wIn, hIn;
   if ( instance.p_metric )
   {
      wCm = wPx/instance.p_resolution.x;
      hCm = hPx/instance.p_resolution.y;
      wIn = wCm/2.54;
      hIn = hCm/2.54;
   }
   else
   {
      wIn = wPx/instance.p_resolution.x;
      hIn = hPx/instance.p_resolution.y;
      wCm = wIn*2.54;
      hCm = hIn*2.54;
   }

   GUI->ScaleX_NumericEdit.SetValue( instance.p_scaleX );
   GUI->ScaleY_NumericEdit.SetValue( instance.p_scaleY );
   GUI->ScaledWidthPx_NumericEdit.SetValue( RoundInt( wPx ) );
   GUI->ScaledHeightPx_NumericEdit.SetValue( RoundInt( hPx ) );
   GUI->ScaledWidthCm_NumericEdit.SetValue( wCm );
   GUI->ScaledHeightCm_NumericEdit.SetValue( hCm );
   GUI->ScaledWidthIn_NumericEdit.SetValue( wIn );
   GUI->ScaledHeightIn_NumericEdit.SetValue( hIn );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateInterpolationControls()
{
   GUI->Algorithm_ComboBox.SetCurrentItem( instance.p_interpolation );

   GUI->ClampingThreshold_NumericEdit.SetValue( instance.p_clampingThreshold );
   GUI->ClampingThreshold_NumericEdit.Enable( InterpolationAlgorithm::IsClampedInterpolation( instance.p_interpolation ) );

   GUI->Smoothness_NumericEdit.SetValue( instance.p_smoothness );
   GUI->Smoothness_NumericEdit.Enable( InterpolationAlgorithm::IsCubicFilterInterpolation( instance.p_interpolation ) );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateResolutionControls()
{
   GUI->HorizontalResolution_NumericEdit.SetValue( instance.p_resolution.x );
   GUI->VerticalResolution_NumericEdit.SetValue( instance.p_resolution.y );

   GUI->CentimeterUnits_RadioButton.SetChecked( instance.p_metric );
   GUI->InchUnits_RadioButton.SetChecked( !instance.p_metric );

   GUI->ForceResolution_CheckBox.SetChecked( instance.p_forceResolution );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateFillColorControls()
{
   GUI->Red_NumericControl.SetValue( instance.p_fillColor[0] );
   GUI->Green_NumericControl.SetValue( instance.p_fillColor[1] );
   GUI->Blue_NumericControl.SetValue( instance.p_fillColor[2] );
   GUI->Alpha_NumericControl.SetValue( instance.p_fillColor[3] );
   GUI->ColorSample_Control.Update();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::UpdateView()
{
   ImageWindow window = m_view.Window();

   DPoint tl, tr, bl, br;
   GetRotatedRect( tl, tr, bl, br );
   double d2 = window.ViewportScalarToImage( window.DisplayPixelRatio()/2 );
   window.UpdateImageRect( Min( Min( Min( tl.x, tr.x ), bl.x ), br.x )-d2,
                      Min( Min( Min( tl.y, tr.y ), bl.y ), br.y )-d2,
                      Max( Max( Max( tl.x, tr.x ), bl.x ), br.x )+d2,
                      Max( Max( Max( tl.y, tr.y ), bl.y ), br.y )+d2 );

   double x = m_center.x;
   double y = m_center.y;
   window.ImageToViewport( x, y );
   int cx = RoundInt( x );
   int cy = RoundInt( y );
   int dr = TruncInt( window.DisplayPixelRatio() * CENTER_RADIUS ) + 1;
   window.UpdateViewportRect( cx-dr, cy-dr, cx+dr, cy+dr );

   if ( m_rotationCenter != m_center )
   {
      x = m_rotationCenter.x;
      y = m_rotationCenter.y;
      window.ImageToViewport( x, y );
      cx = RoundInt( x );
      cy = RoundInt( y );
      window.UpdateViewportRect( cx-dr, cy-dr, cx+dr, cy+dr );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define ISCOLOR   (!m_view.IsNull() && m_view.IsColor())
#define ISGRAY    (!m_view.IsNull() && !m_view.IsColor())

// ----------------------------------------------------------------------------

void DynamicCropInterface::__Size_ValueUpdated( NumericEdit& sender, double value )
{
   if ( m_view.IsNull() )
      return;

   if ( sender == GUI->Width_NumericEdit )
   {
      if ( value != m_width )
      {
         double dw = value - m_width;
         double dL, dR;

         switch ( m_anchorPoint )
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
      if ( value != m_height )
      {
         double dh = value - m_height;
         double dT, dB;

         switch ( m_anchorPoint )
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

// ----------------------------------------------------------------------------

void DynamicCropInterface::__Pos_ValueUpdated( NumericEdit& sender, double value )
{
   if ( m_view.IsNull() )
      return;

   if ( sender == GUI->PosX_NumericEdit )
   {
      if ( value != m_anchor.x )
         MoveTo( m_center + DPoint( value - m_anchor.x, 0.0 ) );
   }
   else if ( sender == GUI->PosY_NumericEdit )
   {
      if ( value != m_anchor.y )
         MoveTo( m_center + DPoint( 0.0, value - m_anchor.y ) );
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__AnchorSelector_Paint( Control& sender, const Rect& updateRect )
{
   Rect r( sender.BoundsRect() );

   double x3 = r.Width()/3.0;
   double x6 = x3 + x3;
   double y3 = r.Height()/3.0;
   double y6 = y3 + y3;
   double f = sender.DisplayPixelRatio();

   VectorGraphics g( sender );
   if ( f > 1 )
      g.EnableAntialiasing();

   g.FillRect( r, RGBAColor( 0, 0, 0 ) );

   g.SetBrush( Brush::Null() );
   g.SetPen( 0xff7f7f7f, f );

   g.DrawLine( x3, 0, x3, r.y1 );
   g.DrawLine( x6, 0, x6, r.y1 );
   g.DrawLine( 0, y3, r.x1, y3 );
   g.DrawLine( 0, y6, r.x1, y6 );

   double x0, y0;
   if ( m_anchorPoint < 3 )
   {
      y0 = 0;
      x0 = (m_anchorPoint == 0) ? 0 : ((m_anchorPoint == 1) ? x3 : x6);
   }
   else if ( m_anchorPoint < 6 )
   {
      y0 = y3;
      x0 = (m_anchorPoint == 3) ? 0 : ((m_anchorPoint == 4) ? x3 : x6);
   }
   else
   {
      y0 = y6;
      x0 = (m_anchorPoint == 6) ? 0 : ((m_anchorPoint == 7) ? x3 : x6);
   }

   g.SetPen( 0xffffffff, f );
   double d3 = f*3;
   g.DrawLine( x0+d3, y0+d3, x0+x3-d3, y0+y3-d3 );
   g.DrawLine( x0+d3, y0+y3-d3, x0+x3-d3, y0+d3 );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__AnchorSelector_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( button != MouseButton::Left )
      return;

   Rect r( sender.BoundsRect() );
   int x3 = RoundInt( r.Width()/3.0 );
   int x6 = RoundInt( 2*r.Width()/3.0 );
   int y3 = RoundInt( r.Height()/3.0 );
   int y6 = RoundInt( 2*r.Height()/3.0 );
   int row = (pos.y < y3) ? 0 : ((pos.y < y6) ? 1 : 2);
   int col = (pos.x < x3) ? 0 : ((pos.x < x6) ? 1 : 2);

   m_anchorPoint = 3*row + col;

   UpdateAnchorPosition();
   UpdateSizePosControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__AnchorSelector_MouseRelease( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   m_dragging = false;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__AnchorSelector_MouseDoubleClick( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( m_view.IsNull() )
      return;

   if ( modifiers & KeyModifier::Shift )
   {
      // Shift+DoubleClick: Move cropping rectangle to anchor position.
      int w = m_view.Width();
      int h = m_view.Height();

      DPoint p = m_center;

      switch ( m_anchorPoint )
      {
      case 0 : // top left
         p = m_center - m_anchor;
         break;
      case 1 : // top middle
         p = m_center + DPoint( 0.5*w - m_anchor.x, -m_anchor.y );
         break;
      case 2 : // top right
         p = m_center + DPoint( w-m_anchor.x, -m_anchor.y );
         break;
      case 3 : // middle left
         p = m_center + DPoint( -m_anchor.x, 0.5*h - m_anchor.y );
         break;
      case 4 : // center
         p = DPoint( 0.5*w, 0.5*h );
         break;
      case 5 : // middle right
         p = m_center + DPoint( w - m_anchor.x, 0.5*h - m_anchor.y );
         break;
      case 6 : // bottom left
         p = m_center + DPoint( -m_anchor.x, h - m_anchor.y );
         break;
      case 7 : // bottom middle
         p = m_center + DPoint( 0.5*w - m_anchor.x, h - m_anchor.y );
         break;
      case 8 : // bottom right
         p = m_center + DPoint( w - m_anchor.x, h - m_anchor.y );
         break;
      }

      MoveTo( p );
   }
   else if ( modifiers & KeyModifier::Control )
   {
      // Ctrl+DoubleClick: Set center of rotation to anchor.
      SetRotationCenter( m_anchor );
   }
   else
   {
      // DoubleClick: Center view on anchor position coordinates.
      m_view.Window().BringToFront();
      m_view.Window().SelectMainView();
      m_view.Window().SetViewport( m_anchor );
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__Angle_ValueUpdated( NumericEdit& sender, double value )
{
   if ( m_view.IsNull() )
      return;

   double a = Rad( value );
   if ( GUI->Clockwise_CheckBox.IsChecked() )
      a = -a;

   SetRotationAngle( a );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__Clockwise_Click( Button& sender, bool checked )
{
   if ( m_view.IsNull() )
      return;

   if ( Round( Abs( Deg( instance.p_angle ) ), 3 ) < 180 )
      SetRotationAngle( -instance.p_angle );
   else
      GUI->Clockwise_CheckBox.Uncheck();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__Center_ValueUpdated( NumericEdit& sender, double value )
{
   if ( m_view.IsNull() )
      return;

   if ( sender == GUI->CenterX_NumericEdit )
      SetRotationCenter( DPoint( value, m_rotationCenter.y ) );
   else if ( sender == GUI->CenterY_NumericEdit )
      SetRotationCenter( DPoint( m_rotationCenter.x, value ) );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__OptimizeFast_Click( Button& sender, bool checked )
{
   instance.p_optimizeFast = checked;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__AngleDial_Paint( Control& sender, const Rect& updateRect )
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

// ----------------------------------------------------------------------------

void DynamicCropInterface::__AngleDial_MouseMove( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( m_dragging )
   {
      double a = Round( Deg( ArcTan( double( (sender.ClientHeight() >> 1) - pos.y ),
                                     double( pos.x - (sender.ClientWidth() >> 1) ) ) ), 3 );
      SetRotationAngle( Rad( a ) );
      //sender.Update();
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__AngleDial_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( button != MouseButton::Left )
      return;

   m_dragging = true;
   __AngleDial_MouseMove( sender, pos, buttons, modifiers );
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__AngleDial_MouseRelease( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   m_dragging = false;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__Scale_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ScaleX_NumericEdit )
   {
      instance.p_scaleX = value;
      if ( GUI->PreserveAspectRatio_CheckBox.IsChecked() )
         instance.p_scaleY = value;
   }
   else if ( sender == GUI->ScaleY_NumericEdit )
   {
      instance.p_scaleY = value;
      if ( GUI->PreserveAspectRatio_CheckBox.IsChecked() )
         instance.p_scaleX = value;
   }

   UpdateScaleControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__ScaledSize_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ScaledWidthPx_NumericEdit )
   {
      instance.p_scaleX = value/m_width;
      if ( GUI->PreserveAspectRatio_CheckBox.IsChecked() )
         instance.p_scaleY = instance.p_scaleX;
   }
   else if ( sender == GUI->ScaledHeightPx_NumericEdit )
   {
      instance.p_scaleY = value/m_height;
      if ( GUI->PreserveAspectRatio_CheckBox.IsChecked() )
         instance.p_scaleX = instance.p_scaleY;
   }
   else if ( sender == GUI->ScaledWidthCm_NumericEdit )
   {
      double wPx = value*instance.p_resolution.x;
      if ( !instance.p_metric )
         wPx /= 2.54;
      if ( wPx < 1 )
         wPx = 1;
      instance.p_scaleX = wPx/m_width;
      if ( GUI->PreserveAspectRatio_CheckBox.IsChecked() )
         instance.p_scaleY = instance.p_scaleX;
   }
   else if ( sender == GUI->ScaledHeightCm_NumericEdit )
   {
      double hPx = value*instance.p_resolution.y;
      if ( !instance.p_metric )
         hPx /= 2.54;
      if ( hPx < 1 )
         hPx = 1;
      instance.p_scaleY = hPx/m_height;
      if ( GUI->PreserveAspectRatio_CheckBox.IsChecked() )
         instance.p_scaleX = instance.p_scaleY;
   }
   else if ( sender == GUI->ScaledWidthIn_NumericEdit )
   {
      double wPx = value*instance.p_resolution.x;
      if ( instance.p_metric )
         wPx *= 2.54;
      if ( wPx < 1 )
         wPx = 1;
      instance.p_scaleX = wPx/m_width;
      if ( GUI->PreserveAspectRatio_CheckBox.IsChecked() )
         instance.p_scaleY = instance.p_scaleX;
   }
   else if ( sender == GUI->ScaledHeightIn_NumericEdit )
   {
      double hPx = value*instance.p_resolution.y;
      if ( instance.p_metric )
         hPx *= 2.54;
      if ( hPx < 1 )
         hPx = 1;
      instance.p_scaleY = hPx/m_height;
      if ( GUI->PreserveAspectRatio_CheckBox.IsChecked() )
         instance.p_scaleX = instance.p_scaleY;
   }

   UpdateScaleControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__Algorithm_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Algorithm_ComboBox )
   {
      instance.p_interpolation = itemIndex;
      UpdateInterpolationControls();
   }
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__Algorithm_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ClampingThreshold_NumericEdit )
      instance.p_clampingThreshold = value;
   else if ( sender == GUI->Smoothness_NumericEdit )
      instance.p_smoothness = value;
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__Resolution_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->HorizontalResolution_NumericEdit )
      instance.p_resolution.x = value;
   else if ( sender == GUI->VerticalResolution_NumericEdit )
      instance.p_resolution.y = value;
   UpdateScaleControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__Units_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->CentimeterUnits_RadioButton )
      instance.p_metric = true;
   else if ( sender == GUI->InchUnits_RadioButton )
      instance.p_metric = false;
   UpdateScaleControls();
}

// ----------------------------------------------------------------------------

void DynamicCropInterface::__ForceResolution_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->ForceResolution_CheckBox  )
      instance.p_forceResolution = checked;
}

// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------

void DynamicCropInterface::__ColorSample_Paint( Control& sender, const Rect& updateRect )
{
   Graphics g( sender );

   RGBA color;

   if ( m_view.IsNull() || m_view.IsColor() )
   {
      color = RGBAColor( float( instance.p_fillColor[0] ),
                         float( instance.p_fillColor[1] ),
                         float( instance.p_fillColor[2] ) );
   }
   else
   {
      RGBColorSystem rgb;
      m_view.Window().GetRGBWS( rgb );
      float L = rgb.Lightness( instance.p_fillColor[0],
                               instance.p_fillColor[1],
                               instance.p_fillColor[2] );
      color = RGBAColor( L, L, L );
   }

   SetAlpha( color, uint8( RoundInt( 255*instance.p_fillColor[3] ) ) );

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
// ----------------------------------------------------------------------------

DynamicCropInterface::GUIData::GUIData( DynamicCropInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Smoothness:" ) + 'T' );
   int labelWidth3 = fnt.Width( String( 'M',  2 ) );
   int editWidth   = fnt.Width( String( '0', 10 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   SizePos_SectionBar.SetTitle( "Size/Position" );
   SizePos_SectionBar.SetSection( SizePos_Control );

   Width_NumericEdit.SetReal();
   Width_NumericEdit.SetPrecision( 2 );
   Width_NumericEdit.EnableFixedPrecision();
   Width_NumericEdit.SetRange( 1, int_max );
   Width_NumericEdit.label.SetText( "Width:" );
   Width_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Width_NumericEdit.edit.SetFixedWidth( editWidth );
   Width_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Size_ValueUpdated, w );

   Height_NumericEdit.SetReal();
   Height_NumericEdit.SetPrecision( 2 );
   Height_NumericEdit.EnableFixedPrecision();
   Height_NumericEdit.SetRange( 1, int_max );
   Height_NumericEdit.label.SetText( "Height:" );
   Height_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Height_NumericEdit.edit.SetFixedWidth( editWidth );
   Height_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Size_ValueUpdated, w );

   PosX_NumericEdit.SetReal();
   PosX_NumericEdit.SetPrecision( 2 );
   PosX_NumericEdit.EnableFixedPrecision();
   PosX_NumericEdit.SetRange( int_min, int_max );
   PosX_NumericEdit.label.SetText( "Anchor X:" );
   PosX_NumericEdit.label.SetFixedWidth( labelWidth1 );
   PosX_NumericEdit.edit.SetFixedWidth( editWidth );
   PosX_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Pos_ValueUpdated, w );

   PosY_NumericEdit.SetReal();
   PosY_NumericEdit.SetPrecision( 2 );
   PosY_NumericEdit.EnableFixedPrecision();
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
   Angle_NumericEdit.EnableFixedPrecision();
   Angle_NumericEdit.SetRange( 0, 180 );
   Angle_NumericEdit.label.SetText( "Angle (\xb0):" );
   Angle_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Angle_NumericEdit.edit.SetFixedWidth( editWidth );
   Angle_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Angle_ValueUpdated, w );

   Clockwise_Label.SetText( "Clockwise:" );
   Clockwise_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Clockwise_Label.SetFixedWidth( labelWidth1 );

   Clockwise_CheckBox.OnClick( (Button::click_event_handler)&DynamicCropInterface::__Clockwise_Click, w );

   Clockwise_Sizer.Add( Clockwise_Label );
   Clockwise_Sizer.AddSpacing( 4 );
   Clockwise_Sizer.Add( Clockwise_CheckBox );
   Clockwise_Sizer.AddStretch();

   CenterX_NumericEdit.SetReal();
   CenterX_NumericEdit.SetPrecision( 2 );
   CenterX_NumericEdit.EnableFixedPrecision();
   CenterX_NumericEdit.SetRange( int_min, int_max );
   CenterX_NumericEdit.label.SetText( "Center X:" );
   CenterX_NumericEdit.label.SetFixedWidth( labelWidth1 );
   CenterX_NumericEdit.edit.SetFixedWidth( editWidth );
   CenterX_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Center_ValueUpdated, w );

   CenterY_NumericEdit.SetReal();
   CenterY_NumericEdit.SetPrecision( 2 );
   CenterY_NumericEdit.EnableFixedPrecision();
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

   RotationBottom_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   RotationBottom_Sizer.Add( OptimizeFast_CheckBox );
   RotationBottom_Sizer.AddStretch();

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
   ScaleX_NumericEdit.sizer.AddStretch();
   ScaleX_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Scale_ValueUpdated, w );

   ScaleY_NumericEdit.SetReal();
   ScaleY_NumericEdit.SetPrecision( 5 );
   ScaleY_NumericEdit.SetRange( 0.00001, 100 );
   ScaleY_NumericEdit.label.SetText( "Scale Y:" );
   ScaleY_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ScaleY_NumericEdit.edit.SetFixedWidth( editWidth );
   ScaleY_NumericEdit.sizer.AddStretch();
   ScaleY_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Scale_ValueUpdated, w );

   ScaledWidthPx_NumericEdit.SetInteger();
   ScaledWidthPx_NumericEdit.SetRange( 1, uint16_max );
   ScaledWidthPx_NumericEdit.label.SetText( "Width (px):" );
   ScaledWidthPx_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ScaledWidthPx_NumericEdit.edit.SetFixedWidth( editWidth );
   ScaledWidthPx_NumericEdit.sizer.AddStretch();
   ScaledWidthPx_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__ScaledSize_ValueUpdated, w );

   ScaledHeightPx_NumericEdit.SetInteger();
   ScaledHeightPx_NumericEdit.SetRange( 1, uint16_max );
   ScaledHeightPx_NumericEdit.label.SetText( "Height (px):" );
   ScaledHeightPx_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ScaledHeightPx_NumericEdit.edit.SetFixedWidth( editWidth );
   ScaledHeightPx_NumericEdit.sizer.AddStretch();
   ScaledHeightPx_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__ScaledSize_ValueUpdated, w );

   ScaledWidthCm_NumericEdit.SetReal();
   ScaledWidthCm_NumericEdit.SetPrecision( 4 );
   ScaledWidthCm_NumericEdit.SetRange( 0.0001, int_max );
   ScaledWidthCm_NumericEdit.label.SetText( "Width (cm):" );
   ScaledWidthCm_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ScaledWidthCm_NumericEdit.edit.SetFixedWidth( editWidth );
   ScaledWidthCm_NumericEdit.sizer.AddStretch();
   ScaledWidthCm_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__ScaledSize_ValueUpdated, w );

   ScaledHeightCm_NumericEdit.SetReal();
   ScaledHeightCm_NumericEdit.SetPrecision( 4 );
   ScaledHeightCm_NumericEdit.SetRange( 0.0001, int_max );
   ScaledHeightCm_NumericEdit.label.SetText( "Height (cm):" );
   ScaledHeightCm_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ScaledHeightCm_NumericEdit.edit.SetFixedWidth( editWidth );
   ScaledHeightCm_NumericEdit.sizer.AddStretch();
   ScaledHeightCm_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__ScaledSize_ValueUpdated, w );

   ScaledWidthIn_NumericEdit.SetReal();
   ScaledWidthIn_NumericEdit.SetPrecision( 3 );
   ScaledWidthIn_NumericEdit.SetRange( 0.001, int_max );
   ScaledWidthIn_NumericEdit.label.SetText( "Width (in):" );
   ScaledWidthIn_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ScaledWidthIn_NumericEdit.edit.SetFixedWidth( editWidth );
   ScaledWidthIn_NumericEdit.sizer.AddStretch();
   ScaledWidthIn_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__ScaledSize_ValueUpdated, w );

   ScaledHeightIn_NumericEdit.SetReal();
   ScaledHeightIn_NumericEdit.SetPrecision( 3 );
   ScaledHeightIn_NumericEdit.SetRange( 0.001, int_max );
   ScaledHeightIn_NumericEdit.label.SetText( "Height (in):" );
   ScaledHeightIn_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ScaledHeightIn_NumericEdit.edit.SetFixedWidth( editWidth );
   ScaledHeightIn_NumericEdit.sizer.AddStretch();
   ScaledHeightIn_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__ScaledSize_ValueUpdated, w );

   PreserveAspectRatio_CheckBox.SetText( "Preserve aspect ratio" );
   PreserveAspectRatio_CheckBox.Check(); // default = on

   PreserveAspectRatio_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PreserveAspectRatio_Sizer.Add( PreserveAspectRatio_CheckBox );
   PreserveAspectRatio_Sizer.AddStretch();

   Scale_Sizer.SetSpacing( 4 );
   Scale_Sizer.Add( ScaleX_NumericEdit );
   Scale_Sizer.Add( ScaleY_NumericEdit );
   Scale_Sizer.Add( ScaledWidthPx_NumericEdit );
   Scale_Sizer.Add( ScaledHeightPx_NumericEdit );
   Scale_Sizer.Add( ScaledWidthCm_NumericEdit );
   Scale_Sizer.Add( ScaledHeightCm_NumericEdit );
   Scale_Sizer.Add( ScaledWidthIn_NumericEdit );
   Scale_Sizer.Add( ScaledHeightIn_NumericEdit );
   Scale_Sizer.Add( PreserveAspectRatio_Sizer );

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
   ClampingThreshold_NumericEdit.SetPrecision( TheDCClampingThresholdParameter->Precision() );
   ClampingThreshold_NumericEdit.SetRange( TheDCClampingThresholdParameter->MinimumValue(),
                                           TheDCClampingThresholdParameter->MaximumValue() );
   ClampingThreshold_NumericEdit.label.SetText( "Clamping:" );
   ClampingThreshold_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ClampingThreshold_NumericEdit.edit.SetFixedWidth( editWidth );
   ClampingThreshold_NumericEdit.sizer.AddStretch();
   ClampingThreshold_NumericEdit.SetToolTip( "<p>Deringing clamping threshold for bicubic spline and Lanczos interpolation algorithms.</p>" );
   ClampingThreshold_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Algorithm_ValueUpdated, w );

   Smoothness_NumericEdit.SetReal();
   Smoothness_NumericEdit.SetPrecision( TheDCSmoothnessParameter->Precision() );
   Smoothness_NumericEdit.SetRange( TheDCSmoothnessParameter->MinimumValue(),
                                    TheDCSmoothnessParameter->MaximumValue() );
   Smoothness_NumericEdit.label.SetText( "Smoothness:" );
   Smoothness_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Smoothness_NumericEdit.edit.SetFixedWidth( editWidth );
   Smoothness_NumericEdit.sizer.AddStretch();
   Smoothness_NumericEdit.SetToolTip( "<p>Smoothness level for cubic filter interpolation algorithms.</p>" );
   Smoothness_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Algorithm_ValueUpdated, w );

   Interpolation_Sizer.SetSpacing( 4 );
   Interpolation_Sizer.Add( Algorithm_Sizer );
   Interpolation_Sizer.Add( ClampingThreshold_NumericEdit );
   Interpolation_Sizer.Add( Smoothness_NumericEdit );

   Interpolation_Control.SetSizer( Interpolation_Sizer );

   //

   Resolution_SectionBar.SetTitle( "Resolution" );
   Resolution_SectionBar.SetSection( Resolution_Control );

   HorizontalResolution_NumericEdit.SetReal();
   HorizontalResolution_NumericEdit.SetPrecision( 3 );
   HorizontalResolution_NumericEdit.SetRange( 1, 10000 );
   HorizontalResolution_NumericEdit.label.SetText( "Horizontal:" );
   HorizontalResolution_NumericEdit.label.SetFixedWidth( labelWidth1 );
   HorizontalResolution_NumericEdit.edit.SetFixedWidth( editWidth );
   HorizontalResolution_NumericEdit.sizer.AddStretch();
   HorizontalResolution_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Resolution_ValueUpdated, w );

   VerticalResolution_NumericEdit.SetReal();
   VerticalResolution_NumericEdit.SetPrecision( 3 );
   VerticalResolution_NumericEdit.SetRange( 1, 10000 );
   VerticalResolution_NumericEdit.label.SetText( "Vertical:" );
   VerticalResolution_NumericEdit.label.SetFixedWidth( labelWidth1 );
   VerticalResolution_NumericEdit.edit.SetFixedWidth( editWidth );
   VerticalResolution_NumericEdit.sizer.AddStretch();
   VerticalResolution_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicCropInterface::__Resolution_ValueUpdated, w );

   CentimeterUnits_RadioButton.SetText( "Centimeters" );
   CentimeterUnits_RadioButton.OnClick( (Button::click_event_handler)&DynamicCropInterface::__Units_ButtonClick, w );

   InchUnits_RadioButton.SetText( "Inches" );
   InchUnits_RadioButton.OnClick( (Button::click_event_handler)&DynamicCropInterface::__Units_ButtonClick, w );

   ResolutionUnit_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ResolutionUnit_Sizer.Add( CentimeterUnits_RadioButton );
   ResolutionUnit_Sizer.AddSpacing( 8 );
   ResolutionUnit_Sizer.Add( InchUnits_RadioButton );
   ResolutionUnit_Sizer.AddStretch();

   ForceResolution_CheckBox.SetText( "Force resolution" );
   ForceResolution_CheckBox.SetToolTip( "Modify resolution metadata of target image(s)" );
   ForceResolution_CheckBox.OnClick( (Button::click_event_handler)&DynamicCropInterface::__ForceResolution_ButtonClick, w );

   ForceResolution_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ForceResolution_Sizer.Add( ForceResolution_CheckBox );
   ForceResolution_Sizer.AddStretch();

   Resolution_Sizer.SetSpacing( 6 );
   Resolution_Sizer.Add( HorizontalResolution_NumericEdit );
   Resolution_Sizer.Add( VerticalResolution_NumericEdit );
   Resolution_Sizer.Add( ResolutionUnit_Sizer );
   Resolution_Sizer.Add( ForceResolution_Sizer );

   Resolution_Control.SetSizer( Resolution_Sizer );

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

   ColorSample_Control.SetScaledFixedHeight( 20 );
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
   Global_Sizer.Add( Resolution_SectionBar );
   Global_Sizer.Add( Resolution_Control );
   Global_Sizer.Add( FillColor_SectionBar );
   Global_Sizer.Add( FillColor_Control );

   w.SetSizer( Global_Sizer );

   w.AdjustToContents();
   w.SetFixedWidth();

   Scale_Control.Hide();
   Interpolation_Control.Hide();
   Resolution_Control.Hide();
   FillColor_Control.Hide();

   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicCropInterface.cpp - Released 2018-12-12T09:25:24Z
