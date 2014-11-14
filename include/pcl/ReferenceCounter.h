// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/ReferenceCounter.h - Released 2014/11/14 17:16:41 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

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
 * \brief Thread-safe reference counter.
 *
 * %ReferenceCounter is used internally by most PCL container classes to
 * implement reference-counted <em>shared data</em>.
 *
 * %ReferenceCounter is a thread-safe reference counter because it implements
 * its internal counter as an instance of the AtomicInt class. This means that
 * counter increment and decrement are implemented as atomic operations.
 *
 * Shared data is a fundamental technique for optimization of memory resources
 * and execution speed in classes and algorithms that involve copying and
 * passing objects between functions. Basically, a set of objects can share the
 * same data with the condition that the data remains unmodified. In this case,
 * the set of objects is said to be \e referencing a single data item, and all
 * objects in the set are considered as \e aliases of the data. When an alias
 * object tries to modify its referenced data, a new copy is generated
 * on-demand and the object uniquely references the newly generated data, which
 * can then be freely modified. This mechanism works in a completely automatic
 * and transparent way, and %ReferenceCounter just provides a counter to keep
 * track of the number of aliased objects that reference the same data at a
 * given time, plus a number of useful related functions.
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
   ReferenceCounter() : count( 1 )
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
    * aliased object references the data being controlled with this counter.
    *
    * \note This attachment operation is thread-safe: the internal reference
    * operation is atomic.
    *
    * \sa Detach()
    */
   void Attach()
   {
      (void)count.Reference();
   }

   /*!
    * Decrements the reference counter by one unit. This happens when an object
    * ceases to reference the data being controlled with this counter.
    *
    * Returns true if the reference counter is greater than zero; false if the
    * reference counter becomes zero after decrementing. When the reference
    * counter is zero, the data being controlled with this counter is no longer
    * referenced by any object and can be safely destroyed.
    *
    * \note This detachment operation is thread-safe: the internal dereference
    * operation is atomic.
    *
    * \sa Attach()
    */
   bool Detach()
   {
#ifdef __PCL_REFCOUNT_CHECK_DETACHMENT
      if ( count == 0 )
         throw Error( "<* Warning *> Detach(): Invalid reference count." );
#else
      PCL_PRECONDITION( count != 0 )
#endif
      return count.Dereference();
   }

   /*!
    * Returns true if the data being controlled with this counter is only
    * referenced by a unique object.
    *
    * \note For a version of this function that performs an atomic integer
    * comparison of the reference counter, see IsUniqueAtomic().
    *
    * \sa IsUniqueAtomic(), Attach(), Detach(), IsGarbage()
    */
   bool IsUnique() const
   {
      return count < 2;
   }

   /*!
    * Returns true if the data being controlled with this counter is only
    * referenced by a unique object.
    *
    * \note This operation is thread-safe: the integer comparison has been
    * implemented as an atomic operation.
    *
    * \sa IsUnique(), Attach(), Detach(), IsGarbage()
    */
   bool IsUniqueAtomic() const
   {
      return const_cast<ReferenceCounter*>( this )->count.FetchAndAdd( 0 ) < 2;
      //return const_cast<ReferenceCounter*>( this )->count.TestAndSet( 1, 1 );
   }

   /*!
    * Returns true if the data being controlled with this counter is not
    * referenced by any object. In such case, the data can be safely destroyed,
    * since no object depends on it.
    *
    * \warning To check if this reference counter is zero (and hence the data
    * is no longer referenced), it is more efficient and secure to use the
    * value returned by Detach(), instead of this member function, when
    * applicable. If Detach() returns false, then this counter is zero after an
    * atomic dereference operation.
    *
    * \sa IsUnique(), Attach(), Detach()
    */
   bool IsGarbage() const
   {
      return count == 0;
   }

private:

   AtomicInt count;
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_ReferenceCounter_h

// ****************************************************************************
// EOF pcl/ReferenceCounter.h - Released 2014/11/14 17:16:41 UTC
