//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// pcl/KernelFilter.h - Released 2015/11/26 15:59:39 UTC
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

#ifndef __PCL_KernelFilter_h
#define __PCL_KernelFilter_h

/// \file pcl/KernelFilter.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Math_h
#include <pcl/Math.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

#ifndef __PCL_Matrix_h
#include <pcl/Matrix.h>
#endif

#ifndef __PCL_SeparableFilter_h
#include <pcl/SeparableFilter.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

#define __PCL_DEFAULT_FILTER_SEPARABILITY_TOLERANCE 1.0e-06F

/*!
 * \class KernelFilter
 * \brief Kernel filter in two dimensions.
 *
 * %KernelFilter stores a square matrix of floating point filter coefficients
 * and an optional filter name. This class provides automatic allocation and
 * deallocation of coefficients, implicit data sharing, thread-safe access and
 * a number of utility functions for filter management.
 *
 * %KernelFilter can be used along with a variety of PCL classes, such as
 * Convolution and ATrousWaveletTransform.
 *
 * For separable filters and convolutions, see the SeparableFilter and
 * SeparableConvolution PCL classes.
 *
 * \sa GaussianFilter, VariableShapeFilter, Convolution, ATrousWaveletTransform
 */
class PCL_CLASS KernelFilter
{
public:

   /*!
    * Represents a filter coefficient.
    */
   typedef float                       coefficient;

   /*!
    * Represents a filter coefficient matrix.
    */
   typedef GenericMatrix<coefficient>  coefficient_matrix;

   /*!
    * Constructs an empty %KernelFilter object with optional \a name.
    */
   KernelFilter( const String& name = String() ) :
      coefficients(), filterName( name ), flipped( false )
   {
   }

   /*!
    * Constructs a %KernelFilter object with the specified size \a n and
    * optional \a name. Filter elements are not initialized, so they will
    * contain unpredictable values.
    *
    * A %KernelFilter object must be initialized with either a zero size
    * (which yields an empty filter), or an odd size >= 3.
    */
   KernelFilter( int n, const String& name = String() ) :
      coefficients( PCL_VALID_KERNEL_SIZE( n ), PCL_VALID_KERNEL_SIZE( n ) ), filterName( name ), flipped( false )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 )
      PCL_PRECONDITION( n == 0 || (n & 1) )
   }

   /*!
    * Constructs a %KernelFilter object with the specified size \a n, initial
    * element value \a x, and optional \a name.
    */
   template <typename T>
   KernelFilter( int n, const T& x, const String& name = String() ) :
      coefficients( x, PCL_VALID_KERNEL_SIZE( n ), PCL_VALID_KERNEL_SIZE( n ) ), filterName( name ), flipped( false )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 )
      PCL_PRECONDITION( n == 0 || (n & 1) )
   }

   /*!
    * Constructs a %KernelFilter object with the specified filter coefficient
    * matrix \a F and optional \a name.
    */
   KernelFilter( const coefficient_matrix& F, const String& name = String() ) :
      coefficients( F ), filterName( name ), flipped( false )
   {
   }

   /*!
    * Constructs a %KernelFilter object with the specified size \a n and
    * optional \a name. Copies \a nxn filter coefficients from the specified
    * static array \a k.
    */
   template <typename T>
   KernelFilter( const T* k, int n, const String& name = String() ) :
      coefficients( k, PCL_VALID_KERNEL_SIZE( n ), PCL_VALID_KERNEL_SIZE( n ) ), filterName( name ), flipped( false )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 )
      PCL_PRECONDITION( n == 0 || (n & 1) )
   }

   /*!
    * Copy constructor.
    */
   KernelFilter( const KernelFilter& f ) :
      coefficients( f.coefficients ), filterName( f.filterName ), flipped( f.flipped )
   {
   }

   /*!
    * Move constructor.
    */
   KernelFilter( KernelFilter&& f ) :
      coefficients( std::move( f.coefficients ) ), filterName( std::move( f.filterName ) ), flipped( f.flipped )
   {
   }

   /*!
    * Destroys a %KernelFilter object.
    */
   virtual ~KernelFilter()
   {
   }

   /*!
    * Returns a pointer to a dynamically allocated duplicate of this kernel
    * filter.
    *
    * \note All derived classes from %KernelFilter must reimplement this
    * virtual member function.
    */
   virtual KernelFilter* Clone() const
   {
      return new KernelFilter( *this );
   }

   /*!
    * Returns a separable filter equivalent to this kernel filter matrix.
    *
    * The default implementation computes the singular value decomposition of
    * the filter matrix, which yields the separable filter as a couple of row
    * and column vectors. Filter separability is then verified by computing the
    * resulting filter by matrix multiplication of the separable filter
    * components. If the resulting filter differs by more than the specified
    * \a tolerance from the original, then the filter is non-separable and an
    * empty SeparableFilter object is returned.
    *
    * For known separable filters, this member function must be reimplemented
    * in derived classes to bypass the default (and expensive) separability
    * tests. For example, Gaussian filters are separable, so this function is
    * conveniently reimplemented by the GaussianFilter class to return a
    * separable filter consisting of the central row and column vectors of the
    * Gaussian filter matrix representation.
    */
   virtual SeparableFilter AsSeparableFilter( float tolerance = __PCL_DEFAULT_FILTER_SEPARABILITY_TOLERANCE ) const;

   /*!
    * Returns true iff this filter is separable,
    *
    * A two-dimensional filter matrix is separable if it can be expressed as
    * the product of two vectors. The default implementation of this function
    * tries to compute a separable filter by calling AsSeparable(). Then it
    * returns true if the computed separable filter is nonempty.
    *
    * For known separable (and non-separable) filters, this member function
    * must be reimplemented in derived classes to bypass the above, expensive
    * procedure.
    */
   virtual bool IsSeparable() const
   {
      return !AsSeparableFilter().IsEmpty();
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   KernelFilter& operator =( const KernelFilter& f )
   {
      coefficients = f.coefficients;
      filterName = f.filterName;
      flipped = f.flipped;
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   KernelFilter& operator =( KernelFilter&& f )
   {
      coefficients = std::move( f.coefficients );
      filterName = std::move( f.filterName );
      flipped = f.flipped;
      return *this;
   }

   /*!
    * Assigns the specified filter coefficient matrix \a F to this object.
    * Returns a reference to this object.
    *
    * The specified matrix \a F must be either an empty matrix or a square
    * matrix of odd size. If these conditions are not met, this member
    * function throws an Error exception.
    */
   KernelFilter& operator =( const coefficient_matrix& F )
   {
      if ( !F.IsEmpty() && (F.Rows() != F.Cols() || (F.Rows() & 1) == 0) )
         throw Error( "KernelFilter: Invalid coefficient matrix assignment." );
      coefficients = F;
      flipped = false;
      return *this;
   }

   /*!
    * Assigns the specified scalar \a x to all filter coefficients. Returns a
    * reference to this object.
    */
   KernelFilter& operator =( const coefficient& x )
   {
      coefficients = x;
      flipped = false;
      return *this;
   }

   /*!
    * Returns true iff this %KernelFilter object is equal to another instance
    * \a f. Two %KernelFilter instances are equal if their name and filter
    * coefficients are equal.
    */
   bool operator ==( const KernelFilter& f ) const
   {
      return Name() == f.Name() && SameCoefficients( f );
   }

   /*!
    * Returns the name of this %KernelFilter object.
    */
   String Name() const
   {
      return filterName;
   }

   /*!
    * Sets the name of this %KernelFilter object.
    */
   virtual void Rename( const String& newName )
   {
      filterName = newName.Trimmed();
   }

   /*!
    * Returns the size of this filter kernel.
    */
   int Size() const
   {
      return coefficients.Rows(); // coefficients *must* be a square matrix
   }

   /*!
    * Sets the size of this filter kernel. Previous filter contents are lost;
    * the internal matrix of filter coefficients will contain unpredictable
    * values after calling this function.
    */
   virtual void Resize( int n )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 )
      PCL_PRECONDITION( n == 0 || (n & 1) )
      if ( n == 0 )
         coefficients = coefficient_matrix();
      else
      {
         n = PCL_VALID_KERNEL_SIZE( n );
         coefficients = coefficient_matrix( n, n );
      }
      flipped = false;
   }

   /*!
    * Returns true iff this is an empty filter, that is, if it has no filter
    * coefficients.
    */
   bool IsEmpty() const
   {
      return coefficients.IsEmpty();
   }

   /*!
    * Returns true iff this is a non-empty filter. Equivalent to !IsEmpty().
    */
   operator bool() const
   {
      return !IsEmpty();
   }

   /*!
    * Returns the total number of filter coefficients, or Size()*Size().
    */
   int NumberOfCoefficients() const
   {
      return coefficients.Rows()*coefficients.Cols();
   }

   /*!
    * Returns a copy of the internal matrix of filter coefficients.
    */
   coefficient_matrix Coefficients() const
   {
      return coefficients;
   }

   /*!
    * Returns a pointer to the beginning of the internal matrix of (immutable)
    * filter coefficients.
    *
    * All filter coefficients are guaranteed to be stored at consecutive
    * locations addressable from the pointer returned by this function. Filter
    * coefficients are stored in <em>row order</em> (all coefficients of row 0
    * followed by all coefficients of row 1, and so on).
    */
   const coefficient* Begin() const
   {
      return coefficients.Begin();
   }

   /*!
    * Returns a pointer to the end of the internal matrix of (immutable) filter
    * coefficients.
    *
    * All filter coefficients are guaranteed to be stored at consecutive
    * locations in reverse order, addressable from the pointer returned by this
    * function <em>minus one</em>. Filter coefficients are stored in
    * <em>row order</em> (all coefficients of row 0 followed by all
    * coefficients of row 1, etc.).
    */
   const coefficient* End() const
   {
      return coefficients.End();
   }

#ifndef __PCL_NO_STL_COMPATIBLE_ITERATORS
   /*!
    * STL-compatible iteration. Equivalent to Begin() const.
    */
   const coefficient* begin() const
   {
      return Begin();
   }

   /*!
    * STL-compatible iteration. Equivalent to End() const.
    */
   const coefficient* end() const
   {
      return End();
   }
#endif   // !__PCL_NO_STL_COMPATIBLE_ITERATORS

   /*!
    * Subscript operator. Returns a pointer to the first (immutable) filter
    * coefficient of the specified \a row in this filter. \a row must be a
    * valid vertical index: it must be 0 <= \a row < Size().
    */
   const coefficient* operator[]( int row ) const
   {
      return coefficients[row];
   }

   /*!
    * Returns the \e weight of this kernel filter. The weight is equal to the
    * sum of all filter coefficients.
    */
   double Weight() const
   {
      return coefficients.Sum();
   }

   /*!
    * Returns the sum of the absolute values of all filter coefficients.
    */
   double Modulus() const
   {
      return coefficients.Modulus();
   }

   /*!
    * Normalizes filter coefficients. Divides each filter coefficient by the
    * result of the Modulus() member function. If Modulus() is zero or
    * insignificant this function does nothing.
    */
   void Normalize()
   {
      double m = Modulus();
      if ( 1 + m != 1 )
         coefficients /= m;
   }

   /*!
    * Returns the normalization of this kernel filter. This member function
    * creates a new %KernelFilter instance as a copy of this object, normalizes
    * it, and returns it.
    */
   KernelFilter Normalized() const
   {
      KernelFilter f( *this );
      f.Normalize();
      return f;
   }

   /*!
    * Flips this kernel filter. Flipping a filter consists of rotating its
    * coefficients by 180 degrees.
    */
   void Flip()
   {
      coefficients.Flip();
      flipped = !flipped;
   }

   /*!
    * Returns a flipped copy of this filter. Flipping a filter consists of
    * rotating its coefficients by 180 degrees.
    */
   KernelFilter Flipped() const
   {
      KernelFilter f( *this );
      f.Flip();
      return f;
   }

   /*!
    * Returns true iff this kernel filter has been flipped (rotated by 180
    * degrees). Note that after an even number of successive flippings (which
    * is a no-op) this member function will return false.
    */
   bool IsFlipped() const
   {
      return flipped;
   }

   /*!
    * Returns true if this is a high-pass kernel filter; false if this is a
    * low-pass kernel filter.
    *
    * A high-pass kernel filter has one or more negative coefficients. A
    * low-pass kernel filter is either empty or all of its coefficients are
    * positive or zero.
    */
   bool IsHighPassFilter() const
   {
      const coefficient* a = coefficients.Begin();
      const coefficient* b = coefficients.End();
      //while ( a < b )
      //   if ( *a++ < 0 || *--b < 0 )
      // The above loop is better for nonsymmetrical filters, which are seldom used.
      while ( a < b )
         if ( *a++ < 0 )
            return true;
      return false;
   }

   /*!
    * Returns true iff this %KernelFilter object has the same coefficients as
    * other instance \a f.
    */
   bool SameCoefficients( const KernelFilter& f ) const
   {
      return coefficients.SameElements( f.coefficients );
   }

   /*!
    * Deallocates filter coefficients and yields an empy %KernelFilter object.
    * Note that this member function does not change the current filter name.
    */
   virtual void Clear()
   {
      coefficients = coefficient_matrix();
      flipped = false;
   }

   /*!
    * Exchanges two kernel filters \a x1 and \a x2.
    */
   friend void Swap( KernelFilter& x1, KernelFilter& x2 )
   {
      pcl::Swap( x1.coefficients, x2.coefficients );
      pcl::Swap( x1.filterName, x2.filterName );
      bool b = x1.flipped; x1.flipped = x2.flipped; x2.flipped = b;
   }

#ifndef __PCL_NO_MATRIX_IMAGE_RENDERING

   /*!
    * Renders this filter as an image. The contents of the specified target
    * image \a img will be replaced with a grayscale rendition of this filter,
    * where each image pixel has a value proportional to its corresponding
    * filter coefficient counterpart.
    *
    * Note that if this filter has out-of-range values for the pixel sample
    * type of the target image, pixel saturation will occur at either the white
    * or black points, or at both. This only happens when the target image is
    * of an integer type and this filter contains floating point values outside
    * the normalized [0,1] range.
    *
    * If the target image is of a floating-point type (either real or complex),
    * it may require a rescaling or normalization operation to constrain all
    * pixel values to the normalized [0,1] range after calling this function.
    */
   template <class P>
   void ToImage( GenericImage<P>& img ) const
   {
      coefficients.ToImage( img );
   }

   /*!
    * Renders this filter as an image. The contents of the image transported by
    * the specified %ImageVariant object \a v will be replaced with a grayscale
    * rendition of this filter, where each image pixel has a value proportional
    * to its corresponding filter coefficient counterpart.
    *
    * If the %ImageVariant object \a v does not transport an image, a new one
    * is created in 32-bit floating point format.
    *
    * Also take into account the information given for
    * ToImage( GenericImage<P>& ), relative to out-of-range values, which is
    * entirely applicable to this member function.
    */
   void ToImage( ImageVariant& v ) const
   {
      coefficients.ToImage( v );
   }

#endif   // !__PCL_NO_MATRIX_IMAGE_RENDERING

protected:

   coefficient_matrix coefficients; // filter coefficients, size*size elements
   String             filterName;   // identifying name
   bool               flipped : 1;  // flag true when filter rotated
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_KernelFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/KernelFilter.h - Released 2015/11/26 15:59:39 UTC
