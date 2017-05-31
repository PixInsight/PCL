//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.04.0827
// ----------------------------------------------------------------------------
// pcl/MessageBox.h - Released 2017-05-28T08:28:50Z
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

#ifndef __PCL_MessageBox_h
#define __PCL_MessageBox_h

/// \file pcl/MessageBox.h

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/String.h>

#ifndef __PCL_MSGBOX_DONT_REMOVE_PREVIOUS_DECLARATION
/*
 * Remove conflicting identifiers from Win32 SDK headers.
 *
 * Unfortunately, MessageBox is #defined as a macro in winuser.h, and this may
 * cause problems with our code on Windows platforms.
 *
 * If for some eccentric reason you really want to use the Win32 API MessageBox
 * function (why should you?), then call either MessageBoxA() or MessageBoxW().
 */
#  ifdef MessageBox
#    undef MessageBox
#  endif
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::StdIcon
 * \brief Standard MessageBox icons.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>StdIcon::NoIcon</td>      <td>No icon will be shown on the %MessageBox</td></tr>
 * <tr><td>StdIcon::Question</td>    <td>Standard question mark icon</td></tr>
 * <tr><td>StdIcon::Information</td> <td>Standard information icon</td></tr>
 * <tr><td>StdIcon::Warning</td>     <td>Standard warning icon</td></tr>
 * <tr><td>StdIcon::Error</td>       <td>Standard error icon</td></tr>
 * </table>
 */
namespace StdIcon
{
   enum value_type
   {
      NoIcon,
      Question,
      Information,
      Warning,
      Error,

      NumberOfStandardIcons
   };
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::StdButton
 * \brief Standard MessageBox buttons.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>StdButton::NoButton</td> <td>No button</td></tr>
 * <tr><td>StdButton::Ok</td>       <td>Ok button</td></tr>
 * <tr><td>StdButton::Cancel</td>   <td>Cancel button</td></tr>
 * <tr><td>StdButton::Yes</td>      <td>Yes button</td></tr>
 * <tr><td>StdButton::No</td>       <td>No button</td></tr>
 * <tr><td>StdButton::Abort</td>    <td>Abort button</td></tr>
 * <tr><td>StdButton::Retry</td>    <td>Retry button</td></tr>
 * <tr><td>StdButton::Ignore</td>   <td>Ignore button</td></tr>
 * <tr><td>StdButton::YesToAll</td> <td>Yes to All button</td></tr>
 * <tr><td>StdButton::NoToAll</td>  <td>No to All button</td></tr>
 * </table>
 */
namespace StdButton
{
   enum value_type
   {
      NoButton,
      Ok,
      Cancel,
      Yes,
      No,
      Abort,
      Retry,
      Ignore,
      YesToAll,
      NoToAll,

      NumberOfStandardButtons
   };
}

// ----------------------------------------------------------------------------

class MessageBoxPrivate;

/*!
 * \class MessageBox
 * \brief High-level interface to a PixInsight %MessageBox object.
 *
 * A %MessageBox object executes as a modal window relative to PixInsight's
 * main window. Message boxes are useful to show brief messages, to notify
 * errors, warnings and other special conditions, and to obtain simple answers
 * (yes/no, ok/cancel, and so on) from the user.
 *
 * \sa Dialog
 */
class PCL_CLASS MessageBox
{
public:

   /*!
    * Represents a standard message box icon.
    */
   typedef StdIcon::value_type   std_icon;

   /*!
    * Represents a standard message box button.
    */
   typedef StdButton::value_type std_button;

   /*!
    * Constructs a %MessageBox object.
    *
    * \param text    The text that will be shown on the message box's client
    *             area. Can be either plain text or include basic html tags
    *             like \<br\>, \<p\>\</p\>, \<b\>\</b\>, \<i\>\</i\>,
    *             \<u\>\</u\>, etc.
    *
    * \param caption The caption of the message box window. If an empty string
    *             is specified, the platform will assign a default caption.
    *
    * \param icon    The message box icon. By default, no icon appears on a
    *             message box.
    *
    * \param button0,button1,button2   Specify up to three buttons that will be
    *             included on the bottom row of this message box. By default a
    *             message box includes just an Ok standard button.
    *
    * \param defaultButtonIdx    The button index (from 0 to 2) of the
    *             <em>default button</em> on this dialog box. The default
    *             button will be activated when the user presses the Return or
    *             Enter keys.
    *
    * \param escapeButtonIdx     The button index (from 0 to 2) of the button
    *             that will be activated when the user presses the Escape key,
    *             or when (s)he closes the message box window explicitly. If a
    *             value < 0 is specified (the default value), pressing the Esc
    *             key simply cancels the dialog and forces a return value of
    *             StdButton::Cancel.
    */
   MessageBox( const String& text             = String(),
               const String& caption          = String(), // default caption
               std_icon      icon             = StdIcon::NoIcon,
               std_button    button0          = StdButton::Ok,
               std_button    button1          = StdButton::NoButton,
               std_button    button2          = StdButton::NoButton,
               int           defaultButtonIdx = 0,
               int           escapeButtonIdx  = -1 ); // escape cancels dialog

   /*!
    * Destroys a %MessageBox object.
    */
   virtual ~MessageBox();

   /*!
    * Executes this message box modally. Returns a StdButton identifier
    * corresponding to the button that the user activated to close the message
    * box.
    *
    * For example, when the user clicks on the Ok button (if that button has
    * been included in this message box), this function returns StdButton::Ok.
    */
   std_button Execute();

   /*!
    * Returns the last modal result, or StdButton::NoButton if this box has not
    * been executed previously.
    */
   std_button Result() const;

   /*!
    * Returns the text that this message box shows on its client area.
    */
   String Text() const;

   /*!
    * Sets the \a text to be shown on the client area of this message box.
    */
   void SetText( const String& text );

   /*!
    * Returns the caption text of this message box.
    */
   String Caption() const;

   /*!
    * Sets the caption \a text of this message box.
    */
   void SetCaption( const String& text );

   /*!
    * Returns the icon that will be shown on this message box.
    */
   std_icon Icon() const;

   /*!
    * Sets the \a icon that will be shown on this message box.
    */
   void SetIcon( std_icon icon );

   /*!
    * Returns the identifier of the first button on this message box.
    */
   std_button FirstButton() const;

   /*!
    * Sets the first button on this message box.
    */
   void SetFirstButton( std_button button );

   /*!
    * Returns the identifier of the second button on this message box.
    */
   std_button SecondButton() const;

   /*!
    * Sets the second button on this message box.
    */
   void SetSecondButton( std_button button );

   /*!
    * Returns the identifier of the third button on this message box.
    */
   std_button ThirdButton() const;

   /*!
    * Sets the third button on this message box.
    */
   void SetThirdButton( std_button button );

   /*!
    * Returns the index, from 0 to 2, of the <em>default button</em> on this
    * message box.
    *
    * The default button will be activated if the user presses the Return or
    * Enter keys.
    */
   int DefaultButtonIndex() const;

   /*!
    * Sets the index, from 0 to 2, of the <em>default button</em> on this
    * message box.
    */
   void SetDefaultButtonIndex( int index );

   /*!
    * Returns the index, from 0 to 2, of the <em>escape button</em> on this
    * message box.
    *
    * The escape button will be activated if the user presses the Escape key,
    * or if (s)he closes the message box's window explicitly.
    */
   int EscapeButtonIndex() const;

   /*!
    * Sets the index, from 0 to 2, of the <em>escape button</em> on this
    * message box.
    */
   void SetEscapeButtonIndex( int index );

private:

   AutoPointer<MessageBoxPrivate> p;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MessageBox_h

// ----------------------------------------------------------------------------
// EOF pcl/MessageBox.h - Released 2017-05-28T08:28:50Z
