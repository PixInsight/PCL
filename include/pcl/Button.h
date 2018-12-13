//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/Button.h - Released 2018-12-12T09:24:21Z
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

#ifndef __PCL_Button_h
#define __PCL_Button_h

/// \file pcl/Button.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Control.h>
#include <pcl/Bitmap.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::CheckState
 * \brief Button check states
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>CheckState::Unchecked</td>   <td>The control is not checked</td></tr>
 * <tr><td>CheckState::Checked</td>     <td>The control is checked</td></tr>
 * <tr><td>CheckState::ThirdState</td>  <td>The check box is in its 'third state'</td></tr>
 * </table>
 *
 */
namespace CheckState
{
   enum value_type
   {
      Unchecked,  // the item is not checked
      Checked,    // the item is checked
      ThirdState  // the check box is in its 'third state'
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class Button
 * \brief Abstract base class for all client-side interfaces to PixInsight
 *        button controls.
 *
 * ### TODO: Write a detailed description for Button
 */
class PCL_CLASS Button : public Control
{
public:

   /*!
    * Represents a button check state. Supported values are defined in the
    * CheckState namespace.
    */
   typedef CheckState::value_type   check_state;

   /*!
    * Destroys a %Button control.
    */
   virtual ~Button()
   {
   }

   /*!
    * Returns the current button text.
    */
   String Text() const;

   /*!
    * Sets the current button text.
    */
   void SetText( const String& );

   /*!
    * Returns the current icon shown by this button, or a null Bitmap object if
    * this button has no associated icon.
    */
   Bitmap Icon() const;

   /*!
    * Sets the icon shown by this button. If a null Bitmap object is specified,
    * no icon will be associated with this button.
    */
   void SetIcon( const Bitmap& );

   /*!
    * Obtains the current icon dimensions of this button in pixels. The icon's
    * \a width and \a height are returned in the referenced variables.
    */
   void GetIconSize( int& width, int& height ) const;

   /*!
    * Returns the current icon width in pixels for this button.
    */
   int IconWidth() const
   {
      int width, dum; GetIconSize( width, dum ); return width;
   }

   /*!
    * Returns the current icon height in pixels for this button.
    */
   int IconHeight() const
   {
      int dum, height; GetIconSize( dum, height ); return height;
   }

   /*!
    * Sets new icon sizes for this button, \a width and \a height in pixels,
    * respectively.
    */
   void SetIconSize( int width, int height );

   /*! #
    */
   void SetIconSize( int size )
   {
      SetIconSize( size, size );
   }

   /*! #
    */
   void GetScaledIconSize( int& width, int& height ) const
   {
      GetIconSize( width, height ); width = PhysicalPixelsToLogical( width ); height = PhysicalPixelsToLogical( height );
   }

   /*! #
    */
   int ScaledIconWidth() const
   {
      int width, dum; GetIconSize( width, dum ); return PhysicalPixelsToLogical( width );
   }

   /*! #
    */
   int ScaledIconHeight() const
   {
      int dum, height; GetIconSize( dum, height ); return PhysicalPixelsToLogical( height );
   }

   /*! #
    */
   void SetScaledIconSize( int width, int height )
   {
      SetIconSize( LogicalPixelsToPhysical( width ), LogicalPixelsToPhysical( height ) );
   }

   /*! #
    */
   void SetScaledIconSize( int size )
   {
      size = LogicalPixelsToPhysical( size );
      SetIconSize( size, size );
   }

   /*!
    * Returns true iff this button can be <em>pushed down</em>.
    *
    * \note This is a pure virtual member function that must be reimplemented
    * by derived classes.
    */
   virtual bool IsPushable() const = 0;

   /*!
    * Returns true iff this button is in <em>pushed down</em> state.
    */
   bool IsPushed() const;

   /*!
    * Sets the current push state of this button.
    */
   void SetPushed( bool = true );

   /*!
    * A convenience synonym for SetPushed( true ).
    */
   void Push()
   {
      SetPushed( true );
   }

   /*!
    * A convenience synonym for SetPushed( false ).
    */
   void Unpush()
   {
      SetPushed( false );
   }

   /*!
    * Returns true iff this button can be <em>checked</em>.
    *
    * \note This is a pure virtual member function that must be reimplemented
    * by derived classes.
    */
   virtual bool IsCheckable() const = 0;

   /*!
    * Returns true iff this button is in <em>checked state</em>.
    */
   bool IsChecked() const;

   /*!
    * Sets the current check state of this button.
    */
   void SetChecked( bool = true );

   /*!
    * A convenience synonym for SetChecked( true ).
    */
   void Check()
   {
      SetChecked( true );
   }

   /*!
    * A convenience synonym for SetChecked( false ).
    */
   void Uncheck()
   {
      SetChecked( false );
   }

   /*!
    * Returns the current check state of this button. This function must be
    * used for button controls that can adopt \e tristate check states.
    */
   check_state State() const;

   /*!
    * Sets the current check state of this button. This function must be used
    * for button controls that can adopt \e tristate check states.
    */
   void SetState( check_state );

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnClick( Button& sender, bool checked );
   // void OnPress( Button& sender );
   // void OnRelease( Button& sender );
   // void OnCheck( Button& sender, Button::check_state state );

   /*!
    * \defgroup button_event_handlers Button Event Handlers
    */

   /*!
    * Defines the prototype of a <em>button click event handler</em>.
    *
    * A button click event is generated when a button control is clicked with
    * the mouse (pressed down then released while the mouse cursor stays within
    * the button) or activated with the keyboard.
    *
    * \param sender  The control that sends a button click event.
    * \param checked Whether the sender button is currently checked.
    *
    * \ingroup button_event_handlers
    */
   typedef void (Control::*click_event_handler)( Button& sender, bool checked );

   /*!
    * Defines the prototype of a <em>button press event handler</em>.
    *
    * A button press event is generated when a button control is either pressed
    * down with the mouse, or released after having being pressed.
    *
    * \param sender  The control that sends a button press event.
    *
    * \ingroup button_event_handlers
    */
   typedef void (Control::*press_event_handler)( Button& sender );

   /*!
    * Defines the prototype of a <em>button check event handler</em>.
    *
    * A button check event is generated when a checkable button control changes
    * its check state, either by clicking it with the mouse, or by activating
    * it with the keyboard.
    *
    * \param sender  The control that sends a button press event.
    * \param state   The current check state of the sender button.
    *
    * \ingroup button_event_handlers
    */
   typedef void (Control::*check_event_handler)( Button& sender, Button::check_state state );

   /*!
    * Sets the button click event handler for this button.
    *
    * \param handler    The button click event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive click events from this
    *                   button.
    *
    * \ingroup button_event_handlers
    */
   void OnClick( click_event_handler handler, Control& receiver );

   /*!
    * Sets the button press event handler for this button.
    *
    * \param handler    The button press event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive press events from this
    *                   button.
    *
    * \ingroup button_event_handlers
    */
   void OnPress( press_event_handler handler, Control& receiver );

   /*!
    * Sets the button release event handler for this button.
    *
    * \param handler    The button release event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive release events from this
    *                   button.
    *
    * \ingroup button_event_handlers
    */
   void OnRelease( press_event_handler handler, Control& receiver );

   /*!
    * Sets the button check event handler for this button.
    *
    * \param handler    The button check event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive check events from this
    *                   button.
    *
    * \ingroup button_event_handlers
    */
   void OnCheck( check_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      click_event_handler onClick   = nullptr;
      press_event_handler onPress   = nullptr;
      press_event_handler onRelease = nullptr;
      check_event_handler onCheck   = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

protected:

   /*!
    * \internal
    */
   Button( void* h ) : Control( h )
   {
   }

   friend class ButtonEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_Button_h

// ----------------------------------------------------------------------------
// EOF pcl/Button.h - Released 2018-12-12T09:24:21Z
