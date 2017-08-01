//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/ChebyshevFit.h - Released 2017-08-01T14:23:31Z
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

#ifndef __PCL_ChebyshevFit_h
#define __PCL_ChebyshevFit_h

/// \file pcl/ChebyshevFit.h

#include <pcl/Defs.h>

#include <pcl/Constants.h>
#include <pcl/ErrorHandler.h>
#include <pcl/Exception.h>
#include <pcl/MultiVector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

template <typename Tx, typename Ty> class GenericScalarChebyshevFit;

/*!
 * \class GenericChebyshevFit
 * \brief Function approximation by Chebyshev polynomial expansion
 *
 * %GenericChebyshevFit approximates a smooth, vector-valued function f(x) in a
 * given interval [a,b] by expansion with a truncated series of Chebyshev
 * polynomials. As is well known, the Chebyshev expansion:
 *
 * T(x) = Sum_i( ci*Ti(x) ),
 *
 * where i belongs to [0,&infin;), Ti(x) is the Chebyshev polynomial of the ith
 * degree, and the ci's are polynomial coefficients, is very close to the
 * optimal approximating polynomial that minimizes the error |T(x) - f(x)|,
 * where x varies over the fitting interval [a,b].
 *
 * For functions converging strongly after a given series length n, one can
 * truncate the Chebyshev series to a smaller length m < n to obtain an
 * approximating polynomial with a maximum error close to |Tm+1(x)|.
 *
 * In addition to Chebyshev expansion, truncation and approximation, this class
 * also implements generation of Chebyshev polynomials to approximate the
 * first derivative and indefinite integral of the fitted function.
 *
 * The template argument Tx represents the type of the independent variable, or
 * the type of the argument x of the fitted function y = f(x). The template
 * argument Ty represents the type of a component of the value y of the fitted
 * function.
 */
template <typename Tx, typename Ty>
class GenericChebyshevFit
{
public:

   /*!
    * Represents an ordered list of Chebyshev polynomial coefficients.
    */
   typedef GenericVector<Ty>        coefficients;

   /*!
    * Represents a set of ordered lists of Chebyshev polynomial coefficients.
    */
   typedef GenericMultiVector<Ty>   coefficient_series;

   /*!
    * Represents a function value.
    */
   typedef GenericVector<Ty>        function_value;

   /*!
    * Represents a set of function values.
    */
   typedef GenericMultiVector<Ty>   function_values;

   /*!
    * Constructs a truncated Chebyshev polynomial expansion with \a n
    * coefficients to approximate the specified N-dimensional, vector-valued
    * function \a f in the interval [\a x1,\a x2] of the independent variable.
    *
    * The function \a f will be called \a n times and should have the following
    * prototype (or equivalent by means of suitable type conversions and/or
    * default arguments):
    *
    * GenericVector&lt;Ty&gt; f( Tx )
    *
    * where the length of a returned vector must be equal to \a N.
    *
    * The expansion process will compute n^2 + n cosines, which may dominate the
    * complexity of the process if the function \a f is comparatively fast.
    *
    * The interval [\a x1,\a x2] must not be empty or insignificant with
    * respect to the machine epsilon. If that happens, this constructor will
    * throw an appropriate Error exception. The interval bounds can be
    * specified in any order, that is, \a x2 can legally be < \a x1; in such
    * case the bounds will be implicitly swapped by this constructor.
    *
    * The interval [\a x1,\a x2] will be also the valid range of evaluation for
    * this object, which can be retrieved with LowerBound() and UpperBound().
    * See also the Evaluate() member function.
    *
    * The length \a n of the polynomial coefficient series should be &ge; 2. If
    * \a n &le; 1, the specified value will be ignored and \a n = 2 will be
    * forced. Typically, a relatively large series length should be used, say
    * between 30 and 100 coefficients, depending on the rate and amplitude of
    * function variations within the fitting interval. The polynomial expansion
    * can be further truncated to approximate the function to the desired error
    * bound. See the Truncate() and Evaluate() member functions for details.
    */
   template <class F>
   GenericChebyshevFit( F f, Tx x1, Tx x2, int N, int n ) :
      dx( Abs( x2 - x1 ) ),
      x0( (x1 + x2)/2 ),
      m( Max( 2, n ), Max( 1, N ) )
   {
      PCL_PRECONDITION( N > 0 )
      PCL_PRECONDITION( n > 1 )
      if ( 1 + dx == 1 )
         throw Error( "GenericChebyshevFit: Empty or insignificant function evaluation interval." );
      N = m.Length();
      n = m[0];
      Tx dx2 = dx/2;
      function_values y( n, N );
      for ( int j = 0; j < n; ++j )
         y[j] = f( x0 + Cos( Const<Ty>::pi()*(j + 0.5)/n )*dx2 );
      c = coefficient_series( N, n );
      Ty k = 2.0/n;
      for ( int i = 0; i < N; ++i )
         for ( int j = 0; j < n; ++j )
         {
            Ty s = Ty( 0 );
            for ( int k = 0; k < n; ++k )
               s += y[k][i] * Cos( Const<Ty>::pi()*j*(k + 0.5)/n );
            c[i][j] = k*s;
         }
   }

   /*!
    * Constructs a truncated Chebyshev polynomial expansion from the specified
    * coefficient series \a ck to approximate a vector-valued function in the
    * interval [\a x1,\a x2] of the independent variable. The dimension of the
    * approximated function and the coefficient series lengths are acquired
    * from the specified container \a ck.
    *
    * This constructor performs basic coherence and structural integrity checks
    * on the specified parameters, and throws the appropriate Error exception
    * if it detects any problem. However, the validity of polynomial expansion
    * coefficients cannot be verified; ensuring it is the responsibility of the
    * caller.
    */
   GenericChebyshevFit( const coefficient_series& ck, Tx x1, Tx x2 ) :
      dx( Abs( x2 - x1 ) ),
      x0( (x1 + x2)/2 ),
      c( ck ),
      m( ck.Length() )
   {
      if ( 1 + dx == 1 )
         throw Error( "GenericChebyshevFit: Empty or insignificant function evaluation interval." );
      if ( c.IsEmpty() )
         throw Error( "GenericChebyshevFit: Empty polynomial expansion." );
      for ( int i = 0; i < m.Length(); ++i )
         if ( (m[i] = c[i].Length()) < 1 )
            throw Error( "GenericChebyshevFit: Invalid coefficient series for dimension " + String( i ) + '.' );
   }

   /*!
    * Copy constructor.
    */
   GenericChebyshevFit( const GenericChebyshevFit& ) = default;

   /*!
    * Move constructor.
    */
#ifndef _MSC_VER
   GenericChebyshevFit( GenericChebyshevFit&& ) = default;
#else
   GenericChebyshevFit( GenericChebyshevFit&& x ) :
      dx( x.dx ),
      x0( x.x0 ),
      c( std::move( x.c ) ),
      m( std::move( x.m ) )
   {
   }
#endif

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   GenericChebyshevFit& operator =( const GenericChebyshevFit& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
#ifndef _MSC_VER
   GenericChebyshevFit& operator =( GenericChebyshevFit&& ) = default;
#else
   GenericChebyshevFit& operator =( GenericChebyshevFit&& x )
   {
      dx = x.dx;
      x0 = x.x0;
      c = std::move( x.c );
      m = std::move( x.m );
      return *this;
   }
#endif

   /*!
    * Returns the lower bound of this Chebyshev fit. This is the smallest value
    * of the independent variable for which the function has been fitted, and
    * hence the smallest value for which this object can be legally evaluated
    * for function approximation.
    *
    * \sa UpperBound()
    */
   Tx LowerBound() const
   {
      return x0 - dx/2;
   }

   /*!
    * Returns the upper bound of this Chebyshev fit. This is the largest value
    * of the independent variable for which the function has been fitted, and
    * hence the largest value for which this object can be legally evaluated
    * for function approximation.
    *
    * \sa LowerBound()
    */
   Tx UpperBound() const
   {
      return x0 + dx/2;
   }

   /*!
    * Returns the number of components in the (vector-valued) dependent
    * variable. This is the number of vector components in a fitted or
    * approximated function value.
    */
   int NumberOfComponents() const
   {
      return m.Length();
   }

   /*!
    * Returns the number of coefficients in the generated Chebyshev polynomial
    * expansion for the specified zero-based vector component index \a i. This
    * is the number of coefficients that was specified or acquired in a class
    * constructor.
    *
    * \sa TruncatedLength()
    */
   int Length( int i = 0 ) const
   {
      PCL_PRECONDITION( i >= 0 && i < NumberOfComponents() )
      return c[i].Length();
   }

   /*!
    * Returns the number of coefficients in the truncated Chebyshev polynomial
    * expansion for the specified zero-based vector component index \a i.
    *
    * If \a i < 0, returns the largest number of polynomial coefficients among
    * all vector components.
    *
    * \sa Truncate(), Length()
    */
   int TruncatedLength( int i = -1 ) const
   {
      PCL_PRECONDITION( i < 0 || i < NumberOfComponents() )
      if ( i < 0 )
         return m.MaxComponent();
      return m[i];
   }

   /*!
    * Returns true iff the Chebyshev polynomial expansion has been truncated
    * for the specified zero-based vector component index \a i.
    *
    * If \a i < 0, returns true iff the expansions have been truncated for all
    * vector components.
    *
    * \sa Truncate(), TruncatedLength()
    */
   bool IsTruncated( int i = -1 ) const
   {
      PCL_PRECONDITION( i < 0 || i < NumberOfComponents() )
      if ( i < 0 )
         return m.MaxComponent() < Length();
      return m[i] < c[i].Length();
   }

   /*!
    * Returns an estimate of the maximum error in the truncated Chebyshev
    * polynomial expansion for the specified zero-based vector component index
    * \a i.
    *
    * If \a i < 0, returns the largest expansion error estimate among all
    * vector components.
    *
    * \sa Truncate()
    */
   Ty TruncationError( int i = -1 ) const
   {
      PCL_PRECONDITION( i < 0 || i < NumberOfComponents() )
      if ( i < 0 )
      {
         int N = NumberOfComponents();
         function_value e( N );
         for ( int j = 0; j < N; ++j )
            e[j] = TruncationError( j );
         return e.MaxComponent();
      }
      if ( m[i] < c[i].Length() )
      {
         Ty e = Ty( 0 );
         for ( int j = c[i].Length(); --j >= m[i]; )
            e += Abs( c[i][j] );
         return e;
      }
      return Abs( c[i][c[i].Length()-1] );
   }

   /*!
    * Attempts to truncate the Chebyshev polynomial expansion for the specified
    * maximum error \a e. Returns \c true iff the expansion could be truncated
    * successfully for all vector components of the fitted function.
    *
    * If n is the length of a fitted polynomial series, this function finds a
    * truncated length 1 &lt; m &le; n such that:
    *
    * Sum_i( |ci| ) < e
    *
    * where ci is a polynomial coefficient and the zero-based subindex i is in
    * the interval [m,n-1].
    *
    * The truncated Chebyshev expansion will approximate the fitted function
    * component with a maximum error close to &plusmn;|e| within the fitting
    * interval.
    *
    * This member function does not remove any polynomial coefficients, so the
    * original polynomial expansion remains intact. This means that the fitted
    * polynomials can be truncated successively to achieve different error
    * bounds, as required.
    *
    * If the polynomial series cannot be truncated to achieve the required
    * tolerance in all function components (that is, if either all coefficients
    * for a given component are larger than \a e in absolute value, or n = 2),
    * this function forces m = n for the components where the requested
    * truncation is not feasible, yielding the original, untruncated Chebyshev
    * polynomials for those components. In such case this function returns
    * \c false.
    */
   bool Truncate( Ty e )
   {
      e = Abs( e );
      int N = NumberOfComponents();
      int tc = 0;
      for ( int i = 0; i < N; ++i )
      {
         Ty s = Ty( 0 );
         for ( m[i] = c[i].Length(); m[i] > 2; --m[i] )
            if ( (s += Abs( c[i][m[i]-1] )) >= e )
               break;
         if ( m[i] < c[i].Length() )
            ++tc;
      }
      return tc == N;
   }

   /*!
    * Returns a reference to the immutable set of Chebyshev polynomial
    * expansion coefficients in this object.
    */
   const coefficient_series& Coefficients() const
   {
      return c;
   }

   /*!
    * Evaluates the truncated Chebyshev polynomial expansion for the specified
    * value \a x of the independent variable, and returns the approximated
    * function value.
    *
    * The specified evaluation point \a x must lie within the fitting interval,
    * given by LowerBound() and UpperBound(), which was specified as the \a x1
    * and \a x2 arguments when the function was initially fitted by the class
    * constructor. For performance reasons, this precondition is not verified
    * by this member function. If an out-of-range evaluation point is
    * specified, this function will return an unpredictable result.
    *
    * If the polynomial series has been truncated by calling Truncate(), this
    * function evaluates the current truncated Chebyshev expansions instead of
    * the original ones.
    *
    * \sa operator ()()
    */
   function_value Evaluate( Tx x ) const
   {
      PCL_PRECONDITION( x >= LowerBound() )
      PCL_PRECONDITION( x <= UpperBound() )
      const Ty y0 = Ty( 2*(x - x0)/dx );
      const Ty y2 = 2*y0;
      function_value y( NumberOfComponents() );
      for ( int i = 0; i < y.Length(); ++i )
      {
         Ty d0 = Ty( 0 );
         Ty d1 = Ty( 0 );
         const Ty* k = c[i].At( m[i] );
         for ( int j = m[i]; --j > 0; )
         {
            Ty d = d1;
            d1 = y2*d1 - d0 + *--k;
            d0 = d;
         }
         y[i] = y0*d1 - d0 + *--k/2;
      }
      return y;
   }

   /*!
    * A synonym for Evaluate().
    */
   function_value operator ()( Tx x ) const
   {
      return Evaluate( x );
   }

   /*!
    * Returns a %GenericChebyshevFit object that approximates the derivative of
    * the function fitted by this object.
    *
    * The returned object can be used to evaluate the derivative within the
    * fitting interval of this object, defined by LowerBound() and
    * UpperBound().
    *
    * The returned object will always own Chebyshev polynomials with the length
    * of the originally fitted series, \e not of the current truncated lengths,
    * if the polynomial expansions have been truncated.
    *
    * \sa Integral()
    */
   GenericChebyshevFit Derivative() const
   {
      int N = NumberOfComponents();
      GenericChebyshevFit ch1;
      ch1.dx = dx;
      ch1.x0 = x0;
      ch1.c = coefficient_series( N );
      ch1.m = IVector( N );
      for ( int i = 0; i < N; ++i )
      {
         int n = Max( 1, c[i].Length()-1 );
         ch1.c[i] = coefficients( n );
         ch1.m[i] = n;
         if ( n > 1 )
         {
            ch1.c[i][n-1] = 2*n*c[i][n];
            ch1.c[i][n-2] = 2*(n-1)*c[i][n-1];
            for ( int j = n-3; j >= 0; --j )
               ch1.c[i][j] = ch1.c[i][j+2] + 2*(j+1)*c[i][j+1];
            ch1.c[i] *= Ty( 2 )/dx;
         }
         else
            ch1.c[i] = Ty( 0 );
      }
      return ch1;
   }

   /*!
    * Returns a %GenericChebyshevFit object that approximates the indefinite
    * integral of the function fitted by this object.
    *
    * The returned object can be used to evaluate the integral within the
    * fitting interval of this object, as defined by LowerBound() and
    * UpperBound(). The constant of integration is set to a value such that the
    * integral is zero at the lower fitting bound.
    *
    * The returned object will always own Chebyshev polynomials with the length
    * of the originally fitted series, \e not of the current truncated lengths,
    * if the polynomial expansions have been truncated.
    *
    * \sa Derivative()
    */
   GenericChebyshevFit Integral() const
   {
      int N = NumberOfComponents();
      GenericChebyshevFit ch;
      ch.dx = dx;
      ch.x0 = x0;
      ch.c = coefficient_series( N );
      ch.m = IVector( N );
      for ( int i = 0; i < N; ++i )
      {
         int n = c[i].Length();
         ch.c[i] = coefficients( n );
         ch.m[i] = n;
         if ( n > 1 )
         {
            const Ty k = Ty( dx )/4;
            Ty s = Ty( 0 );
            int f = 1;
            for ( int j = 1; j < n-1; ++j, f = -f )
               s += f*(ch.c[i][j] = k*(c[i][j-1] - c[i][j+1])/j);
            ch.c[i][0] = 2*(s + f*(ch.c[i][n-1] = k*c[i][n-2]/(n-1)));
         }
         else
            ch.c[i] = Ty( 0 );
      }
      return ch;
   }

private:

   Tx                 dx; // x2 - x1
   Tx                 x0; // (x1 + x2)/2
   coefficient_series c;  // Chebyshev polynomial coefficients
   IVector            m;  // length of the truncated coefficient series

   /*!
    * \internal
    * Default constructor, used internally by several member functions.
    */
   GenericChebyshevFit()
   {
   }

   friend class GenericScalarChebyshevFit<Tx,Ty>;
};

// ----------------------------------------------------------------------------

template <typename Tx, typename Ty>
class GenericScalarChebyshevFit : public GenericChebyshevFit<Tx,Ty>
{
public:

   /*!
    * Constructs a truncated Chebyshev polynomial expansion with \a n
    * coefficients to approximate the specified N-dimensional, scalar-valued
    * function \a f in the interval [\a x1,\a x2] of the independent variable.
    *
    * See GenericChebyshevFit::GenericChebyshevFit() for detailed information.
    */
   template <class F>
   GenericScalarChebyshevFit( F f, Tx x1, Tx x2, int n ) :
      GenericChebyshevFit<Tx,Ty>( [f]( Tx x ){ return GenericVector<Ty>( f( x ), 1 ); }, x1, x2, 1, n )
   {
   }

   /*!
    * Copy constructor.
    */
   GenericScalarChebyshevFit( const GenericScalarChebyshevFit& ) = default;

   /*!
    * Move constructor.
    */
   GenericScalarChebyshevFit( GenericScalarChebyshevFit&& ) = default;

   /*!
    * Copy assignment operator.
    */
   GenericScalarChebyshevFit& operator =( const GenericScalarChebyshevFit& ) = default;

   /*!
    * Move assignment operator.
    */
   GenericScalarChebyshevFit& operator =( GenericScalarChebyshevFit&& ) = default;

   /*!
    * Returns the number of coefficients in the truncated Chebyshev polynomial
    * expansion.
    *
    * \sa Truncate(), Length()
    */
   int TruncatedLength() const
   {
      return GenericChebyshevFit<Tx,Ty>::TruncatedLength( 0 );
   }

   /*!
    * Returns true iff the Chebyshev polynomial expansion has been truncated.
    *
    * \sa Truncate(), TruncatedLength()
    */
   bool IsTruncated() const
   {
      return GenericChebyshevFit<Tx,Ty>::IsTruncated( 0 );
   }

   /*!
    * Returns an estimate of the maximum error in the truncated Chebyshev
    * polynomial expansion.
    *
    * \sa Truncate()
    */
   Ty TruncationError() const
   {
      return GenericChebyshevFit<Tx,Ty>::TruncationError( 0 );
   }

   /*!
    * Evaluates the truncated Chebyshev polynomial expansion for the specified
    * value \a x of the independent variable, and returns the approximated
    * function value.
    *
    * \sa operator ()()
    */
   Ty Evaluate( Tx x ) const
   {
      return GenericChebyshevFit<Tx,Ty>::Evaluate( x )[0];
   }

   /*!
    * A synonym for Evaluate().
    */
   Ty operator ()( Tx x ) const
   {
      return Evaluate( x );
   }

   /*!
    * Returns a %GenericChebyshevFit object that approximates the derivative of
    * the function fitted by this object.
    *
    * See GenericChebyshevFit::Derivative() for detailed information.
    *
    * \sa Integral()
    */
   GenericScalarChebyshevFit Derivative() const
   {
      return GenericChebyshevFit<Tx,Ty>::Derivative();
   }

   /*!
    * Returns a %GenericChebyshevFit object that approximates the indefinite
    * integral of the function fitted by this object.
    *
    * See GenericChebyshevFit::Integral() for detailed information.
    *
    * \sa Derivative()
    */
   GenericScalarChebyshevFit Integral() const
   {
      return GenericChebyshevFit<Tx,Ty>::Integral();
   }

private:

   /*!
    * \internal
    * Private constructor from the base class.
    */
   GenericScalarChebyshevFit( const GenericChebyshevFit<Tx,Ty>& T ) :
      GenericChebyshevFit<Tx,Ty>()
   {
      this->dx = T.dx;
      this->x0 = T.x0;
      this->c = typename GenericChebyshevFit<Tx,Ty>::coefficient_series( 1 );
      this->c[0] = T.c[0];
      this->m = IVector( 1 );
      this->m[0] = T.m[0];
   }
};

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_CHEBYSHEV_FIT_INSTANTIATE

/*!
 * \defgroup chebyshev_fit_types Chebyshev Fit Types
 */

/*!
 * \class pcl::F32ChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 32-bit floating point Chebyshev function approximation.
 *
 * %F32ChebyshevFit is a template instantiation of GenericChebyshevFit for the
 * \c float type.
 */
typedef GenericChebyshevFit<float, float>             F32ChebyshevFit;

/*!
 * \class pcl::FChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 32-bit floating point Chebyshev function approximation.
 *
 * %FChebyshevFit is an alias for F32ChebyshevFit. It is a template
 * instantiation of GenericChebyshevFit for the \c float type.
 */
typedef F32ChebyshevFit                               FChebyshevFit;

/*!
 * \class pcl::F64ChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 64-bit floating point Chebyshev function approximation.
 *
 * %F64ChebyshevFit is a template instantiation of GenericChebyshevFit for the
 * \c double type.
 */
typedef GenericChebyshevFit<double, double>           F64ChebyshevFit;

/*!
 * \class pcl::DChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 64-bit floating point Chebyshev function approximation.
 *
 * %DChebyshevFit is an alias for F64ChebyshevFit. It is a template
 * instantiation of GenericChebyshevFit for the \c double type.
 */
typedef F64ChebyshevFit                               DChebyshevFit;

/*!
 * \class pcl::ChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 64-bit floating point Chebyshev function approximation.
 *
 * %ChebyshevFit is an alias for DChebyshevFit. It is a template instantiation
 * of GenericChebyshevFit for the \c double type.
 */
typedef DChebyshevFit                                 ChebyshevFit;

#ifndef _MSC_VER

/*!
 * \class pcl::F80ChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 80-bit extended precision floating point Chebyshev function
 * approximation.
 *
 * %F80ChebyshevFit is a template instantiation of GenericChebyshevFit for the
 * \c long \c double type.
 *
 * \note This template instantiation is not available on Windows with Visual
 * C++ compilers.
 */
typedef GenericChebyshevFit<long double, long double> F80ChebyshevFit;

/*!
 * \class pcl::LDChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 80-bit extended precision floating point Chebyshev function
 * approximation.
 *
 * %LDChebyshevFit is an alias for F80ChebyshevFit. It is a template
 * instantiation of GenericChebyshevFit for the \c long \c double type.
 *
 * \note This template instantiation is not available on Windows with Visual
 * C++ compilers.
 */
typedef F80ChebyshevFit                               LDChebyshevFit;

#endif   // !_MSC_VER

/*!
 * \class pcl::F32ScalarChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 32-bit floating point scalar Chebyshev function approximation.
 *
 * %F32ScalarChebyshevFit is a template instantiation of
 * GenericScalarChebyshevFit for the \c float type.
 */
typedef GenericScalarChebyshevFit<float, float>       F32ScalarChebyshevFit;

/*!
 * \class pcl::FScalarChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 32-bit floating point scalar Chebyshev function approximation.
 *
 * %FScalarChebyshevFit is an alias for F32ScalarChebyshevFit. It is a template
 * instantiation of GenericScalarChebyshevFit for the \c float type.
 */
typedef F32ScalarChebyshevFit                         FScalarChebyshevFit;

/*!
 * \class pcl::F64ScalarChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 64-bit floating point scalar Chebyshev function approximation.
 *
 * %F64ScalarChebyshevFit is a template instantiation of
 * GenericScalarChebyshevFit for the \c double type.
 */
typedef GenericScalarChebyshevFit<double, double>     F64ScalarChebyshevFit;

/*!
 * \class pcl::DScalarChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 64-bit floating point scalar Chebyshev function approximation.
 *
 * %DScalarChebyshevFit is an alias for F64ScalarChebyshevFit. It is a template
 * instantiation of GenericScalarChebyshevFit for the \c double type.
 */
typedef F64ScalarChebyshevFit                         DScalarChebyshevFit;

/*!
 * \class pcl::ScalarChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 64-bit floating point scalar Chebyshev function approximation.
 *
 * %ScalarChebyshevFit is an alias for DScalarChebyshevFit. It is a template
 * instantiation of GenericScalarChebyshevFit for the \c double type.
 */
typedef DScalarChebyshevFit                           ScalarChebyshevFit;

#ifndef _MSC_VER

/*!
 * \class pcl::F80ScalarChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 80-bit extended precision floating point scalar Chebyshev function
 * approximation.
 *
 * %F80ScalarChebyshevFit is a template instantiation of
 * GenericScalarChebyshevFit for the \c long \c double type.
 *
 * \note This template instantiation is not available on Windows with Visual
 * C++ compilers.
 */
typedef GenericScalarChebyshevFit<long double, long double> F80ScalarChebyshevFit;

/*!
 * \class pcl::LDScalarChebyshevFit
 * \ingroup chebyshev_fit_types
 * \brief 80-bit extended precision floating point scalar Chebyshev function
 * approximation.
 *
 * %LDScalarChebyshevFit is an alias for F80ScalarChebyshevFit. It is a
 * template instantiation of GenericScalarChebyshevFit for the \c long
 * \c double type.
 *
 * \note This template instantiation is not available on Windows with Visual
 * C++ compilers.
 */
typedef F80ScalarChebyshevFit                         LDScalarChebyshevFit;

#endif   // !_MSC_VER

#endif   // !__PCL_NO_CHEBYSHEV_FIT_INSTANTIATE

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_ChebyshevFit_h

// ----------------------------------------------------------------------------
// EOF pcl/ChebyshevFit.h - Released 2017-08-01T14:23:31Z
