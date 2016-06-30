

#ifndef __PCL_ChebyshevFit_h
#define __PCL_ChebyshevFit_h

/// \file pcl/ChebyshevFit.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Constants_h
#include <pcl/Constants.h>
#endif

#ifndef __PCL_MultiVector_h
#include <pcl/MultiVector.h>
#endif

#ifndef __PCL_Exception_h
#include <pcl/Exception.h>
#endif

#ifndef __PCL_ErrorHandler_h
#include <pcl/ErrorHandler.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class GenericChebyshevFit
 * \brief Function approximation by Chebyshev polynomials
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
    * function \a f in the range [\a x1,\a x2] of the independent variable.
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
    * The range [\a x1,\a x2] must not be empty or insignificant with respect
    * to the machine epsilon. If that happens, this constructor will throw an
    * appropriate Error exception. The range bounds can be specified in any
    * order, that is, \a x2 can legally be < \a x1; in such case the bounds
    * will be implicitly swapped by this constructor.
    *
    * The range [\a x1,\a x2] will be also the valid range of evaluation for
    * this object, which can be retrieved with LowerBound() and UpperBound().
    * See also the Evaluate() member function.
    *
    * The length \a n of the polynomial coefficient series should be &ge; 2. If
    * \a n &le; 1, the specified value will be ignored and \a n = 2 will be
    * forced. Typically, a relatively large series length should be used, say
    * between 30 and 100 coefficients, depending on the rate and amplitude of
    * function variations within the fitting range. The polynomial expansion
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
         throw Error( "GenericChebyshevFit: Empty or insignificant function evaluation range." );
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
    * Copy constructor.
    */
   GenericChebyshevFit( const GenericChebyshevFit& ) = default;

   /*!
    * Move constructor.
    */
   GenericChebyshevFit( GenericChebyshevFit&& ) = default;

   /*!
    * Copy assignment operator.
    */
   GenericChebyshevFit& operator =( const GenericChebyshevFit& ) = default;

   /*!
    * Move assignment operator.
    */
   GenericChebyshevFit& operator =( GenericChebyshevFit&& ) = default;

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
    * expansion. This is the number of coefficients that was specified in the
    * class constructor.
    *
    * \sa TruncatedLength()
    */
   int Length() const
   {
      return c[0].Length();
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
    * the range [m,n-1].
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
    * Evaluates the truncated Chebyshev polynomial expansion for the specified
    * value \a x of the independent variable, and returns the approximated
    * function value.
    *
    * The specified evaluation point \a x must lie within the fitting range,
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
   Ty operator ()( Tx x ) const
   {
      return Evaluate( x );
   }

   /*!
    * Returns a %GenericChebyshevFit object that approximates the derivative of
    * the function fitted by this object.
    *
    * The returned object can be used to evaluate the derivative within the
    * fitting range of this object, defined by LowerBound() and UpperBound().
    *
    * The returned object will always own Chebyshev polynomials with the length
    * of the originally fitted series, \e not of the current truncated lengths,
    * if the polynomial expansions have been truncated.
    *
    * \sa Integral()
    */
   GenericChebyshevFit Derivative() const
   {
      int n = Max( 1, c[0].Length() - 1 );
      int N = NumberOfComponents();
      GenericChebyshevFit ch1;
      ch1.dx = dx;
      ch1.x0 = x0;
      ch1.c = coefficient_series( N, n );
      ch1.m = IVector( n, N );
      if ( n > 1 )
         for ( int i = 0; i < N; ++i )
         {
            ch1.c[i][n-1] = 2*n*c[i][n];
            ch1.c[i][n-2] = 2*(n-1)*c[i][n-1];
            for ( int j = n-3; j >= 0; --j )
               ch1.c[i][j] = ch1.c[i][j+2] + 2*(j+1)*c[i][j+1];
            ch1.c[i] *= Ty( 2 )/dx;
         }
      else
         ch1.c = Ty( 0 );
      return ch1;
   }

   /*!
    * Returns a %GenericChebyshevFit object that approximates the indefinite
    * integral of the function fitted by this object.
    *
    * The returned object can be used to evaluate the integral within the
    * fitting range of this object, as defined by LowerBound() and
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
      int n = c[0].Length();
      int N = NumberOfComponents();
      GenericChebyshevFit ch;
      ch.dx = dx;
      ch.x0 = x0;
      ch.c = coefficient_series( N, n );
      ch.m = IVector( n, N );
      if ( n > 1 )
      {
         const Ty k = Ty( dx )/4;
         for ( int i = 0; i < N; ++i )
         {
            Ty s = Ty( 0 );
            int f = 1;
            for ( int j = 1; j < n-1; ++j, f = -f )
               s += f*(ch.c[i][j] = k*(c[i][j-1] - c[i][j+1])/j);
            ch.c[i][0] = 2*(s + f*(ch.c[i][n-1] = k*c[i][n-2]/(n-1)));
         }
      }
      else
         ch.c = Ty( 0 );
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
};

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_CHEBYSHEV_FIT_INSTANTIATE

/*!
 * \defgroup chebyshev_fit_types Predefined Chebyshev Fit Types
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

#endif   // !__PCL_NO_CHEBYSHEV_FIT_INSTANTIATE

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_ChebyshevFit_h
