// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/NumericControl.h - Released 2014/10/29 07:34:06 UTC
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

#ifndef __PCL_NumericControl_h
#define __PCL_NumericControl_h

/// \file pcl/NumericControl.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Control_h
#include <pcl/Control.h>
#endif

#ifndef __PCL_Sizer_h
#include <pcl/Sizer.h>
#endif

#ifndef __PCL_Label_h
#include <pcl/Label.h>
#endif

#ifndef __PCL_Edit_h
#include <pcl/Edit.h>
#endif

#ifndef __PCL_Slider_h
#include <pcl/Slider.h>
#endif

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

   /*! #
    */
   double Value() const
   {
      return value;
   }

   /*! #
    */
   void SetValue( double );

   /*! #
    */
   String ValueAsString( double ) const;

   /*! #
    */
   String ValueAsString() const
   {
      return ValueAsString( value );
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
      return isReal;
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
      return lowerBound;
   }

   /*! #
    */
   double UpperBound() const
   {
      return upperBound;
   }

   /*! #
    */
   void SetRange( double lr, double ur );

   /*! #
    */
   int Precision() const
   {
      return int( precision );
   }

   /*! #
    */
   void SetPrecision( int n );

   /*! #
    */
   bool IsScientificNotationEnabled() const
   {
      return scientific;
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
      return sciTriggerExp;
   }

   /*! #
    */
   void SetScientificNotationTriggerExponent( int exp10 );

   /*! #
    */
   bool IsAutoAdjustEditWidth() const
   {
      return autoEditWidth;
   }

   /*! #
    */
   void EnableAutoAdjustEditWidth( bool enable = true )
   {
      if ( (autoEditWidth = enable) != false )
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

   // -------------------------------------------------------------------------

protected:

   double   value;                  // current value
   double   lowerBound, upperBound; // valid range
   unsigned precision      :  4;    // number of decimal digits in non-sci mode, [0,15]
   bool     isReal         :  1;    // whether this is a real or integer parameter
   bool     autoEditWidth  :  1;    // set width of edit control automatically
   bool     scientific     :  1;    // scientific notation enabled?
   bool     busy           :  1;    // prevent reentrant events
   int                     : 24;
   int      sciTriggerExp;          // exponent (of ten) to trigger sci notation

   value_event_handler onValueUpdated;
   Control*            onValueUpdatedReceiver;

   virtual void UpdateControls();

   virtual void __EditCompleted( Edit& sender );
   virtual void __ReturnPressed( Edit& sender );
   virtual void __GetFocus( Control& sender );
   virtual void __LoseFocus( Control& sender );
   virtual void __MousePress( Control& sender,
      const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
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

   virtual void __ValueUpdated( Slider& sender, int value );
   virtual void __GetFocus( Control& sender );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_NumericControl_h

// ****************************************************************************
// EOF pcl/NumericControl.h - Released 2014/10/29 07:34:06 UTC
