//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/StdAlloc.h - Released 2016/02/21 20:22:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_StdAlloc_h
#define __PCL_StdAlloc_h

/// \file pcl/StdAlloc.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#include <new>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class StandardAllocator
 * \brief A block allocator class that uses the standard new and delete
 *        operators.
 *
 * %StandardAllocator is a <em>block allocator</em> class. It is used by
 * default for all container classes in PCL. It defines default block sizing
 * and growing strategies that work efficiently in most cases.
 *
 * For a complete descrption of block allocators and their fundamental role in
 * PCL, read the documentation for the Allocator class.
 *
 * \sa Allocator
 */
class PCL_CLASS StandardAllocator
{
public:

   /*!
    * Constructs a %StandardAllocator object.
    */
   StandardAllocator() : m_fastGrowth( true )
   {
   }

   /*!
    * Copy constructor.
    */
   StandardAllocator( const StandardAllocator& ) = default;

   /*!
    * Returns the size in bytes of the largest contiguous block that this
    * object is able to allocate.
    *
    * %StandardAllocator can (theoretically) allocate ~size_type( 0 ) bytes.
    *
    * \note This member function is mandatory for a block allocator to be
    * usable by the Allocator class.
    */
   size_type MaxSize() const
   {
      return ~size_type( 0 );
   }

   /*!
    * Returns the size in bytes of an <em>allocation block</em> suitable for
    * storage of at least \a n bytes.
    *
    * See IsFastGrowthEnabled() for more information about block size growing
    * policies in %StandardAllocator.
    *
    * \note This member function is mandatory for a block allocator to be
    * usable by the Allocator class.
    *
    * \sa ReallocatedBlockSize(), IsFastGrowthEnabled()
    */
   size_type BlockSize( size_type n ) const
   {
      // Check for null allocation
      if ( n == 0 )
         return 0;

      // Add 24 bytes for a typical block allocation header
      n += 24;

      // Grow linearly by 8-byte chunks for n < 64 bytes
      if ( n < 64 )
         return ((n >> 3) << 3) + 8-24;

      // Grow exponentially if fast growing is enabled or n < 64K
      if ( n < 65536 || IsFastGrowthEnabled() )
      {
         size_type nn = 64;
         while ( nn < n )
            nn <<= 1;
         return nn - 24;
      }

      // If fast growing is disabled, grow linearly by 4K chunks for n >= 64K
      return ((n >> 12) << 12) + 4096-24;
   }

   /*!
    * Returns the size in bytes of a reallocated data block.
    *
    * \param currentSize  The current size in bytes of an allocated data block.
    *
    * \param newSize      The new size in bytes of the reallocated data block.
    *
    * See IsFastGrowthEnabled() and IsShrinkingEnabled() for information about
    * block size allocation and reallocation policies in %StandardAllocator.
    *
    * \note This member function is mandatory for a block allocator to be
    * usable by the Allocator class.
    *
    * \sa BlockSize(), IsShrinkingEnabled(), IsFastGrowthEnabled()
    */
   size_type ReallocatedBlockSize( size_type currentSize, size_type newSize ) const
   {
      return (currentSize < newSize || m_canShrink) ? BlockSize( newSize ) : currentSize;
   }

   /*!
    * Custom allocation routine. Allocates a contiguous memory block of length
    * \a sz in bytes, and returns the address of the first byte in the
    * newly allocated block.
    *
    * As implemented by %StandardAllocator, this member function simply
    * calls ::operator new( sz ).
    *
    * \note This member function is mandatory for a block allocator to be
    * usable by the Allocator class.
    *
    * \sa DeallocateBlock()
    */
   void* AllocateBlock( size_type sz )
   {
      PCL_PRECONDITION( sz != 0 )
      return ::operator new( sz );
   }

   /*!
    * Custom deallocation routine. Deallocates a previously allocated
    * contiguous memory block that begins at the specified location \a p.
    *
    * As implemented by %StandardAllocator, this member function simply
    * calls ::operator delete( p ).
    *
    * \note This member function is mandatory for a block allocator to be
    * usable by the Allocator class.
    *
    * \sa AllocateBlock()
    */
   void DeallocateBlock( void* p )
   {
      PCL_PRECONDITION( p != nullptr )
      ::operator delete( p );
   }

   /*!
    * Returns true iff <i>fast growth</i> is currently enabled for this
    * allocator.
    *
    * When fast growth is enabled, %StandardAllocator provides block sizes
    * that grow exponentially above 64 bytes: 128, 256, 512, 1K, ... 512K, 1M,
    * and so on. This maximizes speed of dynamic allocation operations, but at
    * the cost of more memory wasted by partially used blocks in
    * contiguous-storage containers such as Array or String.
    *
    * With fast growth disabled, %StandardAllocator follows a more conservative
    * scheme to provide reasonable block sizes while preventing excessive
    * memory consumption. For block sizes below 64 kilobytes, blocks grow
    * exponentially to improve efficiency of allocation for small objects, such
    * as short strings. For blocks larger than 64K, block sizes grow by
    * constant chunks of 4K.
    *
    * The fast block size growing policy is enabled by default.
    *
    * \sa EnableFastGrowth(), DisableFastGrowth(), BlockSize()
    */
   bool IsFastGrowthEnabled() const
   {
      return m_fastGrowth;
   }

   /*!
    * Enables a fast block size growing policy for %StandardAllocator.
    *
    * See IsFastGrowthEnabled() for more information about block size growing
    * policies.
    *
    * \sa IsFastGrowthEnabled(), DisableFastGrowth()
    */
   void EnableFastGrowth( bool enable = true )
   {
      m_fastGrowth = enable;
   }

   /*!
    * Disables the fast block size growing policy for %StandardAllocator.
    *
    * See IsFastGrowthEnabled() for more information about block size growing
    * policies.
    *
    * \sa IsFastGrowthEnabled(), EnableFastGrowth()
    */
   void DisableFastGrowth( bool disable = true )
   {
      EnableFastGrowth( !disable );
   }

   /*!
    * Returns true iff <i>block shrinking</i> is currently enabled for this
    * allocator.
    *
    * When block shrinking is enabled, %StandardAllocator allows size
    * reductions for reallocated blocks in calls to the ReallocatedBlockSize().
    * When block shrinking is disabled, already allocated blocks can only be
    * reallocated with increased lengths.
    *
    * \sa EnableShrinking(), DisableShrinking(), ReallocatedBlockSize()
    */
   bool IsShrinkingEnabled() const
   {
      return m_canShrink;
   }

   /*!
    * Enables a block shrinking policy for %StandardAllocator.
    *
    * See IsShrinkingEnabled() for more information about block shrinking
    * policies.
    *
    * \sa IsShrinkingEnabled(), DisableShrinking()
    */
   void EnableShrinking( bool enable = true )
   {
      m_canShrink = enable;
   }

   /*!
    * Disables a block shrinking policy for %StandardAllocator.
    *
    * See IsShrinkingEnabled() for more information about block shrinking
    * policies.
    *
    * \sa IsShrinkingEnabled(), EnableShrinking()
    */
   void DisableShrinking( bool disable = true )
   {
      EnableShrinking( !disable );
   }

private:

   bool m_fastGrowth : 1;
   bool m_canShrink  : 1;
};

} // pcl

// ----------------------------------------------------------------------------

/* ** !
   Block allocation operator for class StandardAllocator. Allocates a
   contiguous block of memory of length \a sz in bytes, using the
   standard operator new( size_t ).
 */
/*
inline void* operator new( pcl::size_type sz, pcl::StandardAllocator& )
{
   PCL_PRECONDITION( sz != nullptr )
   return ::operator new( sz );
}

#ifdef _MSC_VER

inline void operator delete( void* p, pcl::StandardAllocator& )
{
   PCL_PRECONDITION( p != nullptr )
   ::operator delete( p );
}

#endif
 */

/*!
 * Placement new operator for class StandardAllocator. Returns the specified
 * address \a p.
 */
inline void* operator new( pcl::size_type, void* p, pcl::StandardAllocator& )
{
   PCL_PRECONDITION( p != nullptr )
   return p;
}

#ifdef _MSC_VER

#pragma warning( push )
#pragma warning( disable: 4100 ) // 'unreferenced formal parameter'

inline void operator delete( void* p, void*, pcl::StandardAllocator& )
{
   PCL_PRECONDITION( p != nullptr )
}

#pragma warning( pop )

#endif

// ----------------------------------------------------------------------------

#endif  // __PCL_StdAlloc_h

// ----------------------------------------------------------------------------
// EOF pcl/StdAlloc.h - Released 2016/02/21 20:22:12 UTC
