//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/WordArray.h - Released 2019-01-21T12:06:07Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_WordArray_h
#define __PCL_WordArray_h

/// \file pcl/WordArray.h

#include <pcl/Defs.h>

#include <pcl/Array.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup word_arrays Word Array Classes
 */

/*!
 * \class pcl::WordArray
 * \brief Dynamic array of 16-bit unsigned integers.
 *
 * %WordArray is a template instantiation of Array for \c uint16.
 *
 * \ingroup word_arrays
 * \ingroup dynamic_arrays
 */
typedef Array<uint16>         WordArray;

/*!
 * \class pcl::SortedWordArray
 * \brief Dynamic sorted array of 16-bit unsigned integers.
 *
 * %SortedWordArray is a template instantiation of SortedArray for \c uint16.
 *
 * \ingroup word_arrays
 * \ingroup dynamic_arrays
 */
typedef SortedArray<uint16>   SortedWordArray;

/*!
 * \class pcl::ShortArray
 * \brief Dynamic array of 16-bit signed integers.
 *
 * %ShortArray is a template instantiation of Array for \c int16.
 *
 * \ingroup word_arrays
  * \ingroup dynamic_arrays
*/
typedef Array<int16>          ShortArray;

/*!
 * \class pcl::SortedShortArray
 * \brief Dynamic sorted array of 16-bit signed integers.
 *
 * %SortedShortArray is a template instantiation of SortedArray for \c int16.
 *
 * \ingroup word_arrays
 * \ingroup dynamic_arrays
 */
typedef SortedArray<int16>    SortedShortArray;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_WordArray_h

// ----------------------------------------------------------------------------
// EOF pcl/WordArray.h - Released 2019-01-21T12:06:07Z
