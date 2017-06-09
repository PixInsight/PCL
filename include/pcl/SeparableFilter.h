//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0837
// ----------------------------------------------------------------------------
// pcl/SeparableFilter.h - Released 2017-06-09T08:12:42Z
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

#ifndef __PCL_SeparableFilter_h
#define __PCL_SeparableFilter_h

/// \file pcl/SeparableFilter.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Matrix.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SeparableFilter
 * \brief Separable filter in two dimensions.
 *
 * %SeparableFilter stores two one-dimensional filter vectors of floating point
 * filter coefficients, plus an optional filter name. This class provides
 * automatic allocation and deallocation of coefficients, implicit data sharing,
 * thread-safe access and a number of utility functions for filter management.
 *
 * In the same way as the KernelFilter and Convolution PCL classes,
 * %SeparableFilter can be used with the SeparableConvolution class. In,
 * addition, ATrousWaveletTransform also accepts a %SeparableFilter as the
 * implementation of a wavelet scaling function.
 *
 * The SeparableConvolution PCL class uses separable filters to implement a
 * separable convolution algorithm. This allows for important performance
 * improvements, since separable convolution has O(N*n) complexity, as opposed
 * to O(N*n^2) for non-separable convolution.
 *
 * \sa SeparableConvolution, ATrousWaveletTransform, KernelFilter
 */
class PCL_CLASS SeparableFilter
{
public:

   /*!
    * Represents a filter coefficient.
    */
   typedef float                       coefficient;

   /*!
    * Represents a one-dimensional filter.
    */
   typedef GenericVector<coefficient>  coefficient_vector;

   /*!
    * Represents a two-dimensional filter matrix.
    */
   typedef GenericMatrix<coefficient>  coefficient_matrix;

   /*!
    * Constructs an empty %SeparableFilter object with optional \a name.
    */
   SeparableFilter( const String& name = String() ) :
      rowFilter(), colFilter(), filterName( name )
   {
   }

   /*!
    * Constructs a %SeparableFilter object with the specified size \a n and
    * optional \a name. Filter elements are not initialized, so they will
    * contain unpredictable values.
    *
    * A %SeparableFilter object must be initialized with either a zero size
    * (which yields an empty filter), or an odd size >= 3.
    */
   SeparableFilter( int n, const String& name = String() ) :
      rowFilter( PCL_VALID_KERNEL_SIZE( n ) ),
      colFilter( PCL_VALID_KERNEL_SIZE( n ) ), filterName( name )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 )
      PCL_PRECONDITION( n == 0 || (n & 1) )
   }

   /*!
    * Constructs a %SeparableFilter object with the specified size \a n, initial
    * element value \a x, and optional \a name.
    *
    * This constructor yields a separable filter corresponding to a box average
    * two-dimentional filter whose weight is \a x.
    */
   template <typename T>
   SeparableFilter( int n, const T& x, const String& name = String() ) :
      rowFilter( x, PCL_VALID_KERNEL_SIZE( n ) ),
      colFilter( x, PCL_VALID_KERNEL_SIZE( n ) ), filterName( name )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 )
      PCL_PRECONDITION( n == 0 || (n & 1) )
   }

   /*!
    * Constructs a %SeparableFilter object with the specified one-dimensional
    * row and column filter vectors \a h and \a v, respectively, and an
    * optional \a name.
    */
   SeparableFilter( const coefficient_vector& h, const coefficient_vector& v, const String& name = String() ) :
      rowFilter( h ), colFilter( v ), filterName( name )
   {
      PCL_PRECONDITION( v.Length() == h.Length() )
      PCL_PRECONDITION( v.IsEmpty() || v.Length() >= 3 )
      PCL_PRECONDITION( v.IsEmpty() || (v.Length() & 1) )
   }

   /*!
    * Constructs a %SeparableFilter object with the specified size \a n and
    * optional \a name. Copies \a n filter coefficients from the specified
    * static arrays \a h and \a v to the one-dimensional row and column filter
    * vectors in this object, respectively.
    */
   template <typename T>
   SeparableFilter( const T* h, const T* v, int n, const String& name = String() ) :
      rowFilter( h, PCL_VALID_KERNEL_SIZE( n ) ),
      colFilter( v, PCL_VALID_KERNEL_SIZE( n ) ), filterName( name )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 )
      PCL_PRECONDITION( n == 0 || (n & 1) )
   }

   /*!
    * Copy constructor.
    */
   SeparableFilter( const SeparableFilter& x ) :
      rowFilter( x.rowFilter ), colFilter( x.colFilter ), filterName( x.filterName )
   {
   }

   /*!
    * Move constructor.
    */
   SeparableFilter( SeparableFilter&& x ) :
      rowFilter( std::move( x.rowFilter ) ),
      colFilter( std::move( x.colFilter ) ), filterName( std::move( x.filterName ) )
   {
   }

   /*!
    * Destroys a %SeparableFilter object.
    */
   virtual ~SeparableFilter()
   {
   }

   /*!
    * Returns a pointer to a dynamically allocated duplicate of this separable
    * filter.
    *
    * \note All derived classes from %SeparableFilter must reimplement this
    * virtual member function.
    */
   virtual SeparableFilter* Clone() const
   {
      return new SeparableFilter( *this );
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SeparableFilter& operator =( const SeparableFilter& x )
   {
      rowFilter = x.rowFilter;
      colFilter = x.colFilter;
      filterName = x.filterName;
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   SeparableFilter& operator =( SeparableFilter&& x )
   {
      rowFilter = std::move( x.rowFilter );
      colFilter = std::move( x.colFilter );
      filterName = std::move( x.filterName );
      return *this;
   }

   /*!
    * Assigns the specified scalar \a x to all filter coefficients. Returns a
    * reference to this object.
    */
   SeparableFilter& operator =( const coefficient& x )
   {
      rowFilter = x;
      colFilter = x;
      return *this;
   }

   /*!
    * Returns true iff this %SeparableFilter object is equal to another instance
    * \a f. Two %SeparableFilter instances are equal if their name and filter
    * coefficients are equal.
    */
   bool operator ==( const SeparableFilter& f ) const
   {
      return Name() == f.Name() && SameCoefficients( f );
   }

   /*!
    * Returns the name of this %SeparableFilter object.
    */
   String Name() const
   {
      return filterName;
   }

   /*!
    * Sets the name of this %SeparableFilter object.
    */
   virtual void Rename( const String& newName )
   {
      filterName = newName.Trimmed();
   }

   /*!
    * Returns the size of this separable filter. The filter size is the length
    * of the one-dimensional row and column filter vectors.
    */
   int Size() const
   {
      return rowFilter.Length(); // both 1D filters have the same length
   }

   /*!
    * Sets the size of this separable filter. Previous filter contents are
    * lost; the internal filter coefficient vectors will contain unpredictable
    * values after calling this function.
    */
   virtual void Resize( int n )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 )
      PCL_PRECONDITION( n == 0 || (n & 1) )
      if ( n == 0 )
         colFilter = rowFilter = coefficient_vector();
      else
         colFilter = rowFilter = coefficient_vector( PCL_VALID_KERNEL_SIZE( n ) );
   }

   /*!
    * Returns true iff this is an empy filter, i.e. if it has no coefficients.
    */
   bool IsEmpty() const
   {
      return rowFilter.IsEmpty(); // both 1D filters have the same length
   }

   /*!
    * Returns a copy of the internal one-dimensional row filter vector.
    */
   coefficient_vector RowFilter() const
   {
      return rowFilter;
   }

   /*!
    * Returns a copy of the internal one-dimensional column filter vector.
    */
   coefficient_vector ColumnFilter() const
   {
      return colFilter;
   }

   /*!
    * This member function is an alias to ColumnFilter().
    */
   coefficient_vector ColFilter() const
   {
      return colFilter;
   }

   /*!
    * Returns a copy of the internal one-dimensional filter vector
    * corresponding to the specified \a phase. If \a phase is zero, the row
    * filter vector is returned; otherwise the column filter vector is returned.
    */
   coefficient_vector Filter( int phase ) const
   {
      return phase ? colFilter : rowFilter;
   }

   /*!
    * Returns the weight of this separable filter. The weight is equal to the
    * product of the sums of components in the row and column filter vectors.
    */
   double Weight() const
   {
      return rowFilter.Sum() * colFilter.Sum();
   }

   /*!
    * Returns true if this is a high-pass separable filter; false if this is a
    * low-pass separable filter.
    *
    * A high-pass kernel filter has one or more negative coefficients. A
    * low-pass kernel filter is either empty or all of its coefficients are
    * positive or zero. For separable low-pass filters, all the nonzero
    * coefficients in both the row and column filter vectors have the same
    * sign. If this condition doesn't hold (i.e. if coefficient signs differ)
    * then the object is a high-pass filter.
    */
   bool IsHighPassFilter() const
   {
      int s = -1;
      for ( coefficient_vector::const_iterator i = rowFilter.Begin(); i < rowFilter.End(); ++i )
         if ( *i != 0 )
            for ( s = *i < 0; ++i < rowFilter.End(); )
               if ( *i != 0 && (*i < 0) != s )
                  return true;
      for ( coefficient_vector::const_iterator i = colFilter.Begin(); i < colFilter.End(); ++i )
         if ( *i != 0 )
            for ( s = (s < 0) ? *i++ < 0 : s; i < colFilter.End(); ++i )
               if ( *i != 0 && (*i < 0) != s )
                  return true;
      return false;
   }

   /*!
    * Returns true iff this %SeparableFilter object has the same filter
    * coefficients as another instance \a f.
    */
   bool SameCoefficients( const SeparableFilter& f ) const
   {
      return colFilter == f.colFilter && rowFilter == f.rowFilter;
   }

   /*!
    * Deallocates filter coefficients and yields an empy %SeparableFilter
    * object. This member function does not change the current filter name.
    */
   virtual void Clear()
   {
      colFilter = rowFilter = coefficient_vector();
   }

   /*!
    * Exchanges two separable filters \a x1 and \a x2.
    */
   friend void Swap( SeparableFilter& x1, SeparableFilter& x2 )
   {
      pcl::Swap( x1.rowFilter,  x2.rowFilter );
      pcl::Swap( x1.colFilter,  x2.colFilter );
      pcl::Swap( x1.filterName, x2.filterName );
   }

protected:

   /*
    * Horizontal and vertical one-dimensional filters. The 2D filter matrix is
    * equal to the product of these two vectors.
    */
   coefficient_vector rowFilter;
   coefficient_vector colFilter;

   /*
    * Identifying name.
    */
   String filterName;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SeparableFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/SeparableFilter.h - Released 2017-06-09T08:12:42Z
