// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/FFT1D.h - Released 2014/10/29 07:34:06 UTC
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

#ifndef __PCL_FFT1D_h
#define __PCL_FFT1D_h

/// \file pcl/FFT1D.h

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

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Complex_h
#include <pcl/Complex.h>
#endif

#ifndef __PCL_Vector_h
#include <pcl/Vector.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

class FFT1DBase
{
protected:

   static int   OptimizedLength( int, fcomplex* );
   static int   OptimizedLength( int, dcomplex* );
   static int   OptimizedLength( int, float* );
   static int   OptimizedLength( int, double* );

   static void* Create( int, fcomplex* );
   static void* Create( int, dcomplex* );
   static void* Create( int, float* );
   static void* Create( int, double* );

   static void* CreateInv( int, fcomplex* );
   static void* CreateInv( int, dcomplex* );
   static void* CreateInv( int, float* );
   static void* CreateInv( int, double* );

   static void  Destroy( void* );

   static void  Transform( void*, fcomplex*, const fcomplex* );
   static void  Transform( void*, dcomplex*, const dcomplex* );
   static void  Transform( void*, fcomplex*, const float* );
   static void  Transform( void*, dcomplex*, const double* );
   static void  Transform( void*, float*,    const fcomplex* );
   static void  Transform( void*, double*,   const dcomplex* );
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup fft_1d Fast Fourier Transform Direction Specifiers
 */

/*!
 * \def PCL_FFT_FORWARD
 * \brief Indicates a Fourier transform.
 * \ingroup fft_1d
 */
#define PCL_FFT_FORWARD    -1

/*!
 * \def PCL_FFT_BACKWARD
 * \brief Indicates an inverse Fourier transform.
 * \ingroup fft_1d
 */
#define PCL_FFT_BACKWARD   +1

// ----------------------------------------------------------------------------

/*!
 * \class AbstractFFT
 * \brief Abstract base class of all fast Fourier transform classes
 *
 * The %AbstractFFT template class implements basic properties and functions
 * shared by all %FFT PCL classes.
 *
 * \sa GenericFFT, GenericRealFFT
 */
template <typename T>
class PCL_CLASS AbstractFFT : public FFT1DBase
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
    * Represents the container type used to store an out-of-place discrete
    * Fourier transform.
    */
   typedef complex_vector           transform;

   /*!
    * Constructs an %AbstractFFT object of the specified \a length.
    */
   AbstractFFT( int length ) : m_length( length ), m_handle( 0 ), m_handleInv( 0 ), m_dft()
   {
   }

   /*!
    * Virtual destructor.
    *
    * Destroys all internal control structures in this %AbstractFFT object,
    * including the current discrete Fourier transform, if it exists.
    */
   virtual ~AbstractFFT()
   {
      Release();
   }

   /*!
    * Returns the transform length of this %AbstractFFT object. The length is
    * the number of data items that can be transformed, and is specified in the
    * constructors of all %FFT classes.
    */
   int Length() const
   {
      return m_length;
   }

   /*!
    * Returns a duplicate of the current discrete Fourier transform as a vector
    * of complex values.
    *
    * For complex FFTs, the returned vector has Length() elements. For real
    * transforms, the returned vector has Length()/2 + 1 elements.
    *
    * If no %FFT has been performed on this object, this function returns an
    * empty vector.
   */
   transform DFT() const
   {
      return m_dft;
   }

   /*!
    * Returns a reference to the mutable current discrete Fourier transform in
    * this object. The transform is a vector of complex values.
    *
    * For complex FFTs, the returned vector has Length() elements. For real
    * transforms, the returned vector has Length()/2 + 1 elements.
    *
    * If no %FFT has been performed on this object, this function returns a
    * reference to an empty vector.
   */
   transform& DFT()
   {
      return m_dft;
   }

   /*!
    * Returns a pointer to the immutable first element in the current discrete
    * Fourier transform. The transform is a set of consecutive complex numbers
    * stored at the address returned by this member function.
    *
    * For complex FFTs, the transform is a sequence of Length() elements. For
    * real transforms, there are only Length()/2 + 1 elements.
    *
    * If no %FFT has been performed on this object, this member function
    * returns zero.
   */
   const complex* operator *() const
   {
      return *m_dft;
   }

   /*!
    * Returns a pointer to the mutable first element in the current discrete
    * Fourier transform. The transform is a set of consecutive complex numbers
    * stored at the address returned by this member function.
    *
    * For complex FFTs, the transform is a sequence of Length() elements. For
    * real transforms, there are only Length()/2 + 1 elements.
    *
    * If no %FFT has been performed on this object, this member function
    * returns zero.
   */
   complex* operator *()
   {
      return *m_dft;
   }

   /*!
    * Destroys all internal control structures in this %AbstractFFT object,
    * including the current discrete Fourier transform, if it exists.
    */
   virtual void Release()
   {
      if ( m_handle != 0 )
         this->Destroy( m_handle ), m_handle = 0;
      if ( m_handleInv != 0 )
         this->Destroy( m_handleInv ), m_handleInv = 0;
      m_dft = transform();
   }

private:

   int           m_length;

protected:

   mutable void* m_handle;      // Opaque pointers to internal control structures
   mutable void* m_handleInv;
   transform     m_dft;         // DFT of complex or real data
};

// ----------------------------------------------------------------------------

#define m_handle    this->m_handle
#define m_handleInv this->m_handleInv
#define m_dft       this->m_dft
#define m_length    this->Length()

// ----------------------------------------------------------------------------

/*!
 * \class GenericFFT
 * \brief Generic fast Fourier transform of complex data
 *
 * The %GenericFFT template class performs forward and inverse, out-of-place
 * fast Fourier transforms of complex data.
 *
 * For fast Fourier transforms of real-valued data, see the GenericRealFFT
 * template class.
 *
 * \sa AbstractFFT, GenericRealFFT
 */
template <typename T>
class PCL_CLASS GenericFFT : public AbstractFFT<T>
{
public:

   /*!
    * Identifies the base class of this %FFT class.
    */
   typedef AbstractFFT<T>                 base;

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
    * Represents the container type used to store an out-of-place discrete
    * Fourier transform.
    */
   typedef typename base::transform       transform;

   /*!
    * Constructs a %GenericFFT object of the specified length \a n.
    *
    * The current PCL implementation supports FFTs of arbitrary length, but the
    * underlying routines have been optimized for performance when the length
    * of the input vector can be factorized as follows:
    *
    * <tt>n = 2^n2 * 3^n3 * 4^n4 * 5^n5</tt>
    *
    * where n2, n3, n4, and n5 are arbitrary positive integers. For best
    * performance, you should call GenericFFT::OptimizedLength() to get the
    * smallest optimal length for your data, then place them on an array of the
    * obtained length, padded with zeros.
    */
   GenericFFT( int n ) : base( n )
   {
   }

   /*!
    * Virtual destructor.
    */
   virtual ~GenericFFT()
   {
   }

   /*!
    * Fast Fourier transform. Performs the %FFT of the specified vector of
    * complex values.
    *
    * The argument \a x must be the starting address of a contiguous block of
    * at least Length() elements.
    *
    * Returns a reference to this object.
    */
   GenericFFT& operator <<( const complex* x )
   {
      if ( m_dft.IsEmpty() )
         m_dft = transform( m_length );
      if ( m_handle == 0 )
         m_handle = this->Create( m_length, (complex*)0 );
      this->Transform( m_handle, *m_dft, x );
      return *this;
   }

   /*!
    * Inverse fast Fourier transform. Performs the inverse %FFT and stores the
    * result in the specified vector of complex values.
    *
    * The argument \a y must be the starting address of a contiguous block of
    * at least Length() elements.
    *
    * If no %FFT has been performed on this object (by a previous call to
    * operator <<()), this member function throws an Error exception.
    *
    * Returns a reference to this object.
    */
   GenericFFT& operator >>( complex* y ) const
   {
      if ( m_dft.IsEmpty() )
         throw Error( "Invalid out-of-place inverse FFT: No FFT has been performed." );
      if ( m_handleInv == 0 )
         m_handleInv = this->CreateInv( m_length, (complex*)0 );
      this->Transform( m_handleInv, y, *m_dft );
      return *this;
   }

   /*!
    * Fast Fourier transform. Performs the %FFT of a vector of complex values.
    *
    * The specified vector \a x must have at least Length() elements. Otherwise
    * an Error exception will be thrown.
    *
    * Returns a reference to this object.
    */
   GenericFFT& operator <<( const complex_vector& x )
   {
      PCL_PRECONDITION( x.Length() >= m_length )
      if ( x.Length() < m_length )
         throw Error( "Invalid FFT input vector length." );
      return operator <<( *x );
   }

   /*!
    * Inverse fast Fourier transform. Performs the inverse %FFT and stores the
    * result in a vector of complex values.
    *
    * The specified vector \a y must have at least Length() elements. Otherwise
    * an Error exception will be thrown.
    *
    * If no %FFT has been performed on this object (by a previous call to
    * operator <<()), this member function throws an Error exception.
    *
    * Returns a reference to this object.
    */
   GenericFFT& operator >>( complex_vector& y ) const
   {
      if ( y.Length() < m_length )
         throw Error( "Invalid FFT output vector length." );
      return operator >>( *y );
   }

   /*!
    * Performs the forward or inverse %FFT of an input vector of complex
    * values, and stores the result in a caller-supplied output vector.
    *
    * \param[in] x   Input vector. Must be the starting address of a contiguous
    *                sequence of at least Length() complex numbers.
    *
    * \param[out] y  Output vector, where the result of the transform will be
    *                stored. Must be the starting address of a contiguous
    *                sequence of at least Length() complex numbers.
    *
    * \param dir     Indicates the direction of the Fourier transform:\n
    *                \n
    *                <table border="1" cellpadding="4" cellspacing="0">
    *                <tr><td>PCL_FFT_FORWARD</td>  <td>The direct FFT is calculated.</td></tr>
    *                <tr><td>PCL_FFT_BACKWARD</td> <td>The inverse FFT is calculated.</td></tr>
    *                </table>\n
    *                \n
    *                This parameter is optional; the default value is
    *                PCL_FFT_FORWARD.
    *
    * The specified arguments \a x and \a y must be the starting addresses of
    * two different, non-overlapping contiguous blocks of at least Length()
    * elements.
    *
    * This member function does not change the current Fourier transform in
    * this object, if it exists.
    *
    * Returns a reference to this object.
    */
   GenericFFT& operator()( complex* y, const complex* x, int dir = PCL_FFT_FORWARD ) const
   {
      if ( dir == PCL_FFT_BACKWARD )
      {
         if ( m_handleInv == 0 )
            m_handleInv = this->CreateInv( m_length, (complex*)0 );
         this->Transform( m_handleInv, y, x );
      }
      else
      {
         if ( m_handle == 0 )
            m_handle = this->Create( m_length, (complex*)0 );
         this->Transform( m_handle, y, x );
      }
      return const_cast<GenericFFT&>( *this );
   }

   /*!
    * Returns the <em>optimized complex %FFT length</em> larger than or equal to
    * a given length \a n. The returned length will be optimal to perform a
    * %FFT of complex data with the current PCL implementation. The optimized
    * length can be used as the argument to the constructor of any %FFT class for
    * complex data transforms.
    */
   static int OptimizedLength( int n )
   {
      return FFT1DBase::OptimizedLength( n, (complex*)0 );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class GenericRealFFT
 * \brief Generic fast Fourier transform of real data
 *
 * The %GenericRealFFT template class performs forward and inverse,
 * out-of-place fast Fourier transforms of real-valued data. For real data in
 * the time domain, this class performs nearly twice as fast as Fourier
 * transform classes for complex data.
 *
 * For fast Fourier transforms of complex-valued data, see the GenericFFT
 * template class.
 *
 * \sa AbstractFFT, GenericFFT
 */
template <typename T>
class PCL_CLASS GenericRealFFT : public AbstractFFT<T>
{
public:

   /*!
    * Identifies the base class of this %FFT class.
    */
   typedef AbstractFFT<T>                 base;

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
    * Represents the container type used to store an out-of-place discrete
    * Fourier transform.
    */
   typedef typename base::transform       transform;

   /*!
    * Constructs a %GenericRealFFT object of the specified \a length.
    *
    * The current PCL implementation supports FFTs for real data of any even
    * length, but the underlying routines have been optimized for performance
    * when the length of the input vector can be factorized as follows:
    *
    * <tt>n = 2^n2 * 3^n3 * 4^n4 * 5^n5</tt>
    *
    * where n2, n3, n4, and n5 are arbitrary positive integers, and n is an
    * even integer. For best performance, you should call
    * GenericRealFFT::OptimizedLength() to get the smallest optimal length for
    * your real data, then place them on an array of the obtained length,
    * padded with zeros.
    *
    * \note We stress the fact that the specified \a length  must be an \e even
    * integer.
   */
   GenericRealFFT( int length ) : AbstractFFT<T>( length )
   {
      PCL_PRECONDITION( (length & 1) == 0 )
   }

   /*!
    * Virtual destructor.
    */
   virtual ~GenericRealFFT()
   {
   }

   /*!
    * Fast Fourier transform. Performs the %FFT of the specified vector of
    * real numbers.
    *
    * The argument \a x must be the starting address of a contiguous block of
    * at least Length() scalars.
    *
    * Returns a reference to this object.
    */
   GenericRealFFT& operator <<( const scalar* x )
   {
      if ( m_dft.IsEmpty() )
         m_dft = transform( m_length/2 + 1 );
      if ( m_handle == 0 )
         m_handle = this->Create( m_length, (scalar*)0 );
      this->Transform( m_handle, *m_dft, x );
      return *this;
   }

   /*!
    * Inverse fast Fourier transform. Performs the inverse %FFT and stores the
    * result in the specified vector of real values.
    *
    * The value of \a y must be the starting address of a contiguous block of
    * at least Length() elements.
    *
    * If no %FFT has been performed for this object (by a previous call to
    * operator <<()), this member function throws an Error exception.
    *
    * Returns a reference to this object.
    */
   GenericRealFFT& operator >>( scalar* y ) const
   {
      if ( m_dft.IsEmpty() )
         throw Error( "Invalid out-of-place inverse FFT: No FFT has been performed." );
      if ( m_handleInv == 0 )
         m_handleInv = this->CreateInv( m_length, (scalar*)0 );
      this->Transform( m_handleInv, y, *m_dft );
      return *this;
   }

   /*!
    * Fast Fourier transform. Performs the %FFT of a vector of real numbers.
    *
    * The specified vector \a x must have at least Length() elements. Otherwise
    * an Error exception will be thrown.
    *
    * Returns a reference to this object.
    */
   GenericRealFFT& operator <<( const vector& x )
   {
      if ( x.Length() < m_length )
         throw Error( "Invalid FFT input vector length." );
      return operator <<( *x );
   }

   /*!
    * Inverse fast Fourier transform. Performs the inverse %FFT and stores the
    * result in the specified vector of real values.
    *
    * The specified vector \a y must have at least Length() elements. Otherwise
    * an Error exception will be thrown.
    *
    * If no %FFT has been performed on this object (by a previous call to
    * operator <<()), this member function throws an Error exception.
    *
    * Returns a reference to this object.
    */
   GenericRealFFT& operator >>( vector& y ) const
   {
      if ( y.Length() < m_length )
         throw Error( "Invalid FFT output vector length." );
      return operator >>( *y );
   }

   /*!
    * Performs the %FFT of an input vector of real numbers, and stores the
    * result in a caller-supplied output vector of complex values.
    *
    * \param[in] x   Input vector. Must be the starting address of a contiguous
    *                sequence of at least Length() scalars.
    *
    * \param[out] y  Output vector, where the result of the transform will be
    *                stored. Must be the starting address of a contiguous
    *                sequence of at least Length()/2 + 1 complex numbers.
    *
    * The specified arguments \a x and \a y must be the starting addresses of
    * two different, non-overlapping contiguous blocks of data. The output
    * transform will be a sequence of Length()/2 + 1 complex numbers.
    *
    * This member function does not change the current Fourier transform in
    * this object, if any.
    *
    * Returns a reference to this object.
    */
   GenericRealFFT& operator()( complex* y, const scalar* x ) const
   {
      if ( m_handle == 0 )
         m_handle = this->Create( m_length, (scalar*)0 );
      this->Transform( m_handle, y, x );
      return const_cast<GenericRealFFT&>( *this );
   }

   /*!
    * Performs the inverse %FFT of an input vector of complex numbers, and
    * stores the result in a caller-supplied output vector of real values.
    *
    * \param[in] x   Input vector. Must be the starting address of a contiguous
    *                sequence of at least Length()/2 + 1 complex numbers,
    *                corresponding to a discrete real Fourier transform of
    *                length Length().
    *
    * \param[out] y  Output vector, where the result of the inverse transform
    *                will be stored. Must be the starting address of a
    *                contiguous sequence of at least Length() real numbers.
    *
    * The specified arguments \a x and \a y must be the starting addresses of
    * two different, non-overlapping contiguous blocks of data. The input
    * transform must be a sequence of Length()/2 + 1 complex numbers. The
    * output inverse transform will be a sequence of Length() real numbers.
    *
    * This member function does not change the current Fourier transform in
    * this object, if any.
    *
    * Returns a reference to this object.
    */
   GenericRealFFT& operator()( scalar* y, const complex* x ) const
   {
      if ( m_handleInv == 0 )
         m_handleInv = this->CreateInv( m_length, (scalar*)0 );
      this->Transform( m_handleInv, y, x );
      return const_cast<GenericRealFFT&>( *this );
   }

   /*!
    * Returns the <em>optimized real %FFT length</em> larger than or equal to a
    * given length \a n. The returned length will be optimal to perform a %FFT
    * of real data with the current PCL implementation. The optimized length
    * can be used as the argument to the constructor of any %FFT class for
    * real data transforms.
    */
   static int OptimizedLength( int n )
   {
      return FFT1DBase::OptimizedLength( n, (scalar*)0 );
   }
};

// ----------------------------------------------------------------------------

#undef m_handle
#undef m_handleInv
#undef m_dft
#undef m_length

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_FFT1D_INSTANTIATE

/*!
 * \defgroup fft_types_1d Fast Fourier Transforms
 */

/*!
 * \class pcl::FFFT
 * \ingroup fft_types_1d
 * \brief Fast Fourier transform of 32-bit floating point complex data.
 *
 * %FFFT is a template instantiation of GenericFFT for the \c float type.
 */
typedef GenericFFT<float>           FFFT;

/*!
 * \class pcl::DFFT
 * \ingroup fft_types_1d
 * \brief Fast Fourier transform of 64-bit floating point complex data.
 *
 * %DFFT is a template instantiation of GenericFFT for the \c double type.
 */
typedef GenericFFT<double>          DFFT;

/*!
 * \class pcl::FRealFFT
 * \ingroup fft_types_1d
 * \brief Fast Fourier transform of 32-bit floating point real data.
 *
 * %FRealFFT is a template instantiation of GenericRealFFT for \c float.
 */
typedef GenericRealFFT<float>       FRealFFT;

/*!
 * \class pcl::DRealFFT
 * \ingroup fft_types_1d
 * \brief Fast Fourier transform of 64-bit floating point real data.
 *
 * %DRealFFT is a template instantiation of GenericRealFFT for \c double.
 */
typedef GenericRealFFT<double>      DRealFFT;

/*!
 * \class pcl::FFT
 * \ingroup fft_types_1d
 * \brief Fast Fourier transform of 32-bit floating point complex data.
 *
 * %FFT is an alias for FFFT. It is a template instantiation of GenericFFT for
 * the \c float type.
 */
typedef FFFT                        FFT;

/*!
 * \class pcl::RealFFT
 * \ingroup fft_types_1d
 * \brief Fast Fourier transform of 32-bit floating point real data.
 *
 * %RealFFT is an alias for FRealFFT. It is a template instantiation of
 * GenericRealFFT for the \c float type.
 */
typedef FRealFFT                    RealFFT;

#endif // __PCL_NO_FFT1D_INSTANTIATE

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FFT1D_h

// ****************************************************************************
// EOF pcl/FFT1D.h - Released 2014/10/29 07:34:06 UTC
