//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/Dialog.h - Released 2018-11-01T11:06:36Z
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

#ifndef __PCL_Dialog_h
#define __PCL_Dialog_h

/// \file pcl/Dialog.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Control.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::StdDialogCode
 * \brief Standard dialog return codes
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>StdDialogCode::Ok</td>     <td>The dialog has been accepted (e.g., by clicking the OK button)</td></tr>
 * <tr><td>StdDialogCode::Cancel</td> <td>The dialog has been rejected (e.g., by clicking the Cancel button)</td></tr>
 * </table>
 */
namespace StdDialogCode
{
   enum value_type
   {
      Cancel  = 0,   // The dialog has been rejected (e.g., by clicking the Cancel button)
      Ok      = 1    // The dialog has been accepted (e.g., by clicking the OK button)
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class Dialog
 * \brief Client-side interface to a PixInsight modal dialog
 *
 * Dialogs are top-level windows that execute modally relative to the main
 * window of the PixInsight core application.
 */
class PCL_CLASS Dialog : public Control
{
public:

   /*!
    * Represents a standard dialog return code.
    */
   typedef StdDialogCode::value_type   std_code;

   /*!
    * Constructs a %Dialog object.
    *
    * If the specified \a parent control is a non-null control, the dialog will
    * be shown centered over its parent. Otherwise the dialog window will be
    * shown centered on the current workspace. Note that specifying a dialog
    * parent does not change the dialog's \e modality: all dialog windows are
    * application-modal in PixInsight. See the Execute() and Open() member
    * functions for more information.
    */
   Dialog( Control& parent = Control::Null() );

   /*!
    * Destroys a %Dialog object.
    */
   virtual ~Dialog()
   {
   }

   /*!
    * Shows a modal dialog and executes a separate event loop.
    *
    * This function creates a new event loop and does not return until the loop
    * has exited and the dialog has been closed. During the event loop, the
    * dialog behaves as an <em>application modal window:</em> Only the dialog
    * window allows user interaction while the rest of the GUI is blocked in
    * the PixInsight Core application. To exit the modal event loop, the Ok()
    * and Cancel() member functions can be used to return the standard dialog
    * exit codes StdDialogCode::Ok and StdDialogCode::Cancel, respectively.
    *
    * This function returns a <em>dialog return code</em>. Although dialog
    * return codes are arbitrary (they can be forced to any integer value with
    * the Return() member function), dialogs with standard OK and Cancel
    * buttons should always return a standard code, as defined in the
    * StdDialogCode namespace.
    *
    * For a non-blocking alternative to use dialog windows, see the Open()
    * member function.
    *
    * \sa Ok(), Cancel(), Return(), Open()
    */
   int Execute();

   /*!
    * Shows a modal dialog window.
    *
    * This function opens the dialog window and returns immediately, allowing
    * normal execution to continue without entering a separate event loop. As
    * happens with Dialog::Execute(), this member function creates a modal
    * dialog that blocks the rest of the PixInsight Core application for GUI
    * user interaction. The essential difference between both member functions
    * is that Open() does not enter a new event loop. Typically, this function
    * is used to provide feedback to the user during long procedures, as well
    * as the possibility to interrupt or pause those procedures. For example,
    * a <em>progress dialog</em> can be shown as a modal window during a file
    * copy operation, or while a long calculation routine is being executed.
    *
    * \sa Execute()
    */
   void Open();

   /*!
    * Closes this modal dialog and forces a return value \a retCode for the
    * Execute() member function.
    */
   void Return( int retCode );

   /*!
    * This is a convenience member function equivalent to:\n
    * Return( StdDialogCode::Ok )
    */
   void Ok()
   {
      Return( StdDialogCode::Ok );
   }

   /*!
    * This is a convenience member function equivalent to:\n
    * Return( StdDialogCode::Cancel )
    */
   void Cancel()
   {
      Return( StdDialogCode::Cancel );
   }

   /*!
    * Returns true iff this dialog is user-resizable. User-resizable dialogs can
    * be resized interactively with the mouse and standard window controls.
    */
   bool IsUserResizable() const;

   /*!
    * Enables or disables user resizing for this dialog.
    */
   void EnableUserResizing( bool = true );

   /*!
    * Disables or enables user resizing for this dialog.
    *
    * This is a convenience member function, equivalent to:
    * EnableUserResizing( !disable )
    */
   void DisableUserResizing( bool disable = true )
   {
      EnableUserResizing( !disable );
   }

   /*!
    * Processes pending user interface events.
    *
    * This member function is a convenience wrapper with the same functionality
    * as ProcessInterface::ProcessEvents().
    */
   static void ProcessEvents( bool excludeUserInputEvents = false );

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnExecute( Dialog& sender );
   // void OnReturn( Dialog& sender, int retVal );

   /*!
    * \defgroup dialog_event_handlers Dialog Event Handlers
    */

   /*!
    * Defines the prototype of a <em>dialog execution</em> event handler.
    *
    * A dialog execution event is generated when a dialog control is executed
    * by invoking its Execute() member function.
    *
    * \param sender  The dialog that sends an execution event.
    *
    * \ingroup dialog_event_handlers
    */
   typedef void (Control::*execute_event_handler)( Dialog& sender );

   /*!
    * Defines the prototype of a <em>dialog return</em> event handler.
    *
    * A dialog return event is generated when a dialog control terminates
    * execution by invoking its Return() member function.
    *
    * \param sender  The dialog that sends a return event.
    *
    * \param retCode Return code passed to the Return() member function. This
    *                is also the return value of Execute().
    *
    * \ingroup dialog_event_handlers
    */
   typedef void (Control::*return_event_handler)( Dialog& sender, int retCode );

   /*!
    * Sets the execution event handler for this dialog.
    *
    * \param handler    The dialog execution event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive execution events from
    *                   this dialog.
    *
    * \ingroup dialog_event_handlers
    */
   void OnExecute( execute_event_handler handler, Control& receiver );

   /*!
    * Sets the return event handler for this dialog.
    *
    * \param handler    The dialog return event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive return events from this
    *                   dialog.
    *
    * \ingroup dialog_event_handlers
    */
   void OnReturn( return_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      execute_event_handler onExecute = nullptr;
      return_event_handler  onReturn  = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   friend class DialogEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_Dialog_h

// ----------------------------------------------------------------------------
// EOF pcl/Dialog.h - Released 2018-11-01T11:06:36Z
