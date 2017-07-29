//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/BilinearInterpolation.h - Released 2017-07-18T16:13:52Z
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

#ifndef __PCL_BilinearInterpolation_h
#define __PCL_BilinearInterpolation_h

/// \file pcl/BilinearInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/BidimensionalInterpolation.h>
#include <pcl/Utility.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define m_width      this->m_width
#define m_height     this->m_height
#define m_fillBorder this->m_fillBorder
#define m_fillValue  this->m_fillValue
#define m_data       this->m_data

// ----------------------------------------------------------------------------

/*!
 * \class BilinearInterpolation
 * \brief Bilinear interpolation algorithm.
 *
 * The bilinear interpolation algorithm interpolates from the nearest four
 * mapped source data items. It builds and evaluates two linear interpolation
 * polynomials, one for each plane direction.
 *
 * \sa NearestNeighborInterpolation, BicubicSplineInterpolation,
 * BicubicBSplineInterpolation, BicubicFilterInterpolation,
 */
template <typename T>
class PCL_CLASS BilinearInterpolation : public BidimensionalInterpolation<T>
{
public:

   /*!
    * Constructs a %BilinearInterpolation instance.
    */
   BilinearInterpolation() = default;

   /*!
    * Copy constructor.
    */
   BilinearInterpolation( const BilinearInterpolation& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~BilinearInterpolation()
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

      int j0 = pcl::Range( TruncInt( x ), 0, m_width-1 );
      int i0 = pcl::Range( TruncInt( y ), 0, m_height-1 );

      int j1 = j0 + 1;
      int i1 = i0 + 1;

      double p00, p10, p01, p11;
      const T* fp = m_data + (int64( i0 )*m_width + j0);

      p00 = *fp;
      p10 = (j1 < m_width) ? fp[1] : (m_fillBorder ? m_fillValue : *fp);

      if ( i1 < m_height )
         fp += m_width;
      else if ( m_fillBorder )
      {
         p01 = p11 = m_fillValue;
         goto __1;
      }

      p01 = *fp;
      p11 = (j1 < m_width) ? fp[1] : (m_fillBorder ? m_fillValue : *fp);

__1:
      double dx = x - j0, dx1 = 1 - dx;
      double dy = y - i0, dy1 = 1 - dy;
      return p00*dx1*dy1 + p10*dx*dy1 + p01*dx1*dy + p11*dx*dy;
   }
};

// ----------------------------------------------------------------------------

#undef m_width
#undef m_height
#undef m_fillBorder
#undef m_fillValue
#undef m_data

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BilinearInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/BilinearInterpolation.h - Released 2017-07-18T16:13:52Z
