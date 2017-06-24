//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/Console.h - Released 2017-06-21T11:36:33Z
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

#ifndef __PCL_Console_h
#define __PCL_Console_h

/// \file pcl/Console.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/StringList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS View;

/*!
 * \class  Console
 * \brief  A high-level interface to a PixInsight processing console.
 *
 * The PixInsight platform provides three different user interfaces implemented
 * in the PixInsight core application: a graphical user interface (GUI), a
 * scripting interface (the PixInsight JavaScript runtime), and a command-line
 * interface. The %Console class is a high-level abstraction that represents
 * the %Process %Console window of the PixInsight core application, which
 * implements the entire command-line and console text input/output
 * functionality in PixInsight.
 *
 * A module can instantiate %Console to perform direct text input/output
 * operations. %Console objects are often used to display information about
 * ongoing processes, to let the user abort a process, or to put a process in a
 * wait state to perform a modal user interface operation (e.g., showing a
 * message box) in a thread-safe way.
 *
 * From the module developer perspective, each module has an associated
 * console. Such associations between modules and consoles are managed
 * automatically by internal core and PCL routines: module developers just need
 * to instantiate the %Console class and use its member functions.
 *
 * For example, writing a message to the console can be as simple as:
 *
 * \code
 * Console().WriteLn( "Hello, PixInsight" );
 * \endcode
 *
 * Stream-style console insertion and extraction operators are also available.
 * For example, this is equivalent to the above code:
 *
 * \code
 * Console() << "Hello, PixInsight" << '\n';
 * \endcode
 *
 * When you need to output formatted data, you can use the String::Format()
 * family of functions. These functions are similar to the standard printf() C
 * runtime function:
 *
 * \code
 * Console console;
 * console.WriteLn( String().Format( "Radius = %.6f, Angle = %+.2f&deg;", r, Deg( a ) ) );
 * \endcode
 *
 * PixInsight consoles can understand and reproduce the full set of ISO-8859-1
 * HTML entities. You see an example on the code fragment above: the \c "&deg;"
 * entity prints a <em>degree symbol</em> on the console.
 *
 * PixInsight consoles can also interpret a number of \e tags, similar to HTML,
 * to perform special actions and formatting operations. For example:
 *
 * \code
 * Console() << "<end><cbr>This is <b>bold</b> and this is <i>italics</i><br>";
 * \endcode
 *
 * In this example, the "<end>" tag moves the cursor after the last character
 * in the console, then the "<cbr>" tag (<em>conditional break</em>) generates
 * a new line only if the current line is not empty. This ensures that
 * subsequent text output will start on an empty line at the end of the current
 * console contents. The "<b>" and "</b>" tags write text in a bold type face,
 * and the "<i>" and "</i>" pair write text in italics. This is just a minimal
 * fraction of the rich set of tags understood by PixInsight consoles.
 *
 * As we describe in detail below, PixInsight consoles support standard ANSI
 * escape codes, and can also be used to render inline HTML contents.
 *
 * It must be pointed out that the entire text output functionality of %Console
 * is also available for TextBox controls. A module can instantiate a %TextBox
 * control to provide rich text-based output as a process log, the output of an
 * external process, or an information panel based on HTML tables, just to name
 * a few possibilities implemented frequently.
 *
 * <h2>%Console Tags</h2>
 *
 * Here is a comprehensive list of supported console tags at the time of
 * publishing this documentation. New functionality implemented through console
 * tags will be documented here as new versions are released.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 *
 * <tr><td>\<b\>\</b\></td> <td>Enables/disables bold text.</td></tr>
 * <tr><td>\<i\>\</i\></td> <td>Enables/disables italics text.</td></tr>
 * <tr><td>\<u\>\</u\></td> <td>Enables/disables underlined text.</td></tr>
 * <tr><td>\<o\>\</o\></td> <td>Enables/disables overlined text.</td></tr>
 * <tr><td>\<sub\>\</sub\></td> <td>Enables/disables subscript text.</td></tr>
 * <tr><td>\<sup\>\</sup\></td> <td>Enables/disables superscript text.</td></tr>
 *
 * <tr><td>\<br\></td>      <td>Line break.</td></tr>
 * <tr><td>\<br/\></td>     <td>Line break, XML compliant (same as \<br\>).</td></tr>
 * <tr><td>\<cbr\></td>     <td>Conditional line break: sends a line break if the cursor is not at the beginning of an empty line.</td></tr>
 * <tr><td>\<cbr/\></td>    <td>Conditional line break, XML compliant (same as \<cbr\>).</td></tr>
 *
 * <tr><td>\<bsp\></td>     <td>Backspace: deletes the previous character and moves the cursor one step left, only if it is not at the beginning of a line.</td></tr>
 * <tr><td>\<end\></td>     <td>Moves the cursor to the end of the console.</td></tr>
 * <tr><td>\<beg\></td>     <td>Moves the cursor to the beginning of the console.</td></tr>
 * <tr><td>\<bwd\></td>     <td>Moves the cursor to the previous character, only if it is not at the beginning of a line.</td></tr>
 * <tr><td>\<fwd\></td>     <td>Moves the cursor to the next character, only if it is not at the end of a line.</td></tr>
 * <tr><td>\<eol\></td>     <td>Moves the cursor to the end of the current line.</td></tr>
 * <tr><td>\<bol\></td>     <td>Moves the cursor to the beginning of the current line.</td></tr>
 * <tr><td>\<up\></td>      <td>Moves the cursor to the previous line, if it is not already at the first line.</td></tr>
 * <tr><td>\<down\></td>    <td>Moves the cursor to the next line, if it is not already at the last line.</td></tr>
 *
 * <tr><td>\<clr\></td>     <td>Clears the console. Avoid issuing this tag arbitrarily - please see the documentation for Console::Clear() for more information.</td></tr>
 * <tr><td>\<clreol\></td>  <td>Deletes the text from the current cursor position (included) to the end of the current line.</td></tr>
 * <tr><td>\<clrbol\></td>  <td>Deletes the text from the current cursor position (included) to the beginning of the current line.</td></tr>
 * <tr><td>\<clrend\></td>  <td>Deletes the text from the current cursor position (included) to the end of the console.</td></tr>
 * <tr><td>\<clrbeg\></td>  <td>Deletes the text from the current cursor position (included) to the beginning of the console. Avoid issuing this tag arbitrarily - please see the documentation for Console::Clear() for more information.</td></tr>
 *
 * <tr><td>\<nowrap\>\</nowrap\></td>         <td>Disables/enables word wrapping of text lines.</td></tr>
 * <tr><td>\<notags\>\</notags\></td>         <td>Disables/enables tag interpretation, except the \</notags\> tag, which is always interpreted.</td></tr>
 * <tr><td>\<noentities\>\</noentities\></td> <td>Disables/enables interpretation of ISO-8859-1 entities.</td></tr>
 * <tr><td>\<raw\>\</raw\></td>               <td>Disables/enables interpretation of tags, except the \</raw\> tag, and ISO-8859-1 entities. \<raw\> is equivalent to (but faster than) \<noentities\>\<notags\>.</td></tr>
 *
 * <tr><td>\<monospace\></td>    <td>Selects a platform-dependent, monospaced font.</td></tr>
 * <tr><td>\<sans\></td>         <td>Selects a platform-dependent, sans-serif font.</td></tr>
 * <tr><td>\<serif\></td>        <td>Selects a platform-dependent, serif font.</td></tr>
 * <tr><td>\<courier\></td>      <td>Selects the Courier font, or the nearest available monspaced font, depending on the platform.</td></tr>
 * <tr><td>\<helvetica\></td>    <td>Selects the Helvetica font, or the nearest available sans-serif font, depending on the platform.</td></tr>
 * <tr><td>\<times\></td>        <td>Selects the Times font, or the nearest available serif font, depending on the platform.</td></tr>
 * <tr><td>\<default-font\></td> <td>Selects the default console font (usually a monospaced font; but user-selectable through preferences).</td></tr>
 * <tr><td>\<reset-font\></td>   <td>Resets all text settings and styles to default values.</td></tr>
 *
 * <tr><td>\<show\></td>    <td>If this console corresponds to the processing console window, shows it if it's currently hidden and docked in the PixInsight core application window; otherwise this tag is ignored. Equivalent to calling the Console::Show() member function.</td></tr>
 * <tr><td>\<hide\></td>    <td>If this console corresponds to the processing console window, hides it if it's currently visible and docked in the PixInsight core application window; otherwise this tag is ignored. Equivalent to calling the Console::Hide() member function.</td></tr>
 * <tr><td>\<flush\></td>   <td>Causes any pending data to be written immediately to the console. If there is no unwritten data for this console, this tag has no effect. Equivalent to calling the Console::Flush() member function.</td></tr>
 *
 * <tr><td>\<html\>\</html\></td> <td>Enables/disables HTML mode. In HTML mode, the console interprets and renders a comprehensive set of HTML 4 tags, including full support of tables, as well as a significant part of Level 2 CSS (Cascading Style Sheets) directives. In HTML mode, PixInsight console tags are either ignored or interpreted with their corresponding meaning in HTML 4.</td></tr>
 *
 * </table>
 *
 * In addition, the \\n, \\r and \\b escape characters can be used in place of
 * the \<br\>, \<bol\> and \<bsp\> tags, respectively.
 *
 * <h2>Character Entities</h2>
 *
 * PixInsight consoles fully recognize and interpret the whole set of
 * HTML 4 character entities, including all ISO 8859-1 characters and all
 * character entities for symbols, mathematical symbols, Greek letters,
 * markup-significant and internationalization characters.
 *
 * Character entities are of the form:
 *
 * <tt>\<entity-name\>;</tt>
 *
 * where \<entity-name\> is one of the references included in the
 * following W3C Recommendation document:
 *
 * http://www.w3.org/TR/REC-html40/sgml/entities.html
 *
 * <h2>HTML Mode</h2>
 *
 * The \<html\> tag allows you to put a PixInsight console in <em>HTML
 * mode</em>. In this mode you can embed HTML 4 contents directly. For example,
 * this code:
 *
 * \code
 * Console console;
 * console.Write( "<html>"
 *                "<table border="1" cellpadding="4" cellspacing="0">"
 *                "<tr><td>If this is foo</td>  <td>and this is bar,</td></tr>"
 *                "<tr><td>then</td>            <td>this can only be foo bar.</td></tr>"
 *                "</table>"
 *                "</html>" ); \endcode
 *
 * generates an HTML table with two rows and two columns. PixInsight consoles
 * support most of the HTML 4 specification, including a subset of CSS2. For
 * detaled information, this document describes HTML support in Qt, which is
 * the underlying implementation in current versions of PixInsight:
 *
 * http://qt-project.org/doc/qt-4.8/richtext-html-subset.html
 *
 * In HTML mode, that is, within a pair of \<html\>\</html\> tags, no
 * PixInsight console tags or ANSI escape codes are recognized or interpreted.
 *
 * <h2>ANSI Escape Codes</h2>
 *
 * Since version 1.8.1 of the PixInsight Core application, PixInsight consoles
 * support a large set of ANSI CSI (Control Sequence Introducer) codes. For a
 * detailed description of ANSI escape codes, refer to the following documents:
 *
 * http://en.wikipedia.org/wiki/ANSI_escape_code
 * http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
 *
 * The following table describes the supported CSI codes in the current version
 * of PixInsight. ESC represents the escape control character, whose ASCII code
 * is 27<sub>10</sub> = 1B<sub>16</sub>. The CSI sequence is ESC followed by
 * the left square bracket character, represented as the ESC[ sequence. See the
 * examples given at the end of the table.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 *
 * <tr><td><tt>ESC[ n A</tt></td>    <td>%Cursor up n Times (default n=1) (CUU)</td></tr>
 * <tr><td><tt>ESC[ n B</tt></td>    <td>%Cursor down n Times (default n=1) (CUD)</td></tr>
 * <tr><td><tt>ESC[ n C</tt></td>    <td>%Cursor forward n Times (default n=1) (CUF)</td></tr>
 * <tr><td><tt>ESC[ n D</tt></td>    <td>%Cursor backward n Times (default n=1) (CUB)</td></tr>
 * <tr><td><tt>ESC[ n E</tt></td>    <td>%Cursor to next line n Times (default n=1) (CNL)</td></tr>
 * <tr><td><tt>ESC[ n F</tt></td>    <td>%Cursor to preceding line n Times (default n=1) (CPL)</td></tr>
 * <tr><td><tt>ESC[ n G</tt></td>    <td>%Cursor to character absolute column n (default n=1) (CHA)</td></tr>
 * <tr><td><tt>ESC[ n;m H</tt></td>  <td>%Cursor position to row n and column m (default n=m=1) (CUP)</td></tr>
 * <tr><td><tt>ESC[ n;m f</tt></td>  <td>Horizontal & Vertical Position (HVP). Same as H.</td></tr>
 *
 * <tr><td><tt>ESC[ n J</tt></td>    <td>Erase in Display (ED)<br>
 *       <table border="1" cellpadding="4" cellspacing="0" width="100%">
 *       <tr><td><tt>n = 0</tt></td> <td>Erase below up to the end of text. This is the default if no n is specified.</td></tr>
 *       <tr><td><tt>n = 1</tt></td> <td>Erase above up to the start of text.</td></tr>
 *       <tr><td><tt>n = 2</tt></td> <td>Erase all (entire console).</td></tr>
 *       </table></td></tr>
 *
 * <tr><td><tt>ESC[ n K</tt></td>    <td>Erase in Line (EL)<br>
 *       <table border="1" cellpadding="4" cellspacing="0" width="100%">
 *       <tr><td><tt>n = 0</tt></td> <td>Erase to right up to the end of line. This is the default if no n is specified.</td></tr>
 *       <tr><td><tt>n = 1</tt></td> <td>Erase to left up to the start of line.</td></tr>
 *       <tr><td><tt>n = 2</tt></td> <td>Erase the whole line.</td></tr>
 *       </table></td></tr>
 *
 * <tr><td><tt>ESC[ n L</tt></td>    <td>Insert n line(s) (default n=1) (IL)</td></tr>
 * <tr><td><tt>ESC[ n M</tt></td>    <td>Delete n line(s) (default n=1) (DL)</td></tr>
 * <tr><td><tt>ESC[ n P</tt></td>    <td>Delete n character(s) (default n=1) (DCH)</td></tr>
 *
 * <tr><td><tt>ESC[ ... m</tt></td>  <td>Set graphics rendition (SGR)<br>
 *       The ellipsis represents a list of one or more integer arguments separated by semi-colons. The following arguments are recognized:<br>
 *       <table border="1" cellpadding="4" cellspacing="0" width="100%">
 *       <tr><td><tt>0</tt></td>     <td>Reset all font and color properties to default values.</td></tr>
 *       <tr><td><tt>1</tt></td>     <td>Bold font</td></tr>
 *       <tr><td><tt>2</tt></td>     <td>Faint (decreased intensity) - ignored (not implemented)</td></tr>
 *       <tr><td><tt>3</tt></td>     <td>Italic font</td></tr>
 *       <tr><td><tt>4</tt></td>     <td>Underline font</td></tr>
 *       <tr><td><tt>5</tt></td>     <td>Blink (slow) - reinterpreted - selects a bold font.</td></tr>
 *       <tr><td><tt>6</tt></td>     <td>Blink (fast) - reinterpreted - selects a bold font.</td></tr>
 *       <tr><td><tt>7</tt></td>     <td>Inverse colors (swap default foreground and background)</td></tr>
 *       <tr><td><tt>8</tt></td>     <td>Invisible - ignored (not implemented)</td></tr>
 *       <tr><td><tt>9</tt></td>     <td>Strike out font</td></tr>
 *       <tr><td><tt>21</tt></td>    <td>Bold font: disable</td></tr>
 *       <tr><td><tt>22</tt></td>    <td>Normal color - ignored (not implemented)</td></tr>
 *       <tr><td><tt>23</tt></td>    <td>Italic font: disable</td></tr>
 *       <tr><td><tt>24</tt></td>    <td>Underline font: disable</td></tr>
 *       <tr><td><tt>25</tt></td>    <td>Blink: disable - reinterpreted - disables bold font (same as 21).</td></tr>
 *       <tr><td><tt>27</tt></td>    <td>Inverse colors: disable</td></tr>
 *       <tr><td><tt>28</tt></td>    <td>Visible - ignored (not implemented)</td></tr>
 *       <tr><td><tt>29</tt></td>    <td>Strike out font: disable</td></tr>
 *       <tr><td><tt>30 ... 37</tt></td>  <td>Set foreground color to n-30: 0=black, 1=red, 2=green, 3=yellow, 4=blue, 5=magenta, 6=cyan, 7=white.</td></tr>
 *       <tr><td><tt>38</tt></td>    <td>KDE's Konsole extended 24-bit color space: Set foreground color (see note below).</td></tr>
 *       <tr><td><tt>39</tt></td>    <td>Default foreground color.</td></tr>
 *       <tr><td><tt>40 ... 47</tt></td>  <td>Set background color to n-40. Colors are the same as for 30 ... 37.</td></tr>
 *       <tr><td><tt>48</tt></td>    <td>KDE's Konsole extended 24-bit color space: Set background color (see note below).</td></tr>
 *       <tr><td><tt>49</tt></td>    <td>Default background color.</td></tr>
 *       <tr><td><tt>53</tt></td>    <td>Overline font</td></tr>
 *       <tr><td><tt>55</tt></td>    <td>Overline font: disable</td></tr>
 *       </table></td></tr>
 *
 * <tr><td><tt>ESC[s</tt></td>       <td>Save %Cursor Position (SCP)</td></tr>
 * <tr><td><tt>ESC[u</tt></td>       <td>Restore %Cursor Position (RCP)</td></tr>
 *
 * </table>
 *
 * KDE Konsole 24-bit colors use the following format:
 *
 * <tt>Select RGB foreground color: ESC[ ... 38;2;r;g;b ... m</tt><br>
 * <tt>Select RGB background color: ESC[ ... 48;2;r;g;b ... m</tt>
 *
 * where r, g and b are the red, green and blue components in the [0,255]
 * range. For detailed information on these codes, see the following document:
 *
 * https://projects.kde.org/projects/kde/applications/konsole/repository/revisions/master/entry/doc/user/README.moreColors
 *
 * <b>Examples</b>
 *
 * Write a text line using a bold font:
 *
 * \code
 * Console().WriteLn( "\x1b[1mThis is bold text\x1b[21m" ); \endcode
 *
 * Write a text line in orange color using an italic font:
 *
 * \code
 * String programName;
 * Console().WriteLn( "<end><cbr><br>\x1b[3;38;2;255;128;0m" + programName + "\x1b[39;23m" ); \endcode
 *
 * Write some text at the beginning of the current line, removing any previous
 * contents:
 *
 * \code
 * Console().Write( "\x1b[2KThis text replaces the whole line" ); \endcode
 *
 * \sa StatusMonitor, StandardStatus, SpinStatus, MuteStatus, Thread
 */
class PCL_CLASS Console
{
public:

   /*!
    * Constructs a %Console object.
    */
   Console();

   /*!
    * Destroys a %Console object.
    */
   virtual ~Console();

   /*!
    * Writes an Unicode string \a s to this console.
    */
   void Write( const String& s );

   /*!
    * Writes an Unicode string \a s to this console and appends a newline
    * escape character ('\\n').
    *
    * Note that this function is faster than appending a newline character to
    * the string, such as a call to Write( s + '\\n' ).
    */
   void WriteLn( const String& s );

   /*!
    * Sends a single newline escape character ('\\n') to the console.
    *
    * Note that this function is faster than writing a newline directly with a
    * call to Write( '\\n' ).
    */
   void WriteLn();

   /*!
    * Writes an informative note message to this console.
    *
    * This function writes the specified Unicode string \a s using standard
    * ANSI terminal color number 2 (green in the default palette). The
    * implementation of this member function uses ANSI escape codes.
    */
   void Note( const String& s )
   {
      Write( "\x1b[32m" + s + "\x1b[39m" );
   }

   /*!
    * Writes an informative note message to this console and appends a newline
    * escape character ('\\n').
    *
    * See the description of Note( const String& ).
    */
   void NoteLn( const String& s )
   {
      WriteLn( "\x1b[32m" + s + "\x1b[39m" );
   }

   /*!
    * Writes a warning message to this console.
    *
    * This function writes the specified Unicode string \a s using standard
    * ANSI terminal color number 5 (magenta in the default palette). The
    * implementation of this member function uses ANSI escape codes.
    */
   void Warning( const String& s )
   {
      Write( "\x1b[35m" + s + "\x1b[39m" );
   }

   /*!
    * Writes a warning message to this console and appends a newline escape
    * character ('\\n').
    *
    * See the description of Warning( const String& ).
    */
   void WarningLn( const String& s )
   {
      WriteLn( "\x1b[35m" + s + "\x1b[39m" );
   }

   /*!
    * Writes a critical error message to this console.
    *
    * This function writes the specified Unicode string \a s using standard
    * ANSI terminal color number 1 (red in the default palette). The
    * implementation of this member function uses ANSI escape codes.
    */
   void Critical( const String& s )
   {
      Write( "\x1b[31m" + s + "\x1b[39m" );
   }

   /*!
    * Writes a critical error message to this console and appends a newline
    * escape character ('\\n').
    *
    * See the description of Critical( const String& ).
    */
   void CriticalLn( const String& s )
   {
      WriteLn( "\x1b[31m" + s + "\x1b[39m" );
   }

   /*!
    * Reads a single character from this console and returns it.
    *
    * This function puts the current thread in wait state, then waits until a
    * character can be obtained in the core application's GUI thread. This
    * usually involves waiting for a keyboard event.
    *
    * \note This member function has not been implemented in the current PCL
    * version. Calling it has not effect, and zero is always returned.
    */
   int ReadChar();

   /*!
    * Reads a string from this console and returns it.
    *
    * This function puts the current thread in wait state, then opens a simple
    * modal dialog box with a single-line edit control, where the user may
    * enter a string. If the user cancels the input dialog, an empty string is
    * returned.
    *
    * \note This member function has not been implemented in the current PCL
    * version. Calling it has not effect, and an empty string is always
    * returned.
    */
   String ReadString();

   /*!
    * Returns true iff the current processing thread has been suspended by the
    * PixInsight core application.
    */
   bool Suspended() const;

   /*!
    * Returns true iff the current processing thread is in wait state.
    */
   bool Waiting() const;

   /*!
    * Returns true iff the current processing thread can be aborted by the user.
    */
   bool AbortEnabled() const;

   /*!
    * Returns true iff the user has requested to abort execution of the current
    * processing thread.
    */
   bool AbortRequested() const;

   /*!
    * Resets the current processing thread status.
    *
    * This function is useful to ignore an abort request for the current
    * processing thread. For example, if the user requests to abort execution,
    * a module may ask if she or he really wants to cancel, and if the user
    * answers 'No', then this member function can be called to clear the
    * existing abort request condition.
    *
    * \note This function must only be called when there is an active process
    * running in the current module. It can only be called from the thread
    * where either a reimplemented ProcessImplementation::ExecuteOn() or
    * ProcessImplementation::ExecuteGlobal() member function has been invoked.
    * An Error exception will be thrown otherwise.
    */
   void ResetStatus();

   /*!
    * Enables abort requests for the current processing thread.
    *
    * \note This function must only be called when there is an active process
    * running in the current module. It can only be called from the thread
    * where either a reimplemented ProcessImplementation::ExecuteOn() or
    * ProcessImplementation::ExecuteGlobal() member function has been invoked.
    * An Error exception will be thrown otherwise.
    */
   void EnableAbort();

   /*!
    * Disables abort requests for the current processing thread.
    *
    * \note This function must only be called when there is an active process
    * running in the current module. It can only be called from the thread
    * where either a reimplemented ProcessImplementation::ExecuteOn() or
    * ProcessImplementation::ExecuteGlobal() member function has been invoked.
    * An Error exception will be thrown otherwise.
    */
   void DisableAbort();

   /*!
    * Accepts a pending abort request.
    *
    * \note This function must only be called when there is an active process
    * running in the current module. It can only be called from the thread
    * where either a reimplemented ProcessImplementation::ExecuteOn() or
    * ProcessImplementation::ExecuteGlobal() member function has been invoked.
    * An Error exception will be thrown otherwise.
    */
   void Abort();

   /*!
    * Returns true iff this is a valid console object associated to an active
    * processing thread.
    */
   bool IsValid() const;

   /*!
    * Returns true iff this console is valid and has been created by the
    * calling thread; either by the root thread or by a running Thread object.
    */
   bool IsCurrentThreadConsole() const;

   /*!
    * Causes any pending data to be written immediately to this console. If
    * there is no unwritten data for this console, this function has no effect.
    */
   void Flush();

   /*!
    * Shows the %Process %Console window in the PixInsight Core application.
    *
    * The visibility of a console can only be controlled if it is the process
    * console window. Furthermore, a module cannot show or hide the console if
    * it is not owned by a docked window; the visibility of a floating window
    * cannot be changed programmatically from a module.
    *
    * This function returns true if the console could be shown (or hidden)
    * successfully. Unlike most interface operations, console hide/show
    * operations are not cached, so if this function returns true then you know
    * that the %Process %Console window is currently visible.
    *
    * \note Calling this function from a running thread has no effect. Console
    * visibility can only be changed from the root thread.
    */
   bool Show( bool show = true );

   /*!
    * Hides the %Process %Console window in the PixInsight Core application.
    *
    * This is a convenience function, equivalent to: Show( !hide )
    *
    * Returns true iff the console could be hidden (or shown) successfully.
    * Refer to the Show() member function for more information.
    *
    * \note Calling this function from a running thread has no effect. Console
    * visibility can only be changed from the root thread.
    */
   bool Hide( bool hide = true )
   {
      return Show( !hide );
   }

   /*!
    * Clears the console. This is a convenience function that simply writes the
    * \<clr\> tag to the console.
    *
    * \note Calling this function from a running thread has no effect. The
    * console output text of a Thread object cannot be erased by this function.
    *
    * \note In general, call this function only as a result of an explicit user
    * request to clear the console. Clearing the console arbitrarily, without
    * having an extremely good reason, is considered bad practice and can be
    * a good argument to deny certification of a module.
    */
   void Clear();

   /*!
    *
    */
   void ExecuteCommand( const String& command );

   /*!
    * Executes a script file.
    *
    * \param filePath   Path to the script file that will be executed. Must be
    *                   a path to an existing script on the local filesystem.
    *
    * \param arguments  A dynamic array, possibly empty, of StringKeyValue
    *                   objects. Each object in this array represents a script
    *                   argument. The key member is the parameter name, and the
    *                   value member is the corresponding parameter value.
    *
    * The core application will load, parse, authorize and execute the
    * specified script file. The scripting language will be detected
    * automatically from the file name suffix in \a filePath. Currently the
    * .scp and .js suffixes are interpreted for command-line shell scripts and
    * JavaScript scripts, respectively. Future versions may accept more
    * languages and apply language detection heuristics besides file suffixes.
    *
    * This member function returns normally if the script was loaded and
    * executed. In the event of error, for example if the specified file does
    * not exist or can't be accessed, or in the event of security errors or
    * failure to authorize script execution from the calling module, this
    * member function throws an Error exception.
    *
    * Note that command-line or JavaScript runtime execution errors cannot be
    * reported by this function. That means that this function knows nothing
    * about whether the script worked or not correctly, or as expected.
    *
    * \note If this function is called from a running thread, an Error
    * exception will be thrown. In current versions of the PixInsight platform,
    * scripts can only be executed from the root thread.
    */
   void ExecuteScript( const String& filePath, const StringKeyValueList& arguments = StringKeyValueList() );

   /*!
    *
    */
   void ExecuteScriptGlobal( const String& filePath, const StringKeyValueList& arguments = StringKeyValueList() );

   /*!
    *
    */
   void ExecuteScriptOn( const View& view, const String& filePath, const StringKeyValueList& arguments = StringKeyValueList() );

protected:

   void* m_handle;

private:

   void* m_thread;
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup console_operators Console Insertion and Extraction Operators
 */

/*!
 * Writes an object \a t to the console \a o. The type T must have
 * convert-to-string semantics. Returns a reference to the console \a o.
 * \ingroup console_operators
 */
template <typename T>
inline Console& operator <<( Console& o, T t )
{
   o.Write( String( t ) );
   return o;
}

/*!
 * Writes a null-terminated string \a s to the console \a o.
 * Returns a reference to the console \a o.
 * \ingroup console_operators
 */
inline Console& operator <<( Console& o, const char* s )
{
   o.Write( s );
   return o;
}

/*!
 * Writes a string \a s to the console \a o.
 * Returns a reference to the console \a o.
 * \ingroup console_operators
 */
inline Console& operator <<( Console& o, const String& s )
{
   o.Write( s );
   return o;
}

/*!
 * Reads a character \a c from the console \a o.
 * Returns a reference to the console \a o.
 * \ingroup console_operators
 */
inline Console& operator >>( Console& o, char& c )
{
   c = o.ReadChar();
   return o;
}

/*!
 * Reads a string \a s from the console \a o.
 * Returns a reference to the console \a o.
 * \ingroup console_operators
 */
inline Console& operator >>( Console& o, String& s )
{
   s = o.ReadString();
   return o;
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_Console_h

// ----------------------------------------------------------------------------
// EOF pcl/Console.h - Released 2017-06-21T11:36:33Z
