//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// pcl/TextBox.cpp - Released 2017-05-02T10:39:13Z
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

#include <pcl/TextBox.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

TextBox::TextBox( const String& text, Control& parent ) :
   Frame( (*API->TextBox->CreateTextBox)( ModuleHandle(), this, text.c_str(), parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateTextBox" );
}

// ----------------------------------------------------------------------------

String TextBox::Text() const
{
   size_type len = 0;
   (*API->TextBox->GetTextBoxText)( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->TextBox->GetTextBoxText)( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetTextBoxText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void TextBox::SetText( const String& text )
{
   (*API->TextBox->SetTextBoxText)( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

bool TextBox::IsReadOnly() const
{
   return (*API->TextBox->GetTextBoxReadOnly)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TextBox::SetReadOnly( bool readOnly )
{
   (*API->TextBox->SetTextBoxReadOnly)( handle, readOnly );
}

// ----------------------------------------------------------------------------

void TextBox::SelectAll( bool select )
{
   (*API->TextBox->SetTextBoxSelected)( handle, select );
}

// ----------------------------------------------------------------------------

void TextBox::GetSelection( int& selStart, int& selEnd ) const
{
   selStart = selEnd = 0;
   (*API->TextBox->GetTextBoxSelection)( handle, &selStart, &selEnd );
}

// ----------------------------------------------------------------------------

void TextBox::SetSelection( int selStart, int selEnd )
{
   (*API->TextBox->SetTextBoxSelection)( handle, selStart, selEnd );
}

// ----------------------------------------------------------------------------

String TextBox::SelectedText() const
{
   size_type len = 0;
   (*API->TextBox->GetTextBoxSelectedText)( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->TextBox->GetTextBoxSelectedText)( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetTextBoxSelectedText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

int TextBox::CaretPosition() const
{
   return (*API->TextBox->GetTextBoxCaretPosition)( handle );
}

// ----------------------------------------------------------------------------

void TextBox::SetCaretPosition( int pos )
{
   (*API->TextBox->SetTextBoxCaretPosition)( handle, pos );
}

// ----------------------------------------------------------------------------

void TextBox::Insert( const String& text )
{
   (*API->TextBox->InsertTextBoxText)( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

void TextBox::Delete()
{
   (*API->TextBox->DeleteTextBoxText)( handle );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<TextBox*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class TextBoxEventDispatcher
{
public:

   static void api_func TextUpdated( control_handle hSender, control_handle hReceiver, const char16_type* text )
   {
      if ( handlers->onTextUpdated != nullptr )
      {
         String s( text );
         (receiver->*handlers->onTextUpdated)( *sender, s );
      }
   }

   static void api_func CaretPositionUpdated( control_handle hSender, control_handle hReceiver, int32 oldPos, int32 newPos )
   {
      if ( handlers->onCaretPositionUpdated != nullptr )
         (receiver->*handlers->onCaretPositionUpdated)( *sender, oldPos, newPos );
   }

   static void api_func SelectionUpdated( control_handle hSender, control_handle hReceiver, int32 newStart, int32 newEnd )
   {
      if ( handlers->onSelectionUpdated != nullptr )
         (receiver->*handlers->onSelectionUpdated)( *sender, newStart, newEnd );
   }
}; // TextBoxEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void TextBox::OnTextUpdated( unicode_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TextBox->SetTextBoxUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TextBoxEventDispatcher::TextUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTextBoxUpdatedEventRoutine" );
   m_handlers->onTextUpdated = f;
}

void TextBox::OnCaretPositionUpdated( caret_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TextBox->SetTextBoxCaretPositionUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TextBoxEventDispatcher::CaretPositionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTextBoxCaretPositionUpdatedEventRoutine" );
   m_handlers->onCaretPositionUpdated = f;
}

void TextBox::OnSelectionUpdated( selection_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TextBox->SetTextBoxSelectionUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TextBoxEventDispatcher::SelectionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTextBoxSelectionUpdatedEventRoutine" );
   m_handlers->onSelectionUpdated = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/TextBox.cpp - Released 2017-05-02T10:39:13Z
