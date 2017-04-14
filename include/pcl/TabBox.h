//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/TabBox.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_TabBox_h
#define __PCL_TabBox_h

/// \file pcl/TabBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Bitmap.h>
#include <pcl/Control.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::TabPosition
 * \brief Tab positions in TabBox controls.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>TabPosition::Top</td>    <td>Tabs are located at the top side of the TabBox control</td></tr>
 * <tr><td>TabPosition::Bottom</td> <td>Tabs are located at the bottom side of the TabBox control</td></tr>
 * </table>
 */
namespace TabPosition
{
   enum value_type
   {
      Top,     // Tabs are located at the top side of the tab box control
      Bottom   // Tabs are located at the bottom side of the tab box control
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class TabBox
 * \brief Client-side interface to a PixInsight %TabBox control.
 *
 * ### TODO: Write a detailed description for %TabBox.
 */
class PCL_CLASS TabBox : public Control
{
public:

   /*!
    * Represents a tab position.
    */
   typedef TabPosition::value_type  tab_position;

   /*!
    * Constructs a %TabBox as a child control of \a parent.
    */
   TabBox( Control& parent = Control::Null() );

   /*!
    * Destroys a %TabBox control.
    */
   virtual ~TabBox()
   {
   }

   /*!
    * Returns the number of page controls in this %TabBox.
    */
   int NumberOfPages() const;

   /*!
    * Returns the zero-based index of the current page control in this %TabBox,
    * or -1 if this %TabBox has no page controls.
    */
   int CurrentPageIndex() const;

   /*!
    * Selects the current page control by its zero-based \a index.
    */
   void SetCurrentPageIndex( int index );

   /*!
    * A synonym for SetCurrentPageIndex( index )
    */
   void SelectPage( int index )
   {
      SetCurrentPageIndex( index );
   }

   /*!
    * Returns a reference to the current page control in this %TabBox, or
    * Control::Null() if this %TabBox has no page controls.
    */
   Control& CurrentPageControl() const;

   /*!
    * Returns a reference to the page control at the specified zero-based
    * \a index in this %TabBox, or Control::Null() if this %TabBox has no page
    * controls, or if \a index is not valid.
    */
   Control& PageControlByIndex( int index ) const;

   /*!
    * Inserts a new page control in this %TabBox.
    *
    * \param index   Zero-based index where the new page control will be
    *             inserted. If \a index is greater than or equal to the number
    *             of existing page controls, the new page will be appended to
    *             the list of existing page controls. If \a index is less than
    *             zero, the new page will be prepended to the list of existing
    *             page controls.
    *
    * \param page    The page control to be inserted.
    *
    * \param label   The text that will be shown on the new tab selector.
    *
    * \param icon    The icon that will be shown on the new tab selector.
    *
    * \sa AddPage(), RemovePage()
    */
   void InsertPage( int index, Control& page, const String& label, const Bitmap& icon = Bitmap::Null() );

   /*!
    * Appends a new page control to this %TabBox.
    *
    * This is a convenience member function, equivalent to
    * InsertPage( NumberOfPages(), page, label, icon )
    *
    * \a InsertPage, RemovePage()
    */
   void AddPage( Control& page, const String& label, const Bitmap& icon = Bitmap::Null() )
   {
      InsertPage( NumberOfPages(), page, label, icon );
   }

   /*!
    * Removes the page control at the specified zero-based \a index.
    *
    * This member function does not delete the page control; it simply removes
    * it from the list of page controls in this %TabBox.
    *
    * \sa InsertPage(), AddPage()
    */
   void RemovePage( int idx );

   /*!
    * Returns the position of the tab selectors row in this %TabBox.
    *
    * \sa SetTabPosition()
    */
   tab_position TabPosition() const;

   /*!
    * Sets the position of the tab selectors row in this %TabBox.
    *
    * \sa TabPosition()
    */
   void SetTabPosition( tab_position pos );

   /*!
    * Returns true iff the page control at the specified \a index is enabled.
    *
    * \sa EnablePage(), DisablePage()
    */
   bool IsPageEnabled( int index ) const;

   /*!
    * Enables or disables the page control at the specified \a index.
    *
    * \sa DisablePage(), IsPageEnabled()
    */
   void EnablePage( int index, bool enable = true );

   /*!
    * Disables or enables the page control at the specified \a index.
    *
    * This is a convenience member function, equivalent to
    * EnablePage( index, !disable )
    *
    * \sa EnablePage(), IsPageEnabled()
    */
   void DisablePage( int index, bool disable = true )
   {
      EnablePage( index, !disable );
   }

   /*! #
    */
   String PageLabel( int idx ) const;

   /*! #
    */
   void SetPageLabel( int idx, const String& );

   /*! #
    */
   void ClearPageLabel( int idx )
   {
      SetPageLabel( idx, String() );
   }

   /*! #
    */
   Bitmap PageIcon( int idx ) const;

   /*! #
    */
   void SetPageIcon( int idx, const Bitmap& );

   /*! #
    */
   void ClearPageIcon( int idx )
   {
      SetPageIcon( idx, Bitmap::Null() );
   }

   /*! #
    */
   String PageToolTip( int idx ) const;

   /*! #
    */
   void SetPageToolTip( int idx, const String& );

   /*! #
    */
   void ClearPageToolTip( int idx )
   {
      SetPageToolTip( idx, String() );
   }

   /*! #
    */
   Control& LeftControl() const;

   /*! #
    */
   Control& RightControl() const;

   /*! #
    */
   void SetControls( Control& left, Control& right );

   /*! #
    */
   void SetLeftControl( Control& w )
   {
      SetControls( w, RightControl() );
   }

   /*! #
    */
   void SetRightControl( Control& w )
   {
      SetControls( LeftControl(), w );
   }

   /*! #
    */
   void ClearLeftControl()
   {
      SetLeftControl( Control::Null() );
   }

   /*! #
    */
   void ClearRightControl()
   {
      SetRightControl( Control::Null() );
   }

   /*! #
    */
   void ClearControls()
   {
      SetControls( Control::Null(), Control::Null() );
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnPageSelected( TabBox& sender, int pageIndex );

   /*!
    * \defgroup tab_box_event_handlers TabBox Event Handlers
    */

   /*!
    * Defines the prototype of a <em>tab box page event handler</em>.
    *
    * A tab box page event is generated when the user changes the current page
    * in a tab box control.
    *
    * \param sender     The control that sends a tab box page event.
    *
    * \param pageIndex  The zero-based index of a tab box page.
    *
    * \ingroup tab_box_event_handlers
    */
   typedef void (Control::*page_event_handler)( TabBox& sender, int pageIndex );

   /*!
    * Sets the tab box page selected event handler for this %TabBox control.
    *
    * \param handler    The tab box page event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive tab box page selected
    *                   events from this %TabBox.
    *
    * \ingroup tab_box_event_handlers
    */
   void OnPageSelected( page_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      page_event_handler onPageSelected = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   friend class TabBoxEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_TabBox_h

// ----------------------------------------------------------------------------
// EOF pcl/TabBox.h - Released 2017-04-14T23:04:40Z
