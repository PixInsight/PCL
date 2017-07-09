//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0393
// ----------------------------------------------------------------------------
// ExponentialNumericControl.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "ExponentialNumericControl.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ExponentialNumericControl::ExponentialNumericControl( Control& parent ) : NumericEdit( parent )
{
   SetReal();
   SetPrecision( 8 );
   SetRange( 0.0, 1.0 );

   m_coefficient.label.Hide();
   m_coefficient.slider.SetRange( 0, 180 ); // step size = 0.05
   m_coefficient.slider.SetScaledMinWidth( 250 );
   m_coefficient.SetReal();
   m_coefficient.SetRange( 1.00, 10.00 );
   m_coefficient.SetPrecision( 2 );
   m_coefficient.SetToolTip( "<p>Coefficient value in the range [1.0,9.99]</p>" );
   m_coefficient.OnValueUpdated( (NumericEdit::value_event_handler)&ExponentialNumericControl::__CoefficientValueUpdated, *this );

   m_exponent.SetRange( -8, 0 );
   m_exponent.SetToolTip( "<p>Exponent in the range [-8,0]</p>" );
   m_exponent.OnValueUpdated( (SpinBox::value_event_handler)&ExponentialNumericControl::__ExponentValueUpdated, *this );

   sizer.Add( m_coefficient, 100 );
   sizer.Add( m_exponent );

   AdjustToContents();
}

void ExponentialNumericControl::__CoefficientValueUpdated( NumericEdit& sender, double value )
{
   SetValue( Range( value, 1.0, 9.99 ) * Pow10I<double>()( m_exponent.Value() ) );
   if ( m_handlers != nullptr )
      if ( m_handlers->onValueUpdated != nullptr )
         (m_handlers->onValueUpdatedReceiver->*m_handlers->onValueUpdated)( *this, Value() );
}

void ExponentialNumericControl::__ExponentValueUpdated( SpinBox& sender, int value )
{
   SetValue( m_coefficient.Value() * Pow10I<double>()( value ) );
   if ( m_handlers != nullptr )
      if ( m_handlers->onValueUpdated != nullptr )
         (m_handlers->onValueUpdatedReceiver->*m_handlers->onValueUpdated)( *this, Value() );
}

void ExponentialNumericControl::UpdateControls()
{
   NumericEdit::UpdateControls();

   double x = Abs( Value() );
   if ( 1 + x == 1 )
   {
      SetValue( 1.0e-08 );
      if ( m_handlers != nullptr )
         if ( m_handlers->onValueUpdated != nullptr )
            (m_handlers->onValueUpdatedReceiver->*m_handlers->onValueUpdated)( *this, Value() );
      return;
   }

   int exponent = int( Floor( Log( x ) ) );

   double coefficient = Round( x/Pow10I<double>()( exponent ), 2 );
   if ( Value() < 0 )
      coefficient = -coefficient;

   m_coefficient.SetValue( coefficient );
   m_exponent.SetValue( exponent );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExponentialNumericControl.cpp - Released 2017-07-09T18:07:33Z
