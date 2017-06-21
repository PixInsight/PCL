//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/TextBox.h - Released 2017-06-21T11:36:33Z
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

#ifndef __PCL_TextBox_h
#define __PCL_TextBox_h

/// \file pcl/TextBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Frame.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class TextBox
 * \brief Client-side interface to a PixInsight %TextBox control.
 *
 * %TextBox is a multiline, plain text output control. A %TextBox control
 * implements the entire text output functionality of a PixInsight console;
 * refer to the Console class for full information about console output
 * capabilities.
 *
 * \sa Console, CodeEditor
 */
class PCL_CLASS TextBox : public Frame
{
public:

   /*!
    * Constructs a %TextBox as a child control of \a parent with the specified
    * initial \a text.
    */
   TextBox( const String& text = String(), Control& parent = Control::Null() );

   /*!
    * Destroys a %TextBox control.
    */
   virtual ~TextBox()
   {
   }

   /*!
    * Returns the current text of this %TextBox control.
    */
   String Text() const;

   /*!
    * Sets the text of this %TextBox control.
    */
   void SetText( const String& );

   /*!
    * Returns a string that can be used to write the specified \a text to a
    * %TextBox control as <em>plain text</em>, that is without interpreting
    * tags or character entities.
    */
   template <class S>
   static S PlainText( const S& text )
   {
      return "<raw>" + text + "</raw>";
   }

   /*!
    * Sets the <em>plain text</em> of this %TextBox control.
    *
    * This member function is equivalent to SetText( PlainText( text ) ).
    */
   template <class S>
   void SetPlainText( const S& text )
   {
      SetText( PlainText( text ) );
   }

   /*!
    * Clears (removes) the text in this %TextBox control.
    */
   void Clear()
   {
      SetText( String() );
   }

   /*!
    * Returns true iff this %TextBox control is in read-only state.
    *
    * \sa SetReadOnly(), SetReadWrite()
    */
   bool IsReadOnly() const;

   /*!
    * Enables or disables the read-only state of this %TextBox control.
    *
    * \sa SetReadWrite(), IsReadOnly()
    */
   void SetReadOnly( bool ro = true );

   /*!
    * Disables or enables the read-only state of this %TextBox control.
    *
    * This is a convenience member function, equivalent to
    * SetReadOnly( !rw )
    *
    * \sa SetReadOnly(), IsReadOnly()
    */
   void SetReadWrite( bool rw = true )
   {
      SetReadOnly( !rw );
   }

   /*!
    * Selects all the text in this %TextBox control.
    *
    * \sa Unselect(), GetSelection(), SetSelection()
    */
   void SelectAll( bool = true );

   /*!
    * Clears the current text selection in this %TextBox.
    *
    * This member function does not delete the selected text; it only removes
    * the selection.
    *
    * \sa SelectAll(), GetSelection(), SetSelection()
    */
   void Unselect( bool unsel = true )
   {
      SelectAll( !unsel );
   }

   /*!
    * Gets the current text selection in this %TextBox control.
    *
    * \param selStart   Index of the first selected character.
    * \param selEnd     Index of the last selected character plus one.
    *
    * When selStart < selEnd, there is a text selection in this %TextBox. Both
    * indexes are zero-based.
    *
    * \sa SetSelection()
    */
   void GetSelection( int& selStart, int& selEnd ) const;

   /*!
    * Sets a new text selection in this %TextBox control.
    *
    * \param selStart   Index of the first selected character.
    * \param selEnd     Index of the last selected character plus one.
    *
    * When selStart < selEnd, there is a text selection in this %TextBox. Both
    * indexes are zero-based.
    *
    * \sa GetSelection()
    */
   void SetSelection( int selStart, int selEnd );

   /*!
    * Returns true iff this %TextBox control has a valid selection.
    */
   bool HasSelection() const
   {
      int s0, s1;
      GetSelection( s0, s1 );
      return s0 != s1;
   }

   /*!
    * Returns the selected text in this %TextBox control, or an empty string if
    * there is no selection.
    *
    * \sa GetSelection(), SetSelection(), SelectAll(), Unselect()
    */
   String SelectedText() const;

   /*! #
    */
   int CaretPosition() const;

   /*! #
    */
   void SetCaretPosition( int );

   /*! #
    */
   void Home()
   {
      SetCaretPosition( 0 );
   }

   /*! #
    */
   void End()
   {
      SetCaretPosition( int_max );
   }

   /*! #
    */
   void Insert( const String& text );

   /*! #
    */
   void Delete();

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnTextUpdated( TextBox& sender, const String& text );
   // void OnCaretPositionUpdated( TextBox& sender, int oldPos, int newPos );
   // void OnSelectionUpdated( TextBox& sender, int newStart, int newEnd );

   /*! #
    */
   typedef void (Control::*unicode_event_handler)( TextBox& sender, const String& );

   /*! #
    */
   typedef void (Control::*caret_event_handler)( TextBox& sender, int oldPos, int newPos );

   /*! #
    */
   typedef void (Control::*selection_event_handler)( TextBox& sender, int newStart, int newEnd );

   /*! #
    */
   void OnTextUpdated( unicode_event_handler, Control& );

   /*! #
    */
   void OnCaretPositionUpdated( caret_event_handler, Control& );

   /*! #
    */
   void OnSelectionUpdated( selection_event_handler, Control& );

private:

   struct EventHandlers
   {
      unicode_event_handler   onTextUpdated          = nullptr;
      caret_event_handler     onCaretPositionUpdated = nullptr;
      selection_event_handler onSelectionUpdated     = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   friend class TextBoxEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_TextBox_h

// ----------------------------------------------------------------------------
// EOF pcl/TextBox.h - Released 2017-06-21T11:36:33Z
