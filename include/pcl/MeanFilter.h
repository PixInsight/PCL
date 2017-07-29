//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/MeanFilter.h - Released 2017-06-28T11:58:36Z
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

#ifndef __PCL_MeanFilter_h
#define __PCL_MeanFilter_h

/// \file pcl/MeanFilter.h

#include <pcl/Defs.h>

#include <pcl/KernelFilter.h>
#include <pcl/Math.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MeanFilter
 * \brief Square block average kernel filter
 *
 * A %MeanFilter object is a specialized KernelFilter whose elements are
 * identical and form a square symmetrically distributed around the origin:
 *
 * M(x,y) = 1/(s^2)
 *
 * where s is the filter size, which is the same on the horizontal and vertical
 * axes. This filter is separable.
 *
 * %MeanFilter is a building block of some important algorithms on the
 * PixInsight/PCL platform, such as the Multiscale Local Normalization,
 * algorithm, which uses these kernels to compute local variances.
 *
 * \sa KernelFilter, VariableShapeFilter, MoffatFilter, LinearFilter,
 * GaussianFilter
 */
class PCL_CLASS MeanFilter : public KernelFilter
{
public:

   /*!
    * Constructs an empty %MeanFilter object.
    */
   MeanFilter() = default;

   /*!
    * Constructs a %MeanFilter object given the odd kernel size \a n >= 3, or
    * an empty kernel filter if \a n = 0. Assigns an optional \a name to the
    * new filter object.
    */
   MeanFilter( int n, const String& name = String() )
   {
      Initialize( n );
      Rename( name );
   }

   /*!
    * Copy constructor.
    */
   MeanFilter( const MeanFilter& ) = default;

   /*!
    * Move constructor.
    */
#ifndef _MSC_VER
   MeanFilter( MeanFilter&& ) = default;
#else
   MeanFilter( MeanFilter&& x ) :
      KernelFilter( std::move( x ) )
   {
   }
#endif

   /*!
    */
   virtual KernelFilter* Clone() const
   {
      return new MeanFilter( *this );
   }

   /*!
    * Returns a separable filter equivalent to this Gaussian kernel filter.
    *
    * A block average filter is always separable, so this member function
    * should always return a valid SeparableFilter object.
    */
   virtual SeparableFilter AsSeparableFilter( float tolerance = __PCL_DEFAULT_FILTER_SEPARABILITY_TOLERANCE ) const
   {
      FVector v = coefficients.RowVector( Size()>>1 );
      return SeparableFilter( v, v );
   }

   /*!
    * Returns true iff this filter is separable.
    *
    * A block average filter is separable. As reimplemented in %MeanFilter,
    * this member function returns true.
    */
   virtual bool IsSeparable() const
   {
      return true;
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   MeanFilter& operator =( const MeanFilter& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
#ifndef _MSC_VER
   MeanFilter& operator =( MeanFilter&& ) = default;
#else
   MeanFilter& operator =( MeanFilter&& x )
   {
      (void)KernelFilter::operator =( std::move( x ) );
      return *this;
   }
#endif

   /*!
    * Recalculates filter coefficients for the given odd kernel size \a n >= 3.
    */
   virtual void Resize( int n )  // Inherited from KernelFilter
   {
      Initialize( n );
   }

private:

   void Initialize( int n )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 && (n & 1) != 0 )
      KernelFilter::Resize( n );
      if ( !IsEmpty() )
         coefficients = 1.0/NumberOfCoefficients();
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MeanFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/MeanFilter.h - Released 2017-06-28T11:58:36Z
