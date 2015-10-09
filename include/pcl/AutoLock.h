//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/AutoLock.h - Released 2015/10/08 11:24:12 UTC
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

#ifndef __PCL_AutoLock_h
#define __PCL_AutoLock_h

/// \file pcl/AutoLock.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Atomic_h
#include <pcl/Atomic.h>
#endif

#ifndef __PCL_Mutex_h
#include <pcl/Mutex.h>
#endif

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
 * \sa Mutex
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
    * Locks the monitored mutex object, if it has not been previously locked.
    */
   void Lock()
   {
      if ( m_mutex != nullptr && m_lock.TestAndSet( 0, 1 ) )
         m_mutex->Lock();
   }

   /*!
    * Unlocks the monitored mutex object, if it has been previously locked.
    */
   void Unlock()
   {
      if ( m_mutex != nullptr && m_lock.TestAndSet( 1, 0 ) )
         m_mutex->Unlock();
   }

private:

   pcl::Mutex* m_mutex;
   AtomicInt   m_lock;

   // Cannot copy an AutoLock object
   AutoLock( const AutoLock& ) = delete;
   void operator =( const AutoLock& ) = delete;
};

// ----------------------------------------------------------------------------

/*!
 * \def   Synchronized
 * \brief A macro used to protect a simple fragment of code with a Mutex object.
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
// EOF pcl/AutoLock.h - Released 2015/10/08 11:24:12 UTC
