// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/ScrollBox.cpp - Released 2014/10/29 07:34:21 UTC
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

#include <pcl/ScrollBox.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<ScrollBox*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))

class ScrollBoxEventDispatcher
{
public:

   static void HorizontalScrollPosUpdated( control_handle hSender, control_handle hReceiver, int32 horzPos )
   {
      if ( sender->onHorizontalScrollPosUpdated != 0 )
         (receiver->*sender->onHorizontalScrollPosUpdated)( *sender, horzPos );
   }

   static void VerticalScrollPosUpdated( control_handle hSender, control_handle hReceiver, int32 vertPos )
   {
      if ( sender->onVerticalScrollPosUpdated != 0 )
         (receiver->*sender->onVerticalScrollPosUpdated)( *sender, vertPos );
   }

   static void HorizontalScrollRangeUpdated( control_handle hSender, control_handle hReceiver, int32 minHorzPos, int32 maxHorzPos )
   {
      if ( sender->onHorizontalScrollRangeUpdated != 0 )
         (receiver->*sender->onHorizontalScrollRangeUpdated)( *sender, minHorzPos, maxHorzPos );
   }

   static void VerticalScrollRangeUpdated( control_handle hSender, control_handle hReceiver, int32 minVertPos, int32 maxVertPos )
   {
      if ( sender->onVerticalScrollRangeUpdated != 0 )
         (receiver->*sender->onVerticalScrollRangeUpdated)( *sender, minVertPos, maxVertPos );
   }
}; // ScrollBoxEventDispatcher

#undef sender
#undef receiver

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#  pragma warning( disable: 4355 ) // 'this' : used in base member initializer list
#endif

ScrollBox::ScrollBox( Control& parent ) :
Frame( (*API->ScrollBox->CreateScrollBox)( ModuleHandle(), this, parent.handle, 0 /*flags*/ ) ),
onHorizontalScrollPosUpdated( 0 ),
onVerticalScrollPosUpdated( 0 ),
onHorizontalScrollRangeUpdated( 0 ),
onVerticalScrollRangeUpdated( 0 ),
viewport( 0 )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateScrollBox" );

   viewport.TransferHandle( (*API->ScrollBox->CreateScrollBoxViewport)( handle, &viewport ) );
   if ( viewport.IsNull() )
      throw APIFunctionError( "CreateScrollBoxViewport" );
}

ScrollBox::ScrollBox( void* h ) : Frame( h ),
onHorizontalScrollPosUpdated( 0 ),
onVerticalScrollPosUpdated( 0 ),
onHorizontalScrollRangeUpdated( 0 ),
onVerticalScrollRangeUpdated( 0 ),
viewport( 0 )
{
   if ( !IsNull() )
   {
      viewport.TransferHandle( (*API->ScrollBox->CreateScrollBoxViewport)( handle, &viewport ) );
      if ( viewport.IsNull() )
         throw APIFunctionError( "CreateScrollBoxViewport" );
   }
}

ScrollBox::ScrollBox( void* h, void* hV ) : Frame( h ),
onHorizontalScrollPosUpdated( 0 ),
onVerticalScrollPosUpdated( 0 ),
onHorizontalScrollRangeUpdated( 0 ),
onVerticalScrollRangeUpdated( 0 ),
viewport( hV )
{
}

// ----------------------------------------------------------------------------

void ScrollBox::OnHorizontalScrollPosUpdated( pos_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onHorizontalScrollPosUpdated = 0;
   if ( (*API->ScrollBox->SetScrollBoxHorizontalPosUpdatedEventRoutine)( handle, &receiver,
        (f != 0) ? ScrollBoxEventDispatcher::HorizontalScrollPosUpdated : 0 ) == api_false )
   {
      throw APIFunctionError( "SetScrollBoxHorizontalPosUpdatedEventRoutine" );
   }
   onHorizontalScrollPosUpdated = f;
}

void ScrollBox::OnVerticalScrollPosUpdated( pos_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onVerticalScrollPosUpdated = 0;
   if ( (*API->ScrollBox->SetScrollBoxVerticalPosUpdatedEventRoutine)( handle, &receiver,
      (f != 0) ? ScrollBoxEventDispatcher::VerticalScrollPosUpdated : 0 ) == api_false )
   {
      throw APIFunctionError( "SetScrollBoxVerticalPosUpdatedEventRoutine" );
   }
   onVerticalScrollPosUpdated = f;
}

void ScrollBox::OnHorizontalScrollRangeUpdated( range_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onHorizontalScrollRangeUpdated = 0;
   if ( (*API->ScrollBox->SetScrollBoxHorizontalRangeUpdatedEventRoutine)( handle, &receiver,
      (f != 0) ? ScrollBoxEventDispatcher::HorizontalScrollRangeUpdated : 0 ) == api_false )
   {
      throw APIFunctionError( "SetScrollBoxHorizontalRangeUpdatedEventRoutine" );
   }
   onHorizontalScrollRangeUpdated = f;
}

void ScrollBox::OnVerticalScrollRangeUpdated( range_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onVerticalScrollRangeUpdated = 0;
   if ( (*API->ScrollBox->SetScrollBoxVerticalRangeUpdatedEventRoutine)( handle, &receiver,
      (f != 0) ? ScrollBoxEventDispatcher::VerticalScrollRangeUpdated : 0 ) == api_false )
   {
      throw APIFunctionError( "SetScrollBoxVerticalRangeUpdatedEventRoutine" );
   }
   onVerticalScrollRangeUpdated = f;
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

} // pcl

// ****************************************************************************
// EOF pcl/ScrollBox.cpp - Released 2014/10/29 07:34:21 UTC
