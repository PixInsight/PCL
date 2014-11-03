// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/Control.cpp - Released 2014/10/29 07:34:21 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/AutoLock.h>
#include <pcl/Control.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Control*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))

class ControlEventDispatcher
{
public:

   static api_bool api_func Destroy( control_handle hSender, control_handle hReceiver )
   {
      if ( sender->onDestroy != 0 )
      {
         (receiver->*sender->onDestroy)( *sender );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func Show( control_handle hSender, control_handle hReceiver )
   {
      if ( sender->onShow != 0 )
      {
         (receiver->*sender->onShow)( *sender );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func Hide( control_handle hSender, control_handle hReceiver )
   {
      if ( sender->onHide != 0 )
      {
         (receiver->*sender->onHide)( *sender );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func Close( control_handle hSender, control_handle hReceiver )
   {
      if ( sender->onClose != 0 )
      {
         bool allowClose = true;
         (receiver->*sender->onClose)( *sender, allowClose );
         return api_bool( allowClose );
      }

      return api_true;
   }

   static api_bool api_func GetFocus( control_handle hSender, control_handle hReceiver )
   {
      if ( sender->onGetFocus != 0 )
      {
         (receiver->*sender->onGetFocus)( *sender );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func LoseFocus( control_handle hSender, control_handle hReceiver )
   {
      if ( sender->onLoseFocus != 0 )
      {
         (receiver->*sender->onLoseFocus)( *sender );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func Enter( control_handle hSender, control_handle hReceiver )
   {
      if ( sender->onEnter != 0 )
      {
         (receiver->*sender->onEnter)( *sender );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func Leave( control_handle hSender, control_handle hReceiver )
   {
      if ( sender->onLeave != 0 )
      {
         (receiver->*sender->onLeave)( *sender );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func Move( control_handle hSender, control_handle hReceiver,
                                    int32 x, int32 y, int32 oldX, int32 oldY )
   {
      if ( sender->onMove != 0 )
      {
         pcl::Point pos( x, y );
         pcl::Point oldPos( oldX, oldY );
         (receiver->*sender->onMove)( *sender, pos, oldPos );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func Resize( control_handle hSender, control_handle hReceiver,
                                    int32 w, int32 h, int32 oldW, int32 oldH )
   {
      if ( sender->onResize != 0 )
      {
         (receiver->*sender->onResize)( *sender, w, h, oldW, oldH );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func Paint( control_handle hSender, control_handle hReceiver,
                                    int32 x0, int32 y0, int32 x1, int32 y1 )
   {
      if ( sender->onPaint != 0 )
      {
         pcl::Rect r( x0, y0, x1, y1 );
         (receiver->*sender->onPaint)( *sender, r );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func KeyPress( control_handle hSender, control_handle hReceiver,
                                    api_key_code key, api_key_modifiers modifiers )
   {
      if ( sender->onKeyPress != 0 )
      {
         bool wantsKey = false;
         (receiver->*sender->onKeyPress)( *sender, key, modifiers, wantsKey );
         return wantsKey;
      }

      return api_false;
   }

   static api_bool api_func KeyRelease( control_handle hSender, control_handle hReceiver,
                                    api_key_code key, api_key_modifiers modifiers )
   {
      if ( sender->onKeyRelease != 0 )
      {
         bool wantsKey = false;
         (receiver->*sender->onKeyRelease)( *sender, key, modifiers, wantsKey );
         return wantsKey;
      }

      return api_false;
   }

   static api_bool api_func MouseMove( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( sender->onMouseMove != 0 )
      {
         pcl::Point pos( x, y );
         (receiver->*sender->onMouseMove)( *sender, pos, buttons, modifiers );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func MouseDoubleClick( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( sender->onMouseDoubleClick != 0 )
      {
         pcl::Point pos( x, y );
         (receiver->*sender->onMouseDoubleClick)( *sender, pos, buttons, modifiers );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func MousePress( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, api_mouse_button button,
      api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( sender->onMousePress != 0 )
      {
         pcl::Point pos( x, y );
         (receiver->*sender->onMousePress)( *sender, pos, button, buttons, modifiers );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func MouseRelease( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, api_mouse_button button,
      api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( sender->onMouseRelease != 0 )
      {
         pcl::Point pos( x, y );
         (receiver->*sender->onMouseRelease)( *sender, pos, button, buttons, modifiers );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func MouseWheel( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, int32 delta,
      api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( sender->onMouseWheel != 0 )
      {
         pcl::Point pos( x, y );
         (receiver->*sender->onMouseWheel)( *sender, pos, delta, buttons, modifiers );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func ChildCreate( control_handle hSender, control_handle hReceiver, control_handle hChild )
   {
      if ( sender->onChildCreate != 0 )
      {
         (receiver->*sender->onChildCreate)( *sender, *reinterpret_cast<Control*>( hChild ) );
         return api_true;
      }

      return api_false;
   }

   static api_bool api_func ChildDestroy( control_handle hSender, control_handle hReceiver, control_handle hChild )
   {
      if ( sender->onChildDestroy != 0 )
      {
         (receiver->*sender->onChildDestroy)( *sender, *reinterpret_cast<Control*>( hChild ) );
         return api_true;
      }

      return api_false;
   }
}; // ControlEventDispatcher

#undef sender
#undef receiver

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#  pragma warning( disable: 4355 ) // 'this' : used in base member initializer list
#endif

Control::Control( Control& parent, uint32 flags ) :
   UIObject( (*API->Control->CreateControl)( ModuleHandle(), this, parent.handle, flags ) ),
   onDestroy( 0 ),
   onShow( 0 ),
   onHide( 0 ),
   onClose( 0 ),
   onGetFocus( 0 ),
   onLoseFocus( 0 ),
   onEnter( 0 ),
   onLeave( 0 ),
   onMove( 0 ),
   onResize( 0 ),
   onPaint( 0 ),
   onKeyPress( 0 ),
   onKeyRelease( 0 ),
   onMouseMove( 0 ),
   onMouseDoubleClick( 0 ),
   onMousePress( 0 ),
   onMouseRelease( 0 ),
   onMouseWheel( 0 ),
   onChildCreate( 0 ),
   onChildDestroy( 0 )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateControl" );
}

// ----------------------------------------------------------------------------

Control::Control( const Control& c ) :
   UIObject( c ),
   onDestroy( 0 ),
   onShow( 0 ),
   onHide( 0 ),
   onClose( 0 ),
   onGetFocus( 0 ),
   onLoseFocus( 0 ),
   onEnter( 0 ),
   onLeave( 0 ),
   onMove( 0 ),
   onResize( 0 ),
   onPaint( 0 ),
   onKeyPress( 0 ),
   onKeyRelease( 0 ),
   onMouseMove( 0 ),
   onMouseDoubleClick( 0 ),
   onMousePress( 0 ),
   onMouseRelease( 0 ),
   onMouseWheel( 0 ),
   onChildCreate( 0 ),
   onChildDestroy( 0 )
{
}

// ----------------------------------------------------------------------------

Control::Control( void* h ) :
   UIObject( h ),
   onDestroy( 0 ),
   onShow( 0 ),
   onHide( 0 ),
   onClose( 0 ),
   onGetFocus( 0 ),
   onLoseFocus( 0 ),
   onEnter( 0 ),
   onLeave( 0 ),
   onMove( 0 ),
   onResize( 0 ),
   onPaint( 0 ),
   onKeyPress( 0 ),
   onKeyRelease( 0 ),
   onMouseMove( 0 ),
   onMouseDoubleClick( 0 ),
   onMousePress( 0 ),
   onMouseRelease( 0 ),
   onMouseWheel( 0 ),
   onChildCreate( 0 ),
   onChildDestroy( 0 )
{
}

// ----------------------------------------------------------------------------

Control& Control::operator =( const Control& c )
{
   onDestroy = 0;
   onShow = 0;
   onHide = 0;
   onClose = 0;
   onGetFocus = 0;
   onLoseFocus = 0;
   onEnter = 0;
   onLeave = 0;
   onMove = 0;
   onResize = 0;
   onPaint = 0;
   onKeyPress = 0;
   onKeyRelease = 0;
   onMouseMove = 0;
   onMouseDoubleClick = 0;
   onMousePress = 0;
   onMouseRelease = 0;
   onMouseWheel = 0;
   onChildCreate = 0;
   onChildDestroy = 0;
   SetHandle( c.handle );
   return *this;
}

// ----------------------------------------------------------------------------

Control& Control::Null()
{
   static Control* nullControl = 0;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullControl == 0 )
      nullControl = new Control( reinterpret_cast<void*>( 0 ) );
   return *nullControl;
}

// ----------------------------------------------------------------------------

void Control::OnDestroy( event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onDestroy = 0;
   if ( (*API->Control->SetDestroyEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::Destroy : 0 ) == api_false )
   {
      throw APIFunctionError( "SetDestroyEventRoutine" );
   }
   onDestroy = f;
}

void Control::OnShow( event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onShow = 0;
   if ( (*API->Control->SetShowEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::Show : 0 ) == api_false )
   {
      throw APIFunctionError( "SetShowEventRoutine" );
   }
   onShow = f;
}

void Control::OnHide( event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onHide = 0;
   if ( (*API->Control->SetHideEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::Hide : 0 ) == api_false )
   {
      throw APIFunctionError( "SetHideEventRoutine" );
   }
   onHide = f;
}

void Control::OnClose( close_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onClose = 0;
   if ( (*API->Control->SetCloseEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::Close : 0 ) == api_false )
   {
      throw APIFunctionError( "SetCloseEventRoutine" );
   }
   onClose = f;
}

void Control::OnGetFocus( event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onGetFocus = 0;
   if ( (*API->Control->SetGetFocusEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::GetFocus : 0 ) == api_false )
   {
      throw APIFunctionError( "SetGetFocusEventRoutine" );
   }
   onGetFocus = f;
}

void Control::OnLoseFocus( event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onLoseFocus = 0;
   if ( (*API->Control->SetLoseFocusEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::LoseFocus : 0 ) == api_false )
   {
      throw APIFunctionError( "SetLoseFocusEventRoutine" );
   }
   onLoseFocus = f;
}

void Control::OnEnter( event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onEnter = 0;
   if ( (*API->Control->SetEnterEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::Enter : 0 ) == api_false )
   {
      throw APIFunctionError( "SetEnterEventRoutine" );
   }
   onEnter = f;
}

void Control::OnLeave( event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onLeave = 0;
   if ( (*API->Control->SetLeaveEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::Leave : 0 ) == api_false )
   {
      throw APIFunctionError( "SetLeaveEventRoutine" );
   }
   onLeave = f;
}

void Control::OnMove( move_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onMove = 0;
   if ( (*API->Control->SetMoveEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::Move : 0 ) == api_false )
   {
      throw APIFunctionError( "SetMoveEventRoutine" );
   }
   onMove = f;
}

void Control::OnResize( resize_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onResize = 0;
   if ( (*API->Control->SetResizeEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::Resize : 0 ) == api_false )
   {
      throw APIFunctionError( "SetResizeEventRoutine" );
   }
   onResize = f;
}

void Control::OnPaint( paint_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onPaint = 0;
   if ( (*API->Control->SetPaintEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::Paint : 0 ) == api_false )
   {
      throw APIFunctionError( "SetPaintEventRoutine" );
   }
   onPaint = f;
}

void Control::OnKeyPress( keyboard_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onKeyPress = 0;
   if ( (*API->Control->SetKeyPressEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::KeyPress : 0 ) == api_false )
   {
      throw APIFunctionError( "SetKeyPressEventRoutine" );
   }
   onKeyPress = f;
}

void Control::OnKeyRelease( keyboard_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onKeyRelease = 0;
   if ( (*API->Control->SetKeyReleaseEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::KeyRelease : 0 ) == api_false )
   {
      throw APIFunctionError( "SetKeyReleaseEventRoutine" );
   }
   onKeyRelease = f;
}

void Control::OnMouseMove( mouse_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onMouseMove = 0;
   if ( (*API->Control->SetMouseMoveEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::MouseMove : 0 ) == api_false )
   {
      throw APIFunctionError( "SetMouseMoveEventRoutine" );
   }
   onMouseMove = f;
}

void Control::OnMouseDoubleClick( mouse_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onMouseDoubleClick = 0;
   if ( (*API->Control->SetMouseDoubleClickEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::MouseDoubleClick : 0 ) == api_false )
   {
      throw APIFunctionError( "SetMouseDoubleClickEventRoutine" );
   }
   onMouseDoubleClick = f;
}

void Control::OnMousePress( mouse_button_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onMousePress = 0;
   if ( (*API->Control->SetMousePressEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::MousePress : 0 ) == api_false )
   {
      throw APIFunctionError( "SetMousePressEventRoutine" );
   }
   onMousePress = f;
}

void Control::OnMouseRelease( mouse_button_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onMouseRelease = 0;
   if ( (*API->Control->SetMouseReleaseEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::MouseRelease : 0 ) == api_false )
   {
      throw APIFunctionError( "SetMouseReleaseEventRoutine" );
   }
   onMouseRelease = f;
}

void Control::OnMouseWheel( mouse_wheel_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onMouseWheel = 0;
   if ( (*API->Control->SetWheelEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::MouseWheel : 0 ) == api_false )
   {
      throw APIFunctionError( "SetWheelEventRoutine" );
   }
   onMouseWheel = f;
}

void Control::OnChildCreate( child_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onChildCreate = 0;
   if ( (*API->Control->SetChildCreateEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::ChildCreate : 0 ) == api_false )
   {
      throw APIFunctionError( "SetChildCreateEventRoutine" );
   }
   onChildCreate = f;
}

void Control::OnChildDestroy( child_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onChildDestroy = 0;
   if ( (*API->Control->SetChildDestroyEventRoutine)( handle, &receiver,
            (f != 0) ? ControlEventDispatcher::ChildDestroy : 0 ) == api_false )
   {
      throw APIFunctionError( "SetChildDestroyEventRoutine" );
   }
   onChildDestroy = f;
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
   return (w != 0) ? *w : Null();
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
   return (w != 0) ? *w : Null();
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
   return (w != 0) ? *w : Null();
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
   return (w != 0) ? *w : Null();
}

// ----------------------------------------------------------------------------

Control& Control::ChildToFocus() const
{
   Control* w = reinterpret_cast<Control*>( (*API->Control->GetChildControlToFocus)( handle ) );
   return (w != 0) ? *w : Null();
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
   return (w != 0) ? *w : Null();
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
   if ( len != 0 )
   {
      css.Reserve( len );
      if ( (*API->Control->GetControlStyleSheet)( handle, css.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetControlStyleSheet" );
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
   double op = 0.0;
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

   if ( len != 0 )
   {
      title.Reserve( len );

      if ( (*API->Control->GetWindowTitle)( handle, title.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetWindowTitle" );
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

   if ( len != 0 )
   {
      text.Reserve( len );

      if ( (*API->Control->GetInfoText)( handle, text.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetInfoText" );
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

   if ( len != 0 )
   {
      tip.Reserve( len );

      if ( (*API->Control->GetWindowToolTip)( handle, tip.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetWindowToolTip" );
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

   if ( len != 0 )
   {
      tip.Reserve( len );

      if ( (*API->Global->GetToolTipWindowText)( tip.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetToolTipWindowText" );
   }

   return tip;
}

// ----------------------------------------------------------------------------

void* Control::CloneHandle() const
{
   throw Error( "Cannot clone a Control handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/Control.cpp - Released 2014/10/29 07:34:21 UTC
