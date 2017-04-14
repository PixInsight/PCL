//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/ScrollBox.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_ScrollBox_h
#define __PCL_ScrollBox_h

/// \file pcl/ScrollBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Frame.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ScrollBox
 * \brief Client-side interface to a PixInsight %ScrollBox control.
 *
 * ### TODO: Write a detailed description for %ScrollBox.
 */
class PCL_CLASS ScrollBox : public Frame
{
public:

   /*!
    * Constructs a %ScrollBox as a child control of \a parent.
    */
   ScrollBox( Control& parent = Control::Null() );

   /*!
    * Destroys a %ScrollBox control.
    */
   virtual ~ScrollBox()
   {
   }

   /*!
    * Returns true iff the horizontal scroll bar is currently visible on this
    * scroll box.
    */
   bool IsHorizontalScrollBarVisible() const;

   /*!
    * Returns true iff the vertical scroll bar is currently visible on this
    * scroll box.
    */
   bool IsVerticalScrollBarVisible() const;

   /*!
    * Forces visibility of scroll bars.
    *
    * \param showHorz   Visibility of the horizontal scroll bar.
    * \param showVert   Visibility of the vertical scroll bar.
    */
   void ShowScrollBars( bool showHorz = true, bool showVert = true );

   /*!
    * Forces visibility of scroll bars.
    *
    * This is a convenience member function, equivalent to
    * ShowScrollBars( !hideHorz, !hideVert )
    */
   void HideScrollBars( bool hideHorz = true, bool hideVert = true )
   {
      ShowScrollBars( !hideHorz, !hideVert );
   }

   /*!
    * Forces visibility of the horizontal scroll bar.
    *
    * This is a convenience member function, equivalent to
    * ShowScrollBars( show, IsVerticalScrollBarVisible() )
    */
   void ShowHorizontalScrollBar( bool show = true )
   {
      ShowScrollBars( show, IsVerticalScrollBarVisible() );
   }

   /*!
    * Forces visibility of the horizontal scroll bar.
    *
    * This is a convenience member function, equivalent to
    * ShowHorizontalScrollBar( !hide )
    */
   void HideHorizontalScrollBar( bool hide = true )
   {
      ShowHorizontalScrollBar( !hide );
   }

   /*!
    * Forces visibility of the vertical scroll bar.
    *
    * This is a convenience member function, equivalent to
    * ShowScrollBars( IsHorizontalScrollBarVisible(), show )
    */
   void ShowVerticalScrollBar( bool show = true )
   {
      ShowScrollBars( IsHorizontalScrollBarVisible(), show );
   }

   /*!
    * Forces visibility of the vertical scroll bar.
    *
    * This is a convenience member function, equivalent to
    * ShowVerticalScrollBar( !hide )
    */
   void HideVerticalScrollBar( bool hide = true )
   {
      ShowVerticalScrollBar( !hide );
   }

   /*!
    * Returns true iff the <em>automatic scroll mode</em> is enabled for the
    * horizontal scroll bar.
    *
    * In automatic scroll mode, scroll bars are automatically shown or hidden
    * as necessary. A scroll bar is shown only if the corresponding scrolling
    * range is larger than the visible length of the scrolling area.
    */
   bool IsHorizontalAutoScrollEnabled() const;

   /*!
    * Returns true iff the <em>automatic scroll mode</em> is enabled for the
    * vertical scroll bar.
    *
    * In automatic scroll mode, scroll bars are automatically shown or hidden
    * as necessary. A scroll bar is shown only if the corresponding scrolling
    * range is larger than the visible length of the scrolling area.
    */
   bool IsVerticalAutoScrollEnabled() const;

   /*!
    * Enables the <em>automatic scroll mode</em>.
    *
    * \param enableHorz    New enabled state for the horizontal scroll bar.
    * \param enableVert    New enabled state for the vertical scroll bar.
    *
    * In automatic scroll mode, scroll bars are automatically shown or hidden
    * as necessary. A scroll bar is shown only if the corresponding scrolling
    * range is larger than the visible length of the scrolling area.
    */
   void EnableAutoScroll( bool enableHorz = true, bool enableVert = true );

   /*!
    * Disables the <em>automatic scroll mode</em>.
    *
    * This is a convenience member function, equivalent to
    * EnableAutoScroll( !disableHorz, !disableVert )
    */
   void DisableAutoScroll( bool disableHorz = true, bool disableVert = true )
   {
      EnableAutoScroll( !disableHorz, !disableVert );
   }

   /*!
    * Gets the current horizontal scroll range.
    *
    * \param[out] minPos   Minimum horizontal scrolling position
    * \param[out] maxPos   Maximum horizontal scrolling position
    *
    * The <em>virtual document width</em> is equal to
    * maxPos - minPos + PageWidth() in scrolling units.
    */
   void GetHorizontalScrollRange( int& minPos, int& maxPos ) const;

   /*!
    * Gets the current vertical scroll range.
    *
    * \param[out] minPos   Minimum vertical scrolling position
    * \param[out] maxPos   Maximum vertical scrolling position
    *
    * The <em>virtual document height</em> is equal to
    * maxPos - minPos + PageHeight() in scrolling units.
    */
   void GetVerticalScrollRange( int& minPos, int& maxPos ) const;

   /*!
    * Returns the minimum horizontal scrolling position.
    */
   int MinHorizontalScrollPosition() const
   {
      int minPos, dum; GetHorizontalScrollRange( minPos, dum ); return minPos;
   }

   /*!
    * Returns the maximum horizontal scrolling position.
    */
   int MaxHorizontalScrollPosition() const
   {
      int dum, maxPos; GetHorizontalScrollRange( dum, maxPos ); return maxPos;
   }

   /*!
    * Returns the minimum vertical scrolling position.
    */
   int MinVerticalScrollPosition() const
   {
      int minPos, dum; GetVerticalScrollRange( minPos, dum ); return minPos;
   }

   /*!
    * Returns the maximum horizontal scrolling position.
    */
   int MaxVerticalScrollPosition() const
   {
      int dum, maxPos; GetVerticalScrollRange( dum, maxPos ); return maxPos;
   }

   /*!
    * Sets the horizontal scroll range.
    *
    * \param minPos   Minimum horizontal scrolling position
    * \param maxPos   Maximum horizontal scrolling position
    *
    * The <em>virtual document width</em> will be equal to
    * maxPos - minPos + PageWidth() in scrolling units.
    */
   void SetHorizontalScrollRange( int minPos, int maxPos );

   /*!
    * Sets the vertical scroll range.
    *
    * \param minPos   Minimum vertical scrolling position
    * \param maxPos   Maximum vertical scrolling position
    *
    * The <em>virtual document height</em> will be equal to
    * maxPos - minPos + PageHeight() in scrolling units.
    */
   void SetVerticalScrollRange( int minPos, int maxPos );

   /*!
    * Returns the current <em>page width</em> of this scroll box in scrolling
    * units.
    *
    * The page width is the length of the horizontal scroll bar's slider.
    */
   int PageWidth() const;

   /*!
    * Returns the current <em>page height</em> of this scroll box in scrolling
    * units.
    *
    * The page height is the length of the vertical scroll bar's slider, or
    * the amount scrolled when the user presses the PgUp or PgDn keys.
    */
   int PageHeight() const;

   /*!
    * Sets the <em>page size</em> of this scroll box.
    *
    * \param width   New page width in scrolling units.
    * \param height  New page height in scrolling units.
    */
   void SetPageSize( int width, int height );

   /*!
    * Sets the <em>page width</em> of this scroll box in scrolling units.
    */
   void SetPageWidth( int width )
   {
      SetPageSize( width, PageHeight() );
   }

   /*!
    * Sets the <em>page height</em> of this scroll box in scrolling units.
    */
   void SetPageHeight( int height )
   {
      SetPageSize( PageWidth(), height );
   }

   /*!
    * Returns the current <em>line width</em> of this scroll box control.
    *
    * The line width is the number of scrolling units scrolled when the user
    * clicks on an arrow icon of the horizontal scroll bar.
    */
   int LineWidth() const;

   /*!
    * Returns the current <em>line height</em> of this scroll box control.
    *
    * The line height is the number of scrolling units scrolled when the user
    * clicks on an arrow icon of the vertical scroll bar.
    */
   int LineHeight() const;

   /*!
    * Sets the <em>line sizes</em> of this scroll box.
    *
    * \param width   New line width in scrolling units.
    * \param height  New line height in scrolling units.
    */
   void SetLineSize( int width, int height );

   /*!
    * Sets the <em>line width</em> of this scroll box in scrolling units.
    */
   void SetLineWidth( int width )
   {
      SetLineSize( width, LineHeight() );
   }

   /*!
    * Sets the <em>line height</em> of this scroll box in scrolling units.
    */
   void SetLineHeight( int height )
   {
      SetLineSize( LineWidth(), height );
   }

   /*!
    * Returns the current scroll position. Returned oordinates are expressed in
    * scrolling units.
    */
   pcl::Point ScrollPosition() const;

   /*!
    * Returns the current horizontal scroll position in scrolling units.
    */
   int HorizontalScrollPosition() const
   {
      pcl::Point pos = ScrollPosition(); return pos.x;
   }

   /*!
    * Returns the current vertical scroll position in scrolling units.
    */
   int VerticalScrollPosition() const
   {
      pcl::Point pos = ScrollPosition(); return pos.y;
   }

   /*!
    * Sets the scroll position \a pos of this scroll box control.
    */
   void SetScrollPosition( const pcl::Point& pos )
   {
      SetScrollPosition( pos.x, pos.y );
   }

   /*!
    * Sets the scroll coordinates \a horzPos and \a vertPos of this scroll box
    * control.
    */
   void SetScrollPosition( int horzPos, int vertPos );

   /*!
    * Sets the horizontal scroll position.
    */
   void SetHorizontalScrollPosition( int horzPos )
   {
      SetScrollPosition( horzPos, VerticalScrollPosition() );
   }

   /*!
    * Sets the vertical scroll position.
    */
   void SetVerticalScrollPosition( int vertPos )
   {
      SetScrollPosition( HorizontalScrollPosition(), vertPos );
   }

   /*!
    * Returns true iff the <em>automatic tracking mode</em> is enabled for the
    * horizontal scroll bar of this scroll box control.
    */
   bool IsHorizontalTrackingEnabled() const;

   /*!
    * Returns true iff the <em>automatic tracking mode</em> is enabled for the
    * vertical scroll bar of this scroll box control.
    */
   bool IsVerticalTrackingEnabled() const;

   /*!
    * Enables or disables the <em>automatic tracking mode</em> for this scroll
    * box control.
    *
    * \param enableHorz    New state of automatic tracking mode for the
    *                      horizontal scroll bar.
    *
    * \param enableVert    New state of automatic tracking mode for the
    *                      vertical scroll bar.
    *
    * When automatic tracking is enabled, the viewport receives continuous
    * update requests while the user is moving an scroll bar with the mouse.
    * When automatic tracking is disabled, the viewport is updated only when
    * the mouse is released.
    */
   void EnableTracking( bool enableHorz = true, bool enableVert = true );

   /*!
    * Enables or disables the <em>automatic tracking mode</em> for the
    * horizontal scroll bar of this box control control.
    *
    * This is a convenience member function, equivalent to
    * EnableTracking( enableHorz, IsVerticalTrackingEnabled() )
    */
   void EnableHorizontalTracking( bool enableHorz = true )
   {
      EnableTracking( enableHorz, IsVerticalTrackingEnabled() );
   }

   /*!
    * Enables or disables the <em>automatic tracking mode</em> for the
    * vertical scroll bar of this box control control.
    *
    * This is a convenience member function, equivalent to
    * EnableTracking( IsHorizontalTrackingEnabled(), enableVert )
    */
   void EnableVerticalTracking( bool enableVert = true )
   {
      EnableTracking( IsHorizontalTrackingEnabled(), enableVert );
   }

   /*!
    * Disables or enables the <em>automatic tracking mode</em> for this scroll
    * box control.
    *
    * This is a convenience member function, equivalent to
    * EnableTracking( !disableHorz, !disableVert )
    */
   void DisableTracking( bool disableHorz = true, bool disableVert = true )
   {
      EnableTracking( !disableHorz, !disableVert );
   }

   /*!
    * Disables or enables the <em>automatic tracking mode</em> for the
    * horizontal scroll bar of this box control control.
    *
    * This is a convenience member function, equivalent to
    * EnableTracking( !disableHorz, IsVerticalTrackingEnabled() )
    */
   void DisableHorizontalTracking( bool disableHorz = true )
   {
      EnableTracking( !disableHorz, IsVerticalTrackingEnabled() );
   }

   /*!
    * Disables or enables the <em>automatic tracking mode</em> for the
    * vertical scroll bar of this box control control.
    *
    * This is a convenience member function, equivalent to
    * EnableTracking( IsHorizontalTrackingEnabled(), !disableVert )
    */
   void DisableVerticalTracking( bool disableVert = true )
   {
      EnableTracking( IsHorizontalTrackingEnabled(), !disableVert );
   }

   /*!
    * Returns a constant reference to the <em>viewport control</em> of this
    * scroll box.
    *
    * The viewport control represents the contents, or "virtual document" of
    * the scroll box.
    */
   const Control& Viewport() const
   {
      return m_viewport;
   }

   /*!
    * Returns a reference to the <em>viewport control</em> of this scroll box.
    *
    * The viewport control represents the contents, or "virtual document" of
    * the scroll box.
    */
   Control& Viewport()
   {
      return m_viewport;
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnHorizontalScrollPosUpdated( ScrollBox& sender, int horzPos );
   // void OnVerticalScrollPosUpdated( ScrollBox& sender, int vertPos );
   // void OnHorizontalScrollRangeUpdated( ScrollBox& sender, int minHorzPos, int maxHorzPos );
   // void OnVerticalScrollRangeUpdated( ScrollBox& sender, int minVertPos, int maxVertPos );

   /*!
    * \defgroup scroll_box_event_handlers ScrollBox Event Handlers
    */

   /*!
    * Defines the prototype of a <em>scroll position event handler</em>.
    *
    * A scroll position event is generated after the scroll position has been
    * changed by the user, either by clicking a scroll bar with the mouse or by
    * pressing a direction key when a scroll bar has the keyboard focus.
    *
    * \param sender  The control that sends a scroll position event.
    * \param pos     New scroll position in scrolling units.
    *
    * \ingroup scroll_box_event_handlers
    */
   typedef void (Control::*pos_event_handler)( ScrollBox& sender, int pos );

   /*!
    * Defines the prototype of a <em>scroll range event handler</em>.
    *
    * A scroll range event is generated after a scrolling range has been
    * changed for the horizontal or vertical scroll bars.
    *
    * \param sender     The control that sends a scroll range event.
    * \param minValue   New minimum scroll position.
    * \param maxValue   New maximum scroll position.
    *
    * \ingroup scroll_box_event_handlers
    */
   typedef void (Control::*range_event_handler)( ScrollBox& sender, int minValue, int maxValue );

   /*!
    * Sets the scroll position event handler for the horizontal scroll bar.
    *
    * \param handler    The scroll position event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive horizontal scroll
    *                   position events from this scroll box.
    *
    * \ingroup scroll_box_event_handlers
    */
   void OnHorizontalScrollPosUpdated( pos_event_handler handler, Control& receiver );

   /*!
    * Sets the scroll position event handler for the vertical scroll bar.
    *
    * \param handler    The scroll position event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive vertical scroll
    *                   position events from this scroll box.
    *
    * \ingroup scroll_box_event_handlers
    */
   void OnVerticalScrollPosUpdated( pos_event_handler handler, Control& receiver );

   /*!
    * Sets the scroll range event handler for the horizontal scroll bar.
    *
    * \param handler    The scroll range event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive horizontal scroll
    *                   range events from this scroll box.
    *
    * \ingroup scroll_box_event_handlers
    */
   void OnHorizontalScrollRangeUpdated( range_event_handler handler, Control& receiver );

   /*!
    * Sets the scroll range event handler for the vertical scroll bar.
    *
    * \param handler    The scroll range event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive vertical scroll
    *                   range events from this scroll box.
    *
    * \ingroup scroll_box_event_handlers
    */
   void OnVerticalScrollRangeUpdated( range_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      pos_event_handler   onHorizontalScrollPosUpdated   = nullptr;
      pos_event_handler   onVerticalScrollPosUpdated     = nullptr;
      range_event_handler onHorizontalScrollRangeUpdated = nullptr;
      range_event_handler onVerticalScrollRangeUpdated   = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

protected:

   Control m_viewport;

   /*!
    * \internal
    */
   ScrollBox( void* );

   /*!
    * \internal
    */
   ScrollBox( void*, void* );

   friend class ScrollBoxEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_ScrollBox_h

// ----------------------------------------------------------------------------
// EOF pcl/ScrollBox.h - Released 2017-04-14T23:04:40Z
