//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/Indirect.h - Released 2018-11-23T16:14:19Z
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
    * Constructs an %IndirectUnaryFunction to wrap a unary m_function \a f.
    */
   IndirectUnaryFunction( UF f ) : m_function( f )
   {
   }

   /*!
    * Copy constructor.
    */
   IndirectUnaryFunction( IndirectUnaryFunction<TPtr, UF>& ) = default;

   /*!
    * Function call operator. Applies the wrapped unary function to \a *ptr.
    *
    * If \a ptr is nullptr, the function is not invoked and calling this member
    * function has no effect.
    */
   void operator()( TPtr ptr ) const
   {
      if ( ptr != nullptr )
         m_function( *ptr );
   }

private:

   UF m_function;
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
   IndirectBinaryFunction( BF f ) : m_function( f )
   {
   }

   /*!
    * Copy constructor.
    */
   IndirectBinaryFunction( const IndirectBinaryFunction<T1Ptr, T2Ptr, BF>& ) = default;

   /*!
    * Function call operator. Applies the wrapped binary function to \a *ptr1
    * and \a *ptr2.
    *
    * If either \a ptr1 or \a ptr2 is nullptr, the function is not invoked and
    * calling this member function has no effect.
    */
   void operator()( T1Ptr ptr1, T2Ptr ptr2 ) const
   {
      if ( ptr1 != nullptr )
         if ( ptr2 != nullptr )
            m_function( *ptr1, *ptr2 );
   }

private:

   BF m_function;
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
   IndirectUnaryPredicate( UP p ) : m_predicate( p )
   {
   }

   /*!
    * Copy constructor.
    */
   IndirectUnaryPredicate( const IndirectUnaryPredicate<TPtr, UP>& ) = default;

   /*!
    * Function call operator. Applies the wrapped unary predicate to \a *ptr
    * and returns its resulting value.
    *
    * If \a ptr is nullptr, the predicate is not invoked and false is returned.
    */
   bool operator()( TPtr ptr ) const
   {
      return ptr != nullptr && m_predicate( *ptr );
   }

private:

   UP m_predicate;
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
   IndirectBinaryPredicate( BP p ) : m_predicate( p )
   {
   }

   /*!
    * Copy constructor.
    */
   IndirectBinaryPredicate( const IndirectBinaryPredicate<T1Ptr, T2Ptr, BP>& ) = default;

   /*!
    * Function call operator. Applies the wrapped binary predicate to \a *ptr1
    * and \a *ptr2, and returns its resulting value.
    *
    * If either of \a ptr1 or \a ptr2 is nullptr, the predicate is not invoked
    * and false is returned.
    */
   bool operator()( T1Ptr ptr1, T2Ptr ptr2 ) const
   {
      return ptr1 != nullptr && ptr2 != nullptr && m_predicate( *ptr1, *ptr2 );
   }

private:

   BP m_predicate;
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
   /*!
    * Function call operator. Returns true iff \a *ptr1 == \a *ptr2.
    *
    * If either of \a ptr1 or \a ptr2 is nullptr, no operation is performed on
    * any pointed object and the return value is \a ptr1 == \a ptr2.
    */
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
   /*!
    * Function call operator. Returns true iff \a *ptr1 < \a *ptr2.
    *
    * If either of \a ptr1 or \a ptr2 is nullptr, no operation is performed on
    * any pointed object and the return value is \a ptr2 < \a ptr1. This
    * ensures that when sorting lists of pointers indirectly, all null pointers
    * will be packed at the bottom of the sorted list.
    */
   bool operator ()( TPtr ptr1, TPtr ptr2 ) const
   {
      // When sorting a list, make sure all null pointers are packed at the bottom.
      return ptr1 != nullptr && (ptr2 == nullptr || *ptr1 < *ptr2);
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Indirect_h

// ----------------------------------------------------------------------------
// EOF pcl/Indirect.h - Released 2018-11-23T16:14:19Z
