//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/SpinBox.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_SpinBox_h
#define __PCL_SpinBox_h

/// \file pcl/SpinBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Control.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SpinBox
 * \brief Client-side interface to a PixInsight %SpinBox control.
 *
 * ### TODO: Write a detailed description for %Sizer.
 */
class PCL_CLASS SpinBox : public Control
{
public:

   /*!
    * Constructs a %SpinBox as a child control of \a parent.
    */
   SpinBox( Control& parent = Control::Null() );

   /*!
    * Destroys a %SpinBox object.
    */
   virtual ~SpinBox()
   {
   }

   /*!
    * Returns the current value of this %SpinBox control.
    *
    * \sa SetValue(), NormalizedValue(), SetNormalizedValue()
    */
   int Value() const;

   /*!
    * Sets the value of this %SpinBox control.
    *
    * \sa Value(), NormalizedValue(), SetNormalizedValue()
    */
   void SetValue( int v );

   /*!
    * Returns the current value of this %SpinBox control as a normalized real
    * value in the range [0,1].
    *
    * \sa SetNormalizedValue(), Value()
    */
   double NormalizedValue() const;

   /*!
    * Sets the value of this %SpinBox control as a normalized real value in the
    * range [0,1].
    *
    * \sa NormalizedValue(), SetValue()
    */
   void SetNormalizedValue( double f );

   /*!
    * Gets the range of values for this %SpinBox control.
    *
    * \param[out] minValue, maxValue    References to the variables where the
    *             minimum and maximum values of this %SpinBox control will be
    *             copied, respectively.
    *
    * \sa SetRange(), MinValue(), MaxValue()
    */
   void GetRange( int& minValue, int& maxValue ) const;

   /*!
    * Returns the minimum value of this %SpinBox control.
    *
    * \sa MaxValue(), GetRange()
    */
   int MinValue() const
   {
      int m, dum; GetRange( m, dum ); return m;
   }

   /*!
    * Returns the maximum value of this %SpinBox control.
    *
    * \sa MinValue(), GetRange()
    */
   int MaxValue() const
   {
      int dum, m; GetRange( dum, m ); return m;
   }

   /*!
    * Sets the range of values for this %SpinBox control.
    *
    * \param minValue, maxValue     New minimum and maximum values,
    *             respectively, for this %SpinBox control.
    *
    * \sa GetRange(), SetMinValue(), SetMaxValue()
    */
   void SetRange( int minValue, int maxValue );

   /*!
    * Sets the minimum value of this %SpinBox control.
    *
    * This is a convenience member function, equivalent to
    * SetRange( m, MaxValue() )
    *
    * \sa SetMaxValue(), SetRange(), MinValue(), MaxValue()
    */
   void SetMinValue( int m )
   {
      SetRange( m, MaxValue() );
   }

   /*!
    * Sets the maximum value of this %SpinBox control.
    *
    * This is a convenience member function, equivalent to
    * SetRange( MinValue(), m )
    *
    * \sa SetMinValue(), SetRange(), MinValue(), MaxValue()
    */
   void SetMaxValue( int m )
   {
      SetRange( MinValue(), m );
   }

   /*!
    * Returns the <em>step size</em> of this %SpinBox control
    *
    * The step size is the number of units that are skipped when the user
    * clicks an arrow icon on this %SpinBox control.
    *
    * \sa SetStepSize()
    */
   int StepSize() const;

   /*!
    * Sets the <em>step size</em> of this %SpinBox control
    *
    * The step size is the number of units that are skipped when the user
    * clicks an arrow icon on this %SpinBox control.
    *
    * \sa StepSize()
    */
   void SetStepSize( int );

   /*!
    * Returns true iff <em>wrapping mode</em> is enabled for this %SpinBox
    * control.
    *
    * When wrapping is enabled, a %SpinBox can wrap around its minimum
    * and maximum values: stepping down from MinValue() sets the spin box's
    * value to its MaxValue(), and stepping up from MaxValue() will set its
    * value to MinValue().
    *
    * \sa EnableWrapping(), DisableWrapping()
    */
   bool IsWrappingEnabled() const;

   /*!
    * Enables or disables <em>wrapping mode</em> for this %SpinBox control.
    *
    * See IsWrappingEnabled() for a description of %SpinBox's wrapping mode.
    *
    * \sa IsWrappingEnabled(), DisableWrapping()
    */
   void EnableWrapping( bool = true );

   /*!
    * Disables or enables <em>wrapping mode</em> for this %SpinBox control.
    *
    * See IsWrappingEnabled() for a description of %SpinBox's wrapping mode.
    *
    * This is a convenience member function, equivalent to
    * EnableWrapping( !disable )
    *
    * \sa IsWrappingEnabled(), EnableWrapping()
    */
   void DisableWrapping( bool disable = true )
   {
      EnableWrapping( !disable );
   }

   /*!
    * Returns true iff this %SpinBox control is \e editable.
    *
    * An editable spin box allows the user to enter its current value directly
    * on its Edit part. The value of a non-editable spin box can only be
    * changed by clicking on its arrow icons, or by pressing the Up and Down
    * keys when it has the keyboard focus.
    *
    * \sa SetEditable()
    */
   bool IsEditable() const;

   /*!
    * Enables or disables the editable state of this %SpinBox control.
    *
    * \sa IsEditable()
    */
   void SetEditable( bool = true );

   /*!
    * Returns the prefix string of this %SpinBox control.
    *
    * The prefix is shown prepended to the spin box's text. It is useful to
    * shown measuring or currency units. By default no prefix is shown in
    * spin boxes.
    *
    * \sa SetPrefix(), Suffix(), SetSuffix(), MinimumValueText()
    */
   String Prefix() const;

   /*!
    * Sets the prefix string of this %SpinBox control.
    *
    * \sa Prefix(), Suffix(), SetSuffix(), MinimumValueText()
    */
   void SetPrefix( const String& prefix );

   /*!
    * Disables (clears) the prefix string of this %SpinBox control.
    *
    * This is a convenience member function, equivalent to
    * SetPrefix( String() )
    */
   void ClearPrefix()
   {
      SetPrefix( String() );
   }

   /*!
    * Returns the suffix string of this %SpinBox control.
    *
    * The suffix is shown appended to the spin box's text. It is useful to
    * shown measuring or currency units. By default no suffix is shown in
    * spin boxes.
    *
    * \sa SetSuffix(), Prefix(), SetPrefix(), MinimumValueText()
    */
   String Suffix() const;

   /*!
    * Sets the suffix string of this %SpinBox control.
    *
    * \sa Suffix(), Prefix(), SetPrefix(), MinimumValueText()
    */
   void SetSuffix( const String& );

   /*!
    * Disables (clears) the suffix string of this %SpinBox control.
    *
    * This is a convenience member function, equivalent to
    * SetSuffix( String() )
    */
   void ClearSuffix()
   {
      SetSuffix( String() );
   }

   /*!
    * Returns the <em>minimum value text</em> string for this %SpinBox control.
    *
    * When it is set to a non-empty string, the minimum value text is shown
    * instead of the usual numeric literal when the spin box's value is equal
    * to its minimum value.
    *
    * \sa SetMinimumValueText(), Prefix(), Suffix()
    */
   String MinimumValueText() const;

   /*!
    * Sets the <em>minimum value text</em> string for this %SpinBox control.
    *
    * \sa MinimumValueText(), Prefix(), Suffix()
    */
   void SetMinimumValueText( const String& );

   /*!
    * Disables (clears) the minimum value text of this %SpinBox control.
    *
    * This is a convenience member function, equivalent to
    * SetMinimumValueText( String() )
    */
   void ClearMinimumValueText()
   {
      SetMinimumValueText( String() );
   }

   /*!
    * Returns true iff the text in this %SpinBox control is right-aligned.
    *
    * \sa SetRightAlignment(), IsLeftAligned()
    */
   bool IsRightAligned() const;

   /*!
    * Returns true iff the text in this %SpinBox control is left-aligned.
    *
    * This is a convenience member function, equivalent to
    * !IsRightAligned()
    *
    * \sa SetLeftAlignment(), IsRightAligned()
    */
   bool IsLeftAligned() const
   {
      return !IsRightAligned();
   }

   /*!
    * Enables or disables right alignment for the text in this %SpinBox
    * control.
    *
    * \sa IsRightAligned()
    */
   void SetRightAlignment( bool = true );

   /*!
    * Enables or disables left alignment for the text in this %SpinBox
    * control.
    *
    * This is a convenience member function, equivalent to
    * SetRightAlignment( !left )
    *
    * \sa IsLeftAligned()
    */
   void SetLeftAlignment( bool left = true )
   {
      SetRightAlignment( !left );
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnValueUpdated( SpinBox& sender, int value );
   // void OnRangeUpdated( SpinBox& sender, int minValue, int maxValue );

   /*!
    * \defgroup spin_box_event_handlers SpinBox Event Handlers
    */

   /*!
    * Defines the prototype of a <em>spin box value event handler</em>.
    *
    * A spin box value event is generated when the user changes the value of a
    * spin box control.
    *
    * \param sender  The control that sends a spin box value event.
    *
    * \param value   The current value of the spin box.
    *
    * \ingroup spin_box_event_handlers
    */
   typedef void (Control::*value_event_handler)( SpinBox& sender, int value );

   /*!
    * Defines the prototype of a <em>spin box range event handler</em>.
    *
    * A spin box range event is generated when the range of a spin box control
    * is changed.
    *
    * \param sender  The control that sends a spin box range event.
    *
    * \param minValue, maxValue   The current minimum and maximum values of the
    *                spin box, respectively.
    *
    * \ingroup slider_event_handlers
    */
   typedef void (Control::*range_event_handler)( SpinBox& sender, int minValue, int maxValue );

   /*!
    * Sets the spin box value event handler for this %SpinBox control.
    *
    * \param handler    The spin box value event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive spin box value events
    *                   from this %SpinBox.
    *
    * \ingroup spin_box_event_handlers
    */
   void OnValueUpdated( value_event_handler handler, Control& receiver );

   /*!
    * Sets the slider range event handler for this %SpinBox control.
    *
    * \param handler    The slider range event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive slider range events
    *                   from this %SpinBox.
    *
    * \ingroup spin_box_event_handlers
    */
   void OnRangeUpdated( range_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      value_event_handler onValueUpdated = nullptr;
      range_event_handler onRangeUpdated = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   friend class SpinBoxEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_SpinBox_h

// ----------------------------------------------------------------------------
// EOF pcl/SpinBox.h - Released 2017-07-09T18:07:07Z
