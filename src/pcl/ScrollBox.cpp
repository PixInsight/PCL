//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/ScrollBox.cpp - Released 2017-06-28T11:58:42Z
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

#include <pcl/ScrollBox.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ScrollBox::ScrollBox( Control& parent ) :
   Frame( (*API->ScrollBox->CreateScrollBox)( ModuleHandle(), this, parent.handle, 0/*flags*/ ) ),
   m_viewport( nullptr )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateScrollBox" );

   m_viewport.TransferHandle( (*API->ScrollBox->CreateScrollBoxViewport)( handle, &m_viewport ) );
   if ( m_viewport.IsNull() )
      throw APIFunctionError( "CreateScrollBoxViewport" );
}

ScrollBox::ScrollBox( void* h ) :
   Frame( h ),
   m_viewport( nullptr )
{
   if ( !IsNull() )
   {
      m_viewport.TransferHandle( (*API->ScrollBox->CreateScrollBoxViewport)( handle, &m_viewport ) );
      if ( m_viewport.IsNull() )
         throw APIFunctionError( "CreateScrollBoxViewport" );
   }
}

ScrollBox::ScrollBox( void* h, void* hV ) :
   Frame( h ),
   m_viewport( hV )
{
}

// ----------------------------------------------------------------------------

bool ScrollBox::IsHorizontalScrollBarVisible() const
{
   api_bool h, v;
   (*API->ScrollBox->GetScrollBarsVisible)( handle, &h, &v );
   return h != api_false;
}

// ----------------------------------------------------------------------------

bool ScrollBox::IsVerticalScrollBarVisible() const
{
   api_bool h, v;
   (*API->ScrollBox->GetScrollBarsVisible)( handle, &h, &v );
   return v != api_false;
}

// ----------------------------------------------------------------------------

void ScrollBox::ShowScrollBars( bool showHorz, bool showVert )
{
   (*API->ScrollBox->SetScrollBarsVisible)( handle, showHorz, showVert );
}

// ----------------------------------------------------------------------------

bool ScrollBox::IsHorizontalAutoScrollEnabled() const
{
   api_bool h, v;
   (*API->ScrollBox->GetScrollBoxAutoScrollEnabled)( handle, &h, &v );
   return h != api_false;
}

// ----------------------------------------------------------------------------

bool ScrollBox::IsVerticalAutoScrollEnabled() const
{
   api_bool h, v;
   (*API->ScrollBox->GetScrollBoxAutoScrollEnabled)( handle, &h, &v );
   return v != api_false;
}

// ----------------------------------------------------------------------------

void ScrollBox::EnableAutoScroll( bool enableHorz, bool enableVert )
{
   (*API->ScrollBox->SetScrollBoxAutoScrollEnabled)( handle, enableHorz, enableVert );
}

// ----------------------------------------------------------------------------

void ScrollBox::GetHorizontalScrollRange( int& minPos, int& maxPos ) const
{
   (*API->ScrollBox->GetScrollBoxHorizontalRange)( handle, &minPos, &maxPos );
}

// ----------------------------------------------------------------------------

void ScrollBox::GetVerticalScrollRange( int& minPos, int& maxPos ) const
{
   (*API->ScrollBox->GetScrollBoxVerticalRange)( handle, &minPos, &maxPos );
}

// ----------------------------------------------------------------------------

void ScrollBox::SetHorizontalScrollRange( int minPos, int maxPos )
{
   (*API->ScrollBox->SetScrollBoxHorizontalRange)( handle, minPos, maxPos );
}

// ----------------------------------------------------------------------------

void ScrollBox::SetVerticalScrollRange( int minPos, int maxPos )
{
   (*API->ScrollBox->SetScrollBoxVerticalRange)( handle, minPos, maxPos );
}

// ----------------------------------------------------------------------------

int ScrollBox::PageWidth() const
{
   int32 h, v;
   (*API->ScrollBox->GetScrollBoxPageSize)( handle, &h, &v );
   return h;
}

// ----------------------------------------------------------------------------

int ScrollBox::PageHeight() const
{
   int32 h, v;
   (*API->ScrollBox->GetScrollBoxPageSize)( handle, &h, &v );
   return v;
}

// ----------------------------------------------------------------------------

void ScrollBox::SetPageSize( int width, int height )
{
   (*API->ScrollBox->SetScrollBoxPageSize)( handle, width, height );
}

// ----------------------------------------------------------------------------

int ScrollBox::LineWidth() const
{
   int32 h, v;
   (*API->ScrollBox->GetScrollBoxLineSize)( handle, &h, &v );
   return h;
}

// ----------------------------------------------------------------------------

int ScrollBox::LineHeight() const
{
   int32 h, v;
   (*API->ScrollBox->GetScrollBoxLineSize)( handle, &h, &v );
   return v;
}

// ----------------------------------------------------------------------------

void ScrollBox::SetLineSize( int width, int height )
{
   (*API->ScrollBox->SetScrollBoxLineSize)( handle, width, height );
}

// ----------------------------------------------------------------------------

pcl::Point ScrollBox::ScrollPosition() const
{
   pcl::Point p;
   (*API->ScrollBox->GetScrollBoxPosition)( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

void ScrollBox::SetScrollPosition( int horzPos, int vertPos )
{
   (*API->ScrollBox->SetScrollBoxPosition)( handle, horzPos, vertPos );
}

// ----------------------------------------------------------------------------

bool ScrollBox::IsHorizontalTrackingEnabled() const
{
   api_bool h, v;
   (*API->ScrollBox->GetScrollBoxTrackingEnabled)( handle, &h, &v );
   return h != api_false;
}

// ----------------------------------------------------------------------------

bool ScrollBox::IsVerticalTrackingEnabled() const
{
   api_bool h, v;
   (*API->ScrollBox->GetScrollBoxTrackingEnabled)( handle, &h, &v );
   return v != api_false;
}

// ----------------------------------------------------------------------------

void ScrollBox::EnableTracking( bool enableHorz, bool enableVert )
{
   (*API->ScrollBox->SetScrollBoxTrackingEnabled)( handle, enableHorz, enableVert );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<ScrollBox*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class ScrollBoxEventDispatcher
{
public:

   static void HorizontalScrollPosUpdated( control_handle hSender, control_handle hReceiver, int32 horzPos )
   {
      if ( handlers->onHorizontalScrollPosUpdated != nullptr )
         (receiver->*handlers->onHorizontalScrollPosUpdated)( *sender, horzPos );
   }

   static void VerticalScrollPosUpdated( control_handle hSender, control_handle hReceiver, int32 vertPos )
   {
      if ( handlers->onVerticalScrollPosUpdated != nullptr )
         (receiver->*handlers->onVerticalScrollPosUpdated)( *sender, vertPos );
   }

   static void HorizontalScrollRangeUpdated( control_handle hSender, control_handle hReceiver, int32 minHorzPos, int32 maxHorzPos )
   {
      if ( handlers->onHorizontalScrollRangeUpdated != nullptr )
         (receiver->*handlers->onHorizontalScrollRangeUpdated)( *sender, minHorzPos, maxHorzPos );
   }

   static void VerticalScrollRangeUpdated( control_handle hSender, control_handle hReceiver, int32 minVertPos, int32 maxVertPos )
   {
      if ( handlers->onVerticalScrollRangeUpdated != nullptr )
         (receiver->*handlers->onVerticalScrollRangeUpdated)( *sender, minVertPos, maxVertPos );
   }
}; // ScrollBoxEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void ScrollBox::OnHorizontalScrollPosUpdated( pos_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ScrollBox->SetScrollBoxHorizontalPosUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ScrollBoxEventDispatcher::HorizontalScrollPosUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetScrollBoxHorizontalPosUpdatedEventRoutine" );
   m_handlers->onHorizontalScrollPosUpdated = f;
}

void ScrollBox::OnVerticalScrollPosUpdated( pos_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ScrollBox->SetScrollBoxVerticalPosUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ScrollBoxEventDispatcher::VerticalScrollPosUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetScrollBoxVerticalPosUpdatedEventRoutine" );
   m_handlers->onVerticalScrollPosUpdated = f;
}

void ScrollBox::OnHorizontalScrollRangeUpdated( range_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ScrollBox->SetScrollBoxHorizontalRangeUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ScrollBoxEventDispatcher::HorizontalScrollRangeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetScrollBoxHorizontalRangeUpdatedEventRoutine" );
   m_handlers->onHorizontalScrollRangeUpdated = f;
}

void ScrollBox::OnVerticalScrollRangeUpdated( range_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ScrollBox->SetScrollBoxVerticalRangeUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ScrollBoxEventDispatcher::VerticalScrollRangeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetScrollBoxVerticalRangeUpdatedEventRoutine" );
   m_handlers->onVerticalScrollRangeUpdated = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ScrollBox.cpp - Released 2017-06-28T11:58:42Z
