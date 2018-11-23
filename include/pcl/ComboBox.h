//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/ComboBox.h - Released 2018-11-23T16:14:19Z
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

#ifndef __PCL_ComboBox_h
#define __PCL_ComboBox_h

/// \file pcl/ComboBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Bitmap.h>
#include <pcl/Control.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ComboBox
 * \brief Client-side interface to a PixInsight %ComboBox control
 *
 * ### TODO: Write a detailed description for %ComboBox
 */
class PCL_CLASS ComboBox : public Control
{
public:

   /*!
    * Constructs a %ComboBox as a child control of \a parent.
    */
   ComboBox( Control& parent = Control::Null() );

   /*!
    * Destroys a %ComboBox control.
    */
   virtual ~ComboBox()
   {
   }

   /*!
    * Returns the total number of items in this %ComboBox control.
    */
   int NumberOfItems() const;

   /*!
    * Returns the index of the current item in this combo box, or -1 if no item
    * is currently selected in this combo box.
    */
   int CurrentItem() const;

   /*!
    * Selects a combo box item with the specified \a index.
    */
   void SetCurrentItem( int index );

   /*!
    * Finds an item in this combo box. Returns the index of the item found, or
    * -1 if no item was found that matches the specified criteria.
    *
    * \param text             Text to find.
    *
    * \param fromIdx          Starting index from which the search will begin.
    *
    * \param exactMatch       Whether the search will succeed only if an exact
    *                         match is found, or a partial match is sufficient.
    *
    * \param caseSensitive    True for a case-sensitive search.
    */
   int FindItem( const String& text, int fromIdx = 0, bool exactMatch = false, bool caseSensitive = false ) const;

   /*!
    * Inserts a new item in this combo box.
    *
    * \param index   Insertion point. If an index greater than or equal to the
    *                current length is specified, the new item is appended to
    *                the end of the current set of items. If the index is less
    *                than or equal to zero, the new item will be prepended to
    *                the current list of items.
    *
    * \param text    Text for the new item.
    *
    * \param icon    Item icon.
    */
   void InsertItem( int index, const String& text, const Bitmap& icon = Bitmap::Null() );

   /*!
    * Adds a new item at the end of this combo box.
    *
    * This is a convenience function, equivalent to:
    * InsertItem( NumberOfItems(), text, icon )
    */
   void AddItem( const String& text, const Bitmap& icon = Bitmap::Null() )
   {
      InsertItem( NumberOfItems(), text, icon );
   }

   /*!
    * Inserts a list of combo box items.
    *
    * \param index   Insertion point. If an index greater than or equal to the
    *                current length is specified, the new items are appended to
    *                the end of the current set of items. If the index is less
    *                than or equal to zero, the new items will be prepended to
    *                the current list of items.
    *
    * \param i,j     Two forward iterators defining a range of contiguous
    *                strings that correspond to the texts of the newly created
    *                items.
    */
   template <class FI>
   void InsertItems( int index, FI i, FI j )
   {
      if ( i != j )
      {
         DisableUpdates();
         do
            InsertItem( index++, *i );
         while ( ++i != j );
         EnableUpdates();
         Update();
      }
   }

   /*!
    * Appends a list of combo box items.
    *
    * This is a convenience function, equivalent to:
    * InsertItems( NumberOfItems(), i, j )
    */
   template <class FI>
   void AddItems( FI i, FI j )
   {
      InsertItems( NumberOfItems(), i, j );
   }

   /*!
    * Inserts a list of combo box items from a container.
    *
    * \param index   Insertion point. If an index greater than or equal to the
    *                current length is specified, the new items are appended to
    *                the end of the current set of items. If the index is less
    *                than or equal to zero, the new items will be prepended to
    *                the current list of items.
    *
    * \param c       A reference to a container with the list of strings that
    *                will be assigned to newly created items.
    *
    * This is a convenience function, equivalent to:
    * InsertItems( index, c.Begin(), c.End() )
    */
   template <class C>
   void InsertItems( int index, const C& c )
   {
      InsertItems( index, c.Begin(), c.End() );
   }

   /*!
    * Appends a list of combo box items from a container.
    *
    * \param c    A reference to a container with the list of strings that will
    *             be used for newly created items.
    *
    * This is a convenience function, equivalent to:
    * InsertItems( NumberOfItems(), c );
    */
   template <class C>
   void AddItems( const C& c )
   {
      InsertItems( NumberOfItems(), c );
   }

   /*!
    * Removes the item at the specified \a index. If the specified index is
    * greater than or equal to the current length, or less than zero,, no items
    * are removed.
    */
   void RemoveItem( int index );

   /*!
    * Removes all existing items in this combo box.
    */
   void Clear();

   /*!
    * Returns the current text of the combo box item at the specified \a index.
    */
   String ItemText( int index ) const;

   /*!
    * Sets the current text of the combo box item at the specified \a index.
    */
   void SetItemText( int index, const String& );

   /*!
    * Empties the text of the combo box item at the specified \a index.
    */
   void ClearItemText( int index )
   {
      SetItemText( index, String() );
   }

   /*!
    * Returns the current icon of the combo box item at the specified \a index.
    */
   Bitmap ItemIcon( int index ) const;

   /*!
    * Changes the icon of the combo box item at the specified \a index. If a
    * null bitmap is specified, the combo box item will have no associated icon.
    */
   void SetItemIcon( int index, const Bitmap& );

   /*!
    * Clears the icon of the combo box item at the specified \a index.
    *
    * This is a convenience function, equivalent to:
    * SetItemIcon( index, Bitmap::Null() )
    */
   void ClearItemIcon( int index )
   {
      SetItemIcon( index, Bitmap::Null() );
   }

   /*!
    * Returns true iff this combo box is editable. Editable combo boxes allow
    * the user to edit the text of the currently selected item.
    */
   bool IsEditEnabled() const;

   /*!
    * Enables or disables the editable state of this combo box control.
    */
   void EnableEdit( bool = true );

   /*!
    * Disables or enables the editable state of this combo box control.
    *
    * This is a convenience function, equivalent to:
    * EnableEdit( !disable )
    */
   void DisableEdit( bool disable = true )
   {
      EnableEdit( !disable );
   }

   /*!
    * Returns the current text of the editable part of this combo box control.
    */
   String EditText() const;

   /*!
    * Sets the text contents of the editable part of this combo box control.
    */
   void SetEditText( const String& );

   /*!
    * Empties the editable part of this combo box control.
    */
   void ClearEditText()
   {
      SetEditText( String() );
   }

   /*!
    * Returns true iff auto completion is currently enabled for this combo box
    * control.
    */
   bool IsAutoCompletionEnabled() const;

   /*!
    * Enables or disables auto completion for this combo box control.
    */
   void EnableAutoCompletion( bool = true );

   /*!
    * Disables or enables auto completion for this combo box control.
    *
    * This is a convenience function, equivalent to:
    * EnableAutoCompletion( !disable )
    */
   void DisableAutoCompletion( bool disable = true )
   {
      EnableAutoCompletion( !disable );
   }

   /*!
    * Provides the current icon dimensions for this combo box control.
    *
    * \param[out] width,height   Icon dimensions in pixels.
    */
   void GetIconSize( int& width, int& height ) const;

   /*!
    * Returns the current icon width in pixels for this combo box control.
    */
   int IconWidth() const
   {
      int w, dum; GetIconSize( w, dum ); return w;
   }

   /*!
    * Returns the current icon height in pixels for this combo box control.
    */
   int IconHeight() const
   {
      int dum, h; GetIconSize( dum, h ); return h;
   }

   /*!
    * Sets the icon dimensions in pixels for this combo box control.
    */
   void SetIconSize( int width, int height );

   /*!
    * Sets the icon size in pixels for this combo box control.
    *
    * This is a convenience function, equivalent to:
    * SetIconSize( size, size )
    */
   void SetIconSize( int size )
   {
      SetIconSize( size, size );
   }

   /*! #
    */
   void GetScaledIconSize( int& width, int& height ) const
   {
      GetIconSize( width, height ); width = PhysicalPixelsToLogical( width ); height = PhysicalPixelsToLogical( height );
   }

   /*! #
    */
   int ScaledIconWidth() const
   {
      int width, dum; GetIconSize( width, dum ); return PhysicalPixelsToLogical( width );
   }

   /*! #
    */
   int ScaledIconHeight() const
   {
      int dum, height; GetIconSize( dum, height ); return PhysicalPixelsToLogical( height );
   }

   /*! #
    */
   void SetScaledIconSize( int width, int height )
   {
      SetIconSize( LogicalPixelsToPhysical( width ), LogicalPixelsToPhysical( height ) );
   }

   /*! #
    */
   void SetScaledIconSize( int size )
   {
      size = LogicalPixelsToPhysical( size );
      SetIconSize( size, size );
   }

   /*!
    * Returns the maximum number of items that can be visible when this combo
    * box control is dropped down, or 0 is no specific limit has been set.
    */
   int MaxVisibleItemCount() const;

   /*!
    * Sets the maximum number of items that can be visible when this combo
    * box control is dropped down. If zero is specified, the combo box control
    * will show a convenient number of items automatically in drop-down state.
    */
   void SetMaxVisibleItemCount( int );

   /*!
    * Returns the minimum number of characters that fit in this combo box
    * control, or zero if no specific limit has been set.
    */
   int MinItemCharWidth() const;

   /*!
    * Sets the minimum number of characters that must fit in this combo box
    * control. If zero is specified, the combo box control will adapt its
    * size automatically to its initial contents.
    */
   void SetMinItemCharWidth( int );

   //
   // Drop-down items list
   //

   /*!
    * Forces this combo box control to enter the drop-down state. The list of
    * existing items is shown on a pop-up panel where they can be browsed and
    * selected.
    */
   void ShowList();

   /*!
    * Forces this combo box control to exit the drop-down state. The list of
    * items will be hidden, if it is currently visible.
    */
   void HideList();

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnItemSelected( ComboBox& sender, int itemIndex );
   // void OnItemHighlighted( ComboBox& sender, int itemIndex );
   // void OnEditTextUpdated( ComboBox& sender );

   /*!
    * \defgroup combobox_event_handlers ComboBox Event Handlers
    */

   /*!
    * Defines the prototype of a <em>combo box item event handler</em>.
    *
    * A combo box item event is generated when an item is selected or
    * highlighted in a combo box.
    *
    * \param sender     The ComboBox control that sends a combo box item event.
    *
    * \param itemIndex  The index of the combo box item where the event
    *                   originated, in the range from zero to the number of
    *                   existing combo box items minus one.
    *
    * \ingroup combobox_event_handlers
    */
   typedef void (Control::*item_event_handler)( ComboBox& sender, int itemIndex );

   /*!
    * Defines the prototype of a <em>combo box edit event handler</em>.
    *
    * A combo box edit event is generated when the text in the editable part of
    * a combo box is changed by the user.
    *
    * \param sender     The ComboBox control that sends a combo box edit event.
    *
    * \ingroup combobox_event_handlers
    */
   typedef void (Control::*edit_event_handler)( ComboBox& sender );

   /*!
    * Sets the <em>item selected</em> event handler for this combo box.
    *
    * \param handler    The combo box item event handler. Must be a member
    *                   function of the receiver object's class. This handler
    *                   will be called whenever an item is selected in this
    *                   combo box control.
    *
    * \param receiver   The control that will receive <em>item selected</em>
    *                   events from this combo box.
    *
    * \ingroup combobox_event_handlers
    */
   void OnItemSelected( item_event_handler handler, Control& receiver );

   /*!
    * Sets the <em>item highlighted</em> event handler for this combo box.
    *
    * \param handler    The combo box item event handler. Must be a member
    *                   function of the receiver object's class. This handler
    *                   will be called whenever an item is highlighted in this
    *                   combo box control.
    *
    * \param receiver   The control that will receive <em>item highlighted</em>
    *                   events from this combo box.
    *
    * \ingroup combobox_event_handlers
    */
   void OnItemHighlighted( item_event_handler handler, Control& receiver );

   /*!
    * Sets the <em>edit updated</em> event handler for this combo box.
    *
    * \param handler    The combo box edit event handler. Must be a member
    *                   function of the receiver object's class. This handler
    *                   will be called whenever the text changes in the
    *                   editable part of combo box control.
    *
    * \param receiver   The control that will receive <em>edit updated</em>
    *                   events from this combo box.
    *
    * \ingroup combobox_event_handlers
    */
   void OnEditTextUpdated( edit_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      item_event_handler onItemSelected    = nullptr;
      item_event_handler onItemHighlighted = nullptr;
      edit_event_handler onEditTextUpdated = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   friend class ComboBoxEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_ComboBox_h

// ----------------------------------------------------------------------------
// EOF pcl/ComboBox.h - Released 2018-11-23T16:14:19Z
