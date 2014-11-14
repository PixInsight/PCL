// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/NumericControl.cpp - Released 2014/11/14 17:17:00 UTC
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

#include <pcl/NumericControl.h>
#include <pcl/Parser.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static int ActualPrecision( int precision, double value )
{
   return Max( 0, precision - Max( 0, TruncI( Log( Abs( value ) ) ) ) );
}

NumericEdit::NumericEdit( Control& parent ) : Control( parent ),
value( 0 ),
lowerBound( 0 ), upperBound( 1 ),
precision( 6 ),
isReal( true ),
autoEditWidth( true ),
scientific( false ),
busy( false ),
sciTriggerExp( -1 ),
onValueUpdated( 0 ), onValueUpdatedReceiver( 0 )
{
   label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   label.OnMousePress( (Control::mouse_button_event_handler)&NumericEdit::__MousePress, *this );

   edit.OnEditCompleted( (Edit::edit_event_handler)&NumericEdit::__EditCompleted, *this );
   edit.OnReturnPressed( (Edit::edit_event_handler)&NumericEdit::__ReturnPressed, *this );
   edit.OnGetFocus( (Control::event_handler)&NumericEdit::__GetFocus, *this );
   edit.OnLoseFocus( (Control::event_handler)&NumericEdit::__LoseFocus, *this );

   sizer.SetSpacing( 4 );
   sizer.Add( label );
   sizer.Add( edit );

   SetSizer( sizer );
   AdjustToContents();

#if defined( __PCL_MACOSX ) // ### REMOVEME: This is a workaround for some problematic behavior of QMacStyle
   SetFixedHeight( int( edit.Font().Height() * 1.7 ) );
#else
   SetFixedHeight();
#endif

   SetChildToFocus( edit );

   // Transparent background
   SetBackgroundColor( 0 );
}

// ----------------------------------------------------------------------------

void NumericEdit::SetValue( double v )
{
   value = Range( isReal ? v : Round( v ), lowerBound, upperBound );
   UpdateControls();
}

// ----------------------------------------------------------------------------

String NumericEdit::ValueAsString( double v ) const
{
   v = Range( v, lowerBound, upperBound );

   if ( IsReal() )
   {
      if ( IsScientificNotationEnabled() )
         if ( sciTriggerExp < 0 || v != 0 && (Abs( v ) > Pow10I<double>()( +sciTriggerExp ) ||
                                              Abs( v ) < Pow10I<double>()( -sciTriggerExp )) )
            return String().Format( "%.*e", precision, v );

      return String().Format( "%.*f", ActualPrecision( precision, v ), v );
   }

   return String().Format( "%.0f", v );
}

// ----------------------------------------------------------------------------

int NumericEdit::MinEditWidth() const
{
   int n = int( Max( ValueAsString( lowerBound ).Length(), ValueAsString( upperBound ).Length() ) );
   return edit.Font().Width( String( '0', n+1 ) ) + 1+2+2+1;
}

// ----------------------------------------------------------------------------

void NumericEdit::AdjustEditWidth()
{
   edit.SetFixedWidth( MinEditWidth() );
   AdjustToContents();
}

// ----------------------------------------------------------------------------

void NumericEdit::SetReal( bool real )
{
   if ( real != isReal )
   {
      isReal = real;
      if ( !isReal )
         value = Round( value );
      if ( autoEditWidth )
         AdjustEditWidth();
      SetValue( value );
   }
}

// ----------------------------------------------------------------------------

void NumericEdit::SetRange( double lr, double ur )
{
   lowerBound = Min( lr, ur );
   upperBound = Max( lr, ur );
   if ( autoEditWidth )
      AdjustEditWidth();
   SetValue( value );
}

// ----------------------------------------------------------------------------

void NumericEdit::SetPrecision( int n )
{
   precision = unsigned( Range( n, 0, 15 ) );
   if ( autoEditWidth )
      AdjustEditWidth();
   UpdateControls();
}

// ----------------------------------------------------------------------------

void NumericEdit::EnableScientificNotation( bool enable )
{
   scientific = enable;
   if ( autoEditWidth )
      AdjustEditWidth();
   UpdateControls();
}

// ----------------------------------------------------------------------------

void NumericEdit::SetScientificNotationTriggerExponent( int exp10 )
{
   sciTriggerExp = exp10;
   if ( autoEditWidth )
      AdjustEditWidth();
   UpdateControls();
}

// ----------------------------------------------------------------------------

void NumericEdit::OnValueUpdated( value_event_handler f, Control& c )
{
   __PCL_NO_ALIAS_HANDLER;
   if ( f == 0 || c.IsNull() )
   {
      onValueUpdated = 0;
      onValueUpdatedReceiver = 0;
   }
   else
   {
      onValueUpdated = f;
      onValueUpdatedReceiver = &c;
   }
}

// ----------------------------------------------------------------------------

void NumericEdit::UpdateControls()
{
   edit.SetText( ValueAsString() );
}

// ----------------------------------------------------------------------------

void NumericEdit::__EditCompleted( Edit& sender )
{
   if ( busy )
      return;

   if ( sender.IsReadOnly() )
      return;

   busy = true;

   try
   {
      double newValue;
      bool hasChanged = false;

      if ( isReal )
      {
         double tolerance = Pow10I<double>()( -int( precision + 1 ) );
         newValue = ParseReal( sender.Text(), lowerBound, upperBound, tolerance );
         newValue = Round( newValue, ActualPrecision( precision, newValue ) );
         hasChanged = Sign( newValue ) != Sign( value ) || Abs( newValue - value ) > tolerance;
      }
      else
      {
         newValue = ParseInteger( sender.Text(), lowerBound, upperBound );
         hasChanged = newValue != value;
      }

      if ( hasChanged )
      {
         value = newValue;
         UpdateControls();

         if ( onValueUpdated != 0 )
            (onValueUpdatedReceiver->*onValueUpdated)( *this, value );
      }
      else
         UpdateControls();

      busy = false;

      return;
   }
   ERROR_HANDLER

   busy = false;

   UpdateControls();
}

void NumericEdit::__ReturnPressed( Edit& /*sender*/ )
{
}

// ----------------------------------------------------------------------------

void NumericEdit::__GetFocus( Control& /*sender*/ )
{
   //if ( !edit.IsReadOnly() )
   //   edit.SelectAll();
}

// ----------------------------------------------------------------------------

void NumericEdit::__LoseFocus( Control& /*sender*/ )
{
   if ( !edit.IsReadOnly() )
      __EditCompleted( edit );
}

// ----------------------------------------------------------------------------

void NumericEdit::__MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( !edit.IsReadOnly() )
      if ( sender == label )
      {
         __EditCompleted( edit );
         edit.Focus();
         edit.SelectAll();
      }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

NumericControl::NumericControl( Control& parent ) : NumericEdit( parent )
{
   slider.SetRange( 0, 50 );
   slider.SetMinWidth( 50+16 );
   slider.SetFixedHeight( Height() );
   slider.SetPageSize( 5 );
   slider.SetTickInterval( 5 );
   slider.SetTickStyle( TickStyle::NoTicks );
   slider.SetFocusStyle( FocusStyle::Click );
   slider.OnGetFocus( (Control::event_handler)&NumericControl::__GetFocus, *this );
   slider.OnValueUpdated( (Slider::value_event_handler)&NumericControl::__ValueUpdated, *this );

   sizer.Add( slider, 100 );

   AdjustToContents();
}

// ----------------------------------------------------------------------------

void NumericControl::UpdateControls()
{
   NumericEdit::UpdateControls();

   int i0, i1;
   slider.GetRange( i0, i1 );
   slider.SetValue( i0 + RoundI( (value - lowerBound)/(upperBound - lowerBound)*(i1 - i0) ) );
}

// ----------------------------------------------------------------------------

void NumericControl::__ValueUpdated( Slider& sender, int v )
{
   int i0, i1;
   sender.GetRange( i0, i1 );
   double d = i1 - i0;
   double newValue = Round( lowerBound + (upperBound - lowerBound)*((v - i0)/d),
                            isReal ? Max( 0, TruncI( Log( d ) ) ) : 0 );

   if ( newValue != value )
   {
      value = newValue;

      edit.SetText( ValueAsString() );

      if ( onValueUpdated != 0 )
         (onValueUpdatedReceiver->*onValueUpdated)( *this, value );
   }
}

// ----------------------------------------------------------------------------

void NumericControl::__GetFocus( Control& sender )
{
   if ( sender == slider )
   {
      if ( !edit.IsReadOnly() )
      {
         edit.Focus();
         edit.SelectAll();
      }
   }
   else
   {
      NumericEdit::__GetFocus( sender );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/NumericControl.cpp - Released 2014/11/14 17:17:00 UTC
