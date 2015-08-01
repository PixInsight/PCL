//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/PixelAllocator.h - Released 2015/07/30 17:15:18 UTC
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

#ifndef __PCL_PixelAllocator_h
#define __PCL_PixelAllocator_h

/// \file pcl/PixelAllocator.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_SharedPixelData_h
#include <pcl/SharedPixelData.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

template <class P> class PCL_CLASS GenericImage;

// ----------------------------------------------------------------------------

/*!
 * \class PixelAllocator
 * \brief Manages transparent allocation and deallocation of shared and local
 *        pixel data.
 *
 * %PixelAllocator is responsible for allocation and deallocation of pixel data
 * blocks in PCL. The template argument P corresponds to an instantiation of
 * GenericPixelTraits for a specific pixel sample type.
 *
 * Unless you are implementing geometrical transformations, or processes that
 * perform direct substitutions of pixel data blocks in images, such as a whole
 * channel, you usually should not have to use the %PixelAllocator template
 * class directly in your code. The SharedPixelData and GenericImage classes
 * can perform all the necessary allocations transparently.
 *
 * When you have to allocate or deallocate pixel data blocks, however, you
 * should \e never use the \c new and \c delete operators, or the \c malloc,
 * \c calloc and \c free standard C functions. You must use %PixelAllocator for
 * all allocations and deallocations of pixel data \e exclusively. Among other
 * good reasons for this, pixel data blocks cannot be allocated in your
 * module's local heap if they are being used with shared images, i.e. images
 * living in the PixInsight core application.
 *
 * The \e only safe way to allocate and deallocate pixel data in PCL is by
 * using %PixelAllocator's member functions. Note that you cannot allocate
 * %PixelAllocator directly. For an existing image, you get a reference to a
 * %PixelAllocator instance by calling the Allocator() member function of
 * %GenericImage. This member function returns a reference to the image's
 * internal allocator, which will perform all pixel allocation tasks that you
 * may need transparently, irrespective of whether the object represents a
 * local or shared image.
 *
 * \sa GenericPixelTraits, GenericImage, SharedPixelData
 */
template <class P>
class PCL_CLASS PixelAllocator : public SharedPixelData
{
public:

   /*!
    * Represents the pixel traits class used by this instantiation of
    * %PixelAllocator.
    *
    * The \c pixel_traits type should be an instantiation of
    * GenericPixelTraits. It identifies a class implementing basic storage and
    * functional primitives optimized for a particular pixel sample type.
    */
   typedef P                                 pixel_traits;

   /*!
    * Represents the data type used to store pixel sample values in this
    * template instantiation of %PixelAllocator.
    */
   typedef typename pixel_traits::sample     sample;

   /*!
    * Returns true if this allocator and another instance are working for the
    * same shared image, or if both of them are working for local images.
    *
    * All local images share a unique internal allocator, but each shared image
    * has its own, independent allocator object. This responds to the
    * multithreaded nature of the PixInsight core application.
    */
   bool operator ==( const PixelAllocator<P>& x ) const
   {
      return SharedPixelData::operator ==( x );
   }

   /*!
    * Allocates a contiguous block of memory where at least \a n > 0 pixel
    * samples can be optimally stored.
    *
    * Returns the starting address of the allocated block.
    *
    * This member function throws a \c std::bad_alloc exception if there is not
    * enough memory available to allocate the required contiguous block.
    */
   sample* AllocatePixels( size_type n ) const
   {
      PCL_PRECONDITION( n != 0 )
      return reinterpret_cast<sample*>( SharedPixelData::Allocate( n*sizeof( sample ) ) );
   }

   /*!
    * Allocates a contiguous block of memory where at least \a width *
    * \a height pixel samples can be optimally stored.
    *
    * Returns the starting address of the allocated block. This is a
    * convenience alias for:
    *
    * \code
    * AllocatePixels( size_type( width )*size_type( height ) );
    * \endcode
    */
   sample* AllocatePixels( int width, int height ) const
   {
      return AllocatePixels( size_type( width )*size_type( height ) );
   }

   /*!
    * Allocates an array where at least \a n > 0 <em>channel slots</em> can be
    * optimally stored. A channel slot is a pointer to a contiguous block of
    * pixel samples, where an image can store a single channel.
    *
    * The allocated array is initialized to zero. Returns the starting address
    * of the allocated array.
    *
    * This member function throws a \c std::bad_alloc exception if there is not
    * enough memory available to allocate the required contiguous block.
   */
   sample** AllocateChannelSlots( size_type n ) const
   {
      PCL_PRECONDITION( n != 0 )
      sample** cslots = reinterpret_cast<sample**>( SharedPixelData::Allocate( n*sizeof( sample* ) ) );
      ::memset( cslots, 0, n*sizeof( sample* ) );
      return cslots;
   }

   /*!
    * Deallocates a previously allocated memory block. The deallocated block
    * becomes available for subsequent allocations.
    *
    * \warning <em>Do not</em> use this function to deallocate memory that has
    * not been allocated by \e this %PixelAllocator object. This includes
    * memory blocks allocated by the global \c new operator, or by other
    * %PixelAllocator objects. Failure to follow this rule will lead to severe
    * heap corruption for the calling module.
    */
   template <typename T>
   void Deallocate( T* p ) const
   {
      PCL_PRECONDITION( p != nullptr )
      SharedPixelData::Deallocate( reinterpret_cast<void*>( p ) );
   }

private:

   PixelAllocator() :
      SharedPixelData()
   {
   }

   PixelAllocator( void* handle ) :
      SharedPixelData( handle, P::BitsPerSample(), P::IsFloatSample(), P::IsComplexSample() )
   {
   }

   PixelAllocator( const PixelAllocator<P>& x ) :
      SharedPixelData( x )
   {
   }

   PixelAllocator( int width, int height, int numberOfChannels, int colorSpace ) :
      SharedPixelData( width, height, numberOfChannels, P::BitsPerSample(), P::IsFloatSample(), colorSpace )
   {
   }

   PixelAllocator& operator =( const PixelAllocator<P>& x )
   {
      (void)SharedPixelData::operator =( x );
      return *this;
   }

   sample** GetSharedData() const
   {
      return reinterpret_cast<sample**>( SharedPixelData::GetSharedData() );
   }

   void SetSharedData( sample** ptrToShared )
   {
      SharedPixelData::SetSharedData( reinterpret_cast<void**>( ptrToShared ) );
   }

   friend class pcl::GenericImage<P>;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PixelAllocator_h

// ----------------------------------------------------------------------------
// EOF pcl/PixelAllocator.h - Released 2015/07/30 17:15:18 UTC
