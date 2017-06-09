//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0837
// ----------------------------------------------------------------------------
// pcl/CodeEditor.h - Released 2017-06-09T08:12:42Z
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

#ifndef __PCL_CodeEditor_h
#define __PCL_CodeEditor_h

/// \file pcl/CodeEditor.h

#include <pcl/Defs.h>

#include <pcl/Flags.h>

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/AutoPointer.h>
#include <pcl/Control.h>

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::FindMode
 * \brief Flags to control text find and replacement operations.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>FindMode::RegExp</td>        <td>Search with a regular expression instead of a literal substring.</td></tr>
 * <tr><td>FindMode::Backward</td>      <td>Search toward the beginning of the document.</td></tr>
 * <tr><td>FindMode::CaseSensitive</td> <td>Perform a case-sensitive search.</td></tr>
 * <tr><td>FindMode::WholeWords</td>    <td>Search for whole words only.</td></tr>
 * <tr><td>FindMode::SelectionOnly</td> <td>Search and replace within the current text selection only</td></tr>
 * <tr><td>FindMode::Incremental</td>   <td>Perform an incremental text find operation. The search begins after a previously matched text block.</td></tr>
 * <tr><td>FindMode::Default</td>       <td>The default text find mode, equal to FindMode::CaseSensitive|FindMode::WholeWords.</td></tr>
 * </table>
 */
namespace FindMode
{
   enum mask_type
   {
      RegExp         = 0x00000001,  // Search with a regular expression
      Backward       = 0x00000010,  // Search toward the beginning of text
      CaseSensitive  = 0x00000020,  // Case-sensitive search
      WholeWords     = 0x00000040,  // Search for whole words
      SelectionOnly  = 0x00000100,  // Replace within the current selection
      Incremental    = 0x00000200,  // Perform an incremental find operation
      Default        = CaseSensitive|WholeWords
   };
}

/*!
 * A combination of text find and replacement mode flags.
 */
typedef Flags<FindMode::mask_type>  FindModes;

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

// ----------------------------------------------------------------------------

/*!
 * \class CodeEditor
 * \brief Client-side interface to a PixInsight %CodeEditor control.
 *
 * ### TODO: Write a detailed description for %CodeEditor.
 */
class PCL_CLASS CodeEditor : public Control
{
public:

   /*!
    * Constructs a %CodeEditor control.
    *
    * \param parent     The parent control of this object. The default value is
    *                   a null control.
    */
   CodeEditor( Control& parent = Control::Null() );

   /*!
    * Destroys a %CodeEditor control.
    */
   virtual ~CodeEditor()
   {
   }

   /*!
    * Returns a reference to a <em>line numbers control</em> for this code
    * editor.
    *
    * A line numbers control provides a visual index of the text lines
    * currently displayed by a code editor. Typically, a code editor and its
    * line numbers control are packed side by side in a HorizontalSizer.
    */
   Control& LineNumbersControl();

   /*!
    * Returns the absolute file path associated with this code editor, or an
    * empty string if this object has not been associated with a file.
    */
   String FilePath() const;

   /*!
    * Sets the file path associated with this code editor.
    *
    * If the specified \a path includes a file name suffix (also known as file
    * name \e extension), the control will use it to identify a programming
    * language to perform automatic syntax highlighting of the text in the
    * editor. Currently the following file suffixes and source code languages
    * are supported:
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>.js .jsh</td>                                     <td>JavaScript source files</td></tr>
    * <tr><td>.scp</td>                                         <td>Command-line script files</td></tr>
    * <tr><td>.cpp .h .cxx .hpp .hxx .c .cc</td>                <td>C/C++ source files</td></tr>
    * <tr><td>.py</td>                                          <td>Python source files</td></tr>
    * <tr><td>.xml .xhtml .html .xosm .xpsm (among others)</td> <td>XML source files</td></tr>
    * <tr><td>.pidoc</td>                                       <td>PixInsight documentation source files</td></tr>
    * <tr><td>.pm .pmath</td>                                   <td>PixelMath source files</td></tr>
    * </table>
    *
    * Unrecognized (or absent) file suffixes effectively disable the automatic
    * syntax highlighting feature. The default value of this parameter is an
    * empty string.
    */
   void SetFilePath( const String& path );

   /*!
    * Returns the current text in this code editor in UTF-16 format.
    */
   String Text() const;

   /*!
    * Sets the current text in this code editor in UTF-16 format.
    */
   void SetText( const String& text );

   /*!
    * Returns an encoded 8-bit representation of the text in this code editor.
    *
    * \param encoding   The desired text encoding. Currently only the "UTF-8"
    *                   and "ISO-8859-1" encodings are supported. The default
    *                   value is "UTF-8".
    */
   IsoString EncodedText( const IsoString& encoding = "UTF-8" ) const;

   /*!
    * Returns the text in this code editor encoded in UTF-8 format.
    *
    * This is a convenience function, equivalent to:
    *
    * \code EncodedText( "UTF-8" ); \endcode
    */
   IsoString TextUTF8() const
   {
      return EncodedText( "UTF-8" );
   }

   /*!
    * Returns the text in this code editor encoded in ISO 8859-1 format.
    *
    * This is a convenience function, equivalent to:
    *
    * \code EncodedText( "ISO-8859-1" ); \endcode
    */
   IsoString TextISO88591() const
   {
      return EncodedText( "ISO-8859-1" );
   }

   /*!
    * Sets the current text in this code editor, encoded in an 8-bit format.
    *
    * \param text       The encoded 8-bit text.
    *
    * \param encoding   The text encoding. Currently only the "UTF-8" and
    *                   "ISO-8859-1" encodings are supported. The default value
    *                   is "UTF-8".
    */
   void SetEncodedText( const IsoString& text, const IsoString& encoding = "UTF-8" );

   /*!
    * Sets the current text in this code editor, encoded in UTF-8 format.
    *
    * This is a convenience function, equivalent to:
    *
    * \code SetEncodedText( text, "UTF-8" ); \endcode
    */
   void SetTextUTF8( const IsoString& text )
   {
      SetEncodedText( text, "UTF-8" );
   }

   /*!
    * Sets the current text in this code editor, encoded in ISO 8859-1 format.
    *
    * This is a convenience function, equivalent to:
    *
    * \code SetEncodedText( text, "ISO-8859-1" ); \endcode
    */
   void SetTextISO88591( const IsoString& text )
   {
      SetEncodedText( text, "ISO-8859-1" );
   }

   /*!
    * Clears all the text in this code editor. Also clears all undo/redo
    * buffers, so this action is irreversible.
    */
   void ClearText();

   /*!
    * Returns true iff this code editor is in <em>read-only mode</em>. A
    * read-only editor does not allow any command that can modify its text
    * contents as a result of a direct user interaction. The text can only be
    * changed programmatically for a read-only code editor.
    */
   bool IsReadOnly() const;

   /*!
    * Enables the read-only mode for this code editor. A read-only editor does
    * not allow any command that can modify its text contents as a result of a
    * direct user interaction. The text can only be changed programmatically
    * for a read-only code editor.
    *
    * \param readOnly   Whether to enable or disable the read-only mode for
    *                   this code editor.
    */
   void SetReadOnly( bool readOnly = true );

   /*!
    * Writes the text in this code editor to the specified file, encoded in an
    * 8-bit format.
    *
    * \param filePath   Path to the destination file.
    *
    * \param encoding   The desired text encoding. Currently only the "UTF-8"
    *                   and "ISO-8859-1" encodings are supported. The default
    *                   value is "UTF-8".
    *
    * Returns true if the file was successfully written; false in the event of
    * error.
    *
    * \warning Upon successful completion, this function will overwrite an
    * existing file at the specified path, whose previous contents will be
    * lost.
    */
   bool Save( const String& filePath, const IsoString& encoding = "UTF-8" );

   /*!
    * Writes the text in this code editor to the specified file, encoded in
    * UTF-8 format.
    *
    * This is a convenience function, equivalent to:
    *
    * \code Save( filePath, "UTF-8" ); \endcode
    */
   bool SaveUTF8( const String& filePath )
   {
      return Save( filePath, "UTF-8" );
   }

   /*!
    * Writes the text in this code editor to the specified file, encoded in
    * ISO 8859-1 format.
    *
    * This is a convenience function, equivalent to:
    *
    * \code Save( filePath, "ISO-8859-1" ); \endcode
    */
   bool SaveISO88591( const String& filePath )
   {
      return Save( filePath, "ISO-8859-1" );
   }

   /*!
    * Reads an encoded 8-bit text file and loads it in this code editor.
    *
    * \param filePath   Path to the source file.
    *
    * \param encoding   The text encoding. Currently only the "UTF-8" and
    *                   "ISO-8859-1" encodings are supported. The default
    *                   value is "UTF-8".
    *
    * Returns true if the file was successfully read; false in the event of
    * error.
    */
   bool Load( const String& filePath, const IsoString& encoding = "UTF-8" );

   /*!
    * Reads a text file encoded in UTF-8 format, and loads it in this code
    * editor.
    *
    * This is a convenience function, equivalent to:
    *
    * \code Load( filePath, "UTF-8" ); \endcode
    */
   bool LoadUTF8( const String& filePath )
   {
      return Load( filePath, "UTF-8" );
   }

   /*!
    * Reads a text file encoded in ISO 8859-1 format, and loads it in this code
    * editor.
    *
    * This is a convenience function, equivalent to:
    *
    * \code Load( filePath, "ISO-8859-1" ); \endcode
    */
   bool LoadISO88591( const String& filePath )
   {
      return Load( filePath, "ISO-8859-1" );
   }

   /*!
    * Returns the number of text lines (including empty lines) in this code
    * editor.
    */
   int NumberOfLines() const;

   /*!
    * Returns the total number of characters in the current text of this code
    * editor.
    */
   int NumberOfCharacters() const;

   /*!
    * Returns the current cursor position in visual coordinates. The returned
    * object stores the current line and column of the cursor in its Point::y
    * and Point::x public data members, respectively.
    *
    * Line and column coordinates are counted from zero. Line numbers range
    * from zero to the number of existing text lines minus one. There is no
    * specific limit for the number of text characters in a text line.
    */
   Point CursorPosition() const;

   /*!
    * Returns the line number of the current cursor position.
    */
   int CursorLine() const
   {
      return CursorPosition().y;
   }

   /*!
    * Returns the column number of the current cursor position.
    */
   int CursorColumn() const
   {
      return CursorPosition().x;
   }

   /*!
    * Sets the current cursor position in this code editor to the specified
    * \a line and \a column. See CursorPosition() for more information on
    * visual text coordinates.
    */
   void SetCursorPosition( int line, int column );

   /*!
    * Sets the current cursor position in this code editor through the
    * coordinates of a Point object. The new cursor line will be set to
    * \a pos.y and the new cursor column to \a pos.x, respectively.
    */
   void SetCursorPosition( const Point& pos )
   {
      SetCursorPosition( pos.y, pos.x );
   }

   /*!
    * Returns true iff this code editor is in <em>insert text mode</em>. Returns
    * false if the editor is in <em>replace text mode</em>.
    *
    * In insert mode, newly generated text by direct user interaction is
    * inserted at the current cursor location. In replace mode, newly generated
    * text replaces existing characters starting at the current cursor
    * location. The insert text mode is always enabled by default.
    */
   bool IsInsertMode() const;

   /*!
    * Enables the <em>insert text mode</em> for this code editor.
    *
    * \param insert  Whether to enable the insert text mode (if true) or the
    *                replace text mode (if false). The default value is true.
    */
   void SetInsertMode( bool insert = true );

   /*!
    * Enables the <em>replace text mode</em> for this code editor.
    *
    * \param replace Whether to enable the replace text mode (if true) or the
    *                insert text mode (if false). The default value is true.
    */
   void SetReplaceMode( bool replace = true )
   {
      SetInsertMode( !replace );
   }

   /*!
    * Returns true iff this code editor is in <em>block selection mode</em>;
    * false if it is in <em>line selection mode</em>.
    *
    * In block selection mode, the user can select rectangular areas of text
    * across multiple text lines. In <em>line selection mode</em>, text
    * selections can only include entire text lines except for the first and
    * last lines in a selection. The line selection mode is always enabled by
    * default.
    */
   bool IsBlockSelectionMode() const;

   /*!
    * Enables the block selection mode for this code editor.
    *
    * \param blockMode  Whether to enable the block selection mode (if true) or
    *                   the line selection mode (if false). The default value
    *                   is true.
    */
   void SetBlockSelectionMode( bool blockMode = true );

   /*!
    * Enables the line selection mode for this code editor.
    *
    * \param lineMode   Whether to enable the line selection mode (if true) or
    *                   the block selection mode (if false). The default value
    *                   is true.
    */
   void SetLineSelectionMode( bool lineMode = true )
   {
      SetBlockSelectionMode( !lineMode );
   }

   /*!
    * Returns true iff this code editor is in <em>dynamic word wrap mode</em>.
    *
    * In dynamic word wrap mode, long text lines extending beyond the visible
    * width of the editor's viewport are automatically truncated and continued
    * on successive editor lines, so that the user can always see the entire
    * contents of all text lines, and a horizontal scroll bar is never shown.
    *
    * When the dynamic word wrap mode is disabled, long text lines are never
    * truncated and the editor shows a horizontal scroll bar when necessary.
    * The dynamic word wrap mode is always disabled by default.
    */
   bool IsDynamicWordWrapMode() const;

   /*!
    * Enables the dynamic word wrap mode for this code editor.
    *
    * \param wrapMode   Whether to enable the dynamic word wrap mode.
    */
   void SetDynamicWordWrapMode( bool wrapMode = true );

   /*!
    * Returns the number of \e undo steps available for this code editor.
    */
   int UndoSteps() const;

   /*!
    * Returns the number of \e redo steps available for this code editor.
    */
   int RedoSteps() const;

   /*!
    * Returns true iff this code editor has a text selection defined.
    */
   bool HasSelection() const;

   /*!
    * Returns the visual coordinates of the current selection in this code
    * editor. The selection coordinates are stored in the returned rectangle as
    * follows:
    *
    * \li Rect::y0 - Starting line of the current text selection.
    * \li Rect::x0 - Starting column of the current text selection.
    * \li Rect::y1 - Ending line of the current text selection.
    * \li Rect::x1 - Ending column of the current text selection.
    */
   Rect Selection() const;

   /*!
    * Sets a new text selection defined by its visual coordinates.
    *
    * \param fromLine   Starting line of the new text selection.
    * \param fromCol    Starting column of the new text selection.
    * \param toLine     Ending line of the new text selection.
    * \param toCol      Ending column of the new text selection.
    */
   void SetSelection( int fromLine, int fromCol, int toLine, int toCol );

   /*!
    * Sets a new text selection defined by its visual coordinates.
    *
    * This is a convenience function, equivalent to:
    *
    * \code SetSelection( r.y0, r.x0, r.y1, r.x1 ); \endcode
    */
   void SetSelection( const Rect& r )
   {
      SetSelection( r.y0, r.x0, r.y1, r.x1 );
   }

   /*!
    * Returns the currently selected text in this code editor, or an empty
    * string if this editor has no selection currently defined.
    */
   String SelectedText() const;

   /*!
    * Inserts the specified \a text at the current cursor location.
    *
    * If there is a selection defined, the selected text is replaced with the
    * inserted \a text.
    */
   void InsertText( const String& text );

   /*!
    * Undoes the last editor action. If no action can be undone, this function
    * has no effect.
    */
   void Undo();

   /*!
    * Redoes the last editor action. If no action can be redone, this function
    * has no effect.
    */
   void Redo();

   /*!
    * Copies the current text selection to the clipboard and removes the
    * selected text. If no selection is defined, this function has no effect.
    */
   void Cut();

   /*!
    * Copies the current text selection to the clipboard. If no selection is
    * defined, this function has no effect.
    */
   void Copy();

   /*!
    * Inserts the contents of the clipboard at the current cursor location. If
    * the clipboard is empty, or if its contents cannot be converted to plain
    * text, this function has no effect.
    */
   void Paste();

   /*!
    * Removes the currently selected text. If there is no text selection
    * defined, this function has no effect.
    */
   void Delete();

   /*!
    * Selects all the text in this code editor.
    */
   void SelectAll();

   /*!
    * Clears the current text selection, if any. This function simply unselects
    * the text; it does not remove the selected text.
    */
   void Unselect();

   /*!
    * If the cursor points to a bracket or parenthesis character, look for a
    * matching pair and move the cursor to the corresponding location.
    *
    * Returns true iff a matching pair could be found. If there is no bracket or
    * parenthesis at the current cursor location, or if a matched pair couldn't
    * be found, this function returns false and the cursor is not moved.
    */
   bool GotoMatchedParenthesis();

   /*!
    * Highlights all occurrences of a literal text string or a regular
    * expression.
    *
    * \param toFind  The text or regular expression to search for, depending on
    *                the specified \a mode flags.
    *
    * \param mode    An OR'ed combination of flags to control the text find
    *                operation. Valid flags are enumerated in the pcl::FindMode
    *                namespace.
    *
    * The operation takes place on the whole text document, ignoring the
    * FindMode::Backward and FindMode::SelectionOnly flags. Returns the number
    * of matches found.
    */
   int HighlightAllMatches( const String& toFind, FindModes mode );

   /*!
    * Clears all text highlightings. This function does not remove the
    * highlighted text; it simply clears the visual highlight indications.
    */
   void ClearMatches();

   /*!
    * Finds an occurrence of a literal text string or a regular expression.
    *
    * \param toFind  The text or regular expression to search for, depending on
    *                the specified \a mode flags.
    *
    * \param mode    An OR'ed combination of flags to control the text find
    *                operation. Valid flags are enumerated in the pcl::FindMode
    *                namespace.
    *
    * Returns true iff a match was found.
    */
   bool Find( const String& toFind, FindModes mode );

   /*!
    * Replaces the current selection with the specified text.
    *
    * \param replaceWith   The text that will replace the current selection.
    *                      Can be an empty string (to delete the current
    *                      selection).
    *
    * If there is some text selected in this editor, this function returns
    * true after performing the replacement operation. If there is no text
    * currently selected, this function is ignored and false is returned.
    */
   bool ReplaceSelection( const String& replaceWith );

   /*!
    * Replaces all occurrences of a literal text string or a regular
    * expression with the specified text.
    *
    * \param toFind  The text or regular expression to search for, depending on
    *                the specified \a mode flags.
    *
    * \param replaceWith   The text that will replace all matches found. Can be
    *                an empty string (to delete all matched occurrences).
    *
    * \param mode    An OR'ed combination of flags to control the text find and
    *                replace operation. Valid flags are enumerated in the
    *                pcl::FindMode namespace.
    *
    * Returns the number of matches found and replaced.
    *
    */
   int ReplaceAll( const String& toFind, const String& replaceWith, FindModes mode );

   /*!
    * \defgroup code_editor_event_handlers CodeEditor Event Handlers
    */

   /*!
    * Defines the prototype of an <em>editor event handler</em>.
    *
    * An editor event is generated when a %CodeEditor instance changes its
    * state or contents due to direct user interaction.
    *
    * \param sender  The control that sends an editor event.
    *
    * \ingroup code_editor_event_handlers
    */
   typedef void (Control::*editor_event_handler)( CodeEditor& sender );

   /*!
    * Defines the prototype of a <em>cursor event handler</em>.
    *
    * A cursor event is generated when the cursor position changes in a
    * %CodeEditor instance due to direct user interaction.
    *
    * \param sender  The control that sends a cursor event.
    *
    * \param line    Line number of the new cursor position (>= 0).
    *
    * \param col     Column number of the new cursor position (>= 0).
    *
    * \ingroup code_editor_event_handlers
    */
   typedef void (Control::*cursor_event_handler)( CodeEditor& sender, int line, int col );

   /*!
    * Defines the prototype of a <em>selection event handler</em>.
    *
    * A selection event is generated when the text selection changes in a
    * %CodeEditor instance due to direct user interaction.
    *
    * \param sender  The control that sends a selection event.
    *
    * \param fromLine   Line number of the starting position of the new text
    *                selection.
    *
    * \param fromCol Column number of the starting position of the new text
    *                selection.
    *
    * \param toLine  Line number of the ending position of the new text
    *                selection.
    *
    * \param toCol   Column number of the ending position of the new text
    *                selection.
    *
    * \ingroup code_editor_event_handlers
    */
   typedef void (Control::*selection_event_handler)( CodeEditor& sender, int fromLine, int fromCol, int toLine, int toCol );

   /*!
    * Defines the prototype of a <em>state event handler</em>.
    *
    * A state event is generated when a Boolean property changes in a
    * %CodeEditor instance due to direct user interaction.
    *
    * \param sender  The control that sends a cursor event.
    *
    * \param state   New property state.
    *
    * \ingroup code_editor_event_handlers
    */
   typedef void (Control::*state_event_handler)( CodeEditor& sender, bool state );

   /*!
    * Sets the handler for <em>text updated</em> events generated by this code
    * editor. A text updated event is generated each time the text changes in
    * this editor as a result of a direct user interaction.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive text updated events from
    *                   this code editor.
    *
    * \ingroup code_editor_event_handlers
    */
   void OnTextUpdated( editor_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>cursor position updated</em> events generated by
    * this code editor. A cursor position updated event is generated each time
    * the text cursor (also known as \e caret) moves in this editor as a result
    * of a direct user interaction.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive cursor position updated
    *                   events from this code editor.
    *
    * \ingroup code_editor_event_handlers
    */
   void OnCursorPositionUpdated( cursor_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>selection updated</em> events generated by this
    * code editor. A selection updated event is generated each time the text
    * selection is changed in this editor as a result of a direct user
    * interaction.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive selection updated events
    *                   from this code editor.
    *
    * \ingroup code_editor_event_handlers
    */
   void OnSelectionUpdated( selection_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>overwrite mode updated</em> events generated by
    * this code editor. An overwrite mode updated event is generated each time
    * the overwrite mode is enabled or disabled in this editor (i.e., from
    * overwrite mode to replace mode or vice-versa) as a result of a direct
    * user interaction.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive overwrite mode updated
    *                   events from this code editor.
    *
    * \ingroup code_editor_event_handlers
    */
   void OnOverwriteModeUpdated( state_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>selection mode updated</em> events generated by
    * this code editor. A selection mode updated event is generated each time
    * the selection mode is changed in this editor (i.e., from line selection
    * mode to block selection mode or vice-versa) as a result of a direct user
    * interaction.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive selection mode updated
    *                   events from this code editor.
    *
    * \ingroup code_editor_event_handlers
    */
   void OnSelectionModeUpdated( state_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>dynamic word wrap mode updated</em> events
    * generated by this code editor. A dynamic word wrap mode updated event is
    * generated each time the dynamic word wrap mode is enabled or disabled in
    * this editor as a result of a direct user interaction.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive dynamic word wrap mode
    *                   updated events from this code editor.
    *
    * \ingroup code_editor_event_handlers
    */
   void OnDynamicWordWrapModeUpdated( state_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      editor_event_handler    onTextUpdated                = nullptr;
      cursor_event_handler    onCursorPositionUpdated      = nullptr;
      selection_event_handler onSelectionUpdated           = nullptr;
      state_event_handler     onOverwriteModeUpdated       = nullptr;
      state_event_handler     onSelectionModeUpdated       = nullptr;
      state_event_handler     onDynamicWordWrapModeUpdated = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;
   Control                    m_lineNumbers;

   friend class CodeEditorEventDispatcher;
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_CodeEditor_h

// ----------------------------------------------------------------------------
// EOF pcl/CodeEditor.h - Released 2017-06-09T08:12:42Z
