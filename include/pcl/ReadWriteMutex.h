//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/ReadWriteMutex.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_ReadWriteMutex_h
#define __PCL_ReadWriteMutex_h

/// \file pcl/ReadWriteMutex.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/UIObject.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ReadWriteMutex
 * \brief Client-side interface to a PixInsight read/write mutex object
 *
 * %ReadWriteMutex provides separate thread synchronization for read and write
 * operations. This can be more efficient than plain Mutex synchronization
 * because multiple threads can access a data item for read-only operations
 * simultaneously, which in turn improves concurrency if write accesses are
 * relatively infrequent.
 *
 * \sa Mutex
 */
class PCL_CLASS ReadWriteMutex : public UIObject
{
public:

   /*!
    * Constructs a ReadWriteMutex object.
    */
   ReadWriteMutex();

   /*!
    * Destroys a ReadWriteMutex object.
    *
    * \warning    Destroying a locked %ReadWriteMutex object may result in
    * undefined (mostly catastrophic) behavior. Always ensure that a read/write
    * mutex has been unlocked before destroying it.
    */
   virtual ~ReadWriteMutex()
   {
   }

   /*!
    * Copy constructor. This constructor is disabled because %ReadWriteMutex
    * represents unique server-side objects.
    */
   ReadWriteMutex( const ReadWriteMutex& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %ReadWriteMutex
    * represents unique server-side objects.
    */
   ReadWriteMutex& operator =( const ReadWriteMutex& ) = delete;

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since mutexes are unique objects by definition, calling this member
    * function has no effect.
    */
   virtual void EnsureUnique()
   {
      // ReadWriteMutex instances are unique objects
   }

   /*!
    * Returns a reference to a null %ReadWriteMutex instance. A null
    * %ReadWriteMutex does not correspond to an existing read/write mutex in
    * the PixInsight core application.
    */
   static ReadWriteMutex& Null();

   /*!
    * Locks this read/write mutex object for reading.
    *
    * The calling thread will block its execution if other thread has locked
    * this read/write mutex for writing.
    */
   void LockForRead();

   /*!
    * Locks this read/write mutex object for writing.
    *
    * The calling thread will block its execution if other thread has locked
    * this read/write mutex for reading or writing.
    */
   void LockForWrite();

   /*!
    * Attempts locking this read/write mutex object for reading. Returns true
    * if this mutex has been successfully locked by the calling thread, false
    * otherwise.
    *
    * Locking for reading will only fail if other thread has locked this object
    * for writing. If other threads have locked this object for reading only,
    * the calling thread will also lock it and this function will return true.
    *
    * Unlike LockForRead(), this function does not block execution of the
    * calling thread if this %ReadWriteMutex cannot be locked for reading.
    */
   bool TryLockForRead();

   /*!
    * Attempts locking this read/write mutex object for writing. Returns true if
    * this mutex has been successfully locked by the calling thread, false
    * otherwise.
    *
    * Locking for writing will fail if other thread has locked this object for
    * reading or writing.
    *
    * Unlike LockForWrite(), this function does not block execution of the
    * calling thread if this %ReadWriteMutex cannot be locked for writing.
    */
   bool TryLockForWrite();

   /*!
    * Unlocks this %ReadWriteMutex object.
    */
   void Unlock();

private:

   ReadWriteMutex( void* h ) : UIObject( h )
   {
   }

   virtual void* CloneHandle() const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ReadWriteMutex_h

// ----------------------------------------------------------------------------
// EOF pcl/ReadWriteMutex.h - Released 2017-04-14T23:04:40Z
