//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/RealTimePreview.h - Released 2018-11-01T11:06:36Z
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

#ifndef __PCL_RealTimePreview_h
#define __PCL_RealTimePreview_h

/// \file pcl/RealTimePreview.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ProcessInterface;

/*!
 * \class RealTimePreview
 * \brief Client-side interface to the Real-Time Preview system.
 *
 * %RealTimePreview is a high-level, client-side interface to the Real-Time
 * Preview system available in the PixInsight core application.
 *
 * \note The Real-Time Preview system is undergoing extensive development in
 * PixInsight. Be aware that this class will be updated and enhanced
 * considerably in future PCL releases.
 */
class PCL_CLASS RealTimePreview
{
public:

   /*!
    * Default constructor. This constructor is disabled because
    * %RealTimePreview is not an instantiable class.
    */
   RealTimePreview() = delete;

   /*!
    * Copy constructor. This constructor is disabled because %RealTimePreview
    * is not an instantiable class.
    */
   RealTimePreview( const RealTimePreview& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %RealTimePreview is
    * not an instantiable class.
    */
   RealTimePreview& operator =( const RealTimePreview& ) = delete;

   /*!
    * Destructor. This destructor is disabled because %RealTimePreview is not
    * an instantiable class.
    */
   ~RealTimePreview() = delete;

   /*!
    * Sets the owner of the Real-Time Preview interface.
    *
    * \param owner   Reference to a process interface that will own the
    *                Real-Time Preview interface in the PixInsight core
    *                application.
    *
    * The owner of the Real-Time Preview is the interface currently providing
    * data for generation of real-time image renditions. Only one interface can
    * own the Real-Time Preview at a given time.
    *
    * Calling this function causes the immediate regeneration of the real-time
    * image rendition, in the terms described for the Update() member function.
    */
   static bool SetOwner( ProcessInterface& owner );

   /*!
    * Generates a new Real-Time Preview image rendition.
    *
    * A new real-time preview image can always be generated if there is a view
    * currently selected on the Real-Time Preview window, if the selected view
    * is not read-locked, and if the Real-Time Preview is not already busy.
    * When all of that conditions are met, calling this function causes the
    * immediate regeneration of the real-time preview image.
    */
   static void Update();

   /*!
    * Returns true iff the Real-Time Preview is currently busy, i.e. if there is
    * an active real-time rendition process.
    */
   static bool IsUpdating();

   /*!
    * Requests a modal progress dialog associated with the Real-Time Preview
    * interface during a real-time preview generation process.
    *
    * \param title   The title of the modal progress dialog.
    *
    * \param text    The informative text shown on the progress dialog. Keep in
    *                mind that this string will be shown on a single text line,
    *                so a reasonable maximum length should be about 50-60
    *                characters.
    *
    * \param total   Total number of <em>progress steps</em>. The progress bar
    *                on the dialog can be incremented by successive calls to
    *                SetProgressCount() up to this value. If this parameter is
    *                zero, the progress dialog will be \e unbounded and will
    *                only show a <em>busy state</em> (typically, a progress bar
    *                bouncing horizontally at regular intervals).
    *
    * This function allows you to show a modal dialog with a horizontal
    * progress bar, a single-line text label and a standard Cancel button
    * during long real-time generation procedures. These dialogs are useful to
    * provide feedback to the user and to allow her or him to abort the ongoing
    * real-time rendition task.
    *
    * This member function can only be called during a real-time preview
    * generation process, that is, when a reimplemented
    * ProcessInterface::GenerateRealTimePreview() function is running. Calling
    * this function when no real-time rendition is taking place has no effect.
    *
    * Unlike most PCL routines involving dynamic changes to graphical interface
    * elements, this function is thread-safe. You can safely call it from a
    * running thread, which is a typical use because real-time generation tasks
    * are normally (and \e should normally be) implemented as parallel threads
    * to keep the GUI responsive. However, for the progress dialog to work it
    * is necessary that the application's main event queue be active with
    * sufficient granularity. In general, if you follow good UI programming
    * techniques and use PCL example modules as a starting point to write your
    * code, there should be no practical problems.
    *
    * \note Be aware that the progress dialog shown by this function is
    * application-modal. This means that the only user interaction allowed
    * while the dialog is visible is closing it, either by clicking its Cancel
    * button, by pressing the Esc key, or by clicking its Close title bar icon.
    * An important consequence of this is that you \e must ensure that your
    * real-time generation process will end immediately and unconditionally
    * when the dialog is closed. If your code is coherent with the GUI states
    * of your process interface, the default PCL implementation should work. In
    * some complex scenarios you may need to reimplement the
    * ProcessInterface::CancelRealTimePreview() member function.
    */
   static void ShowProgressDialog( const String& title, const String& text, size_type total );

   /*!
    * Requests closing a progress dialog previously shown by a call to
    * ShowProgressDialog().
    */
   static void CloseProgressDialog();

   /*!
    * Returns true iff a modal progress dialog is currently visible after a call
    * to ShowProgressDialog().
    */
   static bool IsProgressDialogVisible();

   /*!
    * Sets the current value of the modal progress dialog associated with the
    * Real-Time Preview interface to the specified \a count.
    *
    * If the progress dialog is bounded, i.e. if you specified a nonzero total
    * count when you called ShowProgressDialog(), then the \a count value set
    * by this function should never be greater than the dialog's total count.
    * If the dialog is unbounded, then you just have to increment the count to
    * animate the dialog's <em>busy state</em>.
    *
    * If a progress dialog is not visible after a successful call to
    * ShowProgressDialog(), this function is simply ignored.
    */
   static void SetProgressCount( size_type count );

   /*!
    * Sets the current single-line label's \a text shown on the modal progress
    * dialog associated with the Real-Time Preview interface.
    *
    * If a progress dialog is not visible after a successful call to
    * ShowProgressDialog(), this function is simply ignored.
    */
   static void SetProgressText( const String& text );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_RealTimePreview_h

// ----------------------------------------------------------------------------
// EOF pcl/RealTimePreview.h - Released 2018-11-01T11:06:36Z
