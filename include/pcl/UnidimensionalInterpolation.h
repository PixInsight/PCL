//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/UnidimensionalInterpolation.h - Released 2017-06-17T10:55:43Z
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

#ifndef __PCL_UnidimensionalInterpolation_h
#define __PCL_UnidimensionalInterpolation_h

/// \file pcl/UnidimensionalInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class UnidimensionalInterpolation
 * \brief A generic interface to one-dimensional interpolation algorithms.
 *
 * %UnidimensionalInterpolation is an abstract base class for all
 * one-dimensional interpolation algorithm implementations in PCL.
 */
template <typename T>
class PCL_CLASS UnidimensionalInterpolation
{
public:

   typedef GenericVector<T>   vector_type;

   /*!
    * Constructs a %UnidimensionalInterpolation object.
    */
   UnidimensionalInterpolation() : m_x(), m_y()
   {
   }

   /*!
    * Destroys a %UnidimensionalInterpolation object.
    */
   virtual ~UnidimensionalInterpolation()
   {
      Clear();
   }

   /*!
    * Initializes a new interpolation.
    *
    * \param x    %Vector of x-values:\n
    *             \n
    *       \li If this vector is not empty: Must be a set of monotonically\n
    *       increasing, distinct values: x[0] < x[1] < ... < x[n-1].\n
    *       \li If this vector is empty: The interpolation will use implicit
    *       x[i] = i for i = {0,1,...,n-1}.
    *
    * \param y    %Vector of function values for i = {0,1,...,n-1}.
    */
   virtual void Initialize( const vector_type& x, const vector_type& y )
   {
      if ( !y || x && x.Length() < y.Length() )
         throw Error( "Invalid vector length in UnidimensionalInterpolation::Initialize()" );
      m_x = x;
      m_y = y;
   }

   /*!
    * Initializes a new interpolation.
    *
    * \param x    %Array of x-values:\n
    *             \n
    *       \li If \a x != 0: Must be a list of monotonically increasing,
    *       distinct values: x[0] < x[1] < ... < x[n-1].\n
    *       \li If \a x == 0: The interpolation will use implicit
    *       x[i] = i for i = {0,1,...,n-1}.
    *
    * \param y    %Array of function values for i = {0,1,...,n-1}.
    *
    * \param n    Number of data points. The minimum number of required data
    *             points depends on the interpolation algorithm. In general,
    *             all algorithms require at least \a n >= 2.
    *
    * \deprecated This member function has been deprecated. For newly produced
    * code, use Initialize( const vector_type&, const vector_type& ).
    */
   void Initialize( const T* x, const T* y, int n )
   {
      Initialize( vector_type( x, n ), vector_type( y, n ) );
   }

   /*!
    * Returns an interpolated function value at \a x location.
    */
   virtual double operator()( double x ) const = 0;

   /*!
    * Clears auxiliary or intermediate interpolation data. Derived classes
    * overriding this function should call their base class version via
    * explicit downcast.
    */
   virtual void Clear()
   {
      m_x.Clear();
      m_y.Clear();
   }

   /*!
    * Returns a reference to the x-value data vector. Returns an empty vector
    * if this interpolation uses implicit x[i] = i for i = {0,1,...,n-1}.
    */
   const vector_type& X() const
   {
      return m_x;
   }

   /*!
    * Returns a reference to the interpolated function value vector.
    */
   const vector_type& Y() const
   {
      return m_y;
   }

   /*!
    * Returns true iff this interpolation uses implicit
    * x[i] = i for i = {0,1,...,n-1}.
    */
   bool UsingImplicitX() const
   {
      return !m_x && m_y;
   }

   /*!
    * Returns the length of the interpolated function value vector.
    */
   int Length() const
   {
      return m_y.Length();
   }

protected:

   vector_type m_x;  // x-vector (empty if implicit grid x-values)
   vector_type m_y;  // y-vector
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_UnidimensionalInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/UnidimensionalInterpolation.h - Released 2017-06-17T10:55:43Z
