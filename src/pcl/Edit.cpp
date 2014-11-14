// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/Edit.cpp - Released 2014/11/14 17:17:00 UTC
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

#include <pcl/Edit.h>
#include <pcl/TextAlign.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Edit*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))

class EditEventDispatcher
{
public:

   static void api_func EditCompleted( control_handle hSender, control_handle hReceiver )
   {
      if ( sender->onEditCompleted != 0 )
         (receiver->*sender->onEditCompleted)( *sender );
   }

   static void api_func ReturnPressed( control_handle hSender, control_handle hReceiver )
   {
      if ( sender->onReturnPressed != 0 )
         (receiver->*sender->onReturnPressed)( *sender );
   }

   static void api_func TextUpdated( control_handle hSender, control_handle hReceiver, const char16_type* text )
   {
      if ( sender->onTextUpdated != 0 )
      {
         String s( text );
         (receiver->*sender->onTextUpdated)( *sender, s );
      }
   }

   static void api_func CaretPositionUpdated( control_handle hSender, control_handle hReceiver, int32 oldPos, int32 newPos )
   {
      if ( sender->onCaretPositionUpdated != 0 )
         (receiver->*sender->onCaretPositionUpdated)( *sender, oldPos, newPos );
   }

   static void api_func SelectionUpdated( control_handle hSender, control_handle hReceiver, int32 newStart, int32 newEnd )
   {
      if ( sender->onSelectionUpdated != 0 )
         (receiver->*sender->onSelectionUpdated)( *sender, newStart, newEnd );
   }
}; // EditEventDispatcher

#undef sender
#undef receiver

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#  pragma warning( disable: 4355 ) // 'this' : used in base member initializer list
#endif

Edit::Edit( const String& text, Control& parent ) :
Frame( (*API->Edit->CreateEdit)( ModuleHandle(), this, text.c_str(), parent.handle, 0 /*flags*/ ) ),
onEditCompleted( 0 ),
onReturnPressed( 0 ),
onTextUpdated( 0 ),
onCaretPositionUpdated( 0 ),
onSelectionUpdated( 0 )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateEdit" );
}

// ----------------------------------------------------------------------------

void Edit::OnEditCompleted( edit_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onEditCompleted = 0;
   if ( (*API->Edit->SetEditCompletedEventRoutine)( handle, &receiver,
        (f != 0) ? EditEventDispatcher::EditCompleted : 0 ) == api_false )
   {
      throw APIFunctionError( "SetEditCompletedEventRoutine" );
   }
   onEditCompleted = f;
}

void Edit::OnReturnPressed( edit_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onReturnPressed = 0;
   if ( (*API->Edit->SetReturnPressedEventRoutine)( handle, &receiver,
        (f != 0) ? EditEventDispatcher::ReturnPressed : 0 ) == api_false )
   {
      throw APIFunctionError( "SetReturnPressedEventRoutine" );
   }
   onReturnPressed = f;
}

void Edit::OnTextUpdated( text_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onTextUpdated = 0;
   if ( (*API->Edit->SetTextUpdatedEventRoutine)( handle, &receiver,
        (f != 0) ? EditEventDispatcher::TextUpdated : 0 ) == api_false )
   {
      throw APIFunctionError( "SetTextUpdatedEventRoutine" );
   }
   onTextUpdated = f;
}

void Edit::OnCaretPositionUpdated( caret_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onCaretPositionUpdated = 0;
   if ( (*API->Edit->SetCaretPositionUpdatedEventRoutine)( handle, &receiver,
        (f != 0) ? EditEventDispatcher::CaretPositionUpdated : 0 ) == api_false )
   {
      throw APIFunctionError( "SetCaretPositionUpdatedEventRoutine" );
   }
   onCaretPositionUpdated = f;
}

void Edit::OnSelectionUpdated( selection_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onSelectionUpdated = 0;
   if ( (*API->Edit->SetSelectionUpdatedEventRoutine)( handle, &receiver,
        (f != 0) ? EditEventDispatcher::SelectionUpdated : 0 ) == api_false )
   {
      throw APIFunctionError( "SetSelectionUpdatedEventRoutine" );
   }
   onSelectionUpdated = f;
}

// ----------------------------------------------------------------------------

String Edit::Text() const
{
   size_type len = 0;
   (*API->Edit->GetEditText)( handle, 0, &len );

   String text;

   if ( len != 0 )
   {
      text.Reserve( len );

      if ( (*API->Edit->GetEditText)( handle, text.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetEditText" );
   }

   return text;
}

// ----------------------------------------------------------------------------

void Edit::SetText( const String& text )
{
   (*API->Edit->SetEditText)( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

bool Edit::IsReadOnly() const
{
   return (*API->Edit->GetEditReadOnly)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Edit::SetReadOnly( bool readOnly )
{
   (*API->Edit->SetEditReadOnly)( handle, readOnly );
}

// ----------------------------------------------------------------------------

bool Edit::IsModified() const
{
   return (*API->Edit->GetEditModified)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Edit::SetModified( bool modified )
{
   (*API->Edit->SetEditModified)( handle, modified );
}

// ----------------------------------------------------------------------------

bool Edit::IsPasswordMode() const
{
   return (*API->Edit->GetEditPasswordEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Edit::EnablePasswordMode( bool enable )
{
   (*API->Edit->SetEditPasswordEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

int Edit::MaxLength() const
{
   return (*API->Edit->GetEditMaxLength)( handle );
}

// ----------------------------------------------------------------------------

void Edit::SetMaxLength( int n )
{
   (*API->Edit->SetEditMaxLength)( handle, n );
}

// ----------------------------------------------------------------------------
/*
String Edit::Mask() const
{
   size_type len = 0;
   (*API->Edit->GetEditMask)( handle, 0, &len );

   String mask;

   if ( len != 0 )
   {
      mask.Reserve( len );

      if ( (*API->Edit->GetEditMask)( handle, mask.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetEditMask" );
   }

   return mask;
}

// ----------------------------------------------------------------------------

void Edit::SetMask( const String& mask )
{
   (*API->Edit->SetEditMask)( handle, mask.c_str() );
}

// ----------------------------------------------------------------------------

bool Edit::IsValid() const
{
   return true; // ### TODO
}
*/
// ----------------------------------------------------------------------------

void Edit::SelectAll( bool select )
{
   (*API->Edit->SetEditSelected)( handle, select );
}

// ----------------------------------------------------------------------------

void Edit::GetSelection( int& selStart, int& selEnd ) const
{
   selStart = selEnd = 0;
   (*API->Edit->GetEditSelection)( handle, &selStart, &selEnd );
}

// ----------------------------------------------------------------------------

void Edit::SetSelection( int selStart, int selEnd )
{
   (*API->Edit->SetEditSelection)( handle, selStart, selEnd );
}

// ----------------------------------------------------------------------------

String Edit::SelectedText() const
{
   size_type len = 0;
   (*API->Edit->GetEditSelectedText)( handle, 0, &len );

   String text;

   if ( len != 0 )
   {
      text.Reserve( len );

      if ( (*API->Edit->GetEditSelectedText)( handle, text.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetEditSelectedText" );
   }

   return text;
}

// ----------------------------------------------------------------------------

int Edit::CaretPosition() const
{
   return (*API->Edit->GetEditCaretPosition)( handle );
}

// ----------------------------------------------------------------------------

void Edit::SetCaretPosition( int pos )
{
   (*API->Edit->SetEditCaretPosition)( handle, pos );
}

// ----------------------------------------------------------------------------

bool Edit::IsRightAligned() const
{
   return (*API->Edit->GetEditAlignment)( handle ) == TextAlign::Right;
}

// ----------------------------------------------------------------------------

void Edit::SetRightAlignment( bool right )
{
   (*API->Edit->SetEditAlignment)( handle, right ? TextAlign::Right : TextAlign::Left );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/Edit.cpp - Released 2014/11/14 17:17:00 UTC
