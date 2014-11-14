// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/LinearFit.h - Released 2014/11/14 17:16:40 UTC
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

#ifndef __PCL_LinearFit_h
#define __PCL_LinearFit_h

/// \file pcl/LinearFit.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Math_h
#include <pcl/Math.h>
#endif

#ifndef __PCL_Exception_h
#include <pcl/Exception.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS StatusMonitor;

/*!
 * \class LinearFit
 * \brief Robust straight line fitting by minimization of mean absolute
 * deviation
 *
 * Given a set of N data points {xi,yi} for i = {0,...,N-1}, %LinearFit finds
 * the parameters a,b of the linear function:
 *
 * <pre>y = L(x) := a + b*x</pre>
 *
 * that minimizes mean absolute deviation for all the data points. The \e a
 * parameter of L() is the ordinate of its intersection with the Y axis. The
 * \e b parameter is the slope of the fitted straight line.
 */
class PCL_CLASS LinearFit
{
public:

   /*!
    * The \e a parameter of the fitted straight line:
    *
    * <pre>y = a + b*x</pre>
    *
    * is the ordinate of its intersection with the Y axis. It represents a
    * constant additive pedestal present in the whole dataset.
    */
   double a;

   /*!
    * The \e b parameter is the slope of the fitted straight line:
    *
    * <pre>y = a + b*x</pre>
    *
    * If the fitted line passes through two points {x1,y1} and {x2,y2}, then
    * its slope is equal to (y2 - y1)/(x2 - x1).
    */
   double b;

   /*!
    * Mean (or average) absolute deviation of the linear fit. This is the
    * mean absolute deviation computed for all data points with respect to the
    * fitted straight line function.
    */
   double adev;

   /*!
    * Returns true if the fitted straight line model is valid. The model
    * function is valid if the slope parameter is finite, i.e., if the fitted
    * line is not vertical.
    */
   bool IsValid() const
   {
      return IsFinite( b );
   }

   /*!
    * Evaluates the fitted line function for the specified abscissa \a x.
    */
   double operator ()( double x ) const
   {
      return a + b*x;
   }

   /*!
    * Constructs a default %LinearFit object. The resulting object corresponds
    * to a horizontal line crossing at the origin, i.e. the X axis.
    */
   LinearFit() : a( 0 ), b( 0 ), adev( 0 )
   {
   }

   /*!
    * Constructs a %LinearFit object representing the straight line that passes
    * through two points {x1,y1} and {x2,y2}.
    *
    * \warning Do not specify a vertical line, that is, make sure that
    * x1 != x2 w.r.t. the machine epsilon, or this constructor will perform a
    * division by zero.
    */
   template <typename T>
   LinearFit( T x1, T y1, T x2, T y2 )
   {
      b = double( y2 - y1 )/double( x2 - x1 );
      a = (y2 - b*x2 + y1 - b*x1)/2;
      adev = 0;
   }

   /*!
    * Constructs a %LinearFit object to fit a straight line for the specified
    * set of data points.
    *
    * \param x       %Array of abscissae values.
    *
    * \param y       %Array of ordinate values.
    *
    * \param n       Number of data points. Must be >= 2.
    *
    * \param status  If nonzero, pointer to a StatusMonitor object that will be
    *                updated during the fitting process. The linear fit process
    *                is \e unbounded, i.e., the total number of monitoring
    *                steps cannot be known in advance. The default value is
    *                zero (no monitoring).
    *
    * When n >= 3, this constructor fits the straight line that minimizes mean
    * absolute deviation for all the source data points. When n == 2, this
    * function finds the straight line that passes through the two data points.
    * When n < 2, this constructor throws an Error exception.
    */
   template <typename T>
   LinearFit( const T* x, const T* y, size_type n, StatusMonitor* status = 0 )
   {
      if ( n < 2 )
         throw Error( "At least two data points are required in LinearFit::LinearFit()" );

      if ( n == 2 )
      {
         b = double( y[1] - y[0] )/double( x[1] - x[0] );
         a = (y[1] - b*x[1] + y[0] - b*x[0])/2;
         adev = 0;
      }
      else
         Fit( a, b, adev, x, y, n, status );
   }

   /*!
    * Constructs a %LinearFit object to fit a straight line for the specified
    * data containers.
    *
    * \param x       %Container of abscissae values.
    *
    * \param y       %Container of ordinate values.
    *
    * \param status  If nonzero, pointer to a StatusMonitor object that will be
    *                updated during the fitting process. The linear fit process
    *                is \e unbounded, i.e., the total number of monitoring
    *                steps cannot be known in advance. The default value is
    *                zero (no monitoring).
    *
    * The number \a n of data points is the minimum container length, i.e.:
    *
    * <pre>n = Min( x.Length(), y.Length() )</pre>
    *
    * When n >= 3, this constructor fits the straight line that minimizes mean
    * absolute deviation for all the source data points. When n == 2, this
    * function finds the straight line that passes through the two data points.
    * When n < 2, this constructor throws an Error exception.
    */
   template <class C>
   LinearFit( const C& x, const C& y, StatusMonitor* status = 0 )
   {
      size_type n = Min( x.Length(), y.Length() );
      if ( n < 2 )
         throw Error( "At least two data points are required in LinearFit::LinearFit()" );

      if ( n == 2 )
      {
         b = double( y[1] - y[0] )/double( x[1] - x[0] );
         a = (y[1] - b*x[1] + y[0] - b*x[0])/2;
         adev = 0;
      }
      else
         Fit( a, b, adev, x.Begin(), y.Begin(), n, status );
   }

private:

   static void Fit( double& a, double& b, double& adev,
                    const float* fx, const float* fy, size_type n, StatusMonitor* status );
   static void Fit( double& a, double& b, double& adev,
                    const double* fx, const double* fy, size_type n, StatusMonitor* status );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __LinearFit_h

// ****************************************************************************
// EOF pcl/LinearFit.h - Released 2014/11/14 17:16:40 UTC
