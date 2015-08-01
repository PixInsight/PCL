//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/NearestNeighborInterpolation.h - Released 2015/07/30 17:15:18 UTC
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

#ifndef __PCL_NearestNeighborInterpolation_h
#define __PCL_NearestNeighborInterpolation_h

/// \file pcl/NearestNeighborInterpolation.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Utility_h
#include <pcl/Utility.h>
#endif

#ifndef __PCL_BidimensionalInterpolation_h
#include <pcl/BidimensionalInterpolation.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

#define m_width  this->m_width
#define m_height this->m_height
#define m_data      this->m_data

// ----------------------------------------------------------------------------

/*!
 * \class NearestNeighborInterpolation
 * \brief Two-dimensional nearest neighbor interpolation algorithm
 *
 * Nearest neighbor interpolation selects the value of the nearest data point
 * from the source 2-D matrix. This interpolation always returns existing data
 * values in the source matrix, since it doesn't consider the values of other
 * neighboring points.
 *
 * \sa BidimensionalInterpolation, BilinearInterpolation,
 * BicubicSplineInterpolation, BicubicBSplineInterpolation,
 * BicubicFilterInterpolation,
 */
template <typename T>
class PCL_CLASS NearestNeighborInterpolation : public BidimensionalInterpolation<T>
{
public:

   /*!
    * Constructs a %NearestNeighborInterpolation instance.
    */
   NearestNeighborInterpolation() : BidimensionalInterpolation<T>()
   {
   }

   /*!
    * Returns an interpolated value at {\a x,\a y} location.
    *
    * \param x,y  Coordinates of the interpolation point (horizontal,vertical).
    */
   virtual double operator()( double x, double y ) const
   {
      PCL_PRECONDITION( m_data != nullptr )
      PCL_PRECONDITION( m_width > 0 && m_height > 0 )
      return double( *(m_data + (Range( RoundIntArithmetic( y ), 0, m_height-1 )*int64( m_width ) +
                                 Range( RoundIntArithmetic( x ), 0, m_width-1 ))) );
   }
};

// ----------------------------------------------------------------------------

#undef m_width
#undef m_height
#undef m_data

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_NearestNeighborInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/NearestNeighborInterpolation.h - Released 2015/07/30 17:15:18 UTC
