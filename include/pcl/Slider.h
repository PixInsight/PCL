// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/Slider.h - Released 2014/10/29 07:34:07 UTC
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

#ifndef __PCL_Slider_h
#define __PCL_Slider_h

/// \file pcl/Slider.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Flags_h
#include <pcl/Flags.h>
#endif

#ifndef __PCL_Control_h
#include <pcl/Control.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace TickStyle
 * \brief     Slider tick styles.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>TickStyle::NoTicks</td>   <td>The slider has no ticks</td></tr>
 * <tr><td>TickStyle::Top</td>       <td>Ticks are drawn at the top edge of a horizontal slider</td></tr>
 * <tr><td>TickStyle::Left</td>      <td>Ticks are drawn at the left edge of a vertical slider</td></tr>
 * <tr><td>TickStyle::Bottom</td>    <td>Ticks are drawn at the bottom edge of a horizontal slider</td></tr>
 * <tr><td>TickStyle::Right</td>     <td>Ticks are drawn at the right edge of a vertical slider</td></tr>
 * <tr><td>TickStyle::BothSides</td> <td>Ticks are drawn at both edges of the slider. Equal to TickStyle::Top|TickStyle::Bottom</td></tr>
 * </table>
 */
namespace TickStyle
{
   enum mask_type
   {
      NoTicks   = 0x00,
      Top       = 0x01,
      Bottom    = 0x02,
#if defined( _MSC_VER ) && _MSC_VER >= 1700 // VC++ >= 2012
      Left      = 1,
      Right     = 2,
      BothSides = 3
#else
      Left      = Top,
      Right     = Bottom,
      BothSides = Top|Bottom
#endif
   };
}

/*!
 * A collection of slider tick style flags.
 */
typedef Flags<TickStyle::mask_type> TickStyles;

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

// ----------------------------------------------------------------------------

/*!
 * \class Slider
 * \brief Client-side interface to a PixInsight %Slider control.
 *
 * ### TODO: Write a detailed description for %Slider.
 */
class PCL_CLASS Slider : public Control
{
public:

   /*!
    * Constructs a %Slider object as a child control of \a parent. The
    * \a vertical parameter indicates the orientation of the slider control
    * (vertical or horizontal).
    */
   Slider( Control& parent = Control::Null(), bool vertical = false );

   /*!
    * Destroys a %Slider object.
    */
   virtual ~Slider()
   {
   }

   /*!
    * Returns the current value of this %Slider control.
    */
   int Value() const;

   /*!
    * Sets the value of this %Slider control.
    */
   void SetValue( int v );

   /*!
    * Returns the current value of this %Slider control as a normalized real
    * number in the range [0,1].
    */
   double NormalizedValue() const;

   /*!
    * Sets the value of this %Slider control as a normalized real number in the
    * range [0,1].
    */
   void SetNormalizedValue( double f );

   /*!
    * Gets the range of values for this %Slider control.
    *
    * \param[out] minValue, maxValue    References to the variables where the
    *             minimum and maximum values of this %Slider control will be
    *             copied, respectively.
    *
    * \sa SetRange(), MinValue(), MaxValue()
    */
   void GetRange( int& minValue, int& maxValue ) const;

   /*!
    * Returns the minimum value of this %Slider control.
    *
    * \sa MaxValue(), GetRange()
    */
   int MinValue() const
   {
      int m, dum; GetRange( m, dum ); return m;
   }

   /*!
    * Returns the maximum value of this %Slider control.
    *
    * \sa MinValue(), GetRange()
    */
   int MaxValue() const
   {
      int dum, m; GetRange( dum, m ); return m;
   }

   /*!
    * Sets the range of values for this %Slider control.
    *
    * \param minValue, maxValue     New minimum and maximum values,
    *             respectively, for this %Slider control.
    *
    * \sa GetRange(), SetMinValue(), SetMaxValue()
    */
   void SetRange( int minValue, int maxValue );

   /*!
    * Sets the minimum value of this %Slider control.
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
    * Sets the maximum value of this %Slider control.
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
    * Returns the <em>step size</em> of this %Slider control
    *
    * The step size is the number of units that are skipped when the user
    * presses an arrow key while this %Slider control has the keyboard focus.
    *
    * \sa SetStepSize()
    */
   int StepSize() const;

   /*!
    * Sets the <em>step size</em> of this %Slider control
    *
    * The step size is the number of units that are skipped when the user
    * presses an arrow key while this %Slider control has the keyboard focus.
    *
    * \sa StepSize()
    */
   void SetStepSize( int );

   /*!
    * Returns the <em>page size</em> of this %Slider control
    *
    * The page size is the number of units that are skipped when the user
    * clicks on the slider (outside the thumb), or when (s)he presses PgUp or
    * PgDn while this %Slider control has the keyboard focus.
    *
    * \sa SetPageSize()
    */
   int PageSize() const;

   /*!
    * Sets the <em>page size</em> of this %Slider control
    *
    * The page size is the number of units that are skipped when the user
    * clicks on the slider (outside the thumb), or when (s)he presses PgUp or
    * PgDn while this %Slider control has the keyboard focus.
    *
    * \sa PageSize()
    */
   void SetPageSize( int );

   /*!
    * Returns the distance in pixels between two consecutive tick marks.
    */
   int TickInterval() const;

   /*!
    * Sets the distance in pixels between two consecutive tick marks.
    */
   void SetTickInterval( int );

   /*!
    * A synonym for TickInterval().
    */
   int TickFrequency() const
   {
      return TickInterval();
   }

   /*!
    * A synonym for SetTickInterval().
    */
   void SetTickFrequency( int q )
   {
      SetTickInterval( q );
   }

   /*!
    * Returns the current tick style of this %Slider control.
    */
   TickStyles TickStyle() const;

   /*!
    * Sets the tick style of this %Slider control.
    */
   void SetTickStyle( TickStyles );

   /*!
    * Disables ticks for this %Slider control.
    *
    * This is a convenience member function, equivalent to
    * SetTickStyle( TickStyle::NoTicks )
    */
   void DisableTicks()
   {
      SetTickStyle( TickStyle::NoTicks );
   }

   /*!
    * Returns true if <em>automatic tracking</em> is currently enabled for this
    * %Slider control.
    *
    * When automatic tracking is enabled, continuous <em>slider value
    * events</em> are generated while the user is moving the slider's thumb
    * with the mouse. When automatic tracking is disabled, a single slider
    * value event is generated when the user releases the mouse button.
    *
    * \sa EnableTracking(), DisableTracking()
    */
   bool IsTrackingEnabled() const;

   /*!
    * Enables or disables <em>automatic tracking</em> for this %Slider control.
    *
    * \sa DisableTracking(), IsTrackingEnabled()
    */
   void EnableTracking ( bool = true );

   /*!
    * Disables or enables <em>automatic tracking</em> for this %Slider control.
    *
    * This is a convenience member function, equivalent to
    * EnableTracking( !disable )
    *
    * \sa EnableTracking(), IsTrackingEnabled()
    */
   void DisableTracking( bool disable = true )
   {
      EnableTracking( !disable );
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnValueUpdated( Slider& sender, int value );
   // void OnRangeUpdated( Slider& sender, int minValue, int maxValue );

   /*!
    * \defgroup slider_event_handlers Slider Event Handlers
    */

   /*!
    * Defines the prototype of a <em>slider value event handler</em>.
    *
    * A slider value event is generated when the user changes the value of a
    * slider control.
    *
    * \param sender  The control that sends a slider value event.
    *
    * \param value   The current value of the slider.
    *
    * \ingroup slider_event_handlers
    */
   typedef void (Control::*value_event_handler)( Slider& sender, int value );

   /*!
    * Defines the prototype of a <em>slider range event handler</em>.
    *
    * A slider range event is generated when the range of a slider control is
    * changed.
    *
    * \param sender  The control that sends a slider range event.
    *
    * \param minValue, maxValue   The current minimum and maximum values of the
    *                slider, respectively.
    *
    * \ingroup slider_event_handlers
    */
   typedef void (Control::*range_event_handler)( Slider& sender, int minValue, int maxValue );

   /*!
    * Sets the slider value event handler for this %Slider control.
    *
    * \param handler    The slider value event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive slider value events
    *                   from this %Slider.
    */
   void OnValueUpdated( value_event_handler handler, Control& receiver );

   /*!
    * Sets the slider range event handler for this %Slider control.
    *
    * \param handler    The slider range event handler. Must be a member
    *                   function of the receiver object's class.
    *
    * \param receiver   The control that will receive slider range events
    *                   from this %Slider.
    */
   void OnRangeUpdated( range_event_handler handler, Control& receiver );

protected:

   value_event_handler  onValueUpdated;
   range_event_handler  onRangeUpdated;

   friend class SliderEventDispatcher;
};

// ----------------------------------------------------------------------------

/*!
 * \class HorizontalSlider
 * \brief Horizontal slider control.
 *
 * ### TODO: Write a detailed description for %HorizontalSlider.
 */
class PCL_CLASS HorizontalSlider : public Slider
{
public:

   /*!
    * Constructs a %HorizontalSlider as a child control of \a parent.
    */
   HorizontalSlider( Control& parent = Control::Null() ) : Slider( parent, false )
   {
   }

   /*!
    * Destroys a %HorizontalSlider control.
    */
   virtual ~HorizontalSlider()
   {
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class VerticalSlider
 * \brief Vertical slider control.
 *
 * ### TODO: Write a detailed description for %VerticalSlider.
 */
class PCL_CLASS VerticalSlider : public Slider
{
public:

   /*!
    * Constructs a %VerticalSlider as a child control of \a parent.
    */
   VerticalSlider( Control& parent = Control::Null() ) : Slider( parent, true )
   {
   }

   /*!
    * Destroys a %VerticalSlider control.
    */
   virtual ~VerticalSlider()
   {
   }
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Slider_h

// ****************************************************************************
// EOF pcl/Slider.h - Released 2014/10/29 07:34:07 UTC
