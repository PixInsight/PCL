//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/NumericControl.h - Released 2017-06-17T10:55:43Z
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

   /*! #
    */
   void SetRange( double lr, double ur );

   /*! #
    */
   int Precision() const
   {
      return int( m_precision );
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

   double   m_value;             // current value
   double   m_lowerBound;        // acceptable range, lower bound
   double   m_upperBound;        // acceptable range, upper bound
   unsigned m_precision     : 4; // number of decimal digits in non-sci mode, [0,15]
   bool     m_real          : 1; // whether this is a real or integer parameter
   bool     m_fixed         : 1; // precision is literal instead of significant digits?
   bool     m_scientific    : 1; // scientific notation enabled?
   bool     m_autoEditWidth : 1; // set width of edit control automatically
   int      m_sciTriggerExp;     // exponent (of ten) to trigger sci notation

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

protected:

   virtual void UpdateControls();

   virtual void ValueUpdated( Slider&, int );
   virtual void KeyPressed( Control&, int, unsigned, bool& );
   virtual void GetFocus( Control& );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_NumericControl_h

// ----------------------------------------------------------------------------
// EOF pcl/NumericControl.h - Released 2017-06-17T10:55:43Z
