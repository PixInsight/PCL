//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/Button.cpp - Released 2015/07/30 17:15:31 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/Button.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

String Button::Text() const
{
   size_type len = 0;
   (*API->Button->GetButtonText)( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->Button->GetButtonText)( handle, text.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetButtonText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void Button::SetText( const String& text )
{
   (*API->Button->SetButtonText)( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

Bitmap Button::Icon() const
{
   return Bitmap( (*API->Button->GetButtonIcon)( handle ) );
}

// ----------------------------------------------------------------------------

void Button::SetIcon( const Bitmap& icon )
{
   (*API->Button->SetButtonIcon)( handle, icon.handle );
}

// ----------------------------------------------------------------------------

void Button::GetIconSize( int& w, int& h ) const
{
   (*API->Button->GetButtonIconSize)( handle, &w, &h );
}

// ----------------------------------------------------------------------------

void Button::SetIconSize( int w, int h )
{
   (*API->Button->SetButtonIconSize)( handle, w, h );
}

// ----------------------------------------------------------------------------

bool Button::IsPushed() const
{
   return (*API->Button->GetButtonPushed)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Button::SetPushed( bool pushed )
{
   (*API->Button->SetButtonPushed)( handle, pushed );
}

// ----------------------------------------------------------------------------

bool Button::IsChecked() const
{
   return (*API->Button->GetButtonChecked)( handle ) == 1;
}

// ----------------------------------------------------------------------------

void Button::SetChecked( bool checked )
{
   (*API->Button->SetButtonChecked)( handle, checked ? 1 : 0 );
}

// ----------------------------------------------------------------------------

Button::check_state Button::State() const
{
   return check_state( (*API->Button->GetButtonChecked)( handle ) );
}

// ----------------------------------------------------------------------------

void Button::SetState( Button::check_state state )
{
   (*API->Button->SetButtonChecked)( handle, uint32( state ) );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Button*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class ButtonEventDispatcher
{
public:

   static void api_func Click( control_handle hSender, control_handle hReceiver, api_bool checked )
   {
      if ( handlers->onClick != nullptr )
         (receiver->*handlers->onClick)( *sender, checked != api_false );
   }

   static void api_func Press( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onPress != nullptr )
         (receiver->*handlers->onPress)( *sender );
   }

   static void api_func Release( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onRelease != nullptr )
         (receiver->*handlers->onRelease)( *sender );
   }

   static void api_func Check( control_handle hSender, control_handle hReceiver, int32 state )
   {
      if ( handlers->onCheck != nullptr )
         (receiver->*handlers->onCheck)( *sender, Button::check_state( state ) );
   }
}; // ButtonEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers == nullptr )  \
      m_handlers = new EventHandlers

void Button::OnClick( click_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Button->SetButtonClickEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ButtonEventDispatcher::Click : nullptr ) == api_false )
      throw APIFunctionError( "SetButtonClickEventRoutine" );
   m_handlers->onClick = f;
}

void Button::OnPress( press_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Button->SetButtonPressEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ButtonEventDispatcher::Press : nullptr ) == api_false )
      throw APIFunctionError( "SetButtonPressEventRoutine" );
   m_handlers->onPress = f;
}

void Button::OnRelease( press_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Button->SetButtonReleaseEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ButtonEventDispatcher::Release : nullptr ) == api_false )
      throw APIFunctionError( "SetButtonReleaseEventRoutine" );
   m_handlers->onRelease = f;
}

void Button::OnCheck( check_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Button->SetButtonCheckEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ButtonEventDispatcher::Check : nullptr ) == api_false )
      throw APIFunctionError( "SetButtonCheckEventRoutine" );
   m_handlers->onCheck = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Button.cpp - Released 2015/07/30 17:15:31 UTC
