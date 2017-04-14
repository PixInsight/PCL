//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/Relational.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_Relational_h
#define __PCL_Relational_h

/// \file pcl/Relational.h

#include <pcl/Defs.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup generic_relational_operators Generic Relational Operators
 *
 * This section defines the template operator functions <em>not equal to</em>,
 * <em>less than or equal</em>, <em>greater than</em>, and <em>greater than or
 * equal</em>.
 *
 * For applicability of these generic operators, the following conditions are
 * assumed for the template argument type T:
 *
 * \li For the != operator, there must be a meaningful == operator for the
 * type T.
 *
 * \li For the <=, > and >= operators, there must be a meaningful < operator
 * for the type T.
 */

/*!
 * Returns true iff two objects \a a and \a b are not equal.
 * \ingroup generic_relational_operators
 */
template <class T1, class T2> inline
bool operator !=( const T1& a, const T2& b )
{
   return !(a == b);
}

/*!
 * Returns true iff an object \a a is less than or equal to another object \a b.
 * \ingroup generic_relational_operators
 */
template <class T1, class T2> inline
bool operator <=( const T1& a, const T2& b )
{
   return !(b < a);
}

/*!
 * Returns true iff an object \a a is greater than another object \a b.
 * \ingroup generic_relational_operators
 */
template <class T1, class T2> inline
bool operator >( const T1& a, const T2& b )
{
   return b < a;
}

/*!
 * Returns true iff an object \a a is greater than or equal to another object
 * \a b.
 * \ingroup generic_relational_operators
 */
template <class T1, class T2> inline
bool operator >=( const T1& a, const T2& b )
{
   return !(a < b);
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Relational_h

// ----------------------------------------------------------------------------
// EOF pcl/Relational.h - Released 2017-04-14T23:04:40Z
