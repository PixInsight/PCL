//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/Indirect.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_Indirect_h
#define __PCL_Indirect_h

/// \file pcl/Indirect.h

#include <pcl/Defs.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class IndirectUnaryFunction
 * \brief A wrapper class that applies a unary function to pointers to objects.
 *
 * The UF template argument represents a unary function type of the form:
 *
 * <tt>void (*UF)( T )</tt>
 *
 * The TPtr template argument represents a pointer to UF's argument type. TPtr
 * is also the argument type of %IndirectUnaryFunction::operator ()().
 *
 * %IndirectUnaryFunction can be used as UF, but acting on pointers to objects
 * of type T.
 */
template <class TPtr, class UF>
class PCL_CLASS IndirectUnaryFunction
{
public:

   /*!
    * Constructs an %IndirectUnaryFunction to wrap a unary function \a f.
    */
   IndirectUnaryFunction( UF f ) : function( f )
   {
   }

   /*!
    * Copy constructor.
    */
   IndirectUnaryFunction( IndirectUnaryFunction<TPtr, UF>& x ) : function( x.function )
   {
   }

   /*!
    * Function call operator. Applies the wrapped unary function to \a *ptr.
    */
   void operator()( TPtr ptr ) const
   {
      if ( ptr != nullptr )
         function( *ptr );
   }

private:

   UF function;
};

// ----------------------------------------------------------------------------

/*!
 * \class IndirectBinaryFunction
 * \brief A wrapper class that applies a binary function to pointers to objects.
 *
 * The BF template argument represents a binary function type of the form:
 *
 * <tt>void (*BF)( T1, T2 )</tt>
 *
 * The T1Ptr and T2Ptr template arguments represent pointers to BF's first and
 * second argument types, respectively. T1Ptr and T2Ptr are also the argument
 * types of %IndirectBinaryFunction::operator ()().
 *
 * %IndirectBinaryFunction can be used as BF, but acting on pointers to objects
 * of the types T1 and T2.
 */
template <class T1Ptr, class T2Ptr, class BF>
class PCL_CLASS IndirectBinaryFunction
{
public:

   /*!
    * Constructs an %IndirectBinaryFunction to wrap a binary function \a f.
    */
   IndirectBinaryFunction( BF f ) : function( f )
   {
   }

   /*!
    * Copy constructor.
    */
   IndirectBinaryFunction( const IndirectBinaryFunction<T1Ptr, T2Ptr, BF>& x ) : function( x.function )
   {
   }

   /*!
    * Function call operator. Applies the wrapped binary function to \a *ptr1
    * and \a *ptr2.
    */
   void operator()( T1Ptr ptr1, T2Ptr ptr2 ) const
   {
      if ( ptr1 != nullptr && ptr2 != nullptr )
         function( *ptr1, *ptr2 );
   }

private:

   BF function;
};

// ----------------------------------------------------------------------------

/*!
 * \class IndirectUnaryPredicate
 * \brief A wrapper class that applies a unary predicate to pointers to objects.
 *
 * The UP template argument represents a unary predicate type of the form:
 *
 * <tt>bool (*UP)( T )</tt>
 *
 * The TPtr template argument represents a pointer to UP's argument type. TPtr
 * is also the argument type of %IndirectUnaryPredicate::operator ()().
 *
 * %IndirectUnaryPredicate can be used as UP, but acting on pointers to objects
 * of type T.
 */
template <class TPtr, class UP>
class PCL_CLASS IndirectUnaryPredicate
{
public:

   /*!
    * Constructs an %IndirectUnaryFunction to wrap a unary predicate \a p.
    */
   IndirectUnaryPredicate( UP p ) : predicate( p )
   {
   }

   /*!
    * Copy constructor.
    */
   IndirectUnaryPredicate( const IndirectUnaryPredicate<TPtr, UP>& x ) : predicate( x.predicate )
   {
   }

   /*!
    * Function call operator. Applies the wrapped unary predicate to \a *ptr
    * and returns its resulting value.
    */
   bool operator()( TPtr ptr ) const
   {
      return ptr != nullptr && predicate( *ptr );
   }

private:

   UP predicate;
};

// ----------------------------------------------------------------------------

/*!
 * \class IndirectBinaryPredicate
 * \brief A wrapper class that applies a binary predicate to pointers to objects.
 *
 * The BP template argument represents a binary predicate type of the form:
 *
 * <tt>bool (*BP)( T1, T2 )</tt>
 *
 * The T1Ptr and T2Ptr template arguments represent pointers to BP's first and
 * second argument types, respectively. T1Ptr and T2Ptr are also the argument
 * types of %IndirectBinaryPredicate::operator ()().
 *
 * %IndirectBinaryPredicate can be used as BP, but acting on pointers to
 * objects of the types T1 and T2.
 */
template <class T1Ptr, class T2Ptr, class BP>
class PCL_CLASS IndirectBinaryPredicate
{
public:

   /*!
    * Constructs an %IndirectBinaryPredicate to wrap a binary predicate \a p.
    */
   IndirectBinaryPredicate( BP p ) : predicate( p )
   {
   }

   /*!
    * Copy constructor.
    */
   IndirectBinaryPredicate( const IndirectBinaryPredicate<T1Ptr, T2Ptr, BP>& x ) : predicate( x.predicate )
   {
   }

   /*!
    * Function call operator. Applies the wrapped binary predicate to \a *ptr1
    * and \a *ptr2, and returns its resulting value.
    */
   bool operator()( T1Ptr ptr1, T2Ptr ptr2 ) const
   {
      return ptr1 != nullptr && ptr2 != nullptr && predicate( *ptr1, *ptr2 );
   }

private:

   BP predicate;
};

// ----------------------------------------------------------------------------

/*!
 * \class IndirectEqual
 * \brief A functional class that tests two pointers for equality of the
 *        pointed objects.
 */
template <class TPtr>
struct PCL_CLASS IndirectEqual
{
   bool operator ()( TPtr ptr1, TPtr ptr2 ) const
   {
      return (ptr1 != nullptr && ptr2 != nullptr) ? *ptr1 == *ptr2 : ptr1 == ptr2;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class IndirectLess
 * \brief A functional class that applies the <em>less than</em> relational
 *        operator to the objects pointed to by two pointers.
 */
template <class TPtr>
struct PCL_CLASS IndirectLess
{
   bool operator ()( TPtr ptr1, TPtr ptr2 ) const
   {
      return ptr1 != nullptr && (ptr2 == nullptr || *ptr1 < *ptr2);
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Indirect_h

// ----------------------------------------------------------------------------
// EOF pcl/Indirect.h - Released 2017-07-09T18:07:07Z
