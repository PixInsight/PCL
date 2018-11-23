//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/LinearInterpolation.h - Released 2018-11-23T16:14:19Z
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

#ifndef __PCL_LinearInterpolation_h
#define __PCL_LinearInterpolation_h

/// \file pcl/LinearInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/UnidimensionalInterpolation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class LinearInterpolation
 * \brief Linear interpolation algorithm.
 *
 * The linear interpolation algorithm interpolates from two adjacent mapped
 * source data items. It evaluates a linear function defined by the two
 * adjacent points.
 */
template <typename T>
class PCL_CLASS LinearInterpolation : public UnidimensionalInterpolation<T>
{
public:

   typedef typename UnidimensionalInterpolation<T>::vector_type vector_type;

   /*!
    * Constructs a %LinearInterpolation object.
    */
   LinearInterpolation() = default;

   /*!
    * Copy constructor.
    */
   LinearInterpolation( const LinearInterpolation& ) = default;

   /*!
    * Destroys a %LinearInterpolation object.
    */
   virtual ~LinearInterpolation()
   {
   }

   /*!
    * Returns true iff this interpolation is valid, i.e. if it has been
    * correctly initialized and is ready to interpolate function values.
    */
   bool IsValid() const override
   {
      return this->m_y.Length() > 1;
   }

   /*!
    * Interpolated value at \a x location.
    */
   double operator()( double x ) const override
   {
      PCL_PRECONDITION( IsValid() )

      if ( this->m_x )
      {
         // Interpolation with monotonically increasing, explicit x[i].

         PCL_CHECK( this->m_x.Length() >= this->m_y.Length() )
         int i0 = 0;
         int i1 = this->m_x.Length() - 1;
         while ( i1-i0 > 1 )
         {
            int im = (i0 + i1) >> 1;
            if ( x < this->m_x[im] )
               i1 = im;
            else
               i0 = im;
         }
         double x0( this->m_x[i0] );
         double y0( this->m_y[i0] );
         double dx = double( this->m_x[i1] ) - x0;
         if ( 1 + dx != 1 ) // if no infinite slope (to within roundoff)
            y0 += (x - x0)*(double( this->m_y[i1] ) - y0)/dx;
         return y0;
      }
      else
      {
         // Interpolation with implicit x[i] = i for i = {0,1,...,n-1}.

         if ( x <= 0 )
            return this->m_y[0];
         if ( x >= this->m_y.Length()-1 )
            return this->m_y[this->m_y.Length()-1];
         int i0 = TruncInt( x );
         double y0( this->m_y[i0] );
         return y0 + (x - i0)*(double( this->m_y[i0+1] ) - y0);
      }
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_LinearInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/LinearInterpolation.h - Released 2018-11-23T16:14:19Z
