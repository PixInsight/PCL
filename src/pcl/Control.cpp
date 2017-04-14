//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/Control.cpp - Released 2017-04-14T23:04:51Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/AutoLock.h>
#include <pcl/Control.h>
#include <pcl/View.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Control::Control( Control& parent, uint32 flags ) :
   UIObject( (*API->Control->CreateControl)( ModuleHandle(), this, parent.handle, flags ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateControl" );
}

// ----------------------------------------------------------------------------

Control& Control::Null()
{
   static Control* nullControl = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullControl == nullptr )
      nullControl = new Control( nullptr );
   return *nullControl;
}

// ----------------------------------------------------------------------------

pcl::Rect Control::FrameRect() const
{
   pcl::Rect r;
   (*API->Control->GetFrameRect)( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

pcl::Rect Control::ClientRect() const
{
   pcl::Rect r;
   (*API->Control->GetClientRect)( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

void Control::SetClientRect( int x0, int y0, int x1, int y1 )
{
   (*API->Control->SetClientRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void Control::Resize( int w, int h )
{
   (*API->Control->SetControlSize)( handle, w, h );
}

// ----------------------------------------------------------------------------

void Control::AdjustToContents()
{
   (*API->Control->AdjustControlToContents)( handle );
}

// ----------------------------------------------------------------------------

void Control::GetMinSize( int& w, int& h ) const
{
   (*API->Control->GetControlMinSize)( handle, &w, &h );
}

// ----------------------------------------------------------------------------

void Control::SetMinSize( int w, int h )
{
   (*API->Control->SetControlMinSize)( handle, w, h );
}

// ----------------------------------------------------------------------------

void Control::GetMaxSize( int& w, int& h ) const
{
   (*API->Control->GetControlMaxSize)( handle, &w, &h );
}

// ----------------------------------------------------------------------------

void Control::SetMaxSize( int w, int h )
{
   (*API->Control->SetControlMaxSize)( handle, w, h );
}

// ----------------------------------------------------------------------------

void Control::SetFixedSize( int w, int h )
{
   (*API->Control->SetControlFixedSize)( handle, w, h );
}

// ----------------------------------------------------------------------------

bool Control::IsHorizontalExpansionEnabled() const
{
   api_bool h;
   (*API->Control->GetControlExpansionEnabled)( handle, &h, 0 );
   return h != api_false;
}

// ----------------------------------------------------------------------------

bool Control::IsVerticalExpansionEnabled() const
{
   api_bool v;
   (*API->Control->GetControlExpansionEnabled)( handle, 0, &v );
   return v != api_false;
}

// ----------------------------------------------------------------------------

void Control::EnableExpansion( bool horzEnable, bool vertEnable )
{
   (*API->Control->SetControlExpansionEnabled)( handle, horzEnable, vertEnable );
}

// ----------------------------------------------------------------------------

pcl::Point Control::Position() const
{
   pcl::Point p;
   (*API->Control->GetControlPosition)( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

void Control::Move( int x, int y )
{
   (*API->Control->SetControlPosition)( handle, x, y );
}

// ----------------------------------------------------------------------------

bool Control::IsUnderMouse() const
{
   return (*API->Control->GetControlUnderMouseStatus)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::BringToFront()
{
   (*API->Control->BringControlToFront)( handle );
}

// ----------------------------------------------------------------------------

void Control::SendToBack()
{
   (*API->Control->SendControlToBack)( handle );
}

// ----------------------------------------------------------------------------

void Control::StackUnder( Control& w )
{
   (*API->Control->StackControls)( handle, w.handle );
}

// ----------------------------------------------------------------------------

pcl::Sizer Control::Sizer() const
{
   return pcl::Sizer( (*API->Control->GetControlSizer)( handle ) );
}

// ----------------------------------------------------------------------------

void Control::SetSizer( pcl::Sizer& s )
{
   (*API->Control->SetControlSizer)( handle, s.handle );
}

// ----------------------------------------------------------------------------

void Control::GlobalToLocal( int& x, int& y ) const
{
   (*API->Control->GlobalToLocal)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void Control::LocalToGlobal( int& x, int& y ) const
{
   (*API->Control->LocalToGlobal)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void Control::ParentToLocal( int& x, int& y ) const
{
   (*API->Control->ParentToLocal)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void Control::LocalToParent( int& x, int& y ) const
{
   (*API->Control->LocalToParent)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void Control::ControlToLocal( const Control& w, int& x, int& y ) const
{
   (*API->Control->ControlToLocal)( handle, w.handle, &x, &y );
}

// ----------------------------------------------------------------------------

void Control::LocalToControl( const Control& w, int& x, int& y ) const
{
   (*API->Control->LocalToControl)( handle, w.handle, &x, &y );
}

// ----------------------------------------------------------------------------

Control& Control::ChildByPos( int x, int y ) const
{
   Control* w = reinterpret_cast<Control*>( (*API->Control->GetChildByPos)( handle, x, y ) );
   return (w != nullptr) ? *w : Null();
}

// ----------------------------------------------------------------------------

pcl::Rect Control::ChildrenRect() const
{
   pcl::Rect r;
   (*API->Control->GetChildrenRect)( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

bool Control::IsAncestorOf( const Control& w ) const
{
   return (*API->Control->GetControlAncestry)( handle, w.handle ) != api_false;
}

// ----------------------------------------------------------------------------

Control& Control::Parent() const
{
   Control* w = reinterpret_cast<Control*>( (*API->Control->GetControlParent)( handle ) );
   return (w != nullptr) ? *w : Null();
}

// ----------------------------------------------------------------------------

void Control::SetParent( Control& w )
{
   (*API->Control->SetControlParent)( handle, w.handle );
}

// ----------------------------------------------------------------------------

Control& Control::Window() const
{
   Control* w = reinterpret_cast<Control*>( (*API->Control->GetControlWindow)( handle ) );
   return (w != nullptr) ? *w : Null();
}

// ----------------------------------------------------------------------------

bool Control::IsEnabled() const
{
   return (*API->Control->GetControlEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::Enable( bool enabled )
{
   (*API->Control->SetControlEnabled)( handle, enabled );
}

// ----------------------------------------------------------------------------

bool Control::IsMouseTrackingEnabled() const
{
   return (*API->Control->GetControlMouseTrackingEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::EnableMouseTracking( bool enable )
{
   (*API->Control->SetControlMouseTrackingEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

bool Control::IsVisible() const
{
   return (*API->Control->GetControlVisible)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::Show()
{
   (*API->Control->SetControlVisible)( handle, api_true );
}

// ----------------------------------------------------------------------------

pcl::Rect Control::VisibleRect() const
{
   pcl::Rect r;
   (*API->Control->GetControlVisibleRect)( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

void Control::Hide()
{
   (*API->Control->SetControlVisible)( handle, api_false );
}

// ----------------------------------------------------------------------------

bool Control::IsMaximized() const
{
   api_bool m;
   (void)(*API->Control->GetWindowState)( handle, 0, 0, &m, 0 );
   return m != api_false;
}

// ----------------------------------------------------------------------------

bool Control::IsMinimized() const
{
   api_bool m;
   (void)(*API->Control->GetWindowState)( handle, 0, 0, 0, &m );
   return m != api_false;
}

// ----------------------------------------------------------------------------

bool Control::IsModal() const
{
   api_bool m;
   (void)(*API->Control->GetWindowState)( handle, 0, &m, 0, 0 );
   return m != api_false;
}

// ----------------------------------------------------------------------------

bool Control::IsWindow() const
{
   return (*API->Control->GetWindowState)( handle, 0, 0, 0, 0 ) != api_false;
}

// ----------------------------------------------------------------------------

bool Control::IsActiveWindow() const
{
   api_bool a;
   (void)(*API->Control->GetWindowState)( handle, &a, 0, 0, 0 );
   return a != api_false;
}

// ----------------------------------------------------------------------------

void Control::ActivateWindow()
{
   (*API->Control->ActivateWindow)( handle );
}

// ----------------------------------------------------------------------------

bool Control::IsFocused() const
{
   return (*API->Control->GetControlFocus)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::Focus( bool focus )
{
   (*API->Control->SetControlFocus)( handle, focus );
}

// ----------------------------------------------------------------------------

FocusStyles Control::FocusStyle() const
{
   return FocusStyle::mask_type( (*API->Control->GetControlFocusStyle)( handle ) );
}

// ----------------------------------------------------------------------------

void Control::SetFocusStyle( FocusStyles style )
{
   (*API->Control->SetControlFocusStyle)( handle, style );
}

// ----------------------------------------------------------------------------

Control& Control::FocusedChild() const
{
   Control* w = reinterpret_cast<Control*>( (*API->Control->GetFocusChildControl)( handle ) );
   return (w != nullptr) ? *w : Null();
}

// ----------------------------------------------------------------------------

Control& Control::ChildToFocus() const
{
   Control* w = reinterpret_cast<Control*>( (*API->Control->GetChildControlToFocus)( handle ) );
   return (w != nullptr) ? *w : Null();
}

// ----------------------------------------------------------------------------

void Control::SetChildToFocus( Control& w )
{
   (*API->Control->SetChildControlToFocus)( handle, w.handle );
}

// ----------------------------------------------------------------------------

Control& Control::NextSiblingToFocus() const
{
   Control* w = reinterpret_cast<Control*>( (*API->Control->GetNextSiblingControlToFocus)( handle ) );
   return (w != nullptr) ? *w : Null();
}

// ----------------------------------------------------------------------------

void Control::SetNextSiblingToFocus( Control& w )
{
   (*API->Control->SetNextSiblingControlToFocus)( handle, w.handle );
}

// ----------------------------------------------------------------------------

bool Control::CanUpdate() const
{
   return (*API->Control->GetControlUpdatesEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::EnableUpdates( bool enable )
{
   (*API->Control->SetControlUpdatesEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

void Control::Update()
{
   (*API->Control->UpdateControl)( handle );
}

// ----------------------------------------------------------------------------

void Control::Update( int x0, int y0, int x1, int y1 )
{
   (*API->Control->UpdateControlRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void Control::Repaint()
{
   (*API->Control->RepaintControl)( handle );
}

// ----------------------------------------------------------------------------

void Control::Repaint( int x0, int y0, int x1, int y1 )
{
   (*API->Control->RepaintControlRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void Control::Restyle()
{
   (*API->Control->RestyleControl)( handle );
}

// ----------------------------------------------------------------------------

void Control::Scroll( int dx, int dy )
{
   (*API->Control->ScrollControl)( handle, dx, dy );
}

// ----------------------------------------------------------------------------

void Control::Scroll( int dx, int dy, int x0, int y0, int x1, int y1 )
{
   (*API->Control->ScrollControlRect)( handle, dx, dy, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

pcl::Cursor Control::Cursor() const
{
   return pcl::Cursor( (*API->Control->GetControlCursor)( handle ) );
}

// ----------------------------------------------------------------------------

void Control::SetCursor( const pcl::Cursor& c )
{
   (*API->Control->SetControlCursor)( handle, c.handle );
}
// ----------------------------------------------------------------------------

void Control::SetCursorToParent()
{
   (*API->Control->SetControlCursorToParent)( handle );
}

// ----------------------------------------------------------------------------

String Control::StyleSheet() const
{
   size_type len = 0;
   (*API->Control->GetControlStyleSheet)( handle, 0, &len );

   String css;
   if ( len > 0 )
   {
      css.SetLength( len );
      if ( (*API->Control->GetControlStyleSheet)( handle, css.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetControlStyleSheet" );
      css.ResizeToNullTerminated();
   }
   return css;
}

// ----------------------------------------------------------------------------

void Control::SetStyleSheet( const String& css )
{
   (*API->Control->SetControlStyleSheet)( handle, css.c_str() );
}

// ----------------------------------------------------------------------------

RGBA Control::BackgroundColor() const
{
   return (*API->Control->GetControlBackgroundColor)( handle );
}

// ----------------------------------------------------------------------------

void Control::SetBackgroundColor( RGBA color )
{
   (*API->Control->SetControlBackgroundColor)( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::ForegroundColor() const
{
   return (*API->Control->GetControlForegroundColor)( handle );
}

// ----------------------------------------------------------------------------

void Control::SetForegroundColor( RGBA color )
{
   (*API->Control->SetControlForegroundColor)( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::CanvasColor()
{
   return (*API->Control->GetControlCanvasColor)( handle );
}

// ----------------------------------------------------------------------------

void Control::SetCanvasColor( RGBA color )
{
   (*API->Control->SetControlCanvasColor)( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::AlternateCanvasColor() const
{
   return (*API->Control->GetControlAlternateCanvasColor)( handle );
}

// ----------------------------------------------------------------------------

void Control::SetAlternateCanvasColor( RGBA color )
{
   (*API->Control->SetControlAlternateCanvasColor)( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::TextColor() const
{
   return (*API->Control->GetControlTextColor)( handle );
}

// ----------------------------------------------------------------------------

void Control::SetTextColor( RGBA color )
{
   (*API->Control->SetControlTextColor)( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::ButtonColor() const
{
   return (*API->Control->GetControlButtonColor)( handle );
}

// ----------------------------------------------------------------------------

void Control::SetButtonColor( RGBA color )
{
   (*API->Control->SetControlButtonColor)( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::ButtonTextColor() const
{
   return (*API->Control->GetControlTextColor)( handle );
}

// ----------------------------------------------------------------------------

void Control::SetButtonTextColor( RGBA color )
{
   (*API->Control->SetControlTextColor)( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::HighlightColor() const
{
   return (*API->Control->GetControlHighlightColor)( handle );
}

// ----------------------------------------------------------------------------

void Control::SetHighlightColor( RGBA color )
{
   (*API->Control->SetControlHighlightColor)( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::HighlightedTextColor() const
{
   return (*API->Control->GetControlHighlightedTextColor)( handle );
}

// ----------------------------------------------------------------------------

void Control::SetHighlightedTextColor( RGBA color )
{
   (*API->Control->SetControlHighlightedTextColor)( handle, color );
}

// ----------------------------------------------------------------------------

pcl::Font Control::Font() const
{
   return pcl::Font( (*API->Control->GetControlFont)( handle ) );
}

// ----------------------------------------------------------------------------

void Control::SetFont( const pcl::Font& f )
{
   (*API->Control->SetControlFont)( handle, f.handle );
}

// ----------------------------------------------------------------------------

double Control::WindowOpacity() const
{
   double op = 1.0;
   (*API->Control->GetWindowOpacity)( handle, &op );
   return op;
}

// ----------------------------------------------------------------------------

void Control::SetWindowOpacity( double op )
{
   (*API->Control->SetWindowOpacity)( handle, op );
}

// ----------------------------------------------------------------------------

String Control::WindowTitle() const
{
   size_type len = 0;
   (*API->Control->GetWindowTitle)( handle, 0, &len );

   String title;
   if ( len > 0 )
   {
      title.SetLength( len );
      if ( (*API->Control->GetWindowTitle)( handle, title.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetWindowTitle" );
      title.ResizeToNullTerminated();
   }
   return title;
}

// ----------------------------------------------------------------------------

void Control::SetWindowTitle( const String& title )
{
   (*API->Control->SetWindowTitle)( handle, title.c_str() );
}

// ----------------------------------------------------------------------------

String Control::InfoText() const
{
   size_type len = 0;
   (*API->Control->GetInfoText)( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->Control->GetInfoText)( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetInfoText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void Control::SetInfoText( const String& text )
{
   (*API->Control->SetInfoText)( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

bool Control::IsTrackViewActive() const
{
   return (*API->Control->GetTrackViewActive)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::SetTrackViewActive( bool active )
{
   (*API->Control->SetTrackViewActive)( handle, api_bool( active ) );
}

// ----------------------------------------------------------------------------

bool Control::IsRealTimePreviewActive() const
{
   return (*API->Control->GetRealTimePreviewActive)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::SetRealTimePreviewActive( bool active )
{
   (*API->Control->SetRealTimePreviewActive)( handle, api_bool( active ) );
}

// ----------------------------------------------------------------------------

String Control::ToolTip() const
{
   size_type len = 0;
   (*API->Control->GetWindowToolTip)( handle, 0, &len );

   String tip;
   if ( len > 0 )
   {
      tip.SetLength( len );
      if ( (*API->Control->GetWindowToolTip)( handle, tip.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetWindowToolTip" );
      tip.ResizeToNullTerminated();
   }
   return tip;
}

// ----------------------------------------------------------------------------

void Control::SetToolTip( const String& tip )
{
   (*API->Control->SetWindowToolTip)( handle, tip.c_str() );
}

// ----------------------------------------------------------------------------

void Control::ShowToolTip( int x, int y, const String& text, const Control& control, const Rect& rect )
{
   (*API->Global->ShowToolTipWindow)( x, y, text.c_str(), control.handle, rect.x0, rect.y0, rect.x1, rect.y1 );
}

// ----------------------------------------------------------------------------

void Control::HideToolTip()
{
   (*API->Global->HideToolTipWindow)();
}

// ----------------------------------------------------------------------------

String Control::ToolTipText()
{
   size_type len = 0;
   (*API->Global->GetToolTipWindowText)( 0, &len );

   String tip;
   if ( len > 0 )
   {
      tip.SetLength( len );
      if ( (*API->Global->GetToolTipWindowText)( tip.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetToolTipWindowText" );
      tip.ResizeToNullTerminated();
   }
   return tip;
}

// ----------------------------------------------------------------------------

double Control::DisplayPixelRatio() const
{
   double r;
   if ( (*API->Control->GetControlDisplayPixelRatio)( handle, &r ) == api_false )
      throw APIFunctionError( "GetControlDisplayPixelRatio" );
   return r;
}

// ----------------------------------------------------------------------------

double Control::ResourcePixelRatio() const
{
   double r;
   if ( (*API->Control->GetControlResourcePixelRatio)( handle, &r ) == api_false )
      throw APIFunctionError( "GetControlResourcePixelRatio" );
   return r;
}

// ----------------------------------------------------------------------------

void* Control::CloneHandle() const
{
   throw Error( "Cannot clone a Control handle" );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Control*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class ControlEventDispatcher
{
public:

   static api_bool api_func Destroy( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onDestroy != nullptr )
      {
         (receiver->*handlers->onDestroy)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Show( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onShow != nullptr )
      {
         (receiver->*handlers->onShow)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Hide( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onHide != nullptr )
      {
         (receiver->*handlers->onHide)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Close( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onClose != nullptr )
      {
         bool allowClose = true;
         (receiver->*handlers->onClose)( *sender, allowClose );
         return api_bool( allowClose );
      }
      return api_true;
   }

   static api_bool api_func GetFocus( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onGetFocus != nullptr )
      {
         (receiver->*handlers->onGetFocus)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func LoseFocus( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onLoseFocus != nullptr )
      {
         (receiver->*handlers->onLoseFocus)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Enter( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onEnter != nullptr )
      {
         (receiver->*handlers->onEnter)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Leave( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onLeave != nullptr )
      {
         (receiver->*handlers->onLeave)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Move( control_handle hSender, control_handle hReceiver,
                                    int32 x, int32 y, int32 oldX, int32 oldY )
   {
      if ( handlers->onMove != nullptr )
      {
         pcl::Point pos( x, y );
         pcl::Point oldPos( oldX, oldY );
         (receiver->*handlers->onMove)( *sender, pos, oldPos );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Resize( control_handle hSender, control_handle hReceiver,
                                    int32 w, int32 h, int32 oldW, int32 oldH )
   {
      if ( handlers->onResize != nullptr )
      {
         (receiver->*handlers->onResize)( *sender, w, h, oldW, oldH );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Paint( control_handle hSender, control_handle hReceiver,
                                    int32 x0, int32 y0, int32 x1, int32 y1 )
   {
      if ( handlers->onPaint != nullptr )
      {
         pcl::Rect r( x0, y0, x1, y1 );
         (receiver->*handlers->onPaint)( *sender, r );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func KeyPress( control_handle hSender, control_handle hReceiver,
                                    api_key_code key, api_key_modifiers modifiers )
   {
      if ( handlers->onKeyPress != nullptr )
      {
         bool wantsKey = false;
         (receiver->*handlers->onKeyPress)( *sender, key, modifiers, wantsKey );
         return wantsKey;
      }
      return api_false;
   }

   static api_bool api_func KeyRelease( control_handle hSender, control_handle hReceiver,
                                    api_key_code key, api_key_modifiers modifiers )
   {
      if ( handlers->onKeyRelease != nullptr )
      {
         bool wantsKey = false;
         (receiver->*handlers->onKeyRelease)( *sender, key, modifiers, wantsKey );
         return wantsKey;
      }
      return api_false;
   }

   static api_bool api_func MouseMove( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( handlers->onMouseMove != nullptr )
      {
         pcl::Point pos( x, y );
         (receiver->*handlers->onMouseMove)( *sender, pos, buttons, modifiers );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func MouseDoubleClick( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( handlers->onMouseDoubleClick != nullptr )
      {
         pcl::Point pos( x, y );
         (receiver->*handlers->onMouseDoubleClick)( *sender, pos, buttons, modifiers );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func MousePress( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, api_mouse_button button,
      api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( handlers->onMousePress != nullptr )
      {
         pcl::Point pos( x, y );
         (receiver->*handlers->onMousePress)( *sender, pos, button, buttons, modifiers );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func MouseRelease( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, api_mouse_button button,
      api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( handlers->onMouseRelease != nullptr )
      {
         pcl::Point pos( x, y );
         (receiver->*handlers->onMouseRelease)( *sender, pos, button, buttons, modifiers );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func MouseWheel( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, int32 delta,
      api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( handlers->onMouseWheel != nullptr )
      {
         pcl::Point pos( x, y );
         (receiver->*handlers->onMouseWheel)( *sender, pos, delta, buttons, modifiers );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func ChildCreate( control_handle hSender, control_handle hReceiver, control_handle hChild )
   {
      if ( handlers->onChildCreate != nullptr )
      {
         (receiver->*handlers->onChildCreate)( *sender, *reinterpret_cast<Control*>( hChild ) );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func ChildDestroy( control_handle hSender, control_handle hReceiver, control_handle hChild )
   {
      if ( handlers->onChildDestroy != nullptr )
      {
         (receiver->*handlers->onChildDestroy)( *sender, *reinterpret_cast<Control*>( hChild ) );
         return api_true;
      }
      return api_false;
   }

   static StringList MakeFileList( const char16_type* files, const size_type* lengths, size_type count )
   {
      StringList fileList;
      for ( String::const_iterator i = files; count > 0; --count, ++lengths )
      {
         String::const_iterator j = i + *lengths;
         fileList << String( i, j );
         i = j;
      }
      return fileList;
   }

   static api_bool api_func FileDrag( control_handle hSender, control_handle hReceiver,
                                      int32 x, int32 y,
                                      const char16_type* files, const size_type* lengths, size_type count,
                                      api_key_modifiers modifiers )
   {
      if ( handlers->onFileDrag != nullptr )
      {
         StringList fileList = MakeFileList( files, lengths, count );
         bool wantsFiles = false;
         (receiver->*handlers->onFileDrag)( *sender, pcl::Point( x, y ), fileList, modifiers, wantsFiles );
         return wantsFiles;
      }
      return api_false;
   }

   static api_bool api_func FileDrop( control_handle hSender, control_handle hReceiver,
                                      int32 x, int32 y,
                                      const char16_type* files, const size_type* lengths, size_type count,
                                      api_key_modifiers modifiers )
   {
      if ( handlers->onFileDrop != nullptr )
      {
         StringList fileList = MakeFileList( files, lengths, count );
         (receiver->*handlers->onFileDrop)( *sender, pcl::Point( x, y ), fileList, modifiers );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func ViewDrag( control_handle hSender, control_handle hReceiver,
                                      int32 x, int32 y,
                                      const_view_handle hView,
                                      api_key_modifiers modifiers )
   {
      if ( handlers->onViewDrag != nullptr )
      {
         View view( hView );
         bool wantsView = false;
         (receiver->*handlers->onViewDrag)( *sender, pcl::Point( x, y ), view, modifiers, wantsView );
         return wantsView;
      }
      return api_false;
   }

   static api_bool api_func ViewDrop( control_handle hSender, control_handle hReceiver,
                                      int32 x, int32 y,
                                      const_view_handle hView,
                                      api_key_modifiers modifiers )
   {
      if ( handlers->onViewDrop != nullptr )
      {
         View view( hView );
         (receiver->*handlers->onViewDrop)( *sender, pcl::Point( x, y ), view, modifiers );
         return api_true;
      }
      return api_false;
   }
}; // ControlEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new Control::EventHandlers

void Control::OnDestroy( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetDestroyEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Destroy : nullptr ) == api_false )
      throw APIFunctionError( "SetDestroyEventRoutine" );
   m_handlers->onDestroy = f;
}

void Control::OnShow( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetShowEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Show : nullptr ) == api_false )
      throw APIFunctionError( "SetShowEventRoutine" );
   m_handlers->onShow = f;
}

void Control::OnHide( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetHideEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Hide : nullptr ) == api_false )
      throw APIFunctionError( "SetHideEventRoutine" );
   m_handlers->onHide = f;
}

void Control::OnClose( close_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetCloseEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Close : nullptr ) == api_false )
      throw APIFunctionError( "SetCloseEventRoutine" );
   m_handlers->onClose = f;
}

void Control::OnGetFocus( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetGetFocusEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::GetFocus : nullptr ) == api_false )
      throw APIFunctionError( "SetGetFocusEventRoutine" );
   m_handlers->onGetFocus = f;
}

void Control::OnLoseFocus( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetLoseFocusEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::LoseFocus : nullptr ) == api_false )
      throw APIFunctionError( "SetLoseFocusEventRoutine" );
   m_handlers->onLoseFocus = f;
}

void Control::OnEnter( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetEnterEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Enter : nullptr ) == api_false )
      throw APIFunctionError( "SetEnterEventRoutine" );
   m_handlers->onEnter = f;
}

void Control::OnLeave( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetLeaveEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Leave : nullptr ) == api_false )
      throw APIFunctionError( "SetLeaveEventRoutine" );
   m_handlers->onLeave = f;
}

void Control::OnMove( move_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetMoveEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Move : nullptr ) == api_false )
      throw APIFunctionError( "SetMoveEventRoutine" );
   m_handlers->onMove = f;
}

void Control::OnResize( resize_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetResizeEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Resize : nullptr ) == api_false )
      throw APIFunctionError( "SetResizeEventRoutine" );
   m_handlers->onResize = f;
}

void Control::OnPaint( paint_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetPaintEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Paint : nullptr ) == api_false )
      throw APIFunctionError( "SetPaintEventRoutine" );
   m_handlers->onPaint = f;
}

void Control::OnKeyPress( keyboard_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetKeyPressEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::KeyPress : nullptr ) == api_false )
      throw APIFunctionError( "SetKeyPressEventRoutine" );
   m_handlers->onKeyPress = f;
}

void Control::OnKeyRelease( keyboard_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetKeyReleaseEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::KeyRelease : nullptr ) == api_false )
      throw APIFunctionError( "SetKeyReleaseEventRoutine" );
   m_handlers->onKeyRelease = f;
}

void Control::OnMouseMove( mouse_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetMouseMoveEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::MouseMove : nullptr ) == api_false )
      throw APIFunctionError( "SetMouseMoveEventRoutine" );
   m_handlers->onMouseMove = f;
}

void Control::OnMouseDoubleClick( mouse_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetMouseDoubleClickEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::MouseDoubleClick : nullptr ) == api_false )
      throw APIFunctionError( "SetMouseDoubleClickEventRoutine" );
   m_handlers->onMouseDoubleClick = f;
}

void Control::OnMousePress( mouse_button_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetMousePressEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::MousePress : nullptr ) == api_false )
      throw APIFunctionError( "SetMousePressEventRoutine" );
   m_handlers->onMousePress = f;
}

void Control::OnMouseRelease( mouse_button_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetMouseReleaseEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::MouseRelease : nullptr ) == api_false )
      throw APIFunctionError( "SetMouseReleaseEventRoutine" );
   m_handlers->onMouseRelease = f;
}

void Control::OnMouseWheel( mouse_wheel_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetWheelEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::MouseWheel : nullptr ) == api_false )
      throw APIFunctionError( "SetWheelEventRoutine" );
   m_handlers->onMouseWheel = f;
}

void Control::OnChildCreate( child_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetChildCreateEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::ChildCreate : nullptr ) == api_false )
      throw APIFunctionError( "SetChildCreateEventRoutine" );
   m_handlers->onChildCreate = f;
}

void Control::OnChildDestroy( child_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetChildDestroyEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::ChildDestroy : nullptr ) == api_false )
      throw APIFunctionError( "SetChildDestroyEventRoutine" );
   m_handlers->onChildDestroy = f;
}

void Control::OnFileDrag( file_drag_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetFileDragEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::FileDrag : nullptr ) == api_false )
      throw APIFunctionError( "SetFileDragEventRoutine" );
   m_handlers->onFileDrag = f;
}

void Control::OnFileDrop( file_drop_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetFileDropEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::FileDrop : nullptr ) == api_false )
      throw APIFunctionError( "SetFileDropEventRoutine" );
   m_handlers->onFileDrop = f;
}

void Control::OnViewDrag( view_drag_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetViewDragEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::ViewDrag : nullptr ) == api_false )
      throw APIFunctionError( "SetViewDragEventRoutine" );
   m_handlers->onViewDrag = f;
}

void Control::OnViewDrop( view_drop_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Control->SetViewDropEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::ViewDrop : nullptr ) == api_false )
      throw APIFunctionError( "SetViewDropEventRoutine" );
   m_handlers->onViewDrop = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Control.cpp - Released 2017-04-14T23:04:51Z
