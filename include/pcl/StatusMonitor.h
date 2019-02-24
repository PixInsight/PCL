//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/StatusMonitor.h - Released 2019-01-21T12:06:07Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_StatusMonitor_h
#define __PCL_StatusMonitor_h

/// \file pcl/StatusMonitor.h

#include <pcl/Defs.h>

#include <pcl/Exception.h>
#include <pcl/String.h>
#include <pcl/Utility.h>

namespace pcl
{

class PCL_CLASS StatusMonitor;

// ----------------------------------------------------------------------------

/*!
 * \class StatusCallback
 * \brief Provides status monitoring callback functions.
 *
 * %StatusCallback is an abstract base class providing a set of pure virtual
 * functions that must be reimplemented in derived classes. These virtual
 * functions must provide feedback to the user as part of the <em>status
 * monitoring</em> of a process.
 *
 * Status monitoring consists of a set of PCL classes and functions to provide
 * feedback and progress information about ongoing processes. Each instance of
 * a %StatusCallback subclass is directly managed by a <em>status monitor</em>
 * object. A status monitor object is an instance of the StatusMonitor class.
 *
 * Status monitors provide a generalized mechanism to generate progress
 * information while a process is running. A %StatusCallback subclass receives
 * that progress information and provides feedback to the user, controlling the
 * information provided, its format and its appearance.
 *
 * The best example of %StatusCallback subclass is the StandardStatus class.
 * StandardStatus reimplements %StatusCallback's pure virtual functions to show
 * processing progress information on the processing console of the current
 * processing thread.
 *
 * \sa StatusMonitor, StandardStatus, SpinStatus, MuteStatus, Console, Thread
 */
class PCL_CLASS StatusCallback
{
public:

   /*!
    * Constructs a default %StatusCallback object.
    */
   StatusCallback() = default;

   /*!
    * Copy constructor.
    */
   StatusCallback( const StatusCallback& ) = default;

   /*!
    * Move constructor.
    */
   StatusCallback( StatusCallback&& ) = default;

   /*!
    * Destroys this %StatusCallback instance.
    */
   virtual ~StatusCallback()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   StatusCallback& operator =( const StatusCallback& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   StatusCallback& operator =( StatusCallback&& ) = default;

   /*!
    * This function is called by a status \a monitor object when a new
    * monitored process is about to start.
    *
    * The progress <em>total count</em> can be obtained by calling
    * StatusMonitor::Total() for the passed \a monitor object. Similarly, the
    * <em>progress information</em> string is given by StatusMonitor::Info().
    *
    * This function must return zero if the process can continue. If this
    * function returns a nonzero value, the new process is aborted by throwing
    * a ProcessAborted exception.
    */
   virtual int Initialized( const StatusMonitor& monitor ) const = 0;

   /*!
    * Function called by a status \a monitor object to signal an update of the
    * progress count for the current process.
    *
    * The <em>progress count</em> can be obtained by calling
    * StatusMonitor::Count() for the passed \a monitor object. Similarly, the
    * <em>total count</em> is given by StatusMonitor::Total().
    *
    * This function is called repeatedly at regular time intervals until the
    * progress count reaches the total count, or until the process is aborted.
    *
    * This function must return zero if the process can continue. If this
    * function returns a nonzero value, the current process is aborted by
    * throwing a ProcessAborted exception.
    */
   virtual int Updated( const StatusMonitor& monitor ) const = 0;

   /*!
    * Function called by a status \a monitor object to signal that the current
    * process has finished.
    *
    * When this function is invoked by the status \a monitor, its progress
    * count has already reached its total count.
    *
    * This function must return zero if the process can continue. If this
    * function returns a nonzero value, the current process is aborted by
    * throwing a ProcessAborted exception.
    *
    * \note    This function allows aborting a process even when it has
    * finished. This makes sense, since after a process has finished, the
    * PixInsight core application still has some important work to do: update
    * the target view's processing history, organize temporary swap files, send
    * notifications, update masking relations, etc.
    */
   virtual int Completed( const StatusMonitor& monitor ) const = 0;

   /*!
    * Function called by a status \a monitor object when the progress
    * information for the current process has been changed.
    *
    * The <em>progress information</em> is a string that can be obtained by
    * calling StatusMonitor::Info() for the passed \a monitor object.
    */
   virtual void InfoUpdated( const StatusMonitor& monitor ) const = 0;
};

// ----------------------------------------------------------------------------

/*!
 * \class StatusMonitor
 * \brief An asynchronous status monitoring system.
 *
 * Status monitoring consists of a set of PCL classes and functions to manage
 * progress information about ongoing processes. A <em>status monitor</em> is
 * an instance of the %StatusMonitor class.
 *
 * Status monitors provide a generalized mechanism to generate and manage
 * progress information while a process is running. A status monitor sends
 * progress \e events to an instance of a StatusCallback subclass, whose
 * primary responsibility is to provide feedback to the user. Progress feedback
 * typically consists of a running percentage or similar text-based information
 * written to the standard console.
 *
 * Along with providing progress feedback, the status monitoring concept is
 * quite flexible and has other control applications in PCL, some of them quite
 * sophisticated. For example, many standard tools use specialized status
 * monitors to implement their real-time preview routines as asynchronously
 * interruptible processes.
 *
 * %StatusMonitor utilizes a low-priority timing thread to generate callback
 * monitoring calls asynchronously at constant time intervals. This has
 * virtually zero impact on the performance of the monitored processes.
 *
 * \sa StatusCallback, StandardStatus, SpinStatus, MuteStatus, Console, Thread
 */
class PCL_CLASS StatusMonitor
{
public:

   /*!
    * Constructs a default %StatusMonitor object.
    */
   StatusMonitor() = default;

   /*!
    * Constructs a %StatusMonitor object as a duplicate of an existing
    * instance.
    */
   StatusMonitor( const StatusMonitor& x )
   {
      Assign( x );
   }

   /*!
    * Destroys this %StatusMonitor object.
    */
   virtual ~StatusMonitor()
   {
      Clear();
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   StatusMonitor& operator =( const StatusMonitor& x )
   {
      if ( this != &x )
         Assign( x );
      return *this;
   }

   /*!
    * Initializes this status monitor to start a new monitoring procedure.
    *
    * \param info    Progress information.
    *
    * \param count   Total progress count. If a zero total progress count is
    *                specified, this monitor will be <em>unbounded</em>. That
    *                means that the monitor will never reach a completed state
    *                by increasing or incrementing it. To complete an unbounded
    *                status monitor, the Complete() member function must be
    *                called explicitly.
    *
    * If this monitor has an associated status callback object, this function
    * will call the StatusCallback::Initialized() virtual member function of
    * the associated callback object.
    *
    * If this function is invoked while an ongoing monitoring procedure is
    * active, it is interrupted and a new monitoring procedure is initialized.
    */
   void Initialize( const String& info, size_type count = 0 );

   /*!
    * Increments the initialization disabling counter for this status monitor.
    *
    * When the disabling counter is greater than zero, initialization is
    * disabled for this monitor, and subsequent calls to the Initialize()
    * member function have no effect.
    *
    * This is useful if your process calls other subroutines that try to
    * initialize private monitoring procedures. If such subroutines are called
    * as independent processes, they can perform initialization, but if they
    * are used as part of a higher level process, they should update a common
    * status monitor without reinitializing it.
    *
    * Here is an example:
    *
    * \code
    * FunctionA( StatusMonitor& m )
    * {
    *    if ( m.IsInitializationEnabled() )
    *       m.Initialize( "This is function A...", 100 );
    *    // ...
    *    for ( int i = 0; i < 100; ++i, ++m )
    *       // ...
    * }
    *
    * FunctionB( StatusMonitor& m )
    * {
    *    if ( m.IsInitializationEnabled() )
    *       m.Initialize( "This is function B...", 100 );
    *    // ...
    *    for ( int i = 0; i < 10; ++i, m += 10 )
    *       // ...
    * }
    *
    * ALargerFunction( StatusMonitor& m )
    * {
    *    m.Initialize( "This is some high-level routine...", 200 );
    *    m.DisableInitialization();
    *    // ...
    *    FunctionA( m );
    *    FunctionB( m );
    *    // ...
    *    m.EnableInitialization();
    * }
    * \endcode
    *
    * In this example, both FunctionA() and FunctionB() update a %StatusMonitor
    * object by a total count of 100 steps. However, these functions don't try
    * to initialize the passed monitor if initialization has been disabled.
    *
    * ALargerFunction() initializes a status monitor with a total count of 200
    * and its own information string, then disables monitor initialization. The
    * subsequent calls to FunctionA() and FunctionB() will update the monitor
    * by 100 steps each, without reinitializing it.
    *
    * The final call to EnableInitialization() in ALargerFunction() is
    * necessary to return the status monitor to its initial enabled or disabled
    * state.
    */
   void DisableInitialization()
   {
      ++m_initDisableCount;
   }

   /*!
    * Decrements the initialization disabling counter for this status monitor.
    *
    * When the disabling counter is equal or less than zero, initialization is
    * enabled for this monitor.
    *
    * See DisableInitialization() for further information on the initialization
    * enabled/disabled states and a source code example.
    */
   void EnableInitialization()
   {
      --m_initDisableCount;
   }

   /*!
    * Returns true iff monitor initialization has been disabled for this status
    * monitor object.
    *
    * See DisableInitialization() for further information on the initialization
    * enabled/disabled states and a source code example.
    */
   bool IsInitializationEnabled() const
   {
      return m_initDisableCount <= 0;
   }

   /*!
    * Returns true iff this status monitor has been initialized.
    *
    * A status monitor is initialized by a call to its Initialize() member
    * function.
    */
   bool IsInitialized() const
   {
      return m_initialized;
   }

   /*!
    * Returns true iff this status monitor has completed a monitoring procedure.
    *
    * A monitoring procedure is completed if the current monitoring counter
    * reaches the total count, or if the Complete() member function is called.
    */
   bool IsCompleted() const
   {
      return m_completed;
   }

   /*!
    * Returns true iff a monitoring procedure has been aborted.
    *
    * When a monitoring procedure is aborted, the status monitor object throws
    * a ProcessAborted exception.
    *
    * A monitoring procedure can only be aborted by its associated status
    * callback object. This is done by returning a nonzero value from the
    * StatusCallback::Initialized(), StatusCallback::Updated(), or
    * StatusCallback::Completed() virtual member functions implemented by
    * a StatusCallback descendant class.
    */
   bool IsAborted() const
   {
      return m_aborted;
   }

   /*!
    * Changes the progress information text for this status monitor.
    *
    * If there is a status callback object associated with this monitor, its
    * StatusCallback::InfoUpdated() member function is invoked.
    */
   void SetInfo( const String& s )
   {
      m_info = s;
      if ( m_callback != nullptr )
         m_callback->InfoUpdated( *this );
   }

   /*!
    * Increments the progress counter of this status monitor.
    *
    * If the progress counter reaches Total(), and this monitor has an
    * associated status callback object, its StatusCallback::Completed() member
    * function is invoked. If the progress counter is less than Total(), the
    * StatusCallback::Updated() member function will be invoked by the
    * monitoring thread asynchronously.
    *
    * Note that for \e unbounded monitors the completed state is never reached
    * unless a explicit call to Complete() is made, hence the
    * StatusCallback::Completed() member function of the status callback
    * object, if any, will never be called by this increment operator.
    */
   void operator ++()
   {
      if ( ++m_count == m_total || m_needsUpdate  )
         Update();
   }

   /*!
    * Increments the progress counter of this status monitor by the specified
    * number \a n of progress steps.
    *
    * This function behaves essentially like the operator ++() function.
    */
   void operator +=( size_type n )
   {
      m_count += n;
      if ( m_total != 0 ) // if not an unbounded monitor
         if ( m_count > m_total )
            m_count = m_total;
      if ( m_needsUpdate || m_count == m_total )
         Update();
   }

   /*!
    * Forces this monitor to complete the current monitoring procedure, by
    * assigning the total progress count to the progress counter.
    *
    * This function forces a call to StatusCallback::Completed(), if this
    * monitor has an associated status callback object.
    */
   void Complete()
   {
      m_count = m_total;
      Update();
   }

   /*!
    * Returns the address of the immutable status callback object associated
    * with this status monitor. Returns zero if this monitor has no associated
    * status callback object.
    */
   const StatusCallback* Callback() const
   {
      return m_callback;
   }

   /*!
    * Returns the address of the status callback object associated with this
    * status monitor. Returns zero if this monitor has no associated status
    * callback object.
    */
   StatusCallback* Callback()
   {
      return m_callback;
   }

   /*!
    * Associates a status callback object with this status monitor.
    *
    * Calling this function forces a call to Clear(), which interrupts the
    * current monitoring procedure, if any, and reinitializes the internal
    * state of this monitor.
    *
    * To associate no status callback object with this monitor, pass \c nullptr
    * as the argument to this function.
    */
   void SetCallback( StatusCallback* callback )
   {
      Reset();
      m_callback = callback;
   }

   /*!
    * Returns the last status callback return value.
    *
    * The returned value is the return value of the last call to
    * StatusCallback::Initialized(), StatusCallback::Updated() or
    * StatusCallback::Completed() for the associated status callback object, or
    * zero if no status callback object has been associated with this monitor.
    *
    * If a nonzero value is returned, then the current monitoring procedure has
    * already been aborted.
    */
   int ReturnValue() const
   {
      return m_retVal;
   }

   /*!
    * Returns the progress information string that has been set for this status
    * monitor object.
    */
   String Info() const
   {
      return m_info;
   }

   /*!
    * Returns the progress counter for the current monitoring procedure.
    *
    * When the progress counter reaches the total count, the monitoring
    * procedure has been completed.
    */
   size_type Count() const
   {
      return m_count;
   }

   /*!
    * Returns the total progress count for the current monitoring procedure.
    *
    * When the progress counter reaches the total count, the monitoring
    * procedure has been completed.
    */
   size_type Total() const
   {
      return m_total;
   }

   /*!
    * Interrupts the current monitoring procedure, if any, and reinitializes
    * the internal state of this status monitor.
    */
   void Clear()
   {
      Reset();
   }

   /*!
    * Returns the progress monitoring refresh rate in milliseconds.
    *
    * The StatusCallback::Updated() member functions of all active status
    * callback objects are called asynchronously by a low-priority thread. The
    * refresh rate is the interval between successive callback update calls. It
    * can be in the range from 25 to 999 milliseconds.
    */
   static unsigned RefreshRate()
   {
      return s_msRefreshRate;
   }

   /*!
    * Sets a new progress monitoring refresh rate in milliseconds.
    *
    * \param ms   New monitoring refresh rate in milliseconds. The specified
    *             value must be between 25 and 999 milliseconds. If the passed
    *             value is outside that range, it is discarded and the nearest
    *             valid bound is set.
    *
    * The default refresh rate is 250 milliseconds, or 4 monitoring events per
    * second. The new refresh rate does take effect immediately, even if there
    * are active status monitors.
    */
   static void SetRefreshRate( unsigned ms );

private:

   StatusCallback* m_callback = nullptr;
   bool            m_initialized = false;
   bool            m_completed = false;
   bool            m_aborted = false;
   bool            m_needsUpdate = false; // thread-safe flag set by the dispatcher
   int             m_initDisableCount = 0;
   int             m_retVal = 0;
   String          m_info;
   size_type       m_total = 0;
   size_type       m_count = 0;

   static unsigned s_msRefreshRate;

   void Reset();
   void Assign( const StatusMonitor& );
   void Update();

   friend class MonitorDispatcherThread;
};

// ----------------------------------------------------------------------------

}  // pcl

#endif   // __PCL_StatusMonitor_h

// ----------------------------------------------------------------------------
// EOF pcl/StatusMonitor.h - Released 2019-01-21T12:06:07Z
