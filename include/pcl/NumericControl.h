//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/NumericControl.h - Released 2018-11-01T11:06:36Z
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

#ifndef __PCL_NumericControl_h
#define __PCL_NumericControl_h

/// \file pcl/NumericControl.h

#include <pcl/Defs.h>

#include <pcl/Atomic.h>
#include <pcl/AutoPointer.h>
#include <pcl/Control.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/Sizer.h>
#include <pcl/Slider.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class NumericEdit
 * \brief A label/edit compound control to edit numeric parameters.
 *
 * ### TODO: Write a detailed description for %NumericEdit.
 */
class PCL_CLASS NumericEdit : public Control
{
public:

   HorizontalSizer sizer; //!< The Sizer object of this %NumericEdit control
   Label           label; //!< The Label part of this %NumericEdit control
   Edit            edit;  //!< The Edit part of this %NumericEdit control

   /*!
    * Constructs a %NumericEdit object as a child control of \a parent.
    */
   NumericEdit( Control& parent = Null() );

   /*!
    * Destroys a %NumericEdit object.
    */
   virtual ~NumericEdit()
   {
   }

   /*!
    * Returns the current value of this %NumericEdit object.
    */
   double Value() const
   {
      return m_value;
   }

   /*!
    * Sets the current value of this %NumericEdit object.
    */
   void SetValue( double );

   /*! #
    */
   String ValueAsString( double ) const;

   /*! #
    */
   String ValueAsString() const
   {
      return ValueAsString( m_value );
   }

   /*! #
    */
   int MinEditWidth() const;

   /*! #
    */
   void AdjustEditWidth();

   /*! #
    */
   bool IsReal() const
   {
      return m_real;
   }

   /*! #
    */
   void SetReal( bool real = true );

   /*! #
    */
   void SetInteger( bool integer = true )
   {
      SetReal( !integer );
   }

   /*! #
    */
   double LowerBound() const
   {
      return m_lowerBound;
   }

   /*! #
    */
   double UpperBound() const
   {
      return m_upperBound;
   }

   /*!
    * Sets the range of allowed control values.
    *
    * \param lower   Minimum allowed control value, or lower bound.
    *
    * \param upper   Maximum allowed control value, or upper bound.
    *
    * If the specified lower and upper bounds are not sorted in ascending
    * order, this member function will swap them automatically.
    */
   virtual void SetRange( double lower, double upper );

   /*! #
    */
   int Precision() const
   {
      return m_precision;
   }

   /*! #
    */
   void SetPrecision( int n );

   /*!
    * Returns true if the precision property is being applied literally as a
    * fixed number of digits after the decimal separator. Returns false if
    * precision refers to the number of represented significant digits.
    *
    * For example, if the current value is 123.45678 and precision is 4, the
    * represented value would be:
    *
    * With fixed precision enabled: 123.4568 \n
    * With fixed precision disabled: 123.5
    *
    * By default, the precision property of a %NumericEdit control refers to
    * the number of represented significant digits, so fixed precision is
    * disabled by default.
    *
    * \sa EnableFixedPrecision(), Precision()
    */
   bool IsFixedPrecision() const
   {
      return m_fixed;
   }

   /*!
    * Enables the fixed precision property of this %NumericEdit object. See
    * IsFixedPrecision() for detailed information.
    */
   void EnableFixedPrecision( bool enable = true );

   /*!
    * Disables the fixed precision property of this %NumericEdit object. See
    * IsFixedPrecision() for detailed information.
    */
   void DisableFixedPrecision( bool disable = true )
   {
      EnableFixedPrecision( !disable );
   }

   /*! #
    */
   bool IsScientificNotation() const
   {
      return m_scientific;
   }

   /*! #
    */
   void EnableScientificNotation( bool enable = true );

   /*! #
    */
   void DisableScientificNotation( bool disable = true )
   {
      EnableScientificNotation( !disable );
   }

   /*! #
    */
   int ScientificNotationTriggerExponent() const
   {
      return m_sciTriggerExp;
   }

   /*! #
    */
   void SetScientificNotationTriggerExponent( int exp10 );

   /*! #
    */
   bool IsAutoAdjustEditWidth() const
   {
      return m_autoEditWidth;
   }

   /*! #
    */
   void EnableAutoAdjustEditWidth( bool enable = true )
   {
      if ( (m_autoEditWidth = enable) != false )
         AdjustEditWidth();
   }

   /*! #
    */
   void DisableAutoAdjustEditWidth( bool disable = true )
   {
      EnableAutoAdjustEditWidth( !disable );
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnValueUpdated( NumericControl& sender, double value );

   /*! #
    */
   typedef void (Control::*value_event_handler)( NumericEdit& sender, double value );

   /*! #
    */
   void OnValueUpdated( value_event_handler, Control& );

protected:

   struct EventHandlers
   {
      value_event_handler onValueUpdated         = nullptr;
      Control*            onValueUpdatedReceiver = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   double m_value = 0;            // current value
   double m_lowerBound = 0;       // acceptable range, lower bound
   double m_upperBound = 1;       // acceptable range, upper bound
   int    m_precision = 6;        // number of decimal digits in non-sci mode, [0,15]
   bool   m_real = true;          // whether this is a real or integer parameter
   bool   m_fixed = false;        // precision is literal instead of significant digits?
   bool   m_scientific = false;   // scientific notation enabled?
   bool   m_autoEditWidth = true; // set width of edit control automatically
   int    m_sciTriggerExp = -1;   // exponent (of ten) to trigger sci notation

   PCL_MEMBER_REENTRANCY_GUARD( EditCompleted )

   virtual void UpdateControls();

   virtual void EditCompleted( Edit& );
   virtual void KeyPressed( Control&, int, unsigned, bool& );
   virtual void ReturnPressed( Edit& );
   virtual void GetFocus( Control& );
   virtual void LoseFocus( Control& );
   virtual void MousePress( Control&, const pcl::Point&, int, unsigned, unsigned );

   int PrecisionForValue( double ) const;
};

// ----------------------------------------------------------------------------

/*!
 * \class NumericControl
 * \brief A label/edit/slider compound control to edit numeric parameters
 *
 * ### TODO: Write a detailed description for %NumericControl.
 */
class PCL_CLASS NumericControl : public NumericEdit
{
public:

   HorizontalSlider slider;   //!< The Slider part of this %NumericEdit control

   /*!
    * Constructs a %NumericControl object as a child control of \a parent.
    */
   NumericControl( Control& parent = Null() );

   /*!
    * Destroys a %NumericControl object.
    */
   virtual ~NumericControl()
   {
   }

   /*!
    * Sets the range of allowed control values.
    *
    * \param lower   Minimum allowed control value, or lower bound.
    *
    * \param upper   Maximum allowed control value, or upper bound.
    *
    * If the specified lower and upper bounds are not sorted in ascending
    * order, this member function will swap them automatically.
    *
    * If the resulting lower bound is negative, the exponential slider response
    * feature will be implicitly disabled.
    */
   void SetRange( double lower, double upper ) override;

   /*!
    * Returns true if the slider component of this %NumericControl has
    * exponential response. Returns false if the slider has the default linear
    * response.
    *
    * When exponential response is enabled, the slider defines control values
    * following an exponential growth function of the form:
    *
    * y = (1 + y0)*Exp( k*x ) - 1
    *
    * where y is the current control value, y0 is the minimum allowed control
    * value (as returned by LowerBound()), x is the current slider position
    * normalized to the [0,1] range, and k is an automatically calculated
    * exponential growth factor given by:
    *
    * k = Ln( (1 + y1)/(1 + y0) )
    *
    * where y1 is the maximum allowed control value, as returned by
    * UpperBound().
    *
    * Exponential slider response is useful for %NumericControl objects used to
    * define parameters with very large numeric ranges, such as [0,500] or
    * [0,1000] for example. For such large ranges, one normally wants to
    * provide finer control for low parameter values, which is impossible with
    * a linear slider response. The exponential slider response feature is
    * disabled by default, that is, sliders have linear response by default.
    *
    * The exponential slider response feature is only available for controls
    * where the minimum allowed value (given by LowerBound()) is greater than
    * or equal to zero.
    *
    * \sa EnableExponentialResponse(), DisableExponentialResponse()
    */
   bool IsExponentialResponse() const
   {
      return m_exponential;
   }

   /*!
    * Enables the exponential slider response feature. See
    * IsExponentialResponse() for detailed information.
    *
    * If the minimum allowed control value is negative, this member function
    * throws an Error exception: The exponential slider response feature can
    * only be enabled when LowerBound() &ge; 0.
    *
    * \sa IsExponentialResponse(), DisableExponentialResponse()
    */
   void EnableExponentialResponse( bool enable = true );

   /*!
    * Disables the exponential slider response feature. See
    * IsExponentialResponse() for detailed information.
    *
    * \sa IsExponentialResponse(), EnableExponentialResponse()
    */
   void DisableExponentialResponse( bool disable = true )
   {
      EnableExponentialResponse( !disable );
   }

protected:

   void UpdateControls() override;

   virtual void ValueUpdated( Slider&, int );
   void KeyPressed( Control&, int, unsigned, bool& ) override;
   void GetFocus( Control& ) override;

private:

   bool m_exponential = false;

   double SliderValueToControl( int ) const;
   int ControlValueToSlider( double ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_NumericControl_h

// ----------------------------------------------------------------------------
// EOF pcl/NumericControl.h - Released 2018-11-01T11:06:36Z
