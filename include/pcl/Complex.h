//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/Complex.h - Released 2016/02/21 20:22:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_Complex_h
#define __PCL_Complex_h

/// \file pcl/Complex.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Math_h
#include <pcl/Math.h>
#endif

#ifndef __PCL_Constants_h
#include <pcl/Constants.h>
#endif

#ifndef __PCL_Relational_h
#include <pcl/Relational.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * ### NB: Template class Complex<T> cannot have virtual member functions.
 *         This is because sizeof( Complex<T> ) must be equal to 2*sizeof( T ).
 */

#define PCL_ASSERT_COMPLEX_SIZE() \
   static_assert( sizeof( *this ) == 2*sizeof( T ), "Invalid sizeof( Complex<> )" )

#define real   C[0]
#define imag   C[1]

/*!
 * \class Complex
 * \brief Generic complex number
 *
 * %Complex is a careful implementation of complex numbers, including efficient
 * computations of complex arithmetic and transcendental functions.
 *
 * When instantiated for the \c double and \c float types, the generated
 * classes are binary-compatible with the C99 complex standard structure.
 */
template <typename T>
class PCL_CLASS Complex
{
public:

   /*!
    * Represents a component of a complex number: real or imaginary part.
    */
   typedef T   component;

   /*!
    * Constructs an uninitialized complex number. The real and imaginary
    * components will have unpredictable values.
    */
   Complex()
   {
      PCL_ASSERT_COMPLEX_SIZE();
   }

   /*!
    * Constructs a complex number with the specified real and imaginary parts.
    */
   Complex( T r, T i = 0 )
   {
      PCL_ASSERT_COMPLEX_SIZE();
      real = r;
      imag = i;
   }

   /*!
    * Copy constructor.
    */
   template <typename T1>
   Complex( const Complex<T1>& c )
   {
      PCL_ASSERT_COMPLEX_SIZE();
      real = T( c.Real() );
      imag = T( c.Imag() );
   }

   /*!
    * Returns a copy of the real component of this complex number.
    */
   constexpr T Real() const
   {
      return real;
   }

   /*!
    * Returns a reference to the real component of this complex number.
    */
   T& Real()
   {
      return real;
   }

   /*!
    * Returns a copy of the imaginary component of this complex number.
    */
   constexpr T Imag() const
   {
      return imag;
   }

   /*!
    * Returns a reference to the imaginary component of this complex number.
    */
   T& Imag()
   {
      return imag;
   }

   /*!
    * Returns true iff the imaginary part of this complex number is zero.
    */
   constexpr bool IsReal() const
   {
      return imag == 0;
   }

   /*!
    * Assignment operator. Returns a reference to this complex number.
    */
   template <typename T1>
   Complex<T>& operator =( const Complex<T1>& c )
   {
      real = T( c.Real() ), imag = T( c.Imag() );
      return *this;
   }

   /*!
    * Assignment/sum operator. Adds the specified complex number \a c to this.
    * Returns a reference to this complex number.
    */
   template <typename T1>
   Complex<T>& operator +=( const Complex<T1>& c )
   {
      real += c.Real(), imag += c.Imag();
      return *this;
   }

   /*!
    * Assignment/subtract operator. Subtracts the specified complex number \a c
    * from this. Returns a reference to this complex number.
    */
   template <typename T1>
   Complex<T>& operator -=( const Complex<T1>& c )
   {
      real -= c.Real(), imag -= c.Imag();
      return *this;
   }

   /*!
    * Assignment/multiply operator. Multiplies this complex number by the
    * specified complex \a c. Returns a reference to this complex number.
    */
   template <typename T1>
   Complex<T>& operator *=( const Complex<T1>& c )
   {
      T t = T( real*c.Real() - imag*c.Imag() );
      imag = T( imag*c.Real() + real*c.Imag() );
      real = t;
      return *this;
   }

   /*!
    * Assignment/divide operator. Divides this complex number by the specified
    * complex \a c. Returns a reference to this complex number.
    */
   template <typename T1>
   Complex<T>& operator /=( const Complex<T1>& c )
   {
      T r, d, t;
      if ( pcl::Abs( c.Real() ) >= pcl::Abs( c.Imag() ) )
      {
         PCL_PRECONDITION( c.Real() != 0 )
         r = T( c.Imag()/c.Real() );
         d = T( c.Real() + r*c.Imag() );
         t = T( (real + r*imag)/d );
         imag = (imag - r*real)/d;
      }
      else
      {
         PCL_PRECONDITION( c.Imag() != 0 )
         r = T( c.Real()/c.Imag() );
         d = T( c.Imag() + r*c.Real() );
         t = T( (real*r + imag)/d );
         imag = (imag*r - real)/d;
      }
      real = t;
      return *this;
   }

   /*!
    * Assigns a real number to this complex number. The specified scalar \a x
    * is assigned to the real component, and the imaginary component is set to
    * zero. Returns a reference to this complex number.
    */
   template <typename T1>
   Complex<T>& operator =( T1 x )
   {
      real = x, imag = 0;
      return *this;
   }

   /*!
    * Adds a real number to this complex number. The specified scalar \a x is
    * added to the real component. Returns a reference to this complex number.
    */
   template <typename T1>
   Complex<T>& operator +=( T1 x )
   {
      real += x;
      return *this;
   }

   /*!
    * Subtracts a real number from this complex number. The specified scalar \a x
    * is subtracted from the real component. Returns a reference to this
    * complex number.
    */
   template <typename T1>
   Complex<T>& operator -=( T1 x )
   {
      real -= x;
      return *this;
   }

   /*!
    * Multiplies this complex number by a real number. The real component is
    * multiplied by the specified scalar \a x. Returns a reference to this
    * complex number.
    */
   template <typename T1>
   Complex<T>& operator *=( T1 x )
   {
      real *= x, imag *= x;
      return *this;
   }

   /*!
    * Divides this complex number by a real number. The real component is
    * divided by the specified scalar \a x. Returns a reference to this complex
    * number.
    */
   template <typename T1>
   Complex<T>& operator /=( T1 x )
   {
      PCL_PRECONDITION( x != 0 )
      real /= x, imag /= x;
      return *this;
   }

   /*!
    * Unary plus operator. Returns a copy of this complex number.
    */
   Complex<T> operator +() const
   {
      return *this;
   }

   /*!
    * Unary minus operator. Returns a complex number symmetric to this.
    *
    * This function is equivalent to:
    * Complex<T>( -Real(), -Imag() )
    */
   Complex<T> operator -() const
   {
      return Complex<T>( -real, -imag );
   }

   /*!
    * Complex conjugate. This function is equivalent to:
    * Complex<T>( Real(), -Imag() )
    */
   Complex<T> Conj() const
   {
      return Complex<T>( real, -imag );
   }

   /*!
    * Complex conjugate. This operator is a synonym for Conj().
    */
   Complex<T> operator ~() const
   {
      return Conj();
   }

   /*!
    * Assigns to this complex number its complex conjugate. This function
    * changes the sign of the imaginary part of this complex number.
    */
   void SetConj()
   {
      imag = -imag;
   }

   /*!
    * Complex magnitude (modulus).
    *
    * Accurate results are guaranteed even for large ratios between the real
    * and complex components.
    */
   T Mag() const
   {
      T r = pcl::Abs( real );
      T i = pcl::Abs( imag );
      T m;
      if ( r == 0 )
         m = i;
      else if ( i == 0 )
         m = r;
      else
      {
         bool q = r < i;
         m = q ? r/i : i/r;
         m = (q ? i : r) * pcl::Sqrt( 1 + m*m );
      }
      return m;
   }

   /*!
    * Complex norm.
    * This function is equivalent to: Real()*Real() + Imag()*Imag().
    */
   constexpr T Norm() const
   {
      return real*real + imag*imag;
   }

   /*!
    * Complex argument.
    * Returns the arc whose tangent is Imag()/Real(). When both real and
    * imaginary components are zero, this function returns zero.
    */
   constexpr T Arg() const
   {
      // Degenerate cases (real=0) are correctly handled by real ArcTan(). For
      // the undefined case real=imag=0, we silently return zero. Should we
      // throw an exception instead?
      return (real != 0 || imag != 0) ? pcl::ArcTan( imag, real ) : 0;
   }

private:

   /*
    * C99 binary-compatible complex components.
    */
   T C[ 2 ];   // C[0] = real, C[1] = imaginary
};

#undef real
#undef imag

#undef PCL_ASSERT_COMPLEX_SIZE

// ----------------------------------------------------------------------------

/*!
 * \defgroup complex_arithmetic_ops Complex Arithmetic Operators
 */

/*!
 * \defgroup complex_basic_functions Complex Basic Functions
 */

/*!
 * Complex absolute value. This function returns the complex magnitude of the
 * specified complex number \a c.
 * \ingroup complex_basic_functions
 */
template <typename T> inline
T Abs( const Complex<T>& c )
{
   return c.Mag();
}

/*!
 * Returns a complex number from its polar coordinates: radial distance \a r
 * and polar angle theta. The polar angle theta is given by its sine and
 * cosine, \a stheta and \a ctheta, respectively.
 * \ingroup complex_basic_functions
 */
template <typename T> inline
Complex<T> Polar( T r, T stheta, T ctheta )
{
   return Complex<T>( r*ctheta, r*stheta );
}

/*!
 * Returns a complex number from its polar coordinates: radial distance \a r
 * and polar angle \a theta. The polar angle is specified in radians.
 * \ingroup complex_basic_functions
 */
template <typename T> inline
Complex<T> Polar( T r, T theta )
{
   return Polar( r, pcl::Sin( theta ), pcl::Cos( theta ) );
}

// ----------------------------------------------------------------------------

/*!
 * Complex addition. Returns the sum of two complex numbers \a c1 and \a c2.
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T1> operator +( const Complex<T1>& c1, const Complex<T2>& c2 )
{
   return Complex<T1>( T1( c1.Real() + c2.Real() ),
                       T1( c1.Imag() + c2.Imag() ) );
}

/*!
 * Complex addition. Returns the sum of a complex number \a c and a real \a x.
 *
 * \note The commutativity of complex-real addition is implemented by the
 * existence of this function along with operator +( T1, const Complex<T2>& ).
 *
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T1> operator +( const Complex<T1>& c, T2 x )
{
   return Complex<T1>( T1( c.Real()+x ), c.Imag() );
}

/*!
 * Complex addition. Returns the sum of a real number \a x and a complex \a c.
 *
 * \note The commutativity of complex-real addition is implemented by the
 * existence of this function along with operator +( const Complex<T1>&, T2 ).
 *
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T2> operator +( T1 x, const Complex<T2>& c )
{
   return c + x;
}

/*!
 * Complex subtraction. Returns the difference between two complex numbers
 * \a c1 and \a c2.
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T1> operator -( const Complex<T1>& c1, const Complex<T2>& c2 )
{
   return Complex<T1>( T1( c1.Real() - c2.Real() ),
                       T1( c1.Imag() - c2.Imag() ) );
}

/*!
 * Complex subtraction. Returns the difference between a complex number \a c
 * and a real \a x.
 *
 * \note The noncommutativity of complex-real subtraction is implemented by
 * the existence of this function along with
 * operator -( T1, const Complex<T2>& ).
 *
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T1> operator -( const Complex<T1>& c, T2 x )
{
   return Complex<T1>( T1( c.Real()-x ), c.Imag() );
}

/*!
 * Complex subtraction. Returns the difference between a real number \a x
 * and a complex \a c.
 *
 * \note The noncommutativity of complex-real subtraction is implemented by
 * the existence of this function along with
 * operator -( const Complex<T1>&, T2 ).
 *
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T2> operator -( T1 x, const Complex<T2>& c )
{
   return Complex<T2>( T2( x-c.Real() ), -c.Imag() );
}

/*!
 * Complex multiplication. Returns the multiplication of two complex numbers
 * \a c1 and \a c2.
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T1> operator *( const Complex<T1>& c1, const Complex<T2>& c2 )
{
   return Complex<T1>( T1( c1.Real()*c2.Real() - c1.Imag()*c2.Imag() ),
                       T1( c1.Imag()*c2.Real() + c1.Real()*c2.Imag() ) );
}

/*!
 * Complex multiplication. Returns the multiplication of a complex number \a c
 * and a real \a x.
 *
 * \note The commutativity of complex-real multiplication is implemented by the
 * existence of this function along with
 * operator *( T1, const Complex<T2>& ).
 *
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T1> operator *( const Complex<T1>& c, T2 x )
{
   return Complex<T1>( T1( c.Real()*x ), c.Imag()*x );
}

/*!
 * Complex multiplication. Returns the multiplication of a real number \a x and
 * a complex \a c.
 *
 * \note The commutativity of complex-real multiplication is implemented by the
 * existence of this function along with
 * operator *( const Complex<T1>&, T2 ).
 *
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T2> operator *( T1 x, const Complex<T2>& c )
{
   return c * x;
}

/*!
 * Complex division. Returns the division between two complex numbers
 * \a c1 and \a c2.
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T1> operator /( const Complex<T1>& c1, const Complex<T2>& c2 )
{
   Complex<T1> c;
   T2 r, d;
   if ( pcl::Abs( c2.Real() ) >= pcl::Abs( c2.Imag() ) )
   {
      PCL_PRECONDITION( c2.Real() != 0 )
      r = c2.Imag() / c2.Real();
      d = c2.Real() + r*c2.Imag();
      c.Real() = T1( (c1.Real() + r*c1.Imag())/d );
      c.Imag() = T1( (c1.Imag() - r*c1.Real())/d );
   }
   else
   {
      PCL_PRECONDITION( c2.Imag() != 0 )
      r = c2.Real() / c2.Imag();
      d = c2.Imag() + r*c2.Real();
      c.Real() = T1( (c1.Real()*r + c1.Imag())/d );
      c.Imag() = T1( (c1.Imag()*r - c1.Real())/d );
   }
   return c;
}

/*!
 * Complex division. Returns the division of a complex number \a c
 * by a real \a x.
 *
 * \note The noncommutativity of complex-real division is implemented by the
 * existence of this function along with
 * operator /( T1, const Complex<T2>& ).
 *
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T1> operator /( const Complex<T1>& c, T2 x )
{
   PCL_PRECONDITION( x != 0 )
   return Complex<T1>( T1( c.Real()/x ), T1( c.Imag()/x ) );
}

/*!
 * Complex division. Returns the division of a real number \a x and a
 * complex \a c.
 *
 * \note The noncommutativity of complex-real division is implemented by the
 * existence of this function along with
 * operator /( const Complex<T1>&, T2 ).
 *
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T2> operator /( T1 x, const Complex<T2>& c )
{
   // return Complex( x, 0 )/c;
   Complex<T2> c3;
   T2 r, d;
   if ( pcl::Abs( c.Real() ) >= pcl::Abs( c.Imag() ) )
   {
      PCL_PRECONDITION( c.Real() != 0 )
      r = c.Imag()/c.Real();
      d = c.Real() + r*c.Imag();
      c3.Real() = T2( x/d );
      c3.Imag() = T2( -((r*x)/d) );
   }
   else
   {
      PCL_PRECONDITION( c.Imag() != 0 )
      r = c.Real()/c.Imag();
      d = c.Imag() + r*c.Real();
      c3.Real() = T2( (r*x)/d );
      c3.Imag() = T2( -(x/d) );
   }
   return c3;
}

// ----------------------------------------------------------------------------

/*!
 * Complex square root.
 * \ingroup complex_basic_functions
 */
template <typename T> inline
Complex<T> Sqrt( const Complex<T>& c )
{
   if ( c.Real() == 0 && c.Imag() == 0 )
      return Complex<T>( 0 );

   Complex<T> c1;
   T m, r = pcl::Abs( c.Real() ), i = pcl::Abs( c.Imag() );

   if ( r >= i )
   {
      PCL_PRECONDITION( r != 0 )
      T t = i/r;
      m = pcl::Sqrt( r ) * pcl::Sqrt( (1 + pcl::Sqrt( 1 + t*t ))/2 );
   }
   else
   {
      PCL_PRECONDITION( i != 0 )
      T t = r/i;
      m = pcl::Sqrt( i ) * pcl::Sqrt( (t + pcl::Sqrt( 1 + t*t ))/2 );
   }

   if ( c.Real() >= 0 )
   {
      c1.Real() = m;
      c1.Imag() = c.Imag()/(m+m);
   }
   else
   {
      c1.Imag() = (c.Imag() >= 0) ? m : -m;
      c1.Real() = c.Imag()/(c1.Imag()+c1.Imag());
   }

   return c1;
}

/*!
 * Complex exponential function.
 * \ingroup complex_basic_functions
 */
template <typename T> inline
Complex<T> Exp( const Complex<T>& c )
{
   T x = pcl::Exp( c.Real() );
   return Complex<T>( x*pcl::Cos( c.Imag() ), x*pcl::Sin( c.Imag() ) );
}

/*!
 * Complex natural (base e) logarithm.
 * \ingroup complex_basic_functions
 */
template <typename T> inline
Complex<T> Ln( const Complex<T>& c )
{
   return Complex<T>( pcl::Ln( c.Mag() ), c.Arg() );
}

/*!
 * Complex base 10 logarithm.
 * \ingroup complex_basic_functions
 */
template <typename T> inline
Complex<T> Log( const Complex<T>& c )
{
   return pcl::Const<T>::log10e() * pcl::Ln( c );
}

// ----------------------------------------------------------------------------

/*!
 * Complex exponentiation. Returns a complex number \a c raised to a real \a x.
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T1> Pow( const Complex<T1>& c, T2 x )
{
   if ( c.Imag() == 0 )
      return Complex<T1>( pcl::Pow( c.Real(), T1( x ) ) );
   else
      return pcl::Exp( T1( x )*pcl::Ln( c ) );
}

/*!
 * Complex exponentiation. Returns a real number \a x raised to a complex \a c.
 * \ingroup complex_arithmetic_ops
 */
template <typename T1, class T2> inline
Complex<T2> Pow( T1 x, const Complex<T2>& c )
{
   if ( c.Imag() == 0 )
      return Complex<T2>( pcl::Pow( T2( x ), c.Real() ) );
   else
      return pcl::Exp( c*pcl::Ln( T2( x ) ) );
}

/*!
 * Complex exponentiation. Returns a complex number \a c1 raised to a
 * complex \a c2.
 * \ingroup complex_arithmetic_ops
 */
template <typename T> inline
Complex<T> Pow( const Complex<T>& c1, const Complex<T>& c2 )
{
   if ( c2.Imag() == 0 )
      return pcl::Pow( c1, c2.Real() );
   else if ( c1.Imag() == 0 )
      return Complex<T>( pcl::Pow( c1.Real(), c2 ) );
   else
      return pcl::Exp( c2*pcl::Ln( c1 ) );
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup complex_transcendental_ops Complex Transcendental Functions
 */

/*!
 * Complex sine.
 * \ingroup complex_transcendental_ops
 */
template <typename T> inline
Complex<T> Sin( const Complex<T>& c )
{
   return Complex<T>( pcl::Sin( c.Real() )*pcl::Cosh( c.Imag() ),
                      pcl::Cos( c.Real() )*pcl::Sinh( c.Imag() ) );
}

/*!
 * Complex cosine.
 * \ingroup complex_transcendental_ops
 */
template <typename T> inline
Complex<T> Cos( const Complex<T>& c )
{
   return Complex<T>( pcl::Cos( c.Real() )*pcl::Cosh( c.Imag() ),
                     -pcl::Sin( c.Real() )*pcl::Sinh( c.Imag() ) );
}

/*!
 * Complex tangent.
 * \ingroup complex_transcendental_ops
 */
template <typename T> inline
Complex<T> Tan( const Complex<T>& c )
{
   return pcl::Sin( c )/pcl::Cos( c );
}

/*!
 * Complex hyperbolic sine.
 * \ingroup complex_transcendental_ops
 */
template <typename T> inline
Complex<T> Sinh( const Complex<T>& c )
{
   return Complex<T>( pcl::Sinh( c.Real() )*pcl::Cos( c.Imag() ),
                      pcl::Cosh( c.Real() )*pcl::Sin( c.Imag() ) );
}

/*!
 * Complex hyperbolic cosine.
 * \ingroup complex_transcendental_ops
 */
template <typename T> inline
Complex<T> Cosh( const Complex<T>& c )
{
   return Complex<T>( pcl::Cosh( c.Real() )*pcl::Cos( c.Imag() ),
                      pcl::Sinh( c.Real() )*pcl::Sin( c.Imag() ) );
}

/*!
 * Complex hyperbolic tangent.
 * \ingroup complex_transcendental_ops
 */
template <typename T> inline
Complex<T> Tanh( const Complex<T>& c )
{
   return pcl::Sinh( c )/pcl::Cosh( c );
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup complex_relational_ops Complex Relational Operators
 *
 * The elements of the complex plane don't follow any particular "natural"
 * order. Of course, our definitions of the <, <=, > and >= operators for
 * complex numbers are completely arbitrary: we just compare complex
 * magnitudes. This effectively sorts complex numbers by concentric circles
 * around the origin.
 */

/*!
 * Returns true iff two complex numbers \a c1 and \a c2 are equal.
 * \ingroup complex_relational_ops
 */
template <typename T1, class T2> inline
bool operator ==( const Complex<T1>& c1, const Complex<T2>& c2 )
{
   return c1.Real() == c2.Real() && c1.Imag() == c2.Imag();
}

/*!
 * Returns true iff a complex number \a c is equal to a real \a x.
 * \ingroup complex_relational_ops
 */
template <typename T1, class T2> inline
bool operator ==( const Complex<T1>& c, T2 x )
{
   return c.Real() == x && c.Imag() == T1( 0 );
}

/*!
 * Returns true iff a real number \a x is equal to a complex \a c.
 * \ingroup complex_relational_ops
 */
template <typename T1, class T2> inline
bool operator ==( T1 x, const Complex<T2>& c )
{
   return c == x;
}

/*!
 * Returns true iff a complex number \a c1 is less than other complex \a c2.
 * \ingroup complex_relational_ops
 */
template <typename T1, class T2> inline
bool operator <( const Complex<T1>& c1, const Complex<T2>& c2 )
{
   return c1.Mag() < c2.Mag();
}

/*!
 * Returns true iff a complex number \a c is less than a real number \a x.
 * \ingroup complex_relational_ops
 */
template <typename T1, class T2> inline
bool operator <( const Complex<T1>& c, T2 x )
{
   return c.Mag() < pcl::Abs( x );
}

/*!
 * Returns true iff a real number \a x is less than a complex number \a c.
 * \ingroup complex_relational_ops
 */
template <typename T1, class T2> inline
bool operator <( T1 x, const Complex<T2>& c )
{
   return pcl::Abs( x ) < c.Mag();
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup complex_rounding_functions Complex Rounding Functions
 */

/*!
 * Complex rounding. Returns a complex number whose components are the
 * components of a specified complex \a c rounded to their nearest integers.
 * \ingroup complex_rounding_functions
 */
template <typename T> inline
Complex<T> Round( const Complex<T>& c )
{
   return Complex<T>( pcl::Round( c.Real() ), pcl::Round( c.Imag() ) );
}

/*!
 * Complex rounding. Returns a complex number whose components are the
 * components of a specified complex \a c rounded to the specified number of
 * decimal digits \a n.
 * \ingroup complex_rounding_functions
 */
template <typename T> inline
Complex<T> Round( const Complex<T>& c, int n )
{
   PCL_PRECONDITION( n >= 0 )
   return Complex<T>( pcl::Round( c.Real(), n ), pcl::Round( c.Imag(), n ) );
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup phase_matrices Phase Matrices Computation Algorithms
 */

/*!
 * Phase correlation matrix.
 *
 * \param[out] i     Address of the first element in the output phase
 *                   correlation matrix.
 *
 * \param      j     Address of the ending element in the output phase
 *                   correlation matrix.
 *
 * \param      a     Address of the first element in the first operand matrix.
 *
 * \param      b     Address of the first element in the second operand matrix.
 *
 * This routine will compute and store the set of contiguous matrix elements at
 * i, i+1, ..., j-1. The total number of computed elements in the output phase
 * correlation matrix is equal to j-i. The \a i, \a a and \a b matrices must
 * provide storage for at least j-i complex numbers.
 *
 * \ingroup phase_matrices
 */
template <typename T> inline
void PhaseCorrelationMatrix( Complex<T>* i, const Complex<T>* j, const Complex<T>* a, const Complex<T>* b )
{
   const T tiny = T( 1.0e-20 );
   for ( ; i < j; ++i, ++a, ++b )
   {
      Complex<T> n = *a * ~*b;
      *i = n/Max( tiny, Abs( n ) );
   }
}

/*!
 * Cross power spectrum matrix.
 *
 * \param[out] i     Address of the first element in the output cross power
 *                   spectrum matrix.
 *
 * \param      j     Address of the ending element in the output cross power
 *                   spectrum matrix.
 *
 * \param      a     Address of the first element in the first operand matrix.
 *
 * \param      b     Address of the first element in the second operand matrix.
 *
 * This routine will compute and store the set of contiguous matrix elements at
 * i, i+1, ..., j-1. The total number of computed elements in the output cross
 * power spectrum matrix is equal to j-i. The \a i, \a a and \a b matrices must
 * provide storage for at least j-i complex numbers.
 *
 * \ingroup phase_matrices
 */
template <typename T> inline
void CrossPowerSpectrumMatrix( Complex<T>* i, const Complex<T>* j, const Complex<T>* a, const Complex<T>* b )
{
   const T tiny = T( 1.0e-20 );
   for ( ; i < j; ++i, ++a, ++b )
      *i = (*b * ~*a)/Max( tiny, Abs( *a ) * Abs( *b ) );
}

//
// Stream extraction/insertion.
//
/* ### PCL 2.x: Add these along with pcl::DataStream, etc...
template <class S, typename T> inline
S& operator >>( S& s, Complex<T>& c )
{
	return s >> c.Real() >> c.Imag();
}

template <class S, typename T> inline
S& operator <<( S& s, const Complex<T>& c )
{
	return s << c.Real() << c.Imag();
}
 */

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_COMPLEX_INSTANTIATE

/*!
 * \defgroup complex_types Complex Number Types
 */

/*!
 * \class pcl::Complex32
 * \ingroup complex_types
 * \brief A complex number whose components are 32-bit floating point real
 *        numbers.
 *
 * Complex32 is a template instantiation of Complex for the \c float type.
 */
typedef Complex<float>        Complex32;

/*!
 * \class pcl::fcomplex
 * \ingroup complex_types
 * \brief A complex number whose components are 32-bit floating point real
 *        numbers.
 *
 * fcomplex is an alias for Complex32. It is a template instantiation of
 * Complex for the \c float type.
 */
typedef Complex32             fcomplex;

/*!
 * \class pcl::Complex64
 * \ingroup complex_types
 * \brief A complex number whose components are 64-bit floating point real
 *        numbers.
 *
 * Complex64 is a template instantiation of Complex for the \c double type.
 */
typedef Complex<double>       Complex64;

/*!
 * \class pcl::dcomplex
 * \ingroup complex_types
 * \brief A complex number whose components are 64-bit floating point real
 *        numbers.
 *
 * dcomplex is an alias for Complex64. It is a template instantiation of
 * Complex for the \c double type.
 */
typedef Complex64             dcomplex;

/*!
 * \class pcl::complex
 * \ingroup complex_types
 * \brief A complex number whose components are 64-bit floating point real
 *        numbers.
 *
 * complex is an alias for dcomplex. It is a template instantiation of Complex
 * for the \c double type.
 */
typedef dcomplex              complex;

#endif   // __PCL_NO_COMPLEX_INSTANTIATE

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Complex_h

// ----------------------------------------------------------------------------
// EOF pcl/Complex.h - Released 2016/02/21 20:22:12 UTC
