//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/AutoLock.h - Released 2018-11-01T11:06:36Z
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

#ifndef __PCL_AutoLock_h
#define __PCL_AutoLock_h

/// \file pcl/AutoLock.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Atomic.h>
#include <pcl/Mutex.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class AutoLock
 * \brief Automatic mutex lock/unlock.
 *
 * %AutoLock is a convenience class that simplifies using Mutex objects to
 * protect code or data that can be accessed by multiple threads.
 *
 * An %AutoLock object locks a %Mutex object upon construction and unlocks it
 * upon destruction. This ensures that the state of the %Mutex object will
 * always be well defined, and that it will never be left locked, even in
 * critical situations involving multiple function return points and
 * exceptions.
 *
 * \sa AutoLockCounter, Mutex
 */
class PCL_CLASS AutoLock
{
public:

   /*!
    * Constructs an %AutoLock object to monitor a specified Mutex object.
    *
    * \param mutex   A %Mutex object that will be monitored by this %AutoLock
    *                instance.
    *
    * The specified mutex object will be locked by this constructor. It will be
    * unlocked automatically when this %AutoLock object gets out of scope, or
    * if it is destroyed explicitly.
    */
   explicit AutoLock( pcl::Mutex& mutex ) :
      m_mutex( &mutex ), m_lock( 0 )
   {
      Lock();
   }

   /*!
    * Move constructor.
    */
   AutoLock( AutoLock&& x ) :
      m_mutex( x.m_mutex ), m_lock( x.m_lock )
   {
      x.m_mutex = nullptr;
   }

   /*!
    * Destroys this %AutoLock object.
    *
    * If the monitored mutex object (that was specified in the constructor) is
    * locked, it is unlocked by this destructor.
    */
   ~AutoLock()
   {
      Unlock();
      m_mutex = nullptr;
   }

   /*!
    * Copy constructor. This constructor is disabled because %AutoLock objects
    * cannot be copied.
    */
   AutoLock( const AutoLock& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %AutoLock objects
    * cannot be copied.
    */
   AutoLock& operator =( const AutoLock& ) = delete;

   /*!
    * Move assignment. This operator is disabled because %AutoLock objects
    * cannot be move-assigned.
    */
   AutoLock& operator =( AutoLock&& ) = delete;

   /*!
    * Locks the monitored mutex object, if it has not been previously locked by
    * this object.
    */
   void Lock()
   {
      if ( m_mutex != nullptr )
         if ( m_lock.TestAndSet( 0, 1 ) )
            m_mutex->Lock();
   }

   /*!
    * Unlocks the monitored mutex object, if it has been previously locked by
    * this object.
    */
   void Unlock()
   {
      if ( m_mutex != nullptr )
         if ( m_lock.TestAndSet( 1, 0 ) )
            m_mutex->Unlock();
   }

private:

   pcl::Mutex* m_mutex;
   AtomicInt   m_lock;
};

// ----------------------------------------------------------------------------

/*!
 * \class AutoLockCounter
 * \brief Automatic mutex lock/unlock with limited concurrent access allowance.
 *
 * %AutoLockCounter is similar to AutoLock: it allows protecting a section of
 * code against concurrent thread access through a Mutex, with automatic
 * lock/unlock operations upon object construction/destruction. However,
 * %AutoLockCounter is slightly more complex because it can allow a specified
 * amount of concurrent accesses, while %AutoLock forbids them completely.
 *
 * %AutoLockCounter is useful for scenarios where the protected code can
 * exploit a resource concurrently, but only to a given extent that can be
 * predicted or configured. Typical examples are routines performing file
 * read/write operations.
 *
 * Example:
 *
 * \code
 * void ParallelWriteFile( const String& filePath, const ByteArray& data, int limit )
 * {
 *    static Mutex mutex;
 *    static AtomicInt count;
 *    volatile AutoLockCounter lock( mutex, count, limit );
 *    File::WriteFile( filePath, data );
 * }
 * \endcode
 *
 * In this example, the ParallelWriteFile routine allows creating and writing
 * up to \e limit different disk files simultaneously. If \e limit files are
 * already being written concurrently, a new call to ParallelWriteFile() will
 * block the caller thread until at least one of the running tasks terminates.
 *
 * \sa AutoLock, Mutex, AtomicInt
 */
class PCL_CLASS AutoLockCounter
{
public:

   /*!
    * Constructs an %AutoLockCounter object to monitor a Mutex with automatic
    * counting of lock operations and a given maximum number of concurrent
    * accesses.
    *
    * \param mutex   Reference to a Mutex object that will be monitored by this
    *                %AutoLockCounter instance.
    *
    * \param count   Reference to an AtomicInt object that will be updated by
    *                this instance to control the current amount of concurrent
    *                accesses.
    *
    * \param limit   Maximum number of concurrent accesses allowed. The
    *                specified \a count variable will be atomically incremented
    *                by this constructor. If \a count is greater than \a limit
    *                after the increment, this constructor will lock the
    *                specified \a mutex. Otherwise the mutex will not be locked
    *                automatically upon construction. Note that the mutex can
    *                be locked explicitly by calling the Lock() member function
    *                for any AutoLock or AutoLockCounter object sharing the
    *                same \a mutex variable.
    *
    * By specifying a \a limit of zero, no concurrent access will be allowed
    * and this object will be functionally equivalent to an AutoLock instance
    * monitoring the same \a mutex. By setting \a limit to a value greater than
    * or equal to one, the protected code section will be allowed to run once,
    * twice, etc. without a (potentially expensive) lock operation.
    *
    * If the specified \a mutex has been locked by this object, be it
    * automatically or explicitly, it will be unlocked automatically when this
    * %AutoLockCounter object is destroyed or gets out of scope.
    */
   explicit AutoLockCounter( pcl::Mutex& mutex, AtomicInt& count, int limit ) :
      m_mutex( &mutex ), m_count( &count ), m_lock( 0 )
   {
      if ( m_count->FetchAndAdd( 1 ) >= limit-1 )
      {
         Lock();
         if ( m_count->Load() < limit )
            Unlock();
      }
   }

   /*!
    * Move constructor.
    */
   AutoLockCounter( AutoLockCounter&& x ) :
      m_mutex( x.m_mutex ), m_count( x.m_count ), m_lock( x.m_lock )
   {
      x.m_mutex = nullptr;
      x.m_count = nullptr;
   }

   /*!
    * Destroys this %AutoLockCounter object.
    *
    * This destructor performs two separate actions:
    *
    * - If the monitored mutex (which was specified in the constructor) has
    * been locked by this object, it will be unlocked. This applies both if the
    * mutex has been locked automatically upon construction (because the
    * monitored counter was larger than the specified limit), or explicitly by
    * calling the Lock() member function for this object.
    *
    * - The monitored counter, which was atomically incremented upon
    * construction, will be atomically decremented.
    */
   ~AutoLockCounter()
   {
      Unlock();
      m_mutex = nullptr;
      if ( m_count != nullptr )
      {
         m_count->Decrement();
         m_count = nullptr;
      }
   }

   /*!
    * Copy constructor. This constructor is disabled because %AutoLockCounter
    * objects cannot be copied.
    */
   AutoLockCounter( const AutoLockCounter& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %AutoLockCounter
    * objects cannot be copied.
    */
   AutoLockCounter& operator =( const AutoLockCounter& ) = delete;

   /*!
    * Move assignment. This operator is disabled because %AutoLockCounter
    * objects cannot be move-assigned.
    */
   AutoLockCounter& operator =( AutoLockCounter&& ) = delete;

   /*!
    * Locks the monitored mutex object, if it has not been previously locked by
    * this object.
    */
   void Lock()
   {
      if ( m_mutex != nullptr )
         if ( m_lock.TestAndSet( 0, 1 ) )
            m_mutex->Lock();
   }

   /*!
    * Unlocks the monitored mutex object, if it has been previously locked by
    * this object.
    */
   void Unlock()
   {
      if ( m_mutex != nullptr )
         if ( m_lock.TestAndSet( 1, 0 ) )
            m_mutex->Unlock();
   }

private:

   pcl::Mutex* m_mutex;
   AtomicInt*  m_count;
   AtomicInt   m_lock;
};

// ----------------------------------------------------------------------------

/*!
 * \def   Synchronized
 * \brief A macro to protect a simple fragment of code with a Mutex object.
 *
 * Example of use:
 *
 * \code
 * Mutex mutex;
 * ...
 * Synchronized( mutex, count += img.Width(); )
 * \endcode
 *
 * The protected code should not contain tokens that could invalidate macro
 * argument semantics. To synchronize more complex pieces of code, use an
 * AutoLock object explicitly. For example:
 *
 * \code
 * Mutex mutex;
 * ...
 * {
 *    AutoLock locker( mutex );
 *    ... some code to protect here ...
 * }
 * \endcode
 */
#define Synchronized( mutex, code )    \
{                                      \
   pcl::AutoLock _________( mutex );   \
   code                                \
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_AutoLock_h

// ----------------------------------------------------------------------------
// EOF pcl/AutoLock.h - Released 2018-11-01T11:06:36Z
