// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/Association.h - Released 2014/10/29 07:34:11 UTC
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

#ifndef __PCL_Association_h
#define __PCL_Association_h

/// \file pcl/Association.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Relational_h
#include <pcl/Relational.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Association
 * \brief Generic association of two objects.
 */
template <typename T1, typename T2>
class PCL_CLASS Association
{
public:

   T1 first;   //!< First member of this association
   T2 second;  //!< Second member of this association

   /*!
    * Constructs an association with default member values.
    */
   Association()
   {
   }

   /*!
    * Constructs an association copying the specified member values \a x1
    * and \a x2.
    */
   Association( const T1& x1, const T2& x2 ) :
   first( x1 ), second( x2 )
   {
   }

   /*!
    * Copy constructor.
    */
   Association( const Association<T1, T2>& x ) :
   first( x.first ), second( x.second )
   {
   }
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup association_utilities Association Operators and Utility Functions
 */

/*!
 * Returns an Association whose members are copies of the specified objects
 * \a x1 and \a x2.
 * \ingroup association_utilities
 */
template <typename T1, typename T2> inline
Association<T1, T2> Associate( const T1& x1, const T2& x2 )
{
   return Association<T1, T2>( x1, x2 );
}

/*!
 * Returns true if two associations, \a x1 and \a x2, are equal. Two
 * associations are equal if their homolog members are equal.
 * \ingroup association_utilities
 */
template <typename T1, typename T2> inline
bool operator ==( const Association<T1, T2>& x1, const Association<T1, T2>& x2 )
{
   return x1.first == x2.first && x1.second == x2.second;
}

/*!
 * Returns true if an association \a x1 is less than other association \a x2.
 * The comparison algorithm is as follows:
 *
 * \code
 * if ( x1.first == x2.first )
 *    return x1.second < x2.second;
 * else
 *    return x1.first < x2.first;
 * \endcode
 *
 * So in association comparisons the first member of each association has
 * precedence over the second member.
 *
 * The implementation of this operator only requires <em>less than</em>
 * semantics for the two types T1 and T2; it doesn't use equality operators.
 * \ingroup association_utilities
 */
template <typename T1, typename T2> inline
bool operator <( const Association<T1, T2>& x1, const Association<T1, T2>& x2 )
{
   //return (x1.first == x2.first) ? x1.second < x2.second : x1.first < x2.first;
   return (x1.first < x2.first) ? true : ((x2.first < x1.first) ? false : x1.second < x2.second);
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Association_h

// ****************************************************************************
// EOF pcl/Association.h - Released 2014/10/29 07:34:11 UTC
