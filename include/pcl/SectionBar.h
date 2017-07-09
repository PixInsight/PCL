//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/SectionBar.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_SectionBar_h
#define __PCL_SectionBar_h

/// \file pcl/SectionBar.h

#include <pcl/Defs.h>

#include <pcl/CheckBox.h>
#include <pcl/Control.h>
#include <pcl/Label.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SectionBar
 * \brief A control to manage collapsible sections of interface windows and
 *        dialogs.
 *
 * %SectionBar manages collapsible/extensible sections on processing interfaces
 * and dialogs. It includes a <em>section title</em> label and an icon that
 * represents the current collapsed/extended state.
 *
 * When a %SectionBar control is clicked, it toggles visibility of a
 * <em>section control</em> and adjusts the height of the top-level parent
 * window to fit to its new contents. %SectionBar is useful to organize complex
 * user interfaces and is extensively used by most standard process interfaces
 * in PixInsight.
 *
 * \sa ProcessInterface, Dialog
 */
class PCL_CLASS SectionBar : public Control
{
public:

   /*!
    * Constructs a %SectionBar as a child control of \a parent.
    */
   SectionBar( Control& parent = Null() );

   /*!
    * Destroys a %SectionBar object.
    */
   virtual ~SectionBar();

   /*!
    * Returns a reference to the immutable <em>section control</em> managed by
    * this %SectionBar.
    *
    * \sa SetSection()
    */
   const Control& Section() const
   {
      return (m_section != nullptr) ? *m_section : Control::Null();
   }

   /*!
    * Returns a reference to the <em>section control</em> managed by this
    * %SectionBar.
    *
    * \sa SetSection()
    */
   Control& Section()
   {
      return (m_section != nullptr) ? *m_section : Control::Null();
   }

   /*!
    * Sets the <em>section control</em> managed by this %SectionBar.
    *
    * \sa Section()
    */
   void SetSection( Control& );

   /*!
    * Returns the title of this %SectionBar.
    *
    * \sa SetTitle()
    */
   String Title() const
   {
      return Title_Label.Text();
   }

   /*!
    * Sets the title of this %SectionBar.
    *
    * \sa Title()
    */
   void SetTitle( const String& title )
   {
      Title_Label.SetText( title );
   }

   /*!
    * Returns true iff this %SectionBar is \e checkable.
    *
    * \sa EnableTitleCheckBox(), DisableTitleCheckBox(), IsChecked(), OnCheck()
    */
   bool HasTitleCheckBox() const
   {
      return !Title_CheckBox.IsNull();
   }

   /*!
    * Creates (or removes) a check box in this %SectionBar.
    *
    * \sa DisableTitleCheckBox(), HasTitleCheckBox(), IsChecked(), OnCheck()
    */
   void EnableTitleCheckBox( bool = true );

   /*!
    * Removes (or creates) the check box in this %SectionBar.
    *
    * \sa EnableTitleCheckBox(), HasTitleCheckBox(), IsChecked(), OnCheck()
    */
   void DisableTitleCheckBox( bool disable = true )
   {
      EnableTitleCheckBox( !disable );
   }

   /*!
    * Returns true iff there is a check box in this %SectionBar and it is
    * currently checked.
    *
    * \sa SetChecked(), EnableTitleCheckBox(), HasTitleCheckBox(), OnCheck()
    */
   bool IsChecked() const
   {
      return !Title_CheckBox.IsNull() && Title_CheckBox->IsChecked();
   }

   /*!
    * Checks or unchecks the check box in this %SectionBar.
    *
    * The \e enabled state of the section control (if there is one) will be
    * automatically toggled according to the state of the check box: the
    * section control will be enabled if the check box is checked; it will be
    * disabled otherwise.
    *
    * \note If this %SectionBar is not checkable, this function is ignored.
    *
    * \sa IsChecked(), EnableTitleCheckBox(), HasTitleCheckBox(), OnCheck()
    */
   void SetChecked( bool checked = true );

   /*!
    * A convenient synonym for SetChecked( true ).
    */
   void Check()
   {
      SetChecked( true );
   }

   /*!
    * A convenient synonym for SetChecked( false ).
    */
   void Uncheck()
   {
      SetChecked( false );
   }

   /*!
    * Returns true iff this %SectionBar object is enabled.
    *
    * \sa Enable()
    */
   virtual bool IsEnabled() const
   {
      return Title_Label.IsEnabled();
   }

   /*!
    * Enables (or disables) this %SectionBar object.
    *
    * When a SectionBar object is disabled, its section control is also
    * disabled, and in case it has a title checkbox, it is also disabled.
    * However, the collapse/extend functionality of SectionBar is unaffected by
    * its enabled/disabled state.
    *
    * \sa IsEnabled()
    */
   virtual void Enable( bool enabled = true );

   /*!
    * Shows or hides the section control managed by this %SectionBar.
    *
    * In general, this function should be used instead of simply calling
    * Control::SetVisible() (or Control::Show()/Control::Hide()). This function
    * ensures that the parent control (typically a process interface)
    * regenerates its layout correctly after changing visibility of the section
    * control.
    *
    * \sa ShowSection(), HideSection()
    */
   void SetSectionVisible( bool visible = true );

   /*!
    * A convenient synonym for SetSectionVisible( true ).
    */
   void ShowSection()
   {
      SetSectionVisible( true );
   }

   /*!
    * A convenient synonym for SetSectionVisible( false ).
    */
   void HideSection()
   {
      SetSectionVisible( false );
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnToggleSection( SectionBar& sender, Control& section, bool start );

   /*!
    * \defgroup section_bar_event_handlers SectionBar Event Handlers
    */

   /*!
    * Defines the prototype of a <em>section event handler</em>.
    *
    * A section event is generated when the user clicks on this section bar to
    * collapse or extend the managed section control.
    *
    * For each section event, two \e subevents are generated: one \e before the
    * collapse/extend operation, and a second one once the operation has been
    * completed.
    *
    * \param sender  The control that sends a section event.
    *
    * \param section The section control that has been collapsed or extended.
    *
    * \param start   True if this is a first subevent that is being sent before
    *                the collapse/extend operation; false if it is a second
    *                subevent, sent after having completed the operation.
    *
    * \ingroup section_bar_event_handlers
    */
   typedef void (Control::*section_event_handler)( SectionBar& sender, Control& section, bool start );

   /*!
    * Sets the section event handler for this %SectionBar control.
    *
    * \param handler    The section event handler. Must be a member function of
    *                   the receiver object's class.
    *
    * \param receiver   The control that will receive section events from this
    *                   %SectionBar.
    *
    * \ingroup section_bar_event_handlers
    */
   void OnToggleSection( section_event_handler handler, Control& receiver );

   /*!
    * Defines the prototype of a <em>check event handler</em>
    *
    * A check event is generated when the user toggles the state of the check
    * box in a checkable section bar, either by clicking it with the mouse, or
    * by focusing it and pressing the space bar key.
    *
    * \param sender  The control that sends a check event.
    *
    * \param checked True if the check box is now checked in the sender section
    *                bar. False if the check box is currently unchecked.
    *
    * \ingroup section_bar_event_handlers
    */
   typedef void (Control::*check_event_handler)( SectionBar& sender, bool checked );

   /*!
    * Sets the check event handler for this %SectionBar control.
    *
    * \param handler    The check event handler. Must be a member function of
    *                   the receiver object's class.
    *
    * \param receiver   The control that will receive check events from this
    *                   %SectionBar.
    *
    * \ingroup section_bar_event_handlers
    */
   void OnCheck( check_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      section_event_handler onToggleSection         = nullptr;
      Control*              onToggleSectionReceiver = nullptr;

      check_event_handler   onCheck                 = nullptr;
      Control*              onCheckReceiver         = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;
   Control*                   m_section = nullptr;

   VerticalSizer         Global_Sizer;
      HorizontalSizer       Title_Sizer;
         Label                 Title_Label;
         AutoPointer<CheckBox> Title_CheckBox; // non-null for a checkable SectionBar
         ToolButton            Title_ToolButton;

   void ButtonClick( Button&, bool );
   void MousePress( Control&, const pcl::Point&, int, unsigned, unsigned );
   void ControlShow( Control& );
   void ControlHide( Control& );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SectionBar_h

// ----------------------------------------------------------------------------
// EOF pcl/SectionBar.h - Released 2017-07-09T18:07:07Z
