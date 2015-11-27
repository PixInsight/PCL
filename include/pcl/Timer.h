//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// pcl/Timer.h - Released 2015/11/26 15:59:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_Timer_h
#define __PCL_Timer_h

/// \file pcl/Timer.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Control_h
#include <pcl/Control.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Timer
 * \brief Client-side interface to a PixInsight %Timer object.
 *
 * A %Timer object can work in two ways:
 *
 * \li As a <em>periodic timer</em>, which generates a sequence of timer events
 * at regular time intervals.
 *
 * \li As a <em>single-shot timer</em>, which generates a single timer event
 * after a prescribed time interval.
 *
 * ### TODO: Write a detailed description for %Timer.
 */
class PCL_CLASS Timer : public UIObject
{
public:

   /*!
    * Constructs a %Timer object.
    */
   Timer();

   /*!
    * Destroys a %Timer object.
    */
   virtual ~Timer()
   {
   }

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since timers are unique objects by definition, calling this member
    * function has no effect.
    */
   virtual void EnsureUnique()
   {
      // Timers are unique objects by definition
   }

   /*!
    * Returns a reference to a null %Timer instance. A null %Timer does not
    * correspond to an existing timer in the PixInsight core application.
    */
   static Timer& Null();

   /*!
    * Returns the timer interval in seconds.
    */
   double Interval() const;

   /*!
    * Sets the timer interval in seconds.
    */
   void SetInterval( double seconds );

   /*!
    * Returns true iff this is a single-shot timer.
    */
   bool IsSingleShot() const;

   /*!
    * Returns true iff this is a periodic timer.
    *
    * This is a convenience member function, equivalent to
    * !IsSingleShot()
    */
   bool IsPeriodic() const
   {
      return !IsSingleShot();
   }

   /*!
    * Enables or disables single-shot mode for this %Timer object.
    */
   void SetSingleShot( bool = true );

   /*!
    * Enables or disables periodic mode for this %Timer object.
    */
   void SetPeriodic( bool p = true )
   {
      SetSingleShot( !p );
   }

   /*!
    * Returns true iff this %Timer object is active. An active timer is
    * generating timer events.
    */
   bool IsRunning() const;

   /*!
    * Activates this %Timer object. Starts generation of timer event(s).
    */
   void Start();

   /*!
    * Stops this %Timer object, if it is currently active.
    */
   void Stop();

   /*!
    * Returns the number of timer events generated after the last call to
    * Start().
    */
   unsigned Count() const
   {
      return count;
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnTimer( Timer& sender );

   /*!
    * \defgroup timer_event_handlers Timer Event Handlers
    */

   /*!
    * Defines the prototype of a <em>timer event handler</em>.
    *
    * A timer event is generated by an active %Timer object, either at regular
    * intervals (periodic timer), or just once when the timer period elapses
    * (single-shot timer).
    *
    * \param sender  The control that sends a timer event.
    *
    * \ingroup timer_event_handlers
    */
   typedef void (Control::*timer_event_handler)( Timer& sender );

   /*!
    * Sets the timer event handler for this %Timer object.
    *
    * \param handler    The timer event handler. Must be a member function of
    *                   the receiver object's class.
    *
    * \param receiver   The control that will receive timer events from this
    *                   %Timer.
    *
    * \ingroup timer_event_handlers
    */
   void OnTimer( timer_event_handler handler, Control& receiver );

private:

   timer_event_handler  onTimer;

   unsigned count;

   Timer( void* );
   virtual void* CloneHandle() const;

   friend class TimerEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_Timer_h

// ----------------------------------------------------------------------------
// EOF pcl/Timer.h - Released 2015/11/26 15:59:39 UTC
