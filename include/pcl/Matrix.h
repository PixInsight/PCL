// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/Matrix.h - Released 2014/10/29 07:34:07 UTC
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

#ifndef __PCL_Matrix_h
#define __PCL_Matrix_h

/// \file pcl/Matrix.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_ReferenceCounter_h
#include <pcl/ReferenceCounter.h>
#endif

#ifndef __PCL_Math_h
#include <pcl/Math.h>
#endif

#ifndef __PCL_Memory_h
#include <pcl/Memory.h>
#endif

#ifndef __PCL_Utility_h
#include <pcl/Utility.h>
#endif

#ifndef __PCL_Rotate_h
#include <pcl/Rotate.h> // for pcl::Reverse()
#endif

#ifndef __PCL_Vector_h
#include <pcl/Vector.h>
#endif

#ifndef __PCL_Exception_h
#include <pcl/Exception.h>
#endif

#ifndef __PCL_NO_MATRIX_STATISTICS
# ifndef __PCL_Sort_h
#  include <pcl/Sort.h>
# endif
# ifndef __PCL_Selection_h
#  include <pcl/Selection.h>
# endif
#endif

#if !defined( __PCL_NO_MATRIX_IMAGE_RENDERING ) && !defined( __PCL_NO_MATRIX_IMAGE_CONVERSION )
# ifndef __PCL_Image_h
#  include <pcl/Image.h>
# endif
# ifndef __PCL_ImageVariant_h
#  include <pcl/ImageVariant.h>
# endif
#endif

#ifndef __PCL_NO_MATRIX_PHASE_MATRICES
# ifndef __PCL_Complex_h
#  include <pcl/Complex.h>
# endif
#endif

/*
 * Valid filter kernel sizes are odd integers >= 3. This macro is used for the
 * KernelFilter and SeparableFilter classes to ensure validity of filter
 * matrices and vectors.
 */
#define PCL_VALID_KERNEL_SIZE( n )  (n ? Max( 3, n|1 ) : 0)

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class GenericMatrix
 * \brief Generic dynamic matrix of arbitrary dimensions.
 *
 * %GenericMatrix is a lightweight template class implementing a matrix of
 * arbitrary size. This class provides the following main features:
 *
 * \li Reference counted. %GenericMatrix instances can safely be passed as
 * function return values and by-value function arguments.
 *
 * \li Thread-safe. %GenericMatrix instances can safely be accessed from
 * multiple threads. The reference counter implements atomic reference and
 * dereference operations.
 *
 * \li Efficient matrix storage and access to matrix elements. %Matrix
 * elements are dynamically allocated as a single, contiguous memory block.
 *
 * \li Support for basic matrix operations, including matrix inversion and
 * transposition, scalar-to-matrix and matrix-to-matrix arithmetic operations.
 *
 * \sa GenericVector, \ref matrix_operators, \ref matrix_types
 */
template <typename T>
class PCL_CLASS GenericMatrix
{
public:

   /*!
    * Represents a matrix element.
    */
   typedef T         element;

   /*!
    * Constructs an empty matrix.
    * An empty matrix has no elements and zero dimensions.
    */
   GenericMatrix() : data( new Data( 0, 0 ) )
   {
   }

   /*!
    * Constructs an uninitialized matrix.
    *
    * \param rows    Number of matrix rows (>= 0).
    * \param cols    Number of matrix columns (>= 0).
    *
    * Matrix elements are not initialized by this constructor; the newly
    * created matrix will contain unpredictable values.
    */
   GenericMatrix( int rows, int cols ) : data( new Data( rows, cols ) )
   {
   }

   /*!
    * Constructs a matrix and fills it with a constant value.
    *
    * \param x       Initial value for all matrix elements.
    * \param rows    Number of matrix rows (>= 0).
    * \param cols    Number of matrix columns (>= 0).
    */
   GenericMatrix( const T& x, int rows, int cols ) : data( new Data( rows, cols ) )
   {
      pcl::Fill( data->Begin(), data->End(), x );
   }

   /*!
    * Constructs a matrix and initializes it with values from a static buffer.
    *
    * \param a       Address of the first item of a static array for
    *                initialization of matrix elements. The array must provide
    *                at least \a rows x \a cols consecutive elements stored in
    *                row order (all elements of the first matrix row followed
    *                by all elements of the second row, and so on).
    *
    * \param rows    Number of matrix rows (>= 0).
    *
    * \param cols    Number of matrix columns (>= 0).
    */
   template <typename T1>
   GenericMatrix( const T1* a, int rows, int cols ) : data( new Data( rows, cols ) )
   {
      if ( a != 0 )
         pcl::Copy( data->Begin(), a, a + NumberOfElements() );
   }

   /*!
    * Constructs a 3x3 matrix initialized with the specified element values.
    *
    * The resulting square matrix will be initialized as follows:
    *
    * <pre>
    * a00, a01, a02
    * a10, a11, a12
    * a20, a21, a22
    * </pre>
    */
   template <typename T1>
   GenericMatrix( const T1& a00, const T1& a01, const T1& a02,
                  const T1& a10, const T1& a11, const T1& a12,
                  const T1& a20, const T1& a21, const T1& a22 ) : data( new Data( 3, 3 ) )
   {
      T* v = data->Begin();
      v[0] = T( a00 ); v[1] = T( a01 ); v[2] = T( a02 );
      v[3] = T( a10 ); v[4] = T( a11 ); v[5] = T( a12 );
      v[6] = T( a20 ); v[7] = T( a21 ); v[8] = T( a22 );
   }

   /*!
    * Copy constructor. This object references the same data that is being
    * referenced by the specified matrix \a x.
    */
   GenericMatrix( const GenericMatrix<T>& x ) : data( x.data )
   {
      if ( data != 0 )
         data->Attach();
   }

   /*!
    * Constructs a matrix as a copy of a rectangular subset of another matrix.
    *
    * \param x       Source matrix from which this object will acquire element
    *                values.
    *
    * \param i0      Starting row in \a x. Corresponds to the first (top) row
    *                of this matrix.
    *
    * \param j0      Starting column in \a x. Corresponds to the first
    *                (leftmost) column of this matrix.
    *
    * \param rows    Number of matrix rows.
    *
    * \param cols    Number of matrix columns.
    *
    * If the specified submatrix is invalid, i.e. if the specified row or
    * column don't exist, or if the specified number of rows and columns exceed
    * the dimensions of the source matrix \a x, this constructor constrains the
    * submatrix coordinates and dimensions to the nearest valid values,
    * possibly constructing an empty matrix.
    */
   GenericMatrix( const GenericMatrix<T>& x, int i0, int j0, int rows, int cols ) : data( 0 )
   {
      i0 = Range( i0, 0, Max( 0, x.Rows()-1 ) );
      j0 = Range( j0, 0, Max( 0, x.Cols()-1 ) );
      rows = Range( rows, 0, Max( 0, x.Rows()-i0 ) );
      cols = Range( cols, 0, Max( 0, x.Cols()-j0 ) );
      data = new Data( rows, cols );
      for ( int i = 0; i < data->Rows(); ++i, ++i0 )
         for ( int j = 0; j < data->Cols(); ++j, ++j0 )
            data->v[i][j] = x.data->v[i0][j0];
   }

#ifndef __PCL_NO_MATRIX_IMAGE_CONVERSION

   /*!
    * Constructs a matrix from image pixel samples.
    *
    * This constructor converts pixel sample values to matrix elements for a
    * rectangular region of a channel of the specified \a image.
    *
    * \param image   %Image from which matrix elements will be generated.
    *
    * \param rect    A rectangular region in image pixel coordinates, from
    *                which this matrix will be generated. If this parameter is
    *                not specified, or if an empty rectangle is specified, this
    *                function will perform the matrix conversion for the
    *                current rectangular selection in the image, that is, for
    *                image.SelectedRectangle().
    *
    * \param channel Channel index. Must be the zero-based index of an existing
    *                channel in the image, or an integer < 0. If this parameter
    *                is not specified, or if a negative integer is specified,
    *                this function will perform the matrix conversion for the
    *                currently selected channel in the image, that is, for
    *                image.SelectedChannel().
    *
    * The newly created matrix is a representation of the intersection of the
    * specified (or implicitly selected) rectangular region with the image for
    * the scalar data type T of this matrix instantiation. All predefined pixel
    * sample types are supported, including integer, real and complex pixels.
    *
    * If there is no intersection between the rectangular region and the image,
    * this constructor yields an empty matrix.
    *
    * Note that if the source image is of a floating point type (either real or
    * complex) and the scalar type T of this matrix instantiation is of an
    * integer type, out-of-range matrix elements will be truncated to either
    * zero or to the maximum integer value, if the corresponding source pixel
    * sample is outside the normalized [0,1] range.
    */
   template <class P>
   GenericMatrix( const GenericImage<P>& image, const Rect& rect = Rect( 0 ), int channel = -1 ) : data( 0 )
   {
      (void)operator =( FromImage( image, rect, channel ) );
   }

   /*!
    * Constructs a matrix from image pixel samples.
    *
    * This constructor converts pixel sample values to matrix elements for a
    * rectangular region of a channel of the image transported by the specified
    * %ImageVariant.
    *
    * If the specified %ImageVariant object does not transport an image, this
    * constructor yields an empty matrix.
    *
    * Refer to the documentation for
    * GenericMatrix::GenericMatrix( const GenericImage&, const Rect&, int ) for
    * more information about the conversion performed by this constructor.
    */
   GenericMatrix( const ImageVariant& image, const Rect& rect = Rect( 0 ), int channel = -1 )
   {
      (void)operator =( FromImage( image, rect, channel ) );
   }

#endif   // !__PCL_NO_MATRIX_IMAGE_CONVERSION

   /*!
    * Destroys a %GenericMatrix object. This destructor dereferences the matrix
    * data. If the matrix data becomes unreferenced, it is destroyed and
    * deallocated immediately.
    */
   virtual ~GenericMatrix()
   {
      if ( data != 0 )
      {
         if ( !data->Detach() )
            delete data;
         data = 0;
      }
   }

   /*!
    * Deallocates matrix data, yielding an empty matrix.
    */
   void Clear()
   {
      if ( data != 0 )
         if ( data->IsUnique() )
            data->Deallocate();
         else
         {
            data->Detach();
            data = new Data( 0, 0 );
         }
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    *
    * If this instance and the specified source instance \a x reference
    * different matrix data, then the previously referenced data is
    * dereferenced and the new data (which is being referenced by \a x) is
    * referenced by this object. If the previous data becomes unreferenced
    * (garbage), then it is destroyed and deallocated immediately.
    *
    * If this instance and the specified source instance \a x reference the
    * same matrix data, then this function does nothing.
    */
   GenericMatrix& operator =( const GenericMatrix<T>& x )
   {
      if ( x.data != data )
      {
         x.data->Attach();
         if ( data != 0 )
            if ( !data->Detach() )
               delete data;
         data = x.data;
      }
      return *this;
   }

   /*!
    * Exchanges two matrices \a A and \a B.
    *
    * This function is efficient because it simply swaps the internal matrix
    * data pointers owned by both objects.
    */
   friend void Swap( GenericMatrix<T>& A, GenericMatrix<T>& B )
   {
      Swap( A.data, B.data );
   }

   /*!
    * Assigns a constant scalar \a x to all elements of this matrix. Returns a
    * reference to this object.
    *
    * Before assigning a constant value to all matrix elements, this function
    * ensures that this instance uniquely references its matrix data,
    * generating a new matrix data block if necessary.
    */
   GenericMatrix& operator =( const T& x )
   {
      if ( !IsUnique() )
      {
         Data* newData = new Data( data->Rows(), data->Cols() );
         if ( !data->Detach() )
            delete data;
         data = newData;
      }
      pcl::Fill( data->Begin(), data->End(), x );
      return *this;
   }

#define IMPLEMENT_SCALAR_ASSIGN_OP( op )                                      \
      if ( IsUnique() )                                                       \
      {                                                                       \
               T* a = data->Begin();                                          \
         const T* b = data->End();                                            \
         for ( ; a < b; ++a )                                                 \
            *a op##= x;                                                       \
      }                                                                       \
      else                                                                    \
      {                                                                       \
         Data* newData = new Data( data->Rows(), data->Cols() );              \
               T* a = newData->Begin();                                       \
         const T* b = newData->End();                                         \
         const T* c = data->Begin();                                          \
         for ( ; a < b; ++a, ++c )                                            \
            *a = *c op x;                                                     \
         if ( !data->Detach() )                                               \
            delete data;                                                      \
         data = newData;                                                      \
      }                                                                       \
      return *this;

   /*!
    * Adds a constant scalar \a x to all elements of this matrix. Returns a
    * reference to this object.
    *
    * Before adding a constant value to all matrix elements, this function
    * ensures that this instance uniquely references its matrix data,
    * generating a duplicate if necessary.
    */
   GenericMatrix& operator +=( const T& x )
   {
      IMPLEMENT_SCALAR_ASSIGN_OP( + )
   }

   /*!
    * Subtracts a constant scalar \a x from all elements of this matrix.
    * Returns a reference to this object.
    *
    * Before subtracting a constant value from all matrix elements, this
    * function ensures that this instance uniquely references its matrix data,
    * generating a duplicate if necessary.
    */
   GenericMatrix& operator -=( const T& x )
   {
      IMPLEMENT_SCALAR_ASSIGN_OP( - )
   }

   /*!
    * Multiplies all elements of this matrix by a constant scalar \a x. Returns
    * a reference to this object.
    *
    * Before multiplying all matrix elements by a constant value, this function
    * ensures that this instance uniquely references its matrix data,
    * generating a duplicate if necessary.
    */
   GenericMatrix& operator *=( const T& x )
   {
      IMPLEMENT_SCALAR_ASSIGN_OP( * )
   }

   /*!
    * Divides all elements of this matrix by a constant scalar \a x. Returns a
    * reference to this object.
    *
    * Before dividing all matrix elements by a constant value, this function
    * ensures that this instance uniquely references its matrix data,
    * generating a duplicate if necessary.
    */
   GenericMatrix& operator /=( const T& x )
   {
      IMPLEMENT_SCALAR_ASSIGN_OP( / )
   }

   /*!
    * Raises all elements of this matrix to a constant scalar \a x. Returns a
    * reference to this object.
    *
    * Before raising all matrix elements to a constant value, this function
    * ensures that this instance uniquely references its matrix data,
    * generating a duplicate if necessary.
    */
   GenericMatrix& operator ^=( const T& x )
   {
      if ( IsUnique() )
      {
               T* a = data->Begin();
         const T* b = data->End();
         for ( ; a < b; ++a )
            *a = pcl::Pow( *a, x );
      }
      else
      {
         Data* newData = new Data( data->Rows(), data->Cols() );
               T* a = newData->Begin();
         const T* b = newData->End();
         const T* c = data->Begin();
         for ( ; a < b; ++a, ++c )
            *a = pcl::Pow( *c, x );
         if ( !data->Detach() )
            delete data;
         data = newData;
      }
      return *this;
   }

#undef IMPLEMENT_SCALAR_ASSIGN_OP

   /*!
    * Returns the square of this matrix. The result is a new matrix of the same
    * dimensions where each element is the square of its counterpart in this
    * matrix.
    */
   GenericMatrix Sqr() const
   {
      GenericMatrix<T> R( data->Rows(), data->Cols() );
            T* r = R.data->Begin();
      const T* s = R.data->End();
      const T* a = data->Begin();
      for ( ; r < s; ++r, ++a )
         *r = *a * *a;
      return R;
   }

   /*!
    * Replaces all elements of this matrix with their squares.
    *
    * Before performing its task, this function ensures that this instance
    * uniquely references its matrix data, generating a duplicate if necessary.
    */
   void SetSqr()
   {
      if ( IsUnique() )
      {
               T* a = data->Begin();
         const T* b = data->End();
         for ( ; a < b; ++a )
            *a *= *a;
      }
      else
      {
         Data* newData = new Data( data->Rows(), data->Cols() );
               T* a = newData->Begin();
         const T* b = newData->End();
         const T* c = data->Begin();
         for ( ; a < b; ++a, ++c )
            *a = *c * *c;
         if ( !data->Detach() )
            delete data;
         data = newData;
      }
   }

   /*!
    * Returns the square root of this matrix. The result is a new matrix of the
    * same dimensions where each element is the square root of its counterpart
    * in this matrix.
    */
   GenericMatrix Sqrt() const
   {
      GenericMatrix<T> R( data->Rows(), data->Cols() );
            T* r = R.data->Begin();
      const T* s = R.data->End();
      const T* a = data->Begin();
      for ( ; r < s; ++r, ++a )
         *r = pcl::Sqrt( *a );
      return R;
   }

   /*!
    * Replaces all elements of this matrix with their square roots.
    *
    * Before performing its task, this function ensures that this instance
    * uniquely references its matrix data, generating a duplicate if necessary.
    */
   void SetSqrt()
   {
      if ( IsUnique() )
      {
               T* a = data->Begin();
         const T* b = data->End();
         for ( ; a < b; ++a )
            *a = pcl::Sqrt( *a );
      }
      else
      {
         Data* newData = new Data( data->Rows(), data->Cols() );
               T* a = newData->Begin();
         const T* b = newData->End();
         const T* c = data->Begin();
         for ( ; a < b; ++a, ++c )
            *a = pcl::Sqrt( *c );
         if ( !data->Detach() )
            delete data;
         data = newData;
      }
   }

   /*!
    * Returns the absolute value of this matrix. The result is a new matrix of
    * the same dimensions where each element is the absolute value of its
    * counterpart in this matrix.
    */
   GenericMatrix Abs() const
   {
      GenericMatrix<T> R( data->Rows(), data->Cols() );
            T* r = R.data->Begin();
      const T* s = R.data->End();
      const T* a = data->Begin();
      for ( ; r < s; ++r, ++a )
         *r = pcl::Abs( *a );
      return R;
   }

   /*!
    * Replaces all elements of this matrix with their absolute values.
    *
    * Before performing its task, this function ensures that this instance
    * uniquely references its matrix data, generating a duplicate if necessary.
    */
   void SetAbs()
   {
      if ( IsUnique() )
      {
               T* a = data->Begin();
         const T* b = data->End();
         for ( ; a < b; ++a )
            *a = pcl::Abs( *a );
      }
      else
      {
         Data* newData = new Data( data->Rows(), data->Cols() );
               T* a = newData->Begin();
         const T* b = newData->End();
         const T* c = data->Begin();
         for ( ; a < b; ++a, ++c )
            *a = pcl::Abs( *c );
         if ( !data->Detach() )
            delete data;
         data = newData;
      }
   }

   /*!
    * Returns true if this instance uniquely references its matrix data.
    */
   bool IsUnique() const
   {
      return data->IsUnique();
   }

   /*!
    * Returns true if this instance references (shares) the same matrix data as
    * another instance \a x.
    */
   bool IsAliasOf( const GenericMatrix<T>& x ) const
   {
      return data == x.data;
   }

   /*!
    * Ensures that this instance uniquely references its matrix data.
    *
    * If necessary, this member function generates a duplicate of the matrix
    * data, references it, and then decrements the reference counter of the
    * original matrix data.
    */
   void SetUnique()
   {
      if ( !IsUnique() )
      {
         Data* newData = new Data( data->Rows(), data->Cols() );
         pcl::Copy( newData->Begin(), data->Begin(), data->End() );
         if ( !data->Detach() )
            delete data;
         data = newData;
      }
   }

   /*!
    * Returns the number of rows in this matrix. If this object is an empty
    * matrix, this member function returns zero.
    */
   int Rows() const
   {
      return data->Rows();
   }

   /*!
    * Returns the number of columns in this matrix. If this object is an empty
    * matrix, this member function returns zero.
    */
   int Cols() const
   {
      return data->Cols();
   }

   /*!
    * Returns the number of columns in this matrix. If this object is an empty
    * matrix, this member function returns zero.
    *
    * This function is a synonym for Cols().
    */
   int Columns() const
   {
      return Cols();
   }

   /*!
    * Returns true if this is an empty matrix. An empty matrix has no elements,
    * and hence its dimensions are zero.
    */
   bool IsEmpty() const
   {
      return Rows() == 0 || Cols() == 0;
   }

   /*!
    * Returns true if this matrix is not empty. This operator is equivalent to:
    *
    * \code !IsEmpty(); \endcode
    */
   operator bool() const
   {
      return !IsEmpty();
   }

   /*!
    * Returns the total number of matrix elements in this object, or
    * Rows()*Cols().
    */
   size_type NumberOfElements() const
   {
      return data->NumberOfElements();
   }

   /*!
    * Returns the total number of bytes required to store the data contained in
    * this matrix.
    */
   size_type Size() const
   {
      return data->Size();
   }

   /*!
    * Equality operator. Returns true if this matrix is equal to another matrix
    * \a x.
    *
    * %Matrix comparisons are performed element-wise. Two matrices are equal if
    * both have the same dimensions and identical element values.
    */
   bool operator ==( const GenericMatrix<T>& x ) const
   {
      return IsAliasOf( x ) || SameDimensions( x ) && pcl::Equal( Begin(), x.Begin(), x.End() );
   }

   /*!
    * Less than relational operator. Returns true if this matrix is less than
    * another matrix \a x.
    *
    * In this operator, matrix comparisons are performed element-wise,
    * irrespective of matrix dimensions, until either two matrix elements
    * differ, or until the end of one of the matrices is reached. In the latter
    * case the shortest matrix is the lesser one.
    */
   bool operator <( const GenericMatrix<T>& x ) const
   {
      return !IsAliasOf( x ) && pcl::Compare( Begin(), End(), x.Begin(), x.End() ) < 0;
   }

   /*!
    * Returns true if this matrix has the same dimensions, i.e. the same number
    * of rows and columns, as another matrix \a x.
    */
   bool SameDimensions( const GenericMatrix<T>& x ) const
   {
      return Rows() == x.Rows() && Cols() == x.Cols();
   }

   /*!
    * Returns true if this matrix has the same elements as another matrix \a x.
    *
    * In this member function, matrix comparisons are performed element-wise,
    * irrespective of matrix dimensions. Note that two matrices can have
    * exactly the same elements even if their dimensions, i.e. their number of
    * rows and columns, are different. Only the number and order of elements
    * are relevant for this comparison.
    */
   bool SameElements( const GenericMatrix<T>& x ) const
   {
      if ( IsAliasOf( x ) )
         return true;
      if ( NumberOfElements() != x.NumberOfElements() )
         return false;
      const T* a  = Begin();
      const T* a1 = End();
      const T* b  = x.Begin();
      for ( ; ; ++a, ++b )
      {
         if ( a == a1 )
            return true;
         if ( *a != *b )
            return false;
      }
   }

   /*!
    * Returns a reference to the matrix element at row \a i and column \a j.
    *
    * Before returning, this function ensures that this instance uniquely
    * references its matrix data.
    */
   T& Element( int i, int j )
   {
      SetUnique();
      return data->Element( i, j );
   }

   /*!
    * Returns a reference to the immutable matrix element at row \a i and
    * column \a j.
    */
   const T& Element( int i, int j ) const
   {
      return data->Element( i, j );
   }

   /*!
    * Returns a pointer to the first matrix element of row \a i.
    *
    * All elements in row \a i are guaranteed to be stored at consecutive
    * locations addressable from the pointer returned by this function.
    *
    * Before returning, this function ensures that this instance uniquely
    * references its matrix data.
    */
   T* operator []( int i )
   {
      SetUnique();
      return data->v[i];
   }

   /*!
    * Returns a pointer to the immutable first matrix element of row \a i.
    *
    * All elements in row \a i are guaranteed to be stored at consecutive
    * locations addressable from the pointer returned by this function.
    */
   const T* operator []( int i ) const
   {
      return data->v[i];
   }

   /*!
    * Returns a pointer to the first matrix element, that is to the element at
    * row 0 and column 0 of this matrix.
    *
    * All matrix elements are guaranteed to be stored at consecutive locations
    * addressable from the pointer returned by this function. Matrix elements
    * are stored in row order: all elements of row 0 followed by all elements
    * of row 1, and so on.
    *
    * Before returning, this function ensures that this instance uniquely
    * references its matrix data.
    */
   T* Begin()
   {
      SetUnique();
      return data->Begin();
   }

   /*!
    * Returns a pointer to the immutable first matrix element, that is to the
    * element at row 0 and column 0 of this matrix.
    *
    * All matrix elements are guaranteed to be stored at consecutive locations
    * addressable from the pointer returned by this function. Matrix elements
    * are stored in row order: all elements of row 0 followed by all elements
    * of row 1, and so on.
    */
   const T* Begin() const
   {
      return data->Begin();
   }

   /*!
    * Returns a pointer to the first matrix element, that is to the element at
    * row 0 and column 0 of this matrix.
    *
    * This member function is a convenience alias to Begin().
    */
   T* operator *()
   {
      return Begin();
   }

   /*!
    * Returns a pointer to the immutable first matrix element, that is to the
    * element at row 0 and column 0 of this matrix.
    *
    * This member function is a convenience alias to Begin() const.
    */
   const T* operator *() const
   {
      return Begin();
   }

   /*!
    * Returns a pointer past the last matrix element, that is a pointer to an
    * (nonexistent) element located at row=Rows()-1 and column=Cols().
    *
    * All matrix elements are guaranteed to be stored in reverse order at
    * consecutive locations addressable from the pointer returned by this
    * function. Matrix elements are stored in row order (all elements of row n
    * followed by all elements of row n-1, and so on).
    *
    * Before returning, this function ensures that this instance uniquely
    * references its matrix data.
    */
   T* End()
   {
      SetUnique();
      return data->End();
   }

   /*!
    * Returns a pointer past the last immutable matrix element, that is a
    * pointer to an immutable (and nonexistent) element located at row=Rows()-1
    * and column=Cols().
    *
    * All matrix elements are guaranteed to be stored in reverse order at
    * consecutive locations addressable from the pointer returned by this
    * function. Matrix elements are stored in row order (all elements of row n
    * followed by all elements of row n-1, and so on).
    */
   const T* End() const
   {
      return data->End();
   }

   /*!
    * Returns a pointer to a pointer to the first element in this matrix.
    *
    * The returned pointer can be used as a common C array, where all matrix
    * rows are guaranteed to be addressable through consecutive pointers
    * starting from the pointer returned by this function.
    *
    * This member function does nothing to ensure that this instance uniquely
    * references its matrix data. Consequently, the caller must take into
    * account that all modifications made to matrix elements accessed through
    * the value returned by this function will apply to all instances sharing
    * the same matrix data.
    *
    * This function can be used to perform fast operations on matrix elements,
    * avoiding the overhead caused by deep copies of matrix data, when such
    * copies are not necessary. Typically this happens when two or more threads
    * work simultaneously over non-overlapping regions of the same matrix.
    */
   T** DataPtr()
   {
      return data->v;
   }

   /*!
    * Returns a vector with the matrix elements at the specified row \a i.
    */
   GenericVector<T> RowVector( int i ) const
   {
      GenericVector<T> r( data->Cols() );
      for ( int j = 0; j < data->Cols(); ++j )
         r[j] = data->v[i][j];
      return r;
   }

   /*!
    * Returns a vector with the matrix elements at the specified column \a j.
    */
   GenericVector<T> ColumnVector( int j ) const
   {
      GenericVector<T> c( data->Rows() );
      for ( int i = 0; i < data->Rows(); ++i )
         c[i] = data->v[i][j];
      return c;
   }

   /*!
    * Returns a vector with the matrix elements at the specified column \a j.
    *
    * This member function is an alias for ColumnVector().
    */
   GenericVector<T> ColVector( int j ) const
   {
      return ColumnVector( j );
   }

   /*!
    * Assigns the components of a vector \a r to the corresponding elements of
    * a matrix row \a i.
    */
   template <class V>
   void SetRow( int i, const V& r )
   {
      for ( int j = 0; j < data->Cols() && j < r.Length(); ++j )
         data->v[i][j] = T( r[j] );
   }

   /*!
    * Assigns the components of a vector \a c to the corresponding elements of
    * a matrix column \a i.
    */
   template <class V>
   void SetColumn( int j, const V& c )
   {
      for ( int i = 0; i < data->Rows() && i < c.Length(); ++i )
         data->v[i][j] = T( c[i] );
   }

   /*!
    * Assigns the components of a vector \a c to the corresponding elements of
    * a matrix column \a i.
    *
    * This member function is an alias for SetColumn().
    */
   template <class V>
   void SetCol( int j, const V& c )
   {
      SetColumn( j, c );
   }

   /*!
    * Returns a matrix whose first row is a copy of the specified vector \a r.
    * The rest of matrix elements are set to zero. The returned object is a
    * square matrix whose dimensions are equal to the length of \a r.
    */
   static GenericMatrix<T> FromRowVector( const GenericVector<T>& r )
   {
      GenericMatrix<T> R( T( 0 ), r.Length(), r.Length() );
      for ( int j = 0; j < r.Length(); ++j )
         R.data->v[0][j] = r[j];
      return R;
   }

   /*!
    * Returns a matrix whose first column is a copy of the specified vector
    * \a c. The rest of matrix elements are set to zero. The returned object is
    * a square matrix whose dimensions are equal to the length of \a c.
    */
   static GenericMatrix<T> FromColumnVector( const GenericVector<T>& c )
   {
      GenericMatrix<T> C( T( 0 ), c.Length(), c.Length() );
      for ( int i = 0; i < c.Length(); ++i )
         C.data->v[i][0] = c[i];
      return C;
   }

   /*!
    * This member function is an alias for FromColumnVector().
    */
   static GenericMatrix<T> FromColVector( const GenericVector<T>& c )
   {
      return FromColumnVector( c );
   }

   /*!
    * Returns a unit matrix of size \a n.
    *
    * A unit matrix is an <em>n</em> x <em>n</em> square matrix whose elements
    * are ones on its main diagonal and zeros elsewhere.
    */
   static GenericMatrix<T> UnitMatrix( int n )
   {
      GenericMatrix<T> One( T( 0 ), n, n );
      for ( int i = 0; i < n; ++i )
         One[i][i] = T( 1 );
      return One;
   }

   /*!
    * Returns the transpose of this matrix.
    *
    * The transpose of a matrix A is another matrix A<sup>T</sup> whose rows
    * are the columns of A (or, equivalently, whose columns are the rows of A).
    */
   GenericMatrix<T> Transpose() const
   {
      GenericMatrix<T> Tr( data->Cols(), data->Rows() );
      for ( int i = 0; i < data->Rows(); ++i )
         for ( int j = 0; j < data->Cols(); ++j )
            Tr.data->v[j][i] = data->v[i][j];
      return Tr;
   }

   /*!
    * Returns the inverse of this matrix.
    *
    * When a square matrix is multiplied by its inverse, the result is the
    * identity matrix. Non-square matrices do not have inverses.
    *
    * If this is an empty or non-square matrix, or if it is singular (i.e. if
    * its determinant is zero or insignificant), then this function throws the
    * appropriate Error exception.
    */
   GenericMatrix<T> Inverse() const
   {
      if ( Rows() != Cols() || Rows() == 0 )
         throw Error( "Invalid matrix inversion: Non-square or empty matrix." );

      /*
       * Use direct formulae to invert 1x1, 2x2 and 3x3 matrices.
       * Use Gauss-Jordan elimination to invert larger matrices.
       */
      switch ( Rows() )
      {
      case 1:
         {
            if ( 1 + *data->v[0] == 1 )
               throw Error( "Invalid matrix inversion: Singular matrix." );
            GenericMatrix<T> Ai( 1, 1 );
            Ai[0][0] = 1/(*data->v[0]);
            return Ai;
         }
      case 2:
         {
            const T* A0 = data->v[0];
            const T* A1 = data->v[1];

            // Determinant
            T d = A0[0]*A1[1] - A0[1]*A1[0];
            if ( 1 + d == 1 )
               throw Error( "Invalid matrix inversion: Singular matrix." );

            GenericMatrix<T> Ai( 2, 2 );
            Ai[0][0] =  A1[1]/d;
            Ai[0][1] = -A0[1]/d;
            Ai[1][0] = -A1[0]/d;
            Ai[1][1] =  A0[0]/d;
            return Ai;
         }
      case 3:
         {
            const T* A0 = data->v[0];
            const T* A1 = data->v[1];
            const T* A2 = data->v[2];

            // Determinant
            T d1 = A1[1]*A2[2] - A1[2]*A2[1];
            T d2 = A1[2]*A2[0] - A1[0]*A2[2];
            T d3 = A1[0]*A2[1] - A1[1]*A2[0];
            T d  = A0[0]*d1 + A0[1]*d2 + A0[2]*d3;
            if ( 1 + d == 1 )
               throw Error( "Invalid matrix inversion: Singular matrix." );

            GenericMatrix<T> Ai( 3, 3 );
            Ai[0][0] = d1/d;
            Ai[0][1] = (A2[1]*A0[2] - A2[2]*A0[1])/d;
            Ai[0][2] = (A0[1]*A1[2] - A0[2]*A1[1])/d;
            Ai[1][0] = d2/d;
            Ai[1][1] = (A2[2]*A0[0] - A2[0]*A0[2])/d;
            Ai[1][2] = (A0[2]*A1[0] - A0[0]*A1[2])/d;
            Ai[2][0] = d3/d;
            Ai[2][1] = (A2[0]*A0[1] - A2[1]*A0[0])/d;
            Ai[2][2] = (A0[0]*A1[1] - A0[1]*A1[0])/d;
            return Ai;
         }
      default:
         {
            void PCL_FUNC InPlaceGaussJordan( GenericMatrix<T>&, GenericMatrix<T>& );
            GenericMatrix<T> Ai( *this );
            GenericMatrix<T> B = UnitMatrix( Rows() );
            InPlaceGaussJordan( Ai, B );
            return Ai;
         }
      }
   }

   /*!
    * Inverts this matrix.
    *
    * When a square matrix is multiplied by its inverse, the result is the
    * identity matrix. Non-square matrices do not have inverses.
    *
    * If this is an empty or non-square matrix, or if it is singular (i.e. if
    * its determinant is zero or insignificant), then this function throws the
    * appropriate Error exception.
    */
   void Invert()
   {
      if ( Rows() <= 3 )
         (void)operator =( Inverse() );
      else
      {
         if ( Rows() != Cols() || Rows() == 0 )
            throw Error( "Invalid matrix inversion: Non-square or empty matrix." );
         void PCL_FUNC InPlaceGaussJordan( GenericMatrix<T>&, GenericMatrix<T>& );
         GenericMatrix<T> B = UnitMatrix( Rows() );
         InPlaceGaussJordan( *this, B );
      }
   }

   /*!
    * Flips this matrix. Flipping a matrix consists of rotating its elements by
    * 180 degrees.
    *
    * This function ensures that this instance uniquely references its matrix
    * data before performing matrix rotation.
    */
   void Flip()
   {
      SetUnique();
      pcl::Reverse( data->Begin(), data->End() );
   }

   /*!
    * Returns a flipped copy of this matrix. Flipping a matrix consists of
    * rotating its elements by 180 degrees.
    */
   GenericMatrix<T> Flipped() const
   {
      GenericMatrix<T> Tf( data->Cols(), data->Rows() );
      pcl::CopyReversed( Tf.data->End(), data->Begin(), data->End() );
      return Tf;
   }

   /*!
    * Truncates all matrix elements to the specified range.
    *
    * \param f0   Lower bound of the truncation range. If not specified, the
    *             default value is zero.
    *
    * \param f1   Upper bound of the truncation range. If not specified, the
    *             default value is one.
    *
    * After calling this member function, all matrix elements will be within
    * the  [\a f0,\a f1] range.
    */
   void Truncate( const T& f0 = T( 0 ), const T& f1 = T( 1 ) )
   {
      SetUnique();
      T* a = data->Begin();
      T* b = data->End();
      for ( ; a < b; ++a )
         if ( *a < f0 )
            *a = f0;
         else if ( *a > f1 )
            *a = f1;
   }

   /*!
    * Rescales all matrix elements to the specified range.
    *
    * \param f0   Lower bound of the rescaling range. If not specified, the
    *             default value is zero.
    *
    * \param f1   Upper bound of the rescaling range. If not specified, the
    *             default value is one.
    *
    * The rescaling operation is as follows. if \a m and \a M are the minimum
    * and maximum element values in this matrix, respectively, then for each
    * matrix element s, its rescaled value r is given by:
    *
    * \a r = \a f0 + (\a s - \a m)*(\a f1 - \a f0)/(\a M - \a m)
    */
   void Rescale( const T& f0 = T( 0 ), const T& f1 = T( 1 ) )
   {
      T v0 = MinElement();
      T v1 = MaxElement();
      if ( v0 != f0 || v1 != f1 )
      {
         SetUnique();
         if ( v0 != v1 )
         {
            if ( f0 != f1 )
            {
               T* a = data->Begin();
               T* b = data->End();
               double d = (double( f1 ) - double( f0 ))/(double( v1 ) - double( v0 ));
               if ( f0 == T( 0 ) )
                  for ( ; a < b; ++a )
                     *a = T( d*(*a - v0) );
               else
                  for ( ; a < b; ++a )
                     *a = T( d*(*a - v0) + f0 );
            }
            else
               pcl::Fill( data->Begin(), data->End(), f0 );
         }
         else
            pcl::Fill( data->Begin(), data->End(), pcl::Range( v0, f0, f1 ) );
      }
   }

   /*!
    * Sorts the elements of this matrix in ascending order.
    */
   void Sort()
   {
      pcl::Sort( data->Begin(), data->End() );
   }

   /*!
    * Sorts the elements of this matrix in ascending order. Ordering of matrix
    * elements is defined such that for any pair a, b of matrix elements, the
    * specified binary predicate p( a, b ) is true if a precedes b.
    */
   template <class BP>
   void Sort( BP p )
   {
      pcl::Sort( data->Begin(), data->End(), p );
   }

   /*!
    * Returns a pointer to the first immutable matrix element with the
    * specified value \a x, or zero if this matrix does not contain such value.
    */
   const T* Find( const T& x ) const
   {
      const T* p = pcl::LinearSearch( data->Begin(), data->End(), x );
      return (p != data->End()) ? p : 0;
   }

   /*!
    * Returns a pointer to the first immutable matrix element with the
    * specified value \a x, or zero if this matrix does not contain such value.
    * This function is an alias to Find().
    */
   const T* FindFirst( const T& x ) const
   {
      return Find( x );
   }

   /*!
    * Returns a pointer to the last immutable matrix element with the
    * specified value \a x, or zero if this matrix does not contain such value.
    */
   const T* FindLast( const T& x ) const
   {
      const T* p = pcl::LinearSearchLast( data->Begin(), data->End(), x );
      return (p != data->End()) ? p : -1;
   }

   /*!
    * Returns true if this matrix contains the specified value \a x.
    */
   bool Has( const T& x ) const
   {
      return pcl::LinearSearch( data->Begin(), data->End(), x ) != data->End();
   }

#ifndef __PCL_NO_MATRIX_STATISTICS

   /*!
    * Returns the value of the minimum element in this matrix.
    * For empty matrices, this function returns zero.
    */
   T MinElement() const
   {
      if ( !IsEmpty() )
         return *MinItem( data->Begin(), data->End() );
      return T( 0 );
   }

   /*!
    * Returns the value of the minimum element in this matrix and its
    * coordinates in the specified point \a p.
    * For empty matrices, this function returns zero and assigns zero to the
    * point coordinates.
    */
   T MinElement( Point& p ) const
   {
      if ( !IsEmpty() )
      {
         const T* m = MinItem( data->Begin(), data->End() );
         int d = m - data->Begin();
         p.y = d/data->Cols();
         p.x = d%data->Cols();
         return *m;
      }
      p = 0;
      return T( 0 );
   }

   /*!
    * Returns the value of the maximum element in this matrix.
    * For empty matrices, this function returns zero.
    */
   T MaxElement() const
   {
      if ( !IsEmpty() )
         return *MaxItem( data->Begin(), data->End() );
      return T( 0 );
   }

   /*!
    * Returns the value of the maximum element in this matrix and its
    * coordinates in the specified point \a p.
    * For empty matrices, this function returns zero and assigns zero to the
    * point coordinates.
    */
   T MaxElement( Point& p ) const
   {
      if ( !IsEmpty() )
      {
         const T* m = MaxItem( data->Begin(), data->End() );
         int d = m - data->Begin();
         p.y = d/data->Cols();
         p.x = d%data->Cols();
         return *m;
      }
      p = 0;
      return T( 0 );
   }

   /*!
    * Returns the sum of all matrix elements.
    *
    * For empty matrices, this function returns zero.
    */
   double Sum() const
   {
      return pcl::Sum( data->Begin(), data->End() );
   }

   /*!
    * Computes the sum of all matrix elements using a numerically stable
    * summation algorithm to minimize roundoff error.
    *
    * For empty matrices, this function returns zero.
    */
   double StableSum() const
   {
      return pcl::StableSum( data->Begin(), data->End() );
   }

   /*!
    * Returns the sum of the absolute values of all matrix elements.
    *
    * For empty matrices, this function returns zero.
    */
   double Modulus() const
   {
      return pcl::Modulus( data->Begin(), data->End() );
   }

   /*!
    * Computes the sum of the absolute values of all matrix elements using a
    * numerically stable summation algorithm to minimize roundoff error.
    *
    * For empty matrices, this function returns zero.
    */
   double StableModulus() const
   {
      return pcl::StableModulus( data->Begin(), data->End() );
   }

   /*!
    * Returns the sum of the squares of all matrix elements.
    *
    * For empty matrices, this function returns zero.
    */
   double SumOfSquares() const
   {
      return pcl::SumOfSquares( data->Begin(), data->End() );
   }

   /*!
    * Computes the sum of the squares of all matrix elements using a
    * numerically stable summation algorithm to minimize roundoff error.
    *
    * For empty matrices, this function returns zero.
    */
   double StableSumOfSquares() const
   {
      return pcl::StableSumOfSquares( data->Begin(), data->End() );
   }

   /*!
    * Returns the mean of the element values in this matrix.
    *
    * For empty matrices, this function returns zero.
    */
   double Mean() const
   {
      return pcl::Mean( data->Begin(), data->End() );
   }

   /*!
    * Computes the mean of the element values in this matrix using a
    * numerically stable summation algorithm to minimize roundoff error.
    *
    * For empty matrices, this function returns zero.
    */
   double StableMean() const
   {
      return pcl::StableMean( data->Begin(), data->End() );
   }

   /*!
    * Returns the variance from the mean for the values in this matrix.
    *
    * For matrices with less than two elements, this function returns zero.
    */
   double Variance() const
   {
      return pcl::Variance( data->Begin(), data->End() );
   }

   /*!
    * Returns the standard deviation from the mean for the values in this
    * matrix.
    *
    * For matrices with less than two elements, this function returns zero.
    */
   double StdDev() const
   {
      return pcl::StdDev( data->Begin(), data->End() );
   }

   /*!
    * Returns the median of element values in this matrix.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * Before computing the median, this function ensures that this instance
    * uniquely references its matrix data, generating a duplicate if necessary.
    *
    * \note This is a destructive median calculation algorithm: it alters the
    * order in the sequence of matrix elements. For a nondestructive version,
    * see the immutable version of this member function.
    */
   double Median()
   {
      SetUnique();
      return pcl::Median( data->Begin(), data->End() );
   }

   /*!
    * Returns the median of element values in this matrix without modifying
    * this object.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * This is a \e nondestructive median calculation routine that doesn't
    * modify the order of existing matrix elements. To achieve that goal,
    * this routine simply generates a temporary working matrix as a copy of
    * this object, then calls its Median() member function to obtain the
    * function's return value.
    */
   double Median() const
   {
      return GenericMatrix<T>( *this ).Median();
   }

   /*!
    * Returns the average absolute deviation with respect to the specified
    * \a center value.
    *
    * When the median of the matrix elements is used as the center value, this
    * function returns the average absolute deviation from the median, which is
    * a well-known estimator of dispersion.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \note To make the average absolute deviation about the median consistent
    * with the standard deviation of a normal distribution, it must be
    * multiplied by the constant 1.2533.
    */
   double AvgDev( double center ) const
   {
      return pcl::AvgDev( data->Begin(), data->End(), center );
   }

   /*!
    * Computes the average absolute deviation with respect to the specified
    * \a center value, using a numerically stable summation algorithm to
    * minimize roundoff error.
    *
    * When the median of the matrix elements is used as the center value, this
    * function returns the average absolute deviation from the median, which is
    * a well-known estimator of dispersion.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \note To make the average absolute deviation about the median consistent
    * with the standard deviation of a normal distribution, it must be
    * multiplied by the constant 1.2533.
    */
   double StableAvgDev( double center ) const
   {
      return pcl::StableAvgDev( data->Begin(), data->End(), center );
   }

   /*!
    * Returns the average absolute deviation from the median.
    *
    * The mean absolute deviation from the median is a well-known estimator of
    * dispersion.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \note To make the average absolute deviation about the median consistent
    * with the standard deviation of a normal distribution, it must be
    * multiplied by the constant 1.2533.
    */
   double AvgDev() const
   {
      return pcl::AvgDev( data->Begin(), data->End() );
   }

   /*!
    * Computes the average absolute deviation from the median using a
    * numerically stable summation algorithm to minimize roundoff error.
    *
    * The mean absolute deviation from the median is a well-known estimator of
    * dispersion.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \note To make the average absolute deviation about the median consistent
    * with the standard deviation of a normal distribution, it must be
    * multiplied by the constant 1.2533.
    */
   double StableAvgDev() const
   {
      return pcl::StableAvgDev( data->Begin(), data->End() );
   }

   /*!
    * Returns the median absolute deviation (MAD) with respect to the specified
    * \a center value.
    *
    * The MAD is a well-known robust estimator of scale.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \note To make the MAD estimator consistent with the standard deviation of
    * a normal distribution, it must be multiplied by the constant 1.4826.
    */
   double MAD( double center ) const
   {
      return pcl::MAD( data->Begin(), data->End(), center );
   }

   /*!
    * Returns the median absolute deviation from the median (MAD).
    *
    * The MAD is a well-known robust estimator of scale.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \note To make the MAD estimator consistent with the standard deviation of
    * a normal distribution, it must be multiplied by the constant 1.4826.
    */
   double MAD() const
   {
      return pcl::MAD( data->Begin(), data->End() );
   }

   /*!
    * Returns a biweight midvariance (BWMV).
    *
    * \param center  Reference center value. Normally, the median of the matrix
    *                elements should be used.
    *
    * \param sigma   A reference estimate of dispersion. Normally, the median
    *                absolute deviation from the median (MAD) of the matrix
    *                elements should be used.
    *
    * \param k       Rejection limit in sigma units. The default value is k=9.
    *
    * The square root of the biweight midvariance is a robust estimator of
    * scale. It is an efficient estimator with respect to many statistical
    * distributions (about 87% Gaussian efficiency), and appears to have a
    * breakdown point close to 0.5 (the same as MAD).
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \b References
    *
    * Rand R. Wilcox (2012), <em>Introduction to Robust Estimation and Hypothesis
    * Testing, 3rd Edition</em>, Elsevier Inc., Section 3.12.1.
    */
   double BiweightMidvariance( double center, double sigma, int k = 9 ) const
   {
      return pcl::BiweightMidvariance( data->Begin(), data->End(), center, sigma, k );
   }

   /*!
    * Returns the biweight midvariance (BWMV) with respect to the median and
    * median absolute deviation (MAD).
    *
    * \param k       Rejection limit in sigma units. The default value is k=9.
    *
    * The square root of the biweight midvariance is a robust estimator of
    * scale. It is an efficient estimator with respect to many statistical
    * distributions (about 87% Gaussian efficiency), and appears to have a
    * breakdown point close to 0.5 (the same as MAD).
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \b References
    *
    * Rand R. Wilcox (2012), <em>Introduction to Robust Estimation and Hypothesis
    * Testing, 3rd Edition</em>, Elsevier Inc., Section 3.12.1.
    */
   double BiweightMidvariance( int k = 9 ) const
   {
      double center = Median();
      double sigma = 1.4826*MAD( center );
      return pcl::BiweightMidvariance( data->Begin(), data->End(), center, sigma, k );
   }

   /*!
    * Returns a percentage bend midvariance (PBMV).
    *
    * \param center  Reference center value. Normally, the median of the matrix
    *                elements should be used.
    *
    * \param beta    Rejection parameter in the [0,0.5] range. Higher values
    *                improve robustness to outliers (i.e., increase the
    *                breakdown point of the estimator) at the expense of lower
    *                efficiency. The default value is beta=0.2.
    *
    * The square root of the percentage bend midvariance is a robust estimator
    * of scale. With the default beta=0.2, its Gaussian efficiency is 67%. With
    * beta=0.1, its efficiency is 85% but its breakdown is only 0.1.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \b References
    *
    * Rand R. Wilcox (2012), <em>Introduction to Robust Estimation and Hypothesis
    * Testing, 3rd Edition</em>, Elsevier Inc., Section 3.12.3.
    */
   double BendMidvariance( double center, double beta = 0.2 ) const
   {
      return pcl::BendMidvariance( data->Begin(), data->End(), center, beta );
   }

   /*!
    * Returns a percentage bend midvariance (PBMV) with respect to the median.
    *
    * \param beta    Rejection parameter in the [0,0.5] range. Higher values
    *                improve robustness to outliers (i.e., increase the
    *                breakdown point of the estimator) at the expense of lower
    *                efficiency. The default value is beta=0.2.
    *
    * The square root of the percentage bend midvariance is a robust estimator
    * of scale. With the default beta=0.2, its Gaussian efficiency is 67%. With
    * beta=0.1, its efficiency is 85% but its breakdown is only 0.1.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * \b References
    *
    * Rand R. Wilcox (2012), <em>Introduction to Robust Estimation and Hypothesis
    * Testing, 3rd Edition</em>, Elsevier Inc., Section 3.12.3.
    */
   double BendMidvariance( double beta = 0.2 ) const
   {
      return pcl::BendMidvariance( data->Begin(), data->End(), Median(), beta );
   }

   /*!
    * Returns the Sn scale estimator of Rousseeuw and Croux:
    *
    * Sn = c * low_median( high_median( |x_i - x_j| ) )
    *
    * where low_median() is the order statistic of rank (n + 1)/2, and
    * high_median() is the order statistic of rank n/2 + 1.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * The constant c = 1.1926 must be used to make the Sn estimator converge to
    * the standard deviation of a pure normal distribution. However, this
    * implementation does not apply it (it uses c=1 implicitly), for
    * consistency with other implementations of scale estimators.
    *
    * \b References
    *
    * P.J. Rousseeuw and C. Croux (1993), <em>Alternatives to the Median Absolute
    * Deviation,</em> Journal of the American Statistical Association, Vol. 88,
    * pp. 1273-1283.
    */
   double Sn() const
   {
      GenericMatrix<T> A( *this );
      return pcl::Sn( A.Begin(), A.End() );
   }

   /*!
    * Returns the Qn scale estimator of Rousseeuw and Croux:
    *
    * Qn = c * first_quartile( |x_i - x_j| : i < j )
    *
    * where first_quartile() is the order statistic of rank (n + 1)/4.
    *
    * For matrices with less than two elements, this function returns zero.
    *
    * The constant c = 2.2219 must be used to make the Qn estimator converge to
    * the standard deviation of a pure normal distribution. However, this
    * implementation does not apply it (it uses c=1 implicitly), for consistency
    * with other implementations of scale estimators.
    *
    * \b References
    *
    * P.J. Rousseeuw and C. Croux (1993), <em>Alternatives to the Median Absolute
    * Deviation,</em> Journal of the American Statistical Association, Vol. 88,
    * pp. 1273-1283.
    */
   double Qn() const
   {
      GenericMatrix<T> A( *this );
      return pcl::Qn( A.Begin(), A.End() );
   }

#endif   // !__PCL_NO_MATRIX_STATISTICS

#ifndef __PCL_NO_MATRIX_PHASE_MATRICES

   /*!
    * Returns the phase correlation matrix for two matrices \a A and \a B.
    * \ingroup phase_matrices
    */
   static GenericMatrix<T> PhaseCorrelationMatrix( const GenericMatrix<T>& A,
                                                   const GenericMatrix<T>& B )
   {
      if ( B.Rows() != A.Rows() || B.Cols() != A.Cols() )
         throw Error( "Invalid matrix dimensions in PhaseCorrelationMatrix()" );
      GenericMatrix<T> R( A.Rows(), A.Cols() );
      PhaseCorrelationMatrix( R.Begin(), R.End(), A.Begin(), B.Begin() );
      return R;
   }

   /*!
    * Returns the cross power spectrum matrix for two matrices \a A and \a B.
    * \ingroup phase_matrices
    */
   static GenericMatrix<T> CrossPowerSpectrumMatrix( const GenericMatrix<T>& A,
                                                     const GenericMatrix<T>& B )
   {
      if ( B.Rows() != A.Rows() || B.Cols() != A.Cols() )
         throw Error( "Invalid matrix dimensions in CrossPowerSpectrumMatrix()" );
      GenericMatrix<T> R( A.Rows(), A.Cols() );
      CrossPowerSpectrumMatrix( R.Begin(), R.End(), A.Begin(), B.Begin() );
      return R;
   }

#endif   // !__PCL_NO_MATRIX_PHASE_MATRICES

#ifndef __PCL_NO_MATRIX_IMAGE_RENDERING

   /*!
    * Renders this matrix as an image.
    *
    * The contents of the specified \a image will be replaced with a grayscale
    * rendition of this matrix, where the value of each pixel sample will be
    * proportional to its corresponding matrix component counterpart.
    *
    * Note that if this matrix has out-of-range values for the pixel sample
    * type of the target image, pixel saturation will occur at either the white
    * or black points, or both. This only happens when the target image is of
    * an integer type and this matrix contains floating point values outside
    * the normalized [0,1] range.
    *
    * If the target image is of a floating-point type (either real or complex)
    * and this matrix also stores floating point values, the output image may
    * require a rescaling or normalization operation to constrain all pixel
    * values to the normalized [0,1] range after calling this member function.
    */
   template <class P>
   void ToImage( GenericImage<P>& image ) const
   {
      image.AllocateData( Cols(), Rows() );
      typename P::sample* v = *image;
      const T* a = data->Begin();
      const T* b = data->End();
      for ( ; a < b; ++a, ++v )
         *v = P::ToSample( *a );
   }

   /*!
    * Renders this matrix as an image.
    *
    * The contents of the image transported by the specified %ImageVariant
    * object will be replaced with a grayscale rendition of this matrix, where
    * the value of each pixel sample will be proportional to its corresponding
    * matrix component counterpart.
    *
    * If the specified %ImageVariant does not transport an image, a new one
    * will be created in 32-bit floating point format.
    *
    * Also take into account the information given for
    * ToImage( GenericImage& ), relative to out-of-range values, which is
    * entirely applicable to this member function.
    */
   void ToImage( ImageVariant& image ) const
   {
      if ( !image )
         image.CreateFloatImage();

      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: ToImage( static_cast<Image&>( *image ) ); break;
         case 64: ToImage( static_cast<DImage&>( *image ) ); break;
         }
      else if ( image.IsComplexSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: ToImage( static_cast<ComplexImage&>( *image ) ); break;
         case 64: ToImage( static_cast<DComplexImage&>( *image ) ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: ToImage( static_cast<UInt8Image&>( *image ) ); break;
         case 16: ToImage( static_cast<UInt16Image&>( *image ) ); break;
         case 32: ToImage( static_cast<UInt32Image&>( *image ) ); break;
         }
   }

#endif   // !__PCL_NO_MATRIX_IMAGE_RENDERING

#ifndef __PCL_NO_MATRIX_IMAGE_CONVERSION

   /*!
    * Creates a matrix from image pixel samples.
    *
    * This static member function converts pixel sample values to matrix
    * elements for a rectangular region of a channel of the specified \a image.
    *
    * \param image   Image from which matrix elements will be generated.
    *
    * \param rect    A rectangular region in image pixel coordinates, from
    *                which the returned matrix will be generated. If this
    *                parameter is not specified, or if an empty rectangle is
    *                specified, this function will perform the matrix
    *                conversion for the current rectangular selection in the
    *                image, that is, for image.SelectedRectangle().
    *
    * \param channel Channel index. Must be the zero-based index of an existing
    *                channel in the image, or an integer < 0. If this parameter
    *                is not specified, or if a negative integer is specified,
    *                this function will perform the matrix conversion for the
    *                currently selected channel in the image, that is, for
    *                image.SelectedChannel().
    *
    * The returned matrix is a representation of the intersection of the
    * specified (or implicitly selected) rectangular region with the image for
    * the scalar data type T of this matrix instantiation. All predefined pixel
    * sample types are supported, including integer, real and complex pixels.
    *
    * If there is no intersection between the rectangular region and the image,
    * this function returns an empty matrix.
    *
    * Note that if the source image is of a floating point type (either real or
    * complex) and the scalar type T of this matrix instantiation is of an
    * integer type, out-of-range matrix elements will be truncated to either
    * zero or to the maximum integer value, if the corresponding source pixel
    * sample is outside the normalized [0,1] range.
    */
   template <class P>
   static GenericMatrix<T> FromImage( const GenericImage<P>& image, const Rect& rect = Rect( 0 ), int channel = -1 )
   {
      Rect r = rect;
      if ( r.IsRect() )
      {
         if ( !image.Clip( r ) )
            return GenericMatrix<T>();
      }
      else
      {
         if ( image.IsEmptySelection() )
            return GenericMatrix<T>();
         r = image.SelectedRectangle();
      }

      if ( channel < 0 )
         channel = image.SelectedChannel();

      if ( r == image.Bounds() )
      {
         GenericMatrix<T> M( image.Height(), image.Width() );
         const typename P::sample* v = image[channel];
               T* a = M.data->Begin();
         const T* b = M.data->End();
         for ( ; a < b; ++a, ++v )
            P::FromSample( *a, *v );
         return M;
      }
      else
      {
         int w = r.Width();
         int h = r.Height();
         GenericMatrix<T> M( h, w );
         T* a = M.data->Begin();
         for ( int i = r.y0; i < r.y1; ++i )
         {
            const typename P::sample* v = image.PixelAddress( r.x0, i, channel );
            for ( int j = 0; j < w; ++j )
               P::FromSample( *a++, *v++ );
         }
         return M;
      }
   }

   /*!
    * Creates a matrix from image pixel samples.
    *
    * This static member function converts pixel sample values to matrix
    * elements for a rectangular region of a channel of the image transported
    * by the specified %ImageVariant.
    *
    * If the specified %ImageVariant object does not transport an image, this
    * member function returns an empty matrix.
    *
    * Refer to the documentation for
    * FromImage( const GenericImage&, const Rect&, int ) for more information
    * about the conversion performed by this member function.
    */
   static GenericMatrix<T> FromImage( const ImageVariant& image, const Rect& rect = Rect( 0 ), int channel = -1 )
   {
      if ( image )
         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: return FromImage( static_cast<const Image&>( *image ), rect, channel );
            case 64: return FromImage( static_cast<const DImage&>( *image ), rect, channel );
            }
         else if ( image.IsComplexSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: return FromImage( static_cast<const ComplexImage&>( *image ), rect, channel );
            case 64: return FromImage( static_cast<const DComplexImage&>( *image ), rect, channel );
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8: return FromImage( static_cast<const UInt8Image&>( *image ), rect, channel );
            case 16: return FromImage( static_cast<const UInt16Image&>( *image ), rect, channel );
            case 32: return FromImage( static_cast<const UInt32Image&>( *image ), rect, channel );
            }

      return GenericMatrix<T>();
   }

#endif   // !__PCL_NO_MATRIX_IMAGE_CONVERSION

private:

   struct Data : public ReferenceCounter
   {
      int n; // rows
      int m; // columns
      T** v; // elements

      Data( int rows, int cols ) : ReferenceCounter(), n( 0 ), m( 0 ), v( 0 )
      {
         if ( rows > 0 && cols > 0 )
            Allocate( rows, cols );
      }

      ~Data()
      {
         Deallocate();
      }

      int Rows() const
      {
         return n;
      }

      int Cols() const
      {
         return m;
      }

      size_type NumberOfElements() const
      {
         return size_type( n )*size_type( m );
      }

      size_type Size() const
      {
         return NumberOfElements()*sizeof( T );
      }

      T* Begin() const
      {
         return (v != 0) ? *v : 0;
      }

      T* End() const
      {
         return (v != 0) ? *v + NumberOfElements() : 0;
      }

      T& Element( int i, int j ) const
      {
         return v[i][j];
      }

      void Allocate( int rows, int cols )
      {
         n = rows;
         m = cols;
         v = new T*[ n ];
         *v = new T[ NumberOfElements() ];
         for ( int i = 1; i < n; ++i )
            v[i] = v[i-1] + m;
      }

      void Deallocate()
      {
         PCL_PRECONDITION( refCount == 0 )
         if ( v != 0 )
         {
            if ( *v != 0 )
               delete [] *v;
            delete [] v;
            v = 0, n = m = 0;
         }
      }
   };

   Data* data;
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup matrix_operators Matrix Operators
 *
 * This section includes scalar-to-matrix and matrix-to-matrix arithmetic
 * operator functions that are not members of the GenericMatrix template class.
 */

/*!
 * Returns the sum of two matrices \a A and \a B.
 *
 * If the specified matrices are incompatible for matrix addition (because
 * their dimensions are different), this function throws an Error exception.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator +( const GenericMatrix<T>& A, const GenericMatrix<T>& B )
{
   if ( B.Rows() != A.Rows() || B.Cols() != A.Cols() )
      throw Error( "Invalid matrix addition." );
   GenericMatrix<T> R( A.Rows(), A.Cols() );
         T* r = R.Begin();
   const T* s = R.End();
   const T* a = A.Begin();
   const T* b = B.Begin();
   for ( ; r < s; ++r, ++a, ++b )
      *r = *a + *b;
   return R;
}

/*!
 * Returns the sum of a matrix \a A and a scalar \a x.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator +( const GenericMatrix<T>& A, const T& x )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
         T* r = R.Begin();
   const T* s = R.End();
   const T* a = A.Begin();
   for ( ; r < s; ++r, ++a )
      *r = *a + x;
   return R;
}

/*!
 * Returns the sum of a scalar \a x and a matrix \a A.
 *
 * This function exists to implement the commutative property of
 * scalar-to-matrix addition. It is equivalent to A + x.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator +( const T& x, const GenericMatrix<T>& A )
{
   return A + x;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the result of subtracting a matrix \a B from another matrix \a A.
 *
 * If the specified matrices are incompatible for matrix subtraction (because
 * their dimensions are different), this function throws an Error exception.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator -( const GenericMatrix<T>& A, const GenericMatrix<T>& B )
{
   if ( B.Rows() != A.Rows() || B.Cols() != A.Cols() )
      throw Error( "Invalid matrix subtraction." );
   GenericMatrix<T> R( A.Rows(), A.Cols() );
         T* r = R.Begin();
   const T* s = R.End();
   const T* a = A.Begin();
   const T* b = B.Begin();
   for ( ; r < s; ++r, ++a, ++b )
      *r = *a - *b;
   return R;
}

/*!
 * Returns the subtraction of a scalar \a x from a matrix \a A.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator -( const GenericMatrix<T>& A, const T& x )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
         T* r = R.Begin();
   const T* s = R.End();
   const T* a = A.Begin();
   for ( ; r < s; ++r, ++a )
      *r = *a - x;
   return R;
}

/*!
 * Returns the subtraction of a matrix \a A from a scalar \a x.
 *
 * This function exists because scalar-to-matrix subtraction is not a
 * commutative operation. A - x is not equal to x - A (the resulting matrix
 * elements have the same magnitudes but opposite signs).
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator -( const T& x, const GenericMatrix<T>& A )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
         T* r = R.Begin();
   const T* s = R.End();
   const T* a = A.Begin();
   for ( ; r < s; ++r, ++a )
      *r = x - *a;
   return R;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the product of two matrices \a A and \a B.
 *
 * If the specified matrices are incompatible for matrix multiplication
 * (because the number of rows in \a B is not equal to the number of columns in
 * \a A), this function throws an appropriate Error exception.
 *
 * Bear in mind that matrix multiplication has O(N^3) complexity. This is
 * relevant to multiply large matrices.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator *( const GenericMatrix<T>& A, const GenericMatrix<T>& B )
{
   int n = A.Rows();
   int m = B.Cols();
   int p = A.Cols();
   if ( B.Rows() != p )
      throw Error( "Invalid matrix multiplication." );
   GenericMatrix<T> R( n, m );
   for ( int i = 0; i < n; ++i )
      for ( int j = 0; j < m; ++j )
      {
         T rij = 0;
         for ( int k = 0; k < p; ++k )
            rij += A[i][k] * B[k][j];
         R[i][j] = rij;
      }
   return R;
}

/*!
 * Returns the product of a matrix \a A and a scalar \a x.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator *( const GenericMatrix<T>& A, const T& x )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
         T* r = R.Begin();
   const T* s = R.End();
   const T* a = A.Begin();
   for ( ; r < s; ++r, ++a )
      *r = *a * x;
   return R;
}

/*!
 * Returns the product of a scalar \a x and a matrix \a A.
 *
 * This function exists to implement the commutative property of
 * scalar-to-matrix multiplication. It is equivalent to A * x.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator *( const T& x, const GenericMatrix<T>& A )
{
   return A * x;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the result of dividing a matrix \a A by a scalar \a x.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator /( const GenericMatrix<T>& A, const T& x )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
         T* r = R.Begin();
   const T* s = R.End();
   const T* a = A.Begin();
   for ( ; r < s; ++r, ++a )
      *r = *a / x;
   return R;
}

/*!
 * Returns the result of dividing a scalar \a x by a matrix \a A.
 *
 * This function exists because scalar-to-matrix division is not a
 * commutative operation. A/x is not equal to x/A.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator /( const T& x, const GenericMatrix<T>& A )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
         T* r = R.Begin();
   const T* s = R.End();
   const T* a = A.Begin();
   for ( ; r < s; ++r, ++a )
      *r = x / *a;
   return R;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the result of raising a matrix \a A to a scalar \a x.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator ^( const GenericMatrix<T>& A, const T& x )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
         T* r = R.Begin();
   const T* s = R.End();
   const T* a = A.Begin();
   for ( ; r < s; ++r, ++a )
      *r = pcl::Pow( *a, x );
   return R;
}

/*!
 * Returns the result of raising a scalar \a x to a matrix \a A.
 *
 * This function exists because scalar-to-matrix division is not a
 * commutative operation. A/x is not equal to x/A.
 *
 * \ingroup matrix_operators
 */
template <typename T> inline
GenericMatrix<T> operator ^( const T& x, const GenericMatrix<T>& A )
{
   GenericMatrix<T> R( A.Rows(), A.Cols() );
         T* r = R.Begin();
   const T* s = R.End();
   const T* a = A.Begin();
   for ( ; r < s; ++r, ++a )
      *r = pcl::Pow( x, *a );
   return R;
}

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_MATRIX_INSTANTIATE

/*!
 * \defgroup matrix_types Predefined Matrix Types
 */

/*!
 * \class pcl::DMatrix
 * \ingroup matrix_types
 * \brief 64-bit floating point real matrix.
 *
 * %DMatrix is a template instantiation of GenericMatrix for \c double.
 */
typedef GenericMatrix<double>          DMatrix;

/*!
 * \class pcl::FMatrix
 * \ingroup matrix_types
 * \brief 32-bit floating point real matrix.
 *
 * %FMatrix is a template instantiation of GenericMatrix for \c float.
 */
typedef GenericMatrix<float>           FMatrix;

/*!
 * \class pcl::CharMatrix
 * \ingroup matrix_types
 * \brief 8-bit signed integer matrix.
 *
 * %CharMatrix is a template instantiation of GenericMatrix for \c int8.
 */
typedef GenericMatrix<int8>            CharMatrix;

/*!
 * \class pcl::ByteMatrix
 * \ingroup matrix_types
 * \brief 8-bit unsigned integer matrix.
 *
 * %ByteMatrix is a template instantiation of GenericMatrix for \c uint8.
 */
typedef GenericMatrix<uint8>           ByteMatrix;

/*!
 * \class pcl::IMatrix
 * \ingroup matrix_types
 * \brief Integer matrix.
 *
 * %IMatrix is a template instantiation of GenericMatrix for the \c int type.
 */
typedef GenericMatrix<int>             IMatrix;

/*!
 * \class pcl::UIMatrix
 * \ingroup matrix_types
 * \brief Unsigned integer matrix.
 *
 * %UIMatrix is a template instantiation of GenericMatrix for the \c unsigned
 * \c int type.
 */
typedef GenericMatrix<unsigned int>    UIMatrix;

/*!
 * \class pcl::I64Matrix
 * \ingroup matrix_types
 * \brief 64-bit integer matrix.
 *
 * %I64Matrix is a template instantiation of GenericMatrix for \c int64.
 */
typedef GenericMatrix<int64>           I64Matrix;

/*!
 * \class pcl::UI64Matrix
 * \ingroup matrix_types
 * \brief 64-bit unsigned integer matrix.
 *
 * %UI64Matrix is a template instantiation of GenericMatrix for \c int64.
 */
typedef GenericMatrix<uint64>          UI64Matrix;

/*!
 * \class pcl::Matrix
 * \ingroup matrix_types
 * \brief 64-bit floating point real matrix.
 *
 * %Matrix is an alias for DMatrix. It is a template instantiation of
 * GenericMatrix for the \c double type.
 */
typedef DMatrix                        Matrix;

#endif   // !__PCL_NO_MATRIX_INSTANTIATE

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Matrix_h

// ****************************************************************************
// EOF pcl/Matrix.h - Released 2014/10/29 07:34:07 UTC
