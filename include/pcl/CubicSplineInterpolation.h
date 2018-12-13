//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/CubicSplineInterpolation.h - Released 2018-12-12T09:24:21Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_CubicSplineInterpolation_h
#define __PCL_CubicSplineInterpolation_h

/// \file pcl/CubicSplineInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/UnidimensionalInterpolation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class CubicSplineInterpolationBase
 * \brief Base class of interpolating cubic splines in one dimension
 * \internal
 */
class CubicSplineInterpolationBase
{
protected:

   CubicSplineInterpolationBase() = default;

   CubicSplineInterpolationBase( const CubicSplineInterpolationBase& ) = default;

   virtual ~CubicSplineInterpolationBase()
   {
   }

   /*
    * Cubic splines with prescribed, monotonically increasing x-values.
    */
   static void Generate( float* dy2, const float* fx, const float* fy, float dy1, float dyn, int n );
   static void Generate( double* dy2, const double* fx, const double* fy, double dy1, double dyn, int n );
   static void Generate( float* dy2, const float* fx, const float* fy, int n );
   static void Generate( double* dy2, const double* fx, const double* fy, int n );
   static float Interpolate( const float* fx, const float* fy, const float* dy2, int n, double x, int32& k );
   static double Interpolate( const double* fx, const double* fy, const double* dy2, int n, double x, int32& k );

   /*
    * Cubic splines with implicit x[i] = i for i = {0,1,...,n-1}
    */
   static void Generate( float* dy2, const float* fy, int n );
   static void Generate( double* dy2, const double* fy, int n );
   static void Generate( float* dy2, const uint8* fy, int n );
   static void Generate( float* dy2, const uint16* fy, int n );
   static void Generate( double* dy2, const uint32* fy, int n );
   static float Interpolate( const float* fy, const float* dy2, int n, double x );
   static double Interpolate( const double* fy, const double* dy2, int n, double x );
   static float Interpolate( const uint8* fy, const float* dy2, int n, double x );
   static float Interpolate( const uint16* fy, const float* dy2, int n, double x );
   static double Interpolate( const uint32* fy, const double* dy2, int n, double x );
};

/*!
 * \class CubicSplineInterpolation
 * \brief Generic interpolating cubic spline
 *
 * ### TODO: Write a description for %CubicSplineInterpolation.
 */
template <typename T>
class PCL_CLASS CubicSplineInterpolation : public UnidimensionalInterpolation<T>,
                                           private CubicSplineInterpolationBase
{
public:

   typedef typename UnidimensionalInterpolation<T>::vector_type vector_type;

   /*!
    * Constructs a %CubicSplineInterpolation instance.
    */
   CubicSplineInterpolation() = default;

   /*!
    * Copy constructor.
    */
   CubicSplineInterpolation( const CubicSplineInterpolation& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~CubicSplineInterpolation()
   {
   }

   /*!
    * Gets the boundary conditions of this interpolating cubic spline.
    *
    * \param[out] y1    First derivative of the interpolating cubic spline at
    *                   the first data point x[0].
    *
    * \param[out] yn    First derivative of the interpolating cubic spline at
    *                   the last data point x[n-1].
    */
   void GetBoundaryConditions( T& y1, T& yn ) const
   {
      y1 = m_dy1;
      yn = m_dyn;
   }

   /*!
    * Sets the boundary conditions of this interpolating cubic spline.
    *
    * \param y1   First derivative of the interpolating cubic spline at the
    *             first data point x[0].
    *
    * \param yn   First derivative of the interpolating cubic spline at the
    *             last data point x[n-1].
    */
   void SetBoundaryConditions( T y1, T yn )
   {
      Clear();
      m_dy1 = y1;
      m_dyn = yn;
   }

   /*!
    * Generation of an interpolating cubic spline.
    *
    * \param x    %Vector of x-values:\n
    *             \n
    *    \li If \a x is not empty: Must be a vector of monotonically
    *    increasing, distinct values: x[0] < x[1] < ... < x[n-1].\n
    *    \li If \a x is empty: This function will generate a natural cubic
    *    spline with implicit x[i] = i for i = {0,1,...,n-1}.
    *
    * \param y    %Vector of function values for i = {0,1,...,n-1}.
    *
    * When \a x is an empty vector, a <em>natural spline</em> is always
    * generated: boundary conditions are ignored and taken as zero at both ends
    * of the data sequence.
    *
    * The length of the \a y vector (and also the length of a nonempty \a x
    * vector) must be \e n >= 2.
    */
   void Initialize( const vector_type& x, const vector_type& y ) override
   {
      if ( y.Length() < 2 )
         throw Error( "Need two or more data points in CubicSplineInterpolation::Initialize()" );

      try
      {
         Clear();
         UnidimensionalInterpolation<T>::Initialize( x, y );
         m_dy2 = vector_type( this->Length() );
         m_current = -1;   // prepare for 1st interpolation

         if ( this->m_x )
         {
            // Cubic spline with explicit x[i] for i = {0,...,n-1}
            if ( m_dy1 == 0 && m_dyn == 0 )
               // Natural cubic spline
               Generate( m_dy2.Begin(), this->m_x.Begin(), this->m_y.Begin(), this->m_y.Length() );
            else
               // Cubic spline with specified end point derivatives
               Generate( m_dy2.Begin(), this->m_x.Begin(), this->m_y.Begin(), m_dy1, m_dyn, this->m_y.Length() );
         }
         else
         {
            // Natural cubic spline with implicit x[i] = i for i = {0,1,...,n-1}
            Generate( m_dy2.Begin(), this->m_y.Begin(), this->m_y.Length() );
         }
      }
      catch ( ... )
      {
         Clear();
         throw;
      }
   }

   /*!
    * Cubic spline interpolation. Returns an interpolated value at the
    * specified point \a x.
    */
   double operator()( double x ) const override
   {
      PCL_PRECONDITION( IsValid() )

      if ( this->m_x )
         // Cubic spline with explicit x[i] for i = {0,...,n-1}
         return Interpolate( this->m_x.Begin(), this->m_y.Begin(), m_dy2.Begin(), this->m_y.Length(), x, m_current );
      else
         // Natural cubic spline with implicit x[i] = i for i = {0,1,...,n-1}
         return Interpolate( this->m_y.Begin(), m_dy2.Begin(), this->m_y.Length(), x );
   }

   /*!
    * Resets this cubic spline interpolation, deallocating all internal
    * working structures.
    */
   void Clear() override
   {
      UnidimensionalInterpolation<T>::Clear();
      m_dy2.Clear();
   }

   /*!
    * Returns true iff this interpolation is valid, i.e. if it has been
    * correctly initialized and is ready to interpolate function values.
    */
   bool IsValid() const override
   {
      return m_dy2;
   }

private:

           T           m_dy1 = 0;     // 1st derivative of spline at the first data point
           T           m_dyn = 0;     // 1st derivative of spline at the last data point
           vector_type m_dy2;         // second derivatives of the interpolating function at x[i]
   mutable int32       m_current = 0; // index of the current interpolation segment
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_CubicSplineInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/CubicSplineInterpolation.h - Released 2018-12-12T09:24:21Z
