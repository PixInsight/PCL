//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/ReferenceCounter.h - Released 2015/10/08 11:24:12 UTC
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

#ifndef __PCL_ReferenceCounter_h
#define __PCL_ReferenceCounter_h

/// \file pcl/ReferenceCounter.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Atomic_h
#include <pcl/Atomic.h>
#endif

#ifdef __PCL_REFCOUNT_CHECK_DETACHMENT
# ifndef __PCL_Exception_h
#  include <pcl/Exception.h>
# endif
#else
# ifndef __PCL_Diagnostics_h
#  include <pcl/Diagnostics.h>
# endif
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ReferenceCounter
 * \brief Thread-safe reference counter for copy-on-write data structures.
 *
 * %ReferenceCounter is used internally by most PCL container and image classes
 * to implement reference-counted, copy-on-write <em>shared data</em>.
 *
 * %ReferenceCounter is a thread-safe reference counter because it implements
 * its internal counter as an instance of the AtomicInt class. This means that
 * counter increment and decrement are implemented as atomic operations.
 *
 * Copy-on-write is a fundamental technique for optimization of resources and
 * execution speed by preventing unnecessary duplication of data, especially
 * when objects are copied and aliased frequently but seldom modified.
 * Basically, a set of objects can share the same data with the condition that
 * the data remains unmodified. In this case, the set of objects is said to be
 * \e referencing a single data item, and all objects in the set are considered
 * as \e aliases of the data. When an alias object tries to modify its
 * referenced data, a new copy is generated on-demand and the object uniquely
 * references the newly generated data, which can then be freely modified. This
 * mechanism works in a completely automatic and transparent way, and
 * %ReferenceCounter just provides a thread-safe counter to keep track of the
 * number of aliased objects that reference the same data structure at a given
 * time, plus a number of useful related functions.
 *
 * \sa AtomicInt
 */
class PCL_CLASS ReferenceCounter
{
public:

   /*!
    * Constructs a new %ReferenceCounter object. The reference counter is
    * initialized to one, which means that only one object (the caller) is
    * referencing the data associated with this object.
    */
   ReferenceCounter() : m_count( 1 )
   {
   }

   /*!
    * Destroys a %ReferenceCounter object.
    */
   ~ReferenceCounter()
   {
   }

   /*!
    * Increments the reference counter by one unit. This happens when a new
    * object references the data being controlled with this counter.
    *
    * \note This attachment operation is thread-safe: the internal increment
    * operation is atomic.
    *
    * \sa Detach()
    */
   void Attach()
   {
      m_count.Increment();
   }

   /*!
    * Decrements the reference counter by one unit. This happens when an object
    * ceases to reference the data being controlled with this counter.
    *
    * Returns true if the reference counter is greater than zero after the
    * decrement; false if the reference counter becomes zero. When the
    * reference counter is zero, the data being controlled with this counter is
    * no longer referenced by any object and thus can be safely destroyed.
    *
    * \note This detachment operation is thread-safe: the internal dereference
    * operation is atomic.
    *
    * \sa Attach()
    */
   bool Detach()
   {
      PCL_CHECK( m_count.Load() > 0 )
      return m_count.Dereference();
   }

   /*!
    * Returns the current value of this reference counter.
    *
    * \sa RefCountAtomic(), IsUnique()
    */
   int RefCount() const
   {
      return m_count;
   }

   /*!
    * Returns the current value of this reference counter.
    *
    * \note This operation is thread-safe: the integer load has been
    * implemented as an atomic operation.
    *
    * \sa RefCount(), IsUniqueAtomic()
    */
   int RefCountAtomic() const
   {
      return m_count.Load();
   }

   /*!
    * Returns true iff the data being controlled with this counter is not being
    * referenced by more than one object.
    *
    * \sa IsUniqueAtomic(), Attach(), Detach()
    */
   bool IsUnique() const
   {
      return RefCount() < 2;
   }

   /*!
    * Returns true iff the data being controlled with this counter is not being
    * referenced by more than one object.
    *
    * \note This operation is thread-safe: the integer comparison has been
    * implemented as an atomic operation.
    *
    * \sa IsUnique(), Attach(), Detach()
    */
   bool IsUniqueAtomic() const
   {
      return RefCountAtomic() < 2;
   }

private:

   mutable AtomicInt m_count;

   ReferenceCounter( const ReferenceCounter& ) = delete;
   ReferenceCounter& operator =( const ReferenceCounter& ) = delete;
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_ReferenceCounter_h

// ----------------------------------------------------------------------------
// EOF pcl/ReferenceCounter.h - Released 2015/10/08 11:24:12 UTC
