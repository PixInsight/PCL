//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/ViewList.cpp - Released 2015/07/30 17:15:31 UTC
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

#include <pcl/ViewList.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#  pragma warning( disable: 4355 ) // 'this' : used in base member initializer list
#endif

ViewList::ViewList( Control& parent ) :
   Control( (*API->ViewList->CreateViewList)( ModuleHandle(), this, parent.handle, 0/*flags*/ ) ),
   m_handlers( nullptr )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateViewList" );
}

// ----------------------------------------------------------------------------

bool ViewList::IncludesMainViews() const
{
   api_bool mainViews;
   (*API->ViewList->GetViewListContents)( handle, &mainViews, 0, 0 );
   return mainViews != api_false;
}

// ----------------------------------------------------------------------------

bool ViewList::IncludesPreviews() const
{
   api_bool previews;
   (*API->ViewList->GetViewListContents)( handle, 0, &previews, 0 );
   return previews != api_false;
}

// ----------------------------------------------------------------------------

bool ViewList::IncludesRealTimePreview() const
{
   api_bool realTimePreview;
   (*API->ViewList->GetViewListContents)( handle, 0, 0, &realTimePreview );
   return realTimePreview != api_false;
}

// ----------------------------------------------------------------------------

void ViewList::Regenerate( bool mainViews, bool previews, bool realTimePreview )
{
   (*API->ViewList->RegenerateViewList)( handle, mainViews, previews, realTimePreview );
}

// ----------------------------------------------------------------------------

View ViewList::ExcludedView() const
{
   return View( (*API->ViewList->GetViewListExcludedView)( handle ) );
}

// ----------------------------------------------------------------------------

void ViewList::ExcludeView( const View& v )
{
   (*API->ViewList->SetViewListExcludedView)( handle, v.handle );
}

// ----------------------------------------------------------------------------

View ViewList::CurrentView() const
{
   return View( (*API->ViewList->GetViewListCurrentView)( handle ) );
}

// ----------------------------------------------------------------------------

void ViewList::SelectView( const View& v )
{
   (*API->ViewList->SetViewListCurrentView)( handle, v.handle );
}

// ----------------------------------------------------------------------------

bool ViewList::HasView( const View& v )
{
   return (*API->ViewList->FindViewListView)( handle, v.handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ViewList::RemoveView( const View& v )
{
   (*API->ViewList->RemoveViewListView)( handle, v.handle );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<ViewList*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class ViewListEventDispatcher
{
public:

   static void api_func ViewSelected( control_handle hSender, control_handle hReceiver, view_handle hView )
   {
      if ( handlers->onViewSelected != nullptr )
      {
         View view( hView );
         (receiver->*handlers->onViewSelected)( *sender, view );
      }
   }

   static void api_func CurrentViewUpdated( control_handle hSender, control_handle hReceiver, view_handle hView )
   {
      if ( handlers->onCurrentViewUpdated != nullptr )
      {
         View view( hView );
         (receiver->*handlers->onCurrentViewUpdated)( *sender, view );
      }
   }

}; // ViewListEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers == nullptr )  \
      m_handlers = new EventHandlers

void ViewList::OnViewSelected( view_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ViewList->SetViewListViewSelectedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ViewListEventDispatcher::ViewSelected : 0 ) == api_false )
      throw APIFunctionError( "SetViewListViewSelectedEventRoutine" );
   m_handlers->onViewSelected = f;
}

void ViewList::OnCurrentViewUpdated( view_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ViewList->SetViewListCurrentViewUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ViewListEventDispatcher::CurrentViewUpdated : 0 ) == api_false )
      throw APIFunctionError( "SetViewListCurrentViewUpdatedEventRoutine" );
   m_handlers->onCurrentViewUpdated = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ViewList.cpp - Released 2015/07/30 17:15:31 UTC
