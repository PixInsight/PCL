//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/StatusMonitor.cpp - Released 2017-06-21T11:36:44Z
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

#include <pcl/AutoLock.h>
#include <pcl/IndirectArray.h>
#include <pcl/Mutex.h>
#include <pcl/StatusMonitor.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>

#include <pcl/api/APIInterface.h>

#ifdef __PCL_UNIX
#  include <time.h> // for nanosleep()
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Update period
 */
unsigned StatusMonitor::s_msRefreshRate = 250U; // milliseconds for the frontend and Windows
#ifndef __PCL_WINDOWS
static long s_nsRefreshRate = 250000000L;       // nanoseconds on Linux and all Unices
#endif

// ----------------------------------------------------------------------------

/*
 * Process monitor dispatcher thread
 */

typedef IndirectArray<StatusMonitor>   active_monitor_list;
static active_monitor_list s_activeMonitors;

static Mutex s_mutex;

class MonitorDispatcherThread : public Thread
{
public:

   MonitorDispatcherThread() : Thread()
   {
   }

   virtual void Run()
   {
      for ( ;; )
      {
         if ( s_mutex.TryLock() )
         {
            int activeCount = 0;
            for ( active_monitor_list::iterator i = s_activeMonitors.Begin(); i != s_activeMonitors.End(); ++i )
               if ( *i != nullptr )
                  if ( (*i)->IsCompleted() || (*i)->IsAborted() )
                     *i = nullptr;
                  else
                  {
                     ++activeCount;

                     /*
                      * Don't update monitors that have not been incremented yet.
                      * Otherwise unbounded monitors could become completed
                      * prematurely due to 'm_count == m_total == 0' conditions.
                      */
                     if ( (*i)->m_count != 0 )
                        (*i)->m_needsUpdate = true;
                  }

            if ( activeCount == 0 )
               s_activeMonitors.Clear();

            s_mutex.Unlock();

            if ( activeCount == 0 )
               return;
         }

#ifdef __PCL_WINDOWS
         ::Sleep( StatusMonitor::s_msRefreshRate );
#else
         struct timespec ts;
         ts.tv_sec = 0;
         ts.tv_nsec = s_nsRefreshRate;
         ::nanosleep( &ts, 0 );
#endif
      }
   }
};

static MonitorDispatcherThread* s_thread = nullptr;

// ----------------------------------------------------------------------------

/*
 * Monitor initialization
 *
 * Initializes monitor control data, adds this monitor to the active monitor
 * list, and initializes the dispatcher thread.
 */
void StatusMonitor::Initialize( const String& s, size_type n )
{
   if ( !IsInitializationEnabled() )
      return;

   // If we are reinitializing, remove this monitor from the active queue.
   s_mutex.Lock();
   s_activeMonitors.Remove( this );
   s_mutex.Unlock();

   // Initialize monitor control data.
   m_initialized = m_completed = m_aborted = m_needsUpdate = false;
   m_retVal = 0;
   m_info = s;
   m_total = n;
   m_count = 0;

   // Notify monitor initialization, and possibly abort.
   if ( m_callback != nullptr )
   {
      try
      {
         if ( (m_retVal = m_callback->Initialized( *this )) != 0 )
            throw ProcessAborted();
      }
      catch ( ... )
      {
         m_aborted = true;
         throw;
      }
   }

   m_initialized = true;

   // Add this monitor to the active monitor queue.
   s_mutex.Lock();
   s_activeMonitors.Add( this );
   s_mutex.Unlock();

   // Initialize the monitor thread.
   if ( s_thread == nullptr )
      s_thread = new MonitorDispatcherThread;
   if ( !s_thread->IsActive() )
      s_thread->Start( ThreadPriority::Lowest );
}

void StatusMonitor::SetRefreshRate( unsigned ms )
{
   volatile AutoLock lock( s_mutex );
   s_msRefreshRate = Range( ms, 25U, 999U );
#ifndef __PCL_WINDOWS
   s_nsRefreshRate = s_msRefreshRate * 1000000L;
#endif
}

// ----------------------------------------------------------------------------

void StatusMonitor::Reset()
{
   volatile AutoLock lock( s_mutex );

   s_activeMonitors.Remove( this );

   m_callback = nullptr;
   m_initialized = m_completed = m_aborted = m_needsUpdate = false;
   m_initDisableCount = m_retVal = 0;
   m_info.Clear();
   m_total = m_count = 0;
}

void StatusMonitor::Assign( const StatusMonitor& x )
{
   volatile AutoLock lock( s_mutex );

   s_activeMonitors.Remove( this );

   m_callback         = x.m_callback;
   m_initialized      = x.m_initialized;
   m_completed        = x.m_completed;
   m_aborted          = x.m_aborted;
   m_needsUpdate      = x.m_needsUpdate;
   m_initDisableCount = x.m_initDisableCount;
   m_retVal           = x.m_retVal;
   m_info             = x.m_info;
   m_total            = x.m_total;
   m_count            = x.m_count;

   if ( s_activeMonitors.Contains( &x ) )
      s_activeMonitors.Add( this );
}

void StatusMonitor::Update()
{
   m_needsUpdate = false;

   if ( !(m_completed || m_aborted) ) // one and only one call to m_callback->Completed()
   {
      if ( m_count == m_total )
         m_completed = true;

      if ( m_callback != nullptr )
      {
         m_retVal = m_completed ? m_callback->Completed( *this ) :
                                  m_callback->Updated( *this );
         if ( m_retVal != 0 )
         {
            m_aborted = true;
            throw ProcessAborted();
         }
      }
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/StatusMonitor.cpp - Released 2017-06-21T11:36:44Z
