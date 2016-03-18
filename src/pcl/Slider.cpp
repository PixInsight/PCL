//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/Slider.cpp - Released 2016/02/21 20:22:19 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/Math.h>
#include <pcl/Slider.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#  pragma warning( disable: 4355 ) // 'this' : used in base member initializer list
#endif

/*
 * ### TODO: Create a new class to encapsulate the common behavior of SpinBox
 *           and Slider. Proposed class name: RangeControl
 */

Slider::Slider( Control& parent, bool vertical ) :
   Control( (*API->Slider->CreateSlider)( ModuleHandle(), this, vertical, parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateSlider" );
}

// ----------------------------------------------------------------------------

int Slider::Value() const
{
   return (*API->Slider->GetSliderValue)( handle );
}

// ----------------------------------------------------------------------------

void Slider::SetValue( int value )
{
   (*API->Slider->SetSliderValue)( handle, value );
}

// ----------------------------------------------------------------------------

double Slider::NormalizedValue() const
{
   int v0, v1; GetRange( v0, v1 );
   return (v0 != v1) ? double( Value() - v0 )/(v1 - v0) : 0.0;
}

// ----------------------------------------------------------------------------

void Slider::SetNormalizedValue( double f )
{
   int v0, v1; GetRange( v0, v1 );
   SetValue( v0 + RoundI( Range( f, 0.0, 1.0 )*(v1 - v0) ) );
}

// ----------------------------------------------------------------------------

void Slider::GetRange( int& minValue, int& maxValue ) const
{
   (*API->Slider->GetSliderRange)( handle, &minValue, &maxValue );
}

// ----------------------------------------------------------------------------

void Slider::SetRange( int minValue, int maxValue )
{
   (*API->Slider->SetSliderRange)( handle, minValue, maxValue );
}

// ----------------------------------------------------------------------------

int Slider::StepSize() const
{
   return (*API->Slider->GetSliderStepSize)( handle );
}

// ----------------------------------------------------------------------------

void Slider::SetStepSize( int stepSize )
{
   (*API->Slider->SetSliderStepSize)( handle, stepSize );
}

// ----------------------------------------------------------------------------

int Slider::PageSize() const
{
   return (*API->Slider->GetSliderPageSize)( handle );
}

// ----------------------------------------------------------------------------

void Slider::SetPageSize( int pageSize )
{
   (*API->Slider->SetSliderPageSize)( handle, pageSize );
}

// ----------------------------------------------------------------------------

int Slider::TickInterval() const
{
   return (*API->Slider->GetSliderTickInterval)( handle );
}

// ----------------------------------------------------------------------------

void Slider::SetTickInterval( int interval )
{
   (*API->Slider->SetSliderTickInterval)( handle, interval );
}

// ----------------------------------------------------------------------------

TickStyles Slider::TickStyle() const
{
   return TickStyle::mask_type( (*API->Slider->GetSliderTickStyle)( handle ) );
}

// ----------------------------------------------------------------------------

void Slider::SetTickStyle( TickStyles style )
{
   (*API->Slider->SetSliderTickStyle)( handle, style );
}

// ----------------------------------------------------------------------------

bool Slider::IsTrackingEnabled() const
{
   return (*API->Slider->GetSliderTrackingEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Slider::EnableTracking ( bool enable )
{
   (*API->Slider->SetSliderTrackingEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Slider*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class SliderEventDispatcher
{
public:

   static void api_func ValueUpdated( control_handle hSender, control_handle hReceiver, int32 value )
   {
      if ( handlers->onValueUpdated != nullptr )
         (receiver->*handlers->onValueUpdated)( *sender, value );
   }

   static void api_func RangeUpdated( control_handle hSender, control_handle hReceiver, int32 minValue, int32 maxValue )
   {
      if ( handlers->onRangeUpdated != nullptr )
         (receiver->*handlers->onRangeUpdated)( *sender, minValue, maxValue );
   }

}; // SliderEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void Slider::OnValueUpdated( value_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Slider->SetSliderValueUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? SliderEventDispatcher::ValueUpdated : 0 ) == api_false )
      throw APIFunctionError( "SetSliderValueUpdatedEventRoutine" );
   m_handlers->onValueUpdated = f;
}

void Slider::OnRangeUpdated( range_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Slider->SetSliderRangeUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? SliderEventDispatcher::RangeUpdated : 0 ) == api_false )
      throw APIFunctionError( "SetSliderRangeUpdatedEventRoutine" );
   m_handlers->onRangeUpdated = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Slider.cpp - Released 2016/02/21 20:22:19 UTC
