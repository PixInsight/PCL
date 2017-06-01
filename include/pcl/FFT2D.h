//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.04.0827
// ----------------------------------------------------------------------------
// pcl/FFT2D.h - Released 2017-05-28T08:28:50Z
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

#ifndef __PCL_FFT2D_h
#define __PCL_FFT2D_h

/// \file pcl/FFT2D.h

/*
 * The FFT routines used in this PCL version are based on the KISS FFT library
 * by Mark Borgerding: http://sourceforge.net/projects/kissfft/
 *
 * KISS FFT LICENSE INFORMATION
 * ============================
 *
 * Copyright (c) 2003-2009 Mark Borgerding
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the author nor the names of any contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <pcl/Defs.h>

#include <pcl/FFT1D.h>
#include <pcl/Matrix.h>
#include <pcl/StatusMonitor.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FFT2DBase
{
protected:

   static void Transform( int, int, fcomplex*, const fcomplex*, int, StatusMonitor*, bool, int );
   static void Transform( int, int, dcomplex*, const dcomplex*, int, StatusMonitor*, bool, int );
   static void Transform( int, int, fcomplex*, const float*, StatusMonitor*, bool, int );
   static void Transform( int, int, dcomplex*, const double*, StatusMonitor*, bool, int );
   static void Transform( int, int, float*, const fcomplex*, StatusMonitor*, bool, int );
   static void Transform( int, int, double*, const dcomplex*, StatusMonitor*, bool, int );
};

// ----------------------------------------------------------------------------

/*!
 * \class AbstractFFT2D
 * \brief Abstract base class of all two-dimensional fast Fourier transform classes
 *
 * The %AbstractFFT2D template class implements basic properties and functions
 * shared by all 2-D %FFT PCL classes.
 *
 * \sa GenericFFT2D, GenericRealFFT2D
 */
template <typename T>
class PCL_CLASS AbstractFFT2D : public FFT2DBase
{
public:

   /*!
    * Represents a scalar in the context of this %FFT class.
    */
   typedef T                        scalar;

   /*!
    * Represents a complex number in the context of this %FFT class.
    */
   typedef Complex<T>               complex;

   /*!
    * Represents a vector of real numbers.
    */
   typedef GenericVector<scalar>    vector;

   /*!
    * Represents a vector of complex numbers.
    */
   typedef GenericVector<complex>   complex_vector;

   /*!
    * Represents a real matrix.
    */
   typedef GenericMatrix<scalar>    matrix;

   /*!
    * Represents a complex matrix.
    */
   typedef GenericMatrix<complex>   complex_matrix;

   /*!
    * Represents a discrete Fourier transform matrix.
    */
   typedef complex_matrix           transform;

   /*!
    * Constructs an %AbstractFFT2D object of the specified dimensions
    * \a rows and \a cols.
    */
   AbstractFFT2D( int rows, int cols ) :
      m_rows( rows ), m_cols( cols ), m_dft(), m_monitor( 0 ),
      m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
   {
   }

   /*!
    * Constructs an %AbstractFFT2D object of the specified dimensions
    * \a rows and \a cols, using the specified status \a monitor object.
    *
    * On each transform performed with this object, the status monitor will be
    * incremented by the sum of transform dimensions: \a rows + \a cols.
    */
   AbstractFFT2D( int rows, int cols, StatusMonitor& monitor ) :
      m_rows( rows ), m_cols( cols ), m_dft(), m_monitor( &monitor ),
      m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
   {
   }

   /*!
    * Virtual destructor.
    *
    * Destroys all internal control structures in this %AbstractFFT2D object,
    * including the current discrete Fourier transform, if exists.
    */
   virtual ~AbstractFFT2D()
   {
      Release();
   }

   /*!
    * Returns the number of rows in the 2-D transform of this %AbstractFFT2D
    * object. Transform dimensions are specified in the constructors of all 2-D
    * %FFT classes.
    */
   int Rows() const
   {
      return m_rows;
   }

   /*!
    * Returns the number of columns in the 2-D transform of this %AbstractFFT2D
    * object. Transform dimensions are specified in the constructors of all 2-D
    * %FFT classes.
    */
   int Cols() const
   {
      return m_cols;
   }

   /*!
    * Returns the total number of matrix elements in the 2-D data set of this
    * %AbstractFFT2D object, or Rows() multiplied by Cols(). The dimensions are
    * specified in the constructors of all 2-D %FFT classes.
    */
   int NumberOfElements() const
   {
      return m_rows*m_cols;
   }

   /*!
    * Returns a duplicate of the current discrete Fourier transform as a matrix
    * of complex values.
    *
    * For complex data transforms, the returned matrix has Rows()*Cols()
    * elements. For real data, the returned matrix has Rows()*(Cols()/2 + 1)
    * elements.
    *
    * If no %FFT has been performed on this object, this function returns an
    * empty matrix.
    */
   transform DFT() const
   {
      return m_dft;
   }

   /*!
    * Returns a non-const reference to the current discrete Fourier transform
    * in this object. The transform is a matrix of complex values.
    *
    * For complex data transforms, the returned matrix has Rows()*Cols()
    * elements. For real data, the returned matrix has Rows()*(Cols()/2 + 1)
    * elements.
    *
    * If no %FFT has been performed on this object, this function returns a
    * reference to an empty matrix.
    */
   transform& DFT()
   {
      return m_dft;
   }

   /*!
    * Destroys all internal control structures in this %FFT object, including
    * the current Fourier transform, if exists.
    */
   virtual void Release()
   {
      m_dft = transform();
   }

   /*!
    * Returns true iff this object is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this instance.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance. If \a enable is false this parameter is ignored.
    *                A value <= 0 is ignored. The default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( enable && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this instance.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this instance.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   int MaxProcessors() const
   {
      return m_maxProcessors;
   }

   /*!
    * Sets the maximum number of processors allowed for this instance.
    *
    * In the current version of PCL, a module can use a maximum of 1023
    * processors. The term \e processor actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   void SetMaxProcessors( int maxProcessors )
   {
      m_maxProcessors = unsigned( Range( maxProcessors, 1, PCL_MAX_PROCESSORS ) );
   }

private:

   // Transform dimensions
   int            m_rows;
   int            m_cols;

protected:

   transform      m_dft;   // DFT of complex or real data
   StatusMonitor* m_monitor;
   bool           m_parallel      : 1;
   unsigned       m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT; // Maximum number of processors allowed
};

// ----------------------------------------------------------------------------

#define m_dft           this->m_dft
#define m_rows          this->Rows()
#define m_cols          this->Cols()
#define m_monitor       this->m_monitor
#define m_parallel      this->m_parallel
#define m_maxProcessors this->m_maxProcessors

// ----------------------------------------------------------------------------

/*!
 * \class GenericFFT2D
 * \brief Generic two-dimensional fast Fourier transform of complex data
 *
 * The %GenericFFT2D template class performs two-dimensional forward and
 * inverse, out-of-place fast Fourier transforms of complex data.
 *
 * For fast Fourier transforms of 2-D real-valued data, see the
 * GenericRealFFT2D template class.
 *
 * \sa AbstractFFT2D, GenericRealFFT2D
 */
template <typename T>
class PCL_CLASS GenericFFT2D : public AbstractFFT2D<T>
{
public:

   /*!
    * Identifies the base class of this %FFT class.
    */
   typedef AbstractFFT2D<T>               base;

   /*!
    * Represents a scalar in the context of this %FFT class.
    */
   typedef typename base::scalar          scalar;

   /*!
    * Represents a complex number in the context of this %FFT class.
    */
   typedef typename base::complex         complex;

   /*!
    * Represents a vector of real numbers.
    */
   typedef typename base::vector          vector;

   /*!
    * Represents a vector of complex numbers.
    */
   typedef typename base::complex_vector  complex_vector;

   /*!
    * Represents a real matrix.
    */
   typedef typename base::matrix          matrix;

   /*!
    * Represents a complex matrix.
    */
   typedef typename base::complex_matrix  complex_matrix;

   /*!
    * Represents a discrete Fourier transform matrix.
    */
   typedef typename base::transform       transform;

   /*!
    * Constructs a %GenericFFT2D object of the specified dimensions \a rows and
    * \a cols.
    *
    * The current PCL implementation supports FFTs of arbitrary length, but the
    * underlying routines have been optimized for performance when the length
    * of the input vector can be factorized as follows:
    *
    * <tt>n = 2^n2 * 3^n3 * 4^n4 * 5^n5</tt>
    *
    * where n2, n3, n4, and n5 are arbitrary positive integers. For best
    * performance, you should call GenericFFT2D::OptimizedLength() to get the
    * smallest optimal dimensions for your data, then place them on a matrix of
    * the obtained dimensions, padded with zeros.
    */
   GenericFFT2D( int rows, int cols ) : base( rows, cols )
   {
   }

   /*!
    * Constructs a %GenericFFT2D object of the specified dimensions \a rows and
    * \a cols, using the specified status monitoring object \a status.
    *
    * On each transform performed with this object, the status m_monitor will be
    * incremented by the sum of transform dimensions: \a rows + \a cols.
    *
    * See the GenericFFT2D( int, int ) constructor for more information.
    */
   GenericFFT2D( int rows, int cols, StatusMonitor& status ) : base( rows, cols, status )
   {
   }

   /*!
    * Virtual destructor.
    */
   virtual ~GenericFFT2D()
   {
   }

   /*!
    * Fast Fourier transform. Performs the two-dimensional %FFT of the
    * specified matrix of complex values.
    *
    * The argument \a x must be the starting address of a contiguous block of
    * at least NumberOfElements() elements, stored in row order: all elements
    * of the first matrix row followed by all elements of the second row, and
    * so on.
    *
    * Returns a reference to this object.
    */
   GenericFFT2D& operator <<( const complex* x )
   {
      if ( m_dft.IsEmpty() )
         m_dft = transform( m_rows, m_cols );
      this->Transform( m_rows, m_cols, *m_dft, x, PCL_FFT_FORWARD, m_monitor, m_parallel, m_maxProcessors );
      return *this;
   }

   /*!
    * Inverse fast Fourier transform. Performs the two-dimensional inverse %FFT
    * and stores the result in the specified matrix of complex values.
    *
    * The argument \a y must be the starting address of a contiguous block of
    * at least NumberOfElements() elements. The result matrix will be stored in
    * row order: all elements of the first matrix row followed by all elements
    * of the second row, and so on.
    *
    * If no %FFT has been performed on this object (by a previous call to
    * operator <<()), this member function throws an Error exception.
    *
    * Returns a reference to this object.
    */
   GenericFFT2D& operator >>( complex* y ) const
   {
      if ( m_dft.IsEmpty() )
         throw Error( "Invalid out-of-place inverse FFT: No FFT has been performed." );
      this->Transform( m_rows, m_cols, y, *m_dft, PCL_FFT_BACKWARD, m_monitor, m_parallel, m_maxProcessors );
      return *this;
   }

   /*!
    * Fast Fourier transform. Performs the two-dimensional %FFT of a matrix of
    * complex values.
    *
    * The specified matrix \a x must have Rows() and Cols() dimensions.
    * Otherwise an Error exception will be thrown.
    *
    * Returns a reference to this object.
    */
   GenericFFT2D& operator <<( const complex_matrix& x )
   {
      PCL_PRECONDITION( x.Rows() == m_rows )
      PCL_PRECONDITION( x.Cols() == m_cols )
      if ( x.Rows() != m_rows || x.Cols() != m_cols )
         throw Error( "Invalid FFT input matrix dimensions." );
      return operator <<( *x );
   }

   /*!
    * Inverse fast Fourier transform. Performs the two-dimensional inverse %FFT
    * and stores the result in a matrix of complex values.
    *
    * The specified matrix \a y must have Rows() and Cols() dimensions.
    * Otherwise an Error exception will be thrown.
    *
    * If no %FFT has been performed on this object (by a previous call to
    * operator <<()), this member function throws an Error exception.
    *
    * Returns a reference to this object.
    */
   GenericFFT2D& operator >>( complex_matrix& y ) const
   {
      PCL_PRECONDITION( y.Rows() == m_rows )
      PCL_PRECONDITION( y.Cols() == m_cols )
      if ( y.Rows() != m_rows || y.Cols() != m_cols )
         throw Error( "Invalid FFT output matrix dimensions." );
      return operator >>( *y );
   }

   /*!
    * Performs the forward or inverse two-dimensional %FFT of an input matrix
    * of complex values, and stores the result in a caller-supplied output
    * matrix.
    *
    * \param[in] x   Input matrix. Must be the starting address of a contiguous
    *                sequence of at least NumberOfElements() complex numbers,
    *                stored in row order: all elements of the first row
    *                followed by all elements of the second row, and so on.
    *
    * \param[out] y  Output matrix, where the result of the transform will be
    *                stored. Must be the starting address of a contiguous
    *                sequence of at least NumberOfElements() complex numbers.
    *                The result will be stored in row order: all elements of
    *                the first row followed by all elements of the second row,
    *                and so on.
    *
    * \param dir     Indicates the direction of the Fourier transform:\n
    *                \n
    *                <table border="1" cellpadding="4" cellspacing="0">
    *                <tr><td>PCL_FFT_FORWARD</td>  <td>The FFT is calculated.</td></tr>
    *                <tr><td>PCL_FFT_BACKWARD</td> <td>The inverse FFT is calculated.</td></tr>
    *                </table>\n
    *                \n
    *                This parameter is optional; the default value is
    *                PCL_FFT_FORWARD.
    *
    * This member function does not change the current Fourier transform in
    * this object, if it exists.
    *
    * Returns a reference to this object.
    */
   GenericFFT2D& operator ()( complex* y, const complex* x, int dir = PCL_FFT_FORWARD ) const
   {
      this->Transform( m_rows, m_cols, y, x, dir, m_monitor, m_parallel, m_maxProcessors );
      return const_cast<GenericFFT2D&>( *this );
   }

   /*!
    * Returns the <em>optimized complex %FFT length</em> larger than or equal to
    * a given length \a n. The returned length will be optimal to perform a
    * %FFT of complex data with the current PCL implementation. The optimized
    * length can be used as the \a rows or \a cols argument to the constructor
    * of any two-dimensional %FFT class for complex data transforms.
    */
   static int OptimizedLength( int n )
   {
      return GenericFFT<T>::OptimizedLength( n );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class GenericRealFFT2D
 * \brief Generic two-dimensional fast Fourier transform of real data
 *
 * The %GenericRealFFT2D template class performs forward and inverse,
 * out-of-place fast two-dimensional Fourier transforms of real-valued data.
 * For real data in the time domain, this class performs nearly twice as fast
 * as Fourier transform classes for complex data.
 *
 * For two-dimensional fast Fourier transforms of complex-valued data, see the
 * GenericFFT2D template class.
 *
 * \sa AbstractFFT2D, GenericFFT2D
 */
template <typename T>
class PCL_CLASS GenericRealFFT2D : public AbstractFFT2D<T>
{
public:

   /*!
    * Identifies the base class of this %FFT class.
    */
   typedef AbstractFFT2D<T>               base;

   /*!
    * Represents a scalar in the context of this %FFT class.
    */
   typedef typename base::scalar          scalar;

   /*!
    * Represents a complex number in the context of this %FFT class.
    */
   typedef typename base::complex         complex;

   /*!
    * Represents a vector of real numbers.
    */
   typedef typename base::vector          vector;

   /*!
    * Represents a vector of complex numbers.
    */
   typedef typename base::complex_vector  complex_vector;

   /*!
    * Represents a real matrix.
    */
   typedef typename base::matrix          matrix;

   /*!
    * Represents a complex matrix.
    */
   typedef typename base::complex_matrix  complex_matrix;

   /*!
    * Represents a discrete Fourier transform matrix.
    */
   typedef typename base::transform       transform;

   /*!
    * Constructs a %GenericRealFFT2D object of the specified dimensions \a rows
    * and \a cols.
    *
    * The current PCL implementation supports FFTs for real data of any even
    * length, but the underlying routines have been optimized for performance
    * when the length of the input vector can be factorized as follows:
    *
    * <tt>n = 2^n2 * 3^n3 * 4^n4 * 5^n5</tt>
    *
    * where n2, n3, n4, and n5 are arbitrary positive integers, and n is an
    * even integer. For best performance, you should call
    * GenericRealFFT2D::OptimizedLength() to get the smallest optimal
    * dimensions for your real-valued data, then place them on a matrix of the
    * obtained dimensions, padded with zeros.
    *
    * \note We stress the fact that the specified dimensions \a rows and
    * \a cols must be \e even integer numbers.
    */
   GenericRealFFT2D( int rows, int cols ) : base( rows, cols )
   {
   }

   /*!
    * Constructs a %GenericRealFFT2D object of the specified dimensions \a rows
    * and \a cols, using the specified status monitoring object \a status.
    *
    * On each transform performed with this object, the status m_monitor will be
    * incremented by the sum of transform dimensions: \a rows + \a cols.
    *
    * See the GenericRealFFT2D( int, int ) constructor for more information.
    */
   GenericRealFFT2D( int rows, int cols, StatusMonitor& status ) : base( rows, cols, status )
   {
   }

   /*!
    * Virtual destructor.
    */
   virtual ~GenericRealFFT2D()
   {
   }

   /*!
    * Fast Fourier transform. Performs the two-dimensional %FFT of the
    * specified matrix of real values.
    *
    * The argument \a x must be the starting address of a contiguous block of
    * at least NumberOfElements() elements, stored in row order: all elements
    * of the first matrix row followed by all elements of the second row, and
    * so on.
    *
    * Returns a reference to this object.
    */
   GenericRealFFT2D& operator <<( const scalar* x )
   {
      if ( m_dft.IsEmpty() )
         m_dft = transform( m_rows, m_cols/2 + 1 );
      this->Transform( m_rows, m_cols, *m_dft, x, m_monitor, m_parallel, m_maxProcessors );
      return *this;
   }

   /*!
    * Inverse fast Fourier transform. Performs the two-dimensional inverse %FFT
    * and stores the result in the specified matrix of real values.
    *
    * The argument \a y must be the starting address of a contiguous block of
    * at least NumberOfElements() elements. The result matrix will be stored in
    * row order: all elements of the first matrix row followed by all elements
    * of the second row, and so on.
    *
    * If no %FFT has been performed on this object (by a previous call to
    * operator <<()), this member function throws an Error exception.
    *
    * Returns a reference to this object.
    */
   GenericRealFFT2D& operator >>( scalar* y ) const
   {
      if ( m_dft.IsEmpty() )
         throw Error( "Invalid out-of-place inverse FFT: No FFT has been performed." );
      this->Transform( m_rows, m_cols, y, *m_dft, m_monitor, m_parallel, m_maxProcessors );
      return *this;
   }

   /*!
    * Fast Fourier transform. Performs the two-dimensional %FFT of a matrix of
    * real numbers.
    *
    * The specified matrix \a x must have Rows() and Cols() dimensions.
    * Otherwise an Error exception will be thrown.
    *
    * Returns a reference to this object.
    */
   GenericRealFFT2D& operator <<( const matrix& x )
   {
      PCL_PRECONDITION( x.Rows() == m_rows )
      PCL_PRECONDITION( x.Cols() == m_cols )
      if ( x.Rows() != m_rows || x.Cols() != m_cols )
         throw Error( "Invalid FFT input matrix dimensions." );
      return operator <<( *x );
   }

   /*!
    * Inverse fast Fourier transform. Performs the two-dimensional inverse %FFT
    * and stores the result in a matrix of real numbers.
    *
    * The specified matrix \a y must have Rows() and Cols() dimensions.
    * Otherwise an Error exception will be thrown.
    *
    * If no %FFT has been performed on this object (by a previous call to
    * operator <<()), this member function throws an Error exception.
    *
    * Returns a reference to this object.
    */
   GenericRealFFT2D& operator >>( matrix& y ) const
   {
      PCL_PRECONDITION( y.Rows() == m_rows )
      PCL_PRECONDITION( y.Cols() == m_cols )
      if ( y.Rows() != m_rows || y.Cols() != m_cols )
         throw Error( "Invalid FFT output matrix dimensions." );
      return operator >>( *y );
   }

   /*!
    * Performs the two-dimensional %FFT of an input matrix of real numbers, and
    * stores the result in an output matrix of complex numbers.
    *
    * \param[in] x   Input matrix. Must be the starting address of a contiguous
    *                sequence of at least NumberOfElements() real numbers,
    *                stored in row order: all elements of the first row
    *                followed by all elements of the second row, and so on.
    *
    * \param[out] y  Output matrix, where the result of the transform will be
    *                stored. Must be the starting address of a contiguous
    *                sequence of at least Rows()*(Cols()/2 + 1) complex
    *                numbers. The result will be stored in row order: all
    *                elements of the first row followed by all elements of the
    *                second row, and so on, where each row has Cols()/2 + 1
    *                complex numbers.
    *
    * The output matrix will have Rows() rows and Cols()/2 + 1 columns, stored
    * in row order.
    *
    * This member function does not change the current Fourier transform in
    * this object, if it exists.
    *
    * Returns a reference to this object.
    */
   GenericRealFFT2D& operator()( complex* y, const scalar* x ) const
   {
      this->Transform( m_rows, m_cols, y, x, m_monitor, m_parallel, m_maxProcessors );
      return const_cast<GenericRealFFT2D&>( *this );
   }

   /*!
    * Performs the inverse two-dimensional %FFT of an input matrix of complex
    * numbers, and stores the result in an output matrix of real numbers.
    *
    * \param[in] x   Input matrix. Must be the starting address of a contiguous
    *                sequence of at least Rows()*(Cols()/2 + 1) complex
    *                numbers, stored in row order: all elements of the first
    *                row followed by all elements of the second row, and so on,
    *                where each row must have Cols()/2 + 1 complex numbers.
    *
    * \param[out] y  Output matrix, where the result of the transform will be
    *                stored. Must be the starting address of a contiguous
    *                sequence of at least NumberOfElements() real numbers. The
    *                result will be stored in row order: all elements of the
    *                first row followed by all elements of the second row, and
    *                so on.
    *
    * The input matrix must have Rows() rows and Cols()/2 + 1 columns, stored
    * in row order.
    *
    * This member function does not change the current Fourier transform in
    * this object, if it exists.
    *
    * Returns a reference to this object.
    */
   GenericRealFFT2D& operator()( scalar* y, const complex* x ) const
   {
      this->Transform( m_rows, m_cols, y, x, m_monitor, m_parallel, m_maxProcessors );
      return const_cast<GenericRealFFT2D&>( *this );
   }

   /*!
    * Returns the <em>optimized complex %FFT length</em> larger than or equal to
    * a given length \a n. The returned length will be optimal to perform a
    * %FFT of real data with the current PCL implementation. The optimized
    * length can be used as the \a rows or \a cols argument to the constructor
    * of any two-dimensional %FFT class for real-valued data transforms.
    */
   static int OptimizedLength( int n )
   {
      return GenericFFT<T>::OptimizedLength( n );
   }
};

// ----------------------------------------------------------------------------

#undef m_dft
#undef m_rows
#undef m_cols
#undef m_monitor
#undef m_parallel
#undef m_maxProcessors

// ----------------------------------------------------------------------------

/*!
 * \defgroup fft_2d Two-Dimensional Fast Fourier Transforms
 */

#ifndef __PCL_NO_FFT2D_INSTANTIATE

/*!
 * \class pcl::FFFT2D
 * \ingroup fft_2d
 * \brief Fast Fourier transform of 32-bit floating point complex data.
 *
 * %FFFT2D is a template instantiation of GenericFFT2D for the \c float type.
 */
typedef GenericFFT2D<float>         FFFT2D;

/*!
 * \class pcl::DFFT2D
 * \ingroup fft_2d
 * \brief Fast Fourier transform of 64-bit floating point complex data.
 *
 * %DFFT2D is a template instantiation of GenericFFT2D for the \c double type.
 */
typedef GenericFFT2D<double>        DFFT2D;

/*!
 * \class pcl::FRealFFT2D
 * \ingroup fft_2d
 * \brief Fast Fourier transform of 32-bit floating point real data.
 *
 * %FRealFFT2D is a template instantiation of GenericRealFFT2DT for \c float.
 */
typedef GenericRealFFT2D<float>     FRealFFT2D;

/*!
 * \class pcl::DRealFFT2D
 * \ingroup fft_2d
 * \brief Fast Fourier transform of 64-bit floating point real data.
 *
 * %DRealFFT2D is a template instantiation of GenericRealFFT2D for \c double.
 */
typedef GenericRealFFT2D<double>    DRealFFT2D;

/*!
 * \class pcl::FFT2D
 * \ingroup fft_2d
 * \brief Fast Fourier transform of 32-bit floating point complex data.
 *
 * %FFT2D is an alias for FFFT2D. It is a template instantiation of
 * GenericFFT2D for the \c float type.
 */
typedef FFFT2D                      FFT2D;

/*!
 * \class pcl::RealFFT2D
 * \ingroup fft_2d
 * \brief Fast Fourier transform of 32-bit floating point real data.
 *
 * %RealFFT2D is an alias for FRealFFT2D. It is a template instantiation of
 * GenericRealFFT2D for the \c float type.
 */
typedef FRealFFT2D                  RealFFT2D;

#endif // __PCL_NO_FFT2D_INSTANTIATE

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FFT2D_h

// ----------------------------------------------------------------------------
// EOF pcl/FFT2D.h - Released 2017-05-28T08:28:50Z
