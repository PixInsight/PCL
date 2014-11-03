// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/ViewList.cpp - Released 2014/10/29 07:34:21 UTC
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

#include <pcl/ViewList.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<ViewList*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))

class ViewListEventDispatcher
{
public:

   static void api_func ViewSelected( control_handle hSender, control_handle hReceiver, view_handle hView )
   {
      if ( sender->onViewSelected != 0 )
      {
         View view( hView );
         (receiver->*sender->onViewSelected)( *sender, view );
      }
   }

   static void api_func CurrentViewUpdated( control_handle hSender, control_handle hReceiver, view_handle hView )
   {
      if ( sender->onCurrentViewUpdated != 0 )
      {
         View view( hView );
         (receiver->*sender->onCurrentViewUpdated)( *sender, view );
      }
   }

}; // ViewListEventDispatcher

#undef sender
#undef receiver

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#  pragma warning( disable: 4355 ) // 'this' : used in base member initializer list
#endif

ViewList::ViewList( Control& parent ) :
Control( (*API->ViewList->CreateViewList)( ModuleHandle(), this, parent.handle, 0 /*flags*/ ) ),
onViewSelected( 0 ),
onCurrentViewUpdated( 0 )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateViewList" );
}

// ----------------------------------------------------------------------------

void ViewList::OnViewSelected( view_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onViewSelected = 0;
   if ( (*API->ViewList->SetViewListViewSelectedEventRoutine)( handle, &receiver,
        (f != 0) ? ViewListEventDispatcher::ViewSelected : 0 ) == api_false )
   {
      throw APIFunctionError( "SetViewListViewSelectedEventRoutine" );
   }
   onViewSelected = f;
}

void ViewList::OnCurrentViewUpdated( view_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onCurrentViewUpdated = 0;
   if ( (*API->ViewList->SetViewListCurrentViewUpdatedEventRoutine)( handle, &receiver,
        (f != 0) ? ViewListEventDispatcher::CurrentViewUpdated : 0 ) == api_false )
   {
      throw APIFunctionError( "SetViewListCurrentViewUpdatedEventRoutine" );
   }
   onCurrentViewUpdated = f;
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

} // pcl

// ****************************************************************************
// EOF pcl/ViewList.cpp - Released 2014/10/29 07:34:21 UTC
