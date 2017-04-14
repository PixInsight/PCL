//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/Allocator.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_Allocator_h
#define __PCL_Allocator_h

/// \file pcl/Allocator.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Relational.h>
#include <pcl/Utility.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Allocator
 * \brief Provides memory allocation for PCL containers.
 *
 * %Allocator inherits directly from its template argument A, which
 * corresponds to a <em>block allocator</em> class. A block allocator is
 * responsible for allocation and deallocation of untyped blocks of contiguous
 * memory. %Allocator inherits block allocation capabilities and specializes
 * them for allocation of a particular type T.
 *
 * The type T must have default and copy construction semantics.
 *
 * Besides the default and copy constructors, the block allocator class A must
 * define the following member functions:
 *
 * \code
 * size_type A::MaxSize() const
 * \endcode
 *
 * Returns the maximum size in bytes that the block allocator is able to
 * allocate as a contiguous memory block.
 *
 * \code
 * size_type A::BlockSize( size_type n ) const
 * \endcode
 *
 * Returns the size in bytes of an <em>allocation block</em> suitable to store
 * at least \a n contiguous bytes. %Allocator uses this block size to reserve
 * memory in chunks of optimized length. This greatly improves efficiency of
 * containers because it minimizes the frequency of allocation/deallocation
 * operations.
 *
 * \code
 * size_type A::ReallocatedBlockSize( size_type currentSize, size_type newSize ) const
 * \endcode
 *
 * Returns the size in bytes of a reallocated block. \a currentSize is the
 * current size in bytes of the block being reallocated, and \a newSize is the
 * requested block size. This function is similar to A::BlockSize(), but it is
 * called for reallocation of already existing data blocks, for example before
 * deleting a subset of container elements.
 *
 * \code
 * void* AllocateBlock( size_type sz )
 * \endcode
 *
 * Custom allocation routine. Allocates a contiguous memory block of length
 * \a sz in bytes, and returns the address of the first byte in the newly
 * allocated block.
 *
 * \code
 * void DeallocateBlock( void* p )
 * \endcode
 *
 * Custom deallocation routine. Deallocates a contiguous block of memory that
 * has been previously allocated by any allocator of class A.
 *
 * StandardAllocator is an example of a block allocator that uses the standard
 * \c new and \c delete operators.
 *
 * \sa StandardAllocator
 */
template <class T, class A>
class PCL_CLASS Allocator : public A
{
public:

   /*!
    * Default constructor.
    */
   Allocator() : A()
   {
   }

   /*!
    * Constructs an %Allocator instance as a copy of other block allocator.
    */
   Allocator( const A& a ) : A( a )
   {
   }

   /*!
    * Copy constructor.
    */
   Allocator( const Allocator<T,A>& x ) : A( x )
   {
   }

   /*!
    * Allocates a contiguous block of memory, sufficient to store at least \a n
    * instance of class T. Optionally, allocates the necessary space for \a n
    * objects plus \a extra additional bytes.
    *
    * Returns the starting address of the allocated block.
    *
    * \note This member function <em>does not construct</em> any T instance;
    * it only allocates the required memory to store \a n instances of T.
    */
   T* Allocate( size_type n, size_type extra = 0 )
   {
      PCL_PRECONDITION( n+extra > 0 )
      return (T*)this->AllocateBlock( n*sizeof( T )+extra );
      //return (T*)new ( *this ) uint8[ n*sizeof( T )+extra ];
   }

   /*!
    * Deallocates a block of memory.
    *
    * \note This member function <em>does not destruct</em> any T instance; it
    * only deallocates a previously allocated block where an unspecified number
    * of T instances might be stored (either constructed or not).
    */
   void Deallocate( T* p )
   {
      PCL_PRECONDITION( p != nullptr )
      this->DeallocateBlock( (void*)p );
      //this->operator delete( (void*)p );
   }

   /*!
    * Returns the maximum number of instances of class T that this allocator
    * can allocate.
    */
   size_type MaxLength() const
   {
      return A::MaxSize()/sizeof( T );
   }

   /*!
    * Returns the length of a newly allocated data block.
    *
    * Given a number \a n of T instances, returns the corresponding <em>paged
    * length</em> for this allocator. The paged length is the actual number of
    * T instances that would be allocated instead of \a n, which depends on the
    * block allocation policy implemented by the allocator class.
    *
    * The value returned by this member function is always greater than or
    * equal to \a n.
    *
    * \sa ShrunkLength()
    */
   size_type PagedLength( size_type n ) const
   {
      return A::BlockSize( n*sizeof( T ) )/sizeof( T );
   }

   /*!
    * Returns the length of a reallocated data block.
    *
    * \param currentLength The current length of an allocated data block.
    *
    * \param newLength     The new length of the reallocated data block.
    *
    * The returned length is the actual number of T instances that would be
    * allocated instead of \a newLength, which depends on the block allocation
    * policy implemented by the allocator class.
    *
    * The value returned by this member function is always greater than or
    * equal to \a n.
    *
    * \sa PagedLength()
    */
   size_type ReallocatedLength( size_type currentLength, size_type newLength ) const
   {
      return A::ReallocatedBlockSize( currentLength*sizeof( T ), newLength*sizeof( T ) )/sizeof( T );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup object_construction_destruction Construction and&nbsp;\
 * Destruction of Objects with Explicit Allocation
 */

/*!
 * Constructs an object with storage at address \a p and allocator \a a. This
 * function invokes the default constructor of class T for the object stored at
 * \a p.
 * \sa Allocator
 * \ingroup object_construction_destruction
 */
template <class T, class A> inline void Construct( T* p, A& a )
{
   PCL_PRECONDITION( p != nullptr )
   new( (void*)p, a )T();
}

/*!
 * Constructs an object with storage at address \a p, initial value \a v, and
 * allocator \a a. This function invokes the copy constructor of class T, with
 * argument \a v, for the object stored at \a p.
 * \sa Allocator
 * \ingroup object_construction_destruction
 */
template <class T, class T1, class A> inline void Construct( T* p, const T1& v, A& a )
{
   PCL_PRECONDITION( p != nullptr )
   new( (void*)p, a )T( v );
}

#ifdef _MSC_VER
#  pragma warning( push )
#  pragma warning( disable : 4100 ) // unreferenced formal parameter
#endif                              // (VC++ limitation !?)

/*!
 * Destroys an object stored at address \a p. Invokes the destructor of
 * class T for the object stored at \a p.
 * \sa Allocator
 * \ingroup object_construction_destruction
 */
template <class T> inline void Destroy( T* p )
{
   PCL_PRECONDITION( p != nullptr )
   p->~T();
}

/*!
 * Destroys a contiguous sequence of objects. Invokes the destructor of class
 * T for each object in the range [p,q).
 * \sa Allocator
 * \ingroup object_construction_destruction
 */
template <class T> inline void Destroy( T* p, T* q )
{
   PCL_PRECONDITION( p != nullptr && q != nullptr )
   for ( ; p < q; ++p )
      p->~T();
}

#ifdef _MSC_VER
#  pragma warning( pop )
#endif

inline void Destroy( void* )        {}
inline void Destroy( void*, void* ) {}

inline void Destroy( bool* )        {}
inline void Destroy( bool*, bool* ) {}

inline void Destroy( signed char* )               {}
inline void Destroy( signed char*, signed char* ) {}

inline void Destroy( unsigned char* )                 {}
inline void Destroy( unsigned char*, unsigned char* ) {}

inline void Destroy( wchar_t* )           {}
inline void Destroy( wchar_t*, wchar_t* ) {}

inline void Destroy( signed int* )              {}
inline void Destroy( signed int*, signed int* ) {}

inline void Destroy( unsigned int* )                {}
inline void Destroy( unsigned int*, unsigned int* ) {}

inline void Destroy( signed short* )                {}
inline void Destroy( signed short*, signed short* ) {}

inline void Destroy( unsigned short* )                  {}
inline void Destroy( unsigned short*, unsigned short* ) {}

inline void Destroy( signed long* )               {}
inline void Destroy( signed long*, signed long* ) {}

inline void Destroy( unsigned long* )                 {}
inline void Destroy( unsigned long*, unsigned long* ) {}

inline void Destroy( signed long long* )                    {}
inline void Destroy( signed long long*, signed long long* ) {}

inline void Destroy( unsigned long long* )                      {}
inline void Destroy( unsigned long long*, unsigned long long* ) {}

inline void Destroy( float* )         {}
inline void Destroy( float*, float* ) {}

inline void Destroy( double* )          {}
inline void Destroy( double*, double* ) {}

inline void Destroy( long double* )               {}
inline void Destroy( long double*, long double* ) {}

inline void Destroy( void** )         {}
inline void Destroy( void**, void** ) {}

inline void Destroy( bool** )         {}
inline void Destroy( bool**, bool** ) {}

inline void Destroy( signed char** )                {}
inline void Destroy( signed char**, signed char** ) {}

inline void Destroy( unsigned char** )                  {}
inline void Destroy( unsigned char**, unsigned char** ) {}

inline void Destroy( wchar_t** )            {}
inline void Destroy( wchar_t**, wchar_t** ) {}

inline void Destroy( signed int** )               {}
inline void Destroy( signed int**, signed int** ) {}

inline void Destroy( unsigned int** )                 {}
inline void Destroy( unsigned int**, unsigned int** ) {}

inline void Destroy( signed short** )                 {}
inline void Destroy( signed short**, signed short** ) {}

inline void Destroy( unsigned short** )                   {}
inline void Destroy( unsigned short**, unsigned short** ) {}

inline void Destroy( signed long** )                {}
inline void Destroy( signed long**, signed long** ) {}

inline void Destroy( unsigned long** )                  {}
inline void Destroy( unsigned long**, unsigned long** ) {}

inline void Destroy( signed long long** )                     {}
inline void Destroy( signed long long**, signed long long** ) {}

inline void Destroy( unsigned long long** )                       {}
inline void Destroy( unsigned long long**, unsigned long long** ) {}

inline void Destroy( float** )          {}
inline void Destroy( float**, float** ) {}

inline void Destroy( double** )           {}
inline void Destroy( double**, double** ) {}

inline void Destroy( long double** )                {}
inline void Destroy( long double**, long double** ) {}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Allocator_h

// ----------------------------------------------------------------------------
// EOF pcl/Allocator.h - Released 2017-04-14T23:04:40Z
