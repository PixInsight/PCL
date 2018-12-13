//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/ViewList.h - Released 2018-12-12T09:24:21Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#ifndef __PCL_ViewList_h
#define __PCL_ViewList_h

/// \file pcl/ViewList.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Control.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ViewList
 * \brief Client-side interface to a PixInsight %ViewList object.
 *
 * ### TODO: Write a detailed description for %ViewList
 */
class PCL_CLASS ViewList : public Control
{
public:

   /*!
    * Constructs a %ViewList as a child control of \a parent.
    */
   ViewList( Control& parent = Control::Null() );

   /*!
    * Destroys a %ViewList control.
    */
   virtual ~ViewList()
   {
   }

   /*!
    * Returns true iff this %ViewList control includes main views (images).
    *
    * \sa IncludesPreviews(), Regenerate()
    */
   bool IncludesMainViews() const;

   /*!
    * Returns true iff this %ViewList control includes previews.
    *
    * \sa IncludesMainViews(), Regenerate()
    */
   bool IncludesPreviews() const;

   /*!
    * Returns true iff this %ViewList control includes the <em>virtual real-time
    * preview</em>.
    *
    * See the documentation for Regenerate() for more information about the
    * virtual real-time preview.
    *
    * \sa Regenerate()
    */
   bool IncludesRealTimePreview() const;

   /*!
    * Updates this %ViewList control, regenerating the list of items for
    * existing main views exclusively.
    *
    * This is a convenience member function, equivalent to:
    * Regenerate( true, false )
    *
    * Calling this member function implicitly disables the inclusion of
    * previews in the list of items of this %ViewList control.
    *
    * \sa GetPreviews(), GetAll(), Regenerate()
    */
   void GetMainViews()
   {
      Regenerate( true, false );
   }

   /*!
    * Updates this %ViewList control, regenerating the list of items for
    * existing previews exclusively.
    *
    * This is a convenience member function, equivalent to:
    * Regenerate( false, true )
    *
    * Calling this member function implicitly disables the inclusion of main
    * views in the list of items of this %ViewList control.
    *
    * \sa GetMainViews(), GetAll(), Regenerate()
    */
   void GetPreviews()
   {
      Regenerate( false, true );
   }

   /*!
    * Updates this %ViewList control, regenerating the list of items for
    * all existing views, including all main views and previews.
    *
    * This is a convenience member function, equivalent to:
    * Regenerate( true, true )
    *
    * Calling this member function implicitly enables the inclusion of main
    * views and previews in the list of items of this %ViewList control.
    *
    * \sa GetMainViews(), GetPreviews(), Regenerate()
    */
   void GetAll()
   {
      Regenerate( true, true );
   }

   /*!
    * Regenerates the list of view items in this %ViewList control.
    *
    * \param mainViews        Whether to enable inclusion of main views (images) - true by default.
    * \param previews         Whether to enable inclusion of previews - true by default.
    * \param realTimePreview  Whether to include the <em>virtual real-time preview</em> - false by default.
    *
    * The <em>virtual real-time preview</em> is a high-level interface to the
    * image currently represented in the Real-Time Preview window. This is a
    * special view that is not attached to any existing image window, and hence
    * has only limited functionality. For example, the standard
    * HistogramTransformation and Statistics tools enable access to this
    * virtual view to obtain the histogram and statistical data, respectively,
    * of the image being represented on the Real-Time Preview window.
    *
    * In general, a module must not gain access to the virtual real-time
    * preview unless it implements some interactive functionality that depends
    * on the current state of the Real-Time Preview window.
    *
    * \sa View
    */
   void Regenerate( bool mainViews = true, bool previews = true, bool realTimePreview = false );

   /*!
    * Returns the <em>excluded view</em> of this %ViewList control, or
    * View::Null() if this %ViewList excludes no view.
    *
    * The excluded view is never included in the list of items of a %ViewList
    * control, regardless of the current state of inclusion for main views and
    * previews. This feature is useful when there is a view that shouldn't be
    * referenced in a processing interface. For example, a dynamic interface
    * can easily avoid references to its own dynamic target by excluding it
    * from a %ViewList.
    *
    * By default, %ViewList controls exclude no views.
    *
    * \sa ExcludeView()
    */
   View ExcludedView() const;

   /*!
    * Sets the <em>excluded view</em> of this %ViewList control, or clears it
    * (to exclude no view) if View::Null() is specified.
    *
    * \sa ExcludedView()
    */
   void ExcludeView( const View& v );

   /*!
    * Returns the view that is currently selected in this %ViewList control, or
    * View::Null() if there is no view selected.
    *
    * \sa SelectView()
    */
   View CurrentView() const;

   /*!
    * Selects the specified \a view in this %ViewList control.
    *
    * If View::Null() is specified, the %ViewList control selects a special
    * item (usually, the first item) with the "No View Selected" text. This is
    * equivalent to a "no selection" state.
    *
    * If the specified view is not included in this %ViewList, calling this
    * member function has no effect.
    *
    * \sa CurrentView(), HasView()
    */
   void SelectView( const View& view );

   /*!
    * Returns true iff the specified \a view has been included in the list of
    * items of this %ViewList control.
    */
   bool HasView( const View& view );

   /*!
    * Removes the specified \a view from the list of existing items of this
    * %ViewList control.
    */
   void RemoveView( const View& view );

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnViewSelected( ViewList& sender, View& view );
   // void OnCurrentViewUpdated( ViewList& sender, View& view );

   /*!
    * \defgroup view_list_event_handlers ViewList Event Handlers
    */

   /*!
    * Defines the prototype of a <em>view event handler</em>.
    *
    * A view event is generated when the user activates a view item on a
    * %ViewList control, or when she changes the current view item in a
    * %ViewList control.
    *
    * \param sender  The control that sends a view event.
    *
    * \param view    The view whose associated item has been selected or
    *                activated.
    *
    * \ingroup view_list_event_handlers
    */
   typedef void (Control::*view_event_handler)( ViewList& sender, View& view );

   /*!
    * Sets the <em>view selection</em> event handler of this %ViewList control.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   receiver object's class.
    *
    * \param receiver   The control that will receive view selection events
    *                   from this %ViewList.
    *
    * \ingroup view_list_event_handlers
    */
   void OnViewSelected( view_event_handler handler, Control& receiver );

   /*!
    * Sets the <em>current view updated</em> event handler for this %ViewList
    * control.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   receiver object's class.
    *
    * \param receiver   The control that will receive <em>current view
    *                   updated</em> events from this %ViewList.
    *
    * \ingroup view_list_event_handlers
    */
   void OnCurrentViewUpdated( view_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      view_event_handler onViewSelected       = nullptr;
      view_event_handler onCurrentViewUpdated = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   friend class ViewListEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_ViewList_h

// ----------------------------------------------------------------------------
// EOF pcl/ViewList.h - Released 2018-12-12T09:24:21Z
