//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// pcl/CodeEditor.cpp - Released 2015/11/26 15:59:45 UTC
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

#include <pcl/CodeEditor.h>
#include <pcl/MetaModule.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CodeEditor::CodeEditor( Control& parent ) :
   Control( (*API->CodeEditor->CreateCodeEditor)( ModuleHandle(), this, parent.handle, 0/*flags*/ ) ),
   m_lineNumbers( nullptr )
{
   if ( handle == nullptr )
      throw APIFunctionError( "CreateCodeEditor" );
}

// ----------------------------------------------------------------------------

Control& CodeEditor::LineNumbersControl()
{
   if ( m_lineNumbers.IsNull() )
   {
      m_lineNumbers.TransferHandle( (*API->CodeEditor->CreateEditorLineNumbersControl)( handle, &m_lineNumbers, handle, 0/*flags*/ ) );
      if ( m_lineNumbers.IsNull() )
         throw APIFunctionError( "CreateEditorLineNumbersControl" );
   }
   return m_lineNumbers;
}

// ----------------------------------------------------------------------------

String CodeEditor::FilePath() const
{
   size_type len = 0;
   (*API->CodeEditor->GetEditorFilePath)( handle, 0, &len );

   String path;
   if ( len > 0 )
   {
      path.SetLength( len );
      if ( (*API->CodeEditor->GetEditorFilePath)( handle, path.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetEditorFilePath" );
      path.ResizeToNullTerminated();
   }
   return path;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetFilePath( const String& filePath )
{
   (*API->CodeEditor->SetEditorFilePath)( handle, filePath.c_str() );
}

// ----------------------------------------------------------------------------

String CodeEditor::Text() const
{
   size_type len = 0;
   (*API->CodeEditor->GetEditorText)( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->CodeEditor->GetEditorText)( handle, text.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetEditorText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetText( const String& text )
{
   (*API->CodeEditor->SetEditorText)( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

IsoString CodeEditor::EncodedText( const IsoString& encoding ) const
{
   size_type len = 0;
   (*API->CodeEditor->GetEditorEncodedText)( handle, 0, &len, encoding.c_str() );

   IsoString text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->CodeEditor->GetEditorEncodedText)( handle, text.c_str(), &len, encoding.c_str() ) == api_false )
         throw APIFunctionError( "GetEditorEncodedText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetEncodedText( const IsoString& text, const IsoString& encoding )
{
   if ( (*API->CodeEditor->SetEditorEncodedText)( handle, text.c_str(), encoding.c_str() ) == api_false )
      throw APIFunctionError( "SetEditorEncodedText" );
}

// ----------------------------------------------------------------------------

void CodeEditor::ClearText()
{
   (*API->CodeEditor->ClearEditorText)( handle );
}

// ----------------------------------------------------------------------------

bool CodeEditor::IsReadOnly() const
{
   return (*API->CodeEditor->GetEditorReadOnly)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetReadOnly( bool readOnly )
{
   (*API->CodeEditor->SetEditorReadOnly)( handle, readOnly );
}

// ----------------------------------------------------------------------------

bool CodeEditor::Save( const String& filePath, const IsoString& encoding )
{
   return (*API->CodeEditor->SaveEditorText)( handle, filePath.c_str(), encoding.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool CodeEditor::Load( const String& filePath, const IsoString& encoding )
{
   return (*API->CodeEditor->LoadEditorText)( handle, filePath.c_str(), encoding.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

int CodeEditor::NumberOfLines() const
{
   return (*API->CodeEditor->GetEditorLineCount)( handle );
}

// ----------------------------------------------------------------------------

int CodeEditor::NumberOfCharacters() const
{
   return (*API->CodeEditor->GetEditorCharacterCount)( handle );
}

// ----------------------------------------------------------------------------

Point CodeEditor::CursorPosition() const
{
   Point pos;
   (*API->CodeEditor->GetEditorCursorCoordinates)( handle, &pos.y, &pos.x );
   return pos;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetCursorPosition( int line, int column )
{
   (*API->CodeEditor->SetEditorCursorCoordinates)( handle, line, column );
}

// ----------------------------------------------------------------------------

bool CodeEditor::IsInsertMode() const
{
   return (*API->CodeEditor->GetEditorInsertMode)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetInsertMode( bool insert )
{
   (*API->CodeEditor->SetEditorInsertMode)( handle, insert );
}

// ----------------------------------------------------------------------------

bool CodeEditor::IsBlockSelectionMode() const
{
   return (*API->CodeEditor->GetEditorBlockSelectionMode)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetBlockSelectionMode( bool blockMode )
{
   (*API->CodeEditor->SetEditorBlockSelectionMode)( handle, blockMode );
}

// ----------------------------------------------------------------------------

bool CodeEditor::IsDynamicWordWrapMode() const
{
   return (*API->CodeEditor->GetEditorDynamicWordWrapMode)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetDynamicWordWrapMode( bool wrapMode )
{
   (*API->CodeEditor->SetEditorDynamicWordWrapMode)( handle, wrapMode );
}

// ----------------------------------------------------------------------------

int CodeEditor::UndoSteps() const
{
   return (*API->CodeEditor->GetEditorUndoSteps)( handle );
}

// ----------------------------------------------------------------------------

int CodeEditor::RedoSteps() const
{
   return (*API->CodeEditor->GetEditorRedoSteps)( handle );
}

// ----------------------------------------------------------------------------

bool CodeEditor::HasSelection() const
{
   return (*API->CodeEditor->GetEditorHasSelection)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

String CodeEditor::SelectedText() const
{
   size_type len = 0;
   (*API->CodeEditor->GetEditorSelectedText)( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->CodeEditor->GetEditorSelectedText)( handle, text.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetEditorSelectedText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

Rect CodeEditor::Selection() const
{
   Rect r;
   (*API->CodeEditor->GetEditorSelectionCoordinates)( handle, &r.y0, &r.x0, &r.y1, &r.x1 );
   return r;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetSelection( int fromLine, int fromCol, int toLine, int toCol )
{
   (*API->CodeEditor->SetEditorSelectionCoordinates)( handle, fromLine, fromCol, toLine, toCol );
}

// ----------------------------------------------------------------------------

void CodeEditor::InsertText( const String& text )
{
   (*API->CodeEditor->InsertEditorText)( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

void CodeEditor::Undo()
{
   (*API->CodeEditor->EditorUndo)( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::Redo()
{
   (*API->CodeEditor->EditorRedo)( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::Cut()
{
   (*API->CodeEditor->EditorCut)( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::Copy()
{
   (*API->CodeEditor->EditorCopy)( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::Paste()
{
   (*API->CodeEditor->EditorPaste)( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::Delete()
{
   (*API->CodeEditor->EditorDelete)( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::SelectAll()
{
   (*API->CodeEditor->EditorSelectAll)( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::Unselect()
{
   (*API->CodeEditor->EditorUnselect)( handle );
}

// ----------------------------------------------------------------------------

bool CodeEditor::GotoMatchedParenthesis()
{
   return (*API->CodeEditor->EditorGotoMatchedParenthesis)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

int CodeEditor::HighlightAllMatches( const String& toFind, FindModes mode )
{
   return (*API->CodeEditor->EditorHighlightAllMatches)( handle, toFind.c_str(), unsigned( mode ) );
}

// ----------------------------------------------------------------------------

void CodeEditor::ClearMatches()
{
   (*API->CodeEditor->EditorClearMatches)( handle );
}

// ----------------------------------------------------------------------------

bool CodeEditor::Find( const String& toFind, FindModes mode )
{
   return (*API->CodeEditor->EditorFind)( handle, toFind.c_str(), unsigned( mode ) ) != api_false;
}

// ----------------------------------------------------------------------------

bool CodeEditor::ReplaceSelection( const String& replaceWith )
{
   return (*API->CodeEditor->EditorReplace)( handle, replaceWith.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

int CodeEditor::ReplaceAll( const String& toFind, const String& replaceWith, FindModes mode )
{
   return (*API->CodeEditor->EditorReplaceAll)( handle, toFind.c_str(), replaceWith.c_str(), unsigned( mode ) );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<CodeEditor*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class CodeEditorEventDispatcher
{
public:

   static void TextUpdated( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onTextUpdated != nullptr )
         (receiver->*handlers->onTextUpdated)( *sender );
   }

   static void CursorPositionUpdated( control_handle hSender, control_handle hReceiver, int32 line, int32 col )
   {
      if ( handlers->onCursorPositionUpdated != nullptr )
         (receiver->*handlers->onCursorPositionUpdated)( *sender, line, col );
   }

   static void SelectionUpdated( control_handle hSender, control_handle hReceiver, int32 fromLine, int32 fromCol, int32 toLine, int32 toCol )
   {
      if ( handlers->onSelectionUpdated != nullptr )
         (receiver->*handlers->onSelectionUpdated)( *sender, fromLine, fromCol, toLine, toCol );
   }

   static void OverwriteModeUpdated( control_handle hSender, control_handle hReceiver, api_bool overwrite )
   {
      if ( handlers->onOverwriteModeUpdated != nullptr )
         (receiver->*handlers->onOverwriteModeUpdated)( *sender, overwrite != api_false );
   }

   static void SelectionModeUpdated( control_handle hSender, control_handle hReceiver, api_bool blockMode )
   {
      if ( handlers->onSelectionModeUpdated != nullptr )
         (receiver->*handlers->onSelectionModeUpdated)( *sender, blockMode != api_false );
   }

   static void DynamicWordWrapModeUpdated( control_handle hSender, control_handle hReceiver, api_bool wordWrap )
   {
      if ( handlers->onDynamicWordWrapModeUpdated != nullptr )
         (receiver->*handlers->onDynamicWordWrapModeUpdated)( *sender, wordWrap != api_false );
   }

}; // CodeEditorEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void CodeEditor::OnTextUpdated( editor_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->CodeEditor->SetEditorTextUpdatedEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? CodeEditorEventDispatcher::TextUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetEditorTextUpdatedEventRoutine" );
   m_handlers->onTextUpdated = handler;
}

void CodeEditor::OnCursorPositionUpdated( cursor_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->CodeEditor->SetEditorCursorPositionUpdatedEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? CodeEditorEventDispatcher::CursorPositionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetEditorCursorPositionUpdatedEventRoutine" );
   m_handlers->onCursorPositionUpdated = handler;
}

void CodeEditor::OnSelectionUpdated( selection_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->CodeEditor->SetEditorSelectionUpdatedEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? CodeEditorEventDispatcher::SelectionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetEditorSelectionUpdatedEventRoutine" );
   m_handlers->onSelectionUpdated = handler;
}

void CodeEditor::OnOverwriteModeUpdated( state_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->CodeEditor->SetEditorOverwriteModeUpdatedEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? CodeEditorEventDispatcher::OverwriteModeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetEditorOverwriteModeUpdatedEventRoutine" );
   m_handlers->onOverwriteModeUpdated = handler;
}

void CodeEditor::OnSelectionModeUpdated( state_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->CodeEditor->SetEditorSelectionModeUpdatedEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? CodeEditorEventDispatcher::SelectionModeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetEditorSelectionModeUpdatedEventRoutine" );
   m_handlers->onSelectionModeUpdated = handler;
}

void CodeEditor::OnDynamicWordWrapModeUpdated( state_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->CodeEditor->SetEditorDynamicWordWrapModeUpdatedEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? CodeEditorEventDispatcher::DynamicWordWrapModeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetEditorDynamicWordWrapModeUpdatedEventRoutine" );
   m_handlers->onDynamicWordWrapModeUpdated = handler;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/CodeEditor.cpp - Released 2015/11/26 15:59:45 UTC
