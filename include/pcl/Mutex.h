//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/Mutex.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_Mutex_h
#define __PCL_Mutex_h

/// \file pcl/Mutex.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Utility.h>

#ifdef __PCL_UNIX
#  include <pcl/Atomic.h>
#  include <stdio.h>
#  include <pthread.h>
#endif

#ifdef __PCL_WINDOWS
#  include <windows.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Mutex
 * \brief Adaptive mutual exclusion lock variable
 *
 * The word \e mutex is an abbreviation for <em>mutual exclusion</em>. A mutex
 * object provides access synchronization for threads. A mutex protects one or
 * more objects or a code section, so that only one thread can access them at a
 * given time.
 *
 * To understand how mutual thread exclusion works and why is it needed,
 * consider the following example code:
 *
 * \code
 * int data;
 *
 * void functionOne()
 * {
 *    data += 2;
 *    data *= 2;
 * }
 *
 * void functionTwo()
 * {
 *    data -= 3;
 *    data *= 3;
 * }
 * \endcode
 *
 * If functionOne() and functionTwo() are called in sequence, this happens:
 *
 * \code
 * data = 3;
 * functionOne();   // data is now = 10
 * functionTwo();   // data is now = 21
 * \endcode
 *
 * Now suppose that we define two threads that call the above functions:
 *
 * \code
 * class ThreadOne : public Thread
 * {
 *    //...
 *    virtual void Run()
 *    {
 *       functionOne();
 *    }
 * };
 *
 * class ThreadTwo : public Thread
 * {
 *    //...
 *    virtual void Run()
 *    {
 *       functionTwo();
 *    }
 * };
 * \endcode
 *
 * If we start both threads in sequence:
 *
 * \code
 * data = 3;
 * ThreadOne one;
 * ThreadTwo two;
 * one.Start();
 * two.Start();
 * \endcode
 *
 * then the following might happen:
 *
 * \code
 * // ThreadOne calls functionOne:
 * data += 2;    // data is now = 5
 *
 * // ThreadTwo calls functionTwo. This causes ThreadOne to enter a wait state
 * // until ThreadTwo terminates:
 * data -= 3;    // data is now = 2
 * data *= 3;    // data is now = 6
 *
 * // ThreadOne resumes execution:
 * data *= 2;    // data is now = 12
 * \endcode
 *
 * Because both threads can access data in any order, and there is no
 * guarantee as to when a given thread starts execution, the result may not be
 * what we expect (we get 12 instead of 21). Adding synchronization with a
 * %Mutex object in functionOne() and functionTwo() solves the problem:
 *
 * \code
 * int data;
 * Mutex mutex;
 *
 * void functionOne()
 * {
 *    mutex.Lock();
 *    data += 2;
 *    data *= 2;
 *    mutex.Unlock();
 * }
 *
 * void functionTwo()
 * {
 *    mutex.Lock();
 *    data -= 3;
 *    data *= 3;
 *    mutex.Unlock();
 * }
 * \endcode
 *
 * A mutex can only be locked by a single thread at a time. After Lock() has
 * been called from a thread T, other threads that call Lock() on the same
 * mutex object block their execution until the thread T calls Unlock().
 *
 * To attempt locking a mutex without blocking execution, the Mutex::TryLock()
 * member function can be used. This can provide much higher performance than
 * Mutex::Lock() when the calling threads don't depend on gaining exclusive
 * access to the shared data being protected by the mutex object.
 *
 * %Mutex implements <em>spinning locking</em>, a technique that can also
 * improve performance by avoiding expensive semaphore wait operations under
 * high levels of contention. See the documentation for Mutex::Mutex( int ) and
 * Mutex::Lock() for more information.
 *
 * %Mutex has been implemented as a low-level PCL class that does not depend on
 * the PixInsight Core application. On Windows platforms, %Mutex has been
 * implemented as a wrapper to a <em>critical section</em>. On UNIX/Linux
 * platforms, %Mutex uses atomic int operations implemented as inline assembly
 * code and direct calls to the pthreads library.
 *
 * \sa AutoLock, ReadWriteMutex
 */
class PCL_CLASS Mutex
{
public:

   /*!
    * Constructs a %Mutex object.
    *
    * \param spin Maximum number of <em>spinning loops</em> to do before
    *             performing a semaphore wait operation when a thread attempts
    *             to lock this mutex and it has already been locked by another
    *             thread. If this mutex becomes unlocked during the spinning
    *             loops, the expensive wait operation can be avoided. The spin
    *             count must be >= 0. The default value is 512.
    */
   Mutex( int spin = 512 ) :
      m_spinCount( Max( 0, spin ) )
   {
#ifdef __PCL_WINDOWS
      (void)InitializeCriticalSectionAndSpinCount( &criticalSection, DWORD( m_spinCount ) );
#else
      (void)PThreadInitMutex();
#endif
   }

   /*!
    * Destroys a %Mutex object.
    *
    * \warning Destroying a locked %Mutex object may result in undefined
    * (mostly catastrophic) behavior. Always make sure that a mutex has been
    * unlocked before destroying it.
    */
   virtual ~Mutex()
   {
#ifdef __PCL_WINDOWS
      DeleteCriticalSection( &criticalSection );
#else
      (void)PThreadDestroyMutex();
#endif
   }

   /*!
    * Copy constructor. This constructor is disabled because mutexes are unique
    * objects.
    */
   Mutex( const Mutex& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because mutexes are unique
    * objects.
    */
   Mutex& operator =( const Mutex& ) = delete;

   /*!
    * Locks this %Mutex object.
    *
    * When a mutex has been locked in a thread T, other threads cannot lock it
    * until the thread T unlocks it. When a thread attempts to lock a %Mutex
    * object that has been previously locked, it blocks its execution until the
    * %Mutex object is unlocked.
    *
    * If the mutex has already been locked by another thread, this routine
    * performs a number of <em>spin loops</em> before doing an (expensive) wait
    * operation on a semaphore associated with this mutex object. If this
    * mutex becomes unlocked during the spinning loops, the wait operation can
    * be avoided to lock the mutex in the calling thread. This can greatly
    * improve efficiency of multithreaded code under high levels of contention
    * (e.g. several running threads that depend on frequent concurrent accesses
    * to shared data). For fine control and performance tuning, the maximum
    * number of spinning loops performed can be specified as a parameter to the
    * Mutex::Mutex( int ) constructor.
    */
   void Lock()
   {
#ifdef __PCL_WINDOWS
      EnterCriticalSection( &criticalSection );
#else
      for ( int spin = m_spinCount; ; )
      {
         // Is the mutex free? If so, get it now and don't look back!
         if ( m_lockState.TestAndSet( 0, 1 ) )
         {
            (void)PThreadLockMutex();
            break;
         }

         if ( --spin < 0 )
         {
            // Either no spinning, or spinned to no avail...
            // Block thread until we can get this mutex. This is expensive.
            (void)PThreadLockMutex();
            m_lockState.Store( 1 );
            break;
         }
      }
#endif
   }

   /*!
    * Unlocks this %Mutex object.
    *
    * See the Lock() documentation for more information.
    */
   void Unlock()
   {
#ifdef __PCL_WINDOWS
      LeaveCriticalSection( &criticalSection );
#else
      m_lockState.Store( 0 );
      (void)PThreadUnlockMutex();
#endif
   }

   /*!
    * Function call operator. This is a convenience operator that performs the
    * lock and unlock operations in an alternative, perhaps more elegant way.
    *
    * \param lock    Whether the mutex should be locked (when true) or unlocked
    *                (when \a lock is false).
    *
    * For example, the following code snippet:
    *
    * \code
    * Mutex mutex;
    * //...
    * mutex( true );
    * // some code to be protected
    * mutex( false );
    * \endcode
    *
    * is equivalent to:
    *
    * \code
    * Mutex mutex;
    * //...
    * mutex.Lock();
    * // some code to be protected
    * mutex.Unlock();
    * \endcode
    */
   void operator ()( bool lock = true )
   {
      if ( lock )
         Lock();
      else
         Unlock();
   }

   /*!
    * Attempts locking this %Mutex object. Returns true iff this mutex has been
    * successfully locked.
    *
    * Unlike Lock(), this function does not block execution of the calling
    * thread if this mutex cannot be locked.
    */
   bool TryLock()
   {
#ifdef __PCL_WINDOWS
      return TryEnterCriticalSection( &criticalSection ) != FALSE;
#else
      // ### N.B.: This code is performance and stability critical. DO NOT
      // modify it unless you are absolutely sure of what you are doing.
      return m_lockState == 0 && m_lockState.TestAndSet( 0, 1 ) && PThreadLockMutex();
#endif
   }

   /*!
    * Returns the spin count of this %Mutex object.
    *
    * The spin count is a read-only property that can only be set upon object
    * construction. For information on mutex spin counts, refer to %Mutex's
    * constructor: Mutex::Mutex( int ).
    */
   int SpinCount() const
   {
      return m_spinCount;
   }

private:

#ifdef __PCL_WINDOWS

   CRITICAL_SECTION criticalSection;

#else // Linux/UNIX

   AtomicInt       m_lockState; // 0=unlocked, 1=acquired
   pthread_mutex_t m_mutex;

   bool PThreadInitMutex()
   {
      return PThreadCheckError( pthread_mutex_init( &m_mutex, 0 ), "pthread_mutex_init" );
   }

   bool PThreadDestroyMutex()
   {
      return PThreadCheckError( pthread_mutex_destroy( &m_mutex ), "pthread_mutex_destroy" );
   }

   bool PThreadLockMutex()
   {
      return PThreadCheckError( pthread_mutex_lock( &m_mutex ), "pthread_mutex_lock" );
   }

   bool PThreadUnlockMutex()
   {
      return PThreadCheckError( pthread_mutex_unlock( &m_mutex ), "pthread_mutex_unlock" );
   }

   static bool PThreadCheckError( int errorCode, const char* funcName )
   {
      if ( errorCode == 0 )
         return true;
      fprintf( stderr, "%s() failed. Error code: %d\n", funcName, errorCode );
      return false;
   }

#endif   // __PCL_WINDOWS

   int m_spinCount;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Mutex_h

// ----------------------------------------------------------------------------
// EOF pcl/Mutex.h - Released 2017-07-09T18:07:07Z
