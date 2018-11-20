//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/Timer.cpp - Released 2018-11-01T11:06:52Z
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

#include <pcl/AutoLock.h>
#include <pcl/Math.h>
#include <pcl/Timer.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Timer*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))

class TimerEventDispatcher
{
public:

   static void api_func TimerNotify( timer_handle hSender, control_handle hReceiver )
   {
      ++sender->count;
      if ( sender->onTimer != nullptr )
         (receiver->*sender->onTimer)( *sender );
   }

}; // TimerEventDispatcher

#undef sender
#undef receiver

// ----------------------------------------------------------------------------

Timer::Timer() :
   UIObject( (*API->Timer->CreateTimer)( ModuleHandle(), this, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateTimer" );
}

// ----------------------------------------------------------------------------

void Timer::OnTimer( timer_event_handler f, Control& receiver )
{
   onTimer = nullptr;
   count = 0;
   if ( (*API->Timer->SetTimerNotifyEventRoutine)( handle, &receiver,
        (f != nullptr) ? TimerEventDispatcher::TimerNotify : nullptr ) == api_false )
   {
      throw APIFunctionError( "SetTimerNotifyEventRoutine" );
   }
   onTimer = f;
}

// ----------------------------------------------------------------------------

Timer& Timer::Null()
{
   static Timer* nullTimer = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullTimer == nullptr )
      nullTimer = new Timer( reinterpret_cast<void*>( 0 ) );
   return *nullTimer;
}

// ----------------------------------------------------------------------------

double Timer::Interval() const
{
   uint32 msec = 0;
   (*API->Timer->GetTimerInterval)( handle, &msec );
   return 0.001*double( msec );
}

// ----------------------------------------------------------------------------

void Timer::SetInterval( double sec )
{
   count = 0;
   (*API->Timer->SetTimerInterval)( handle, (uint32)(1000*Round( sec, 3 )) );
}

// ----------------------------------------------------------------------------

bool Timer::IsSingleShot() const
{
   return (*API->Timer->GetTimerSingleShot)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Timer::SetSingleShot( bool singleShot )
{
   count = 0;
   (*API->Timer->SetTimerSingleShot)( handle, singleShot );
}

// ----------------------------------------------------------------------------

bool Timer::IsRunning() const
{
   return (*API->Timer->IsTimerActive)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Timer::Start()
{
   count = 0;
   if ( (*API->Timer->StartTimer)( handle ) == api_false )
      throw APIFunctionError( "StartTimer" );
}

// ----------------------------------------------------------------------------

void Timer::Stop()
{
   (*API->Timer->StopTimer)( handle );
}

// ----------------------------------------------------------------------------

void* Timer::CloneHandle() const
{
   throw Error( "Cannot clone a Timer handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Timer.cpp - Released 2018-11-01T11:06:52Z
