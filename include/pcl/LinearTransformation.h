//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/LinearTransformation.h - Released 2017-06-17T10:55:43Z
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

#ifndef __PCL_LinearTransformation_h
#define __PCL_LinearTransformation_h

/// \file pcl/LinearTransformation.h

#include <pcl/Defs.h>

#include <pcl/Matrix.h>
#include <pcl/Point.h>
#include <pcl/String.h>
#include <pcl/Vector.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class LinearTransformation
 * \brief A linear geometric transformation on the plane defined as a 2x3
 * matrix of 64-bit floating point scalars.
 *
 * %LinearTransformation is a simple structure where the six transformation
 * coefficients are stored directly as \c double scalars. Since the third row
 * of the transformation matrix is (0, 0, 1) implicitly, this class can only
 * represent translations, rotations and scale changes. This class is primarily
 * used to support WCS coordinate transformations.
 *
 * \ingroup astrometry_support
 */
class LinearTransformation
{
public:

   /*!
    * Default constructor. Constructs a unit linear transformation.
    */
   LinearTransformation() = default;

   /*!
    * Copy constructor.
    */
   LinearTransformation( const LinearTransformation& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   LinearTransformation& operator =( const LinearTransformation& ) = default;

   /*!
    * Constructs a linear transformation with the specified matrix elements.
    */
   LinearTransformation( double a00, double a01, double a02, double a10, double a11, double a12 ) :
      m_a00( a00 ), m_a01( a01 ), m_a02( a02 ),
      m_a10( a10 ), m_a11( a11 ), m_a12( a12 ),
      m_det( a00*a11 - a01*a10 )
   {
   }

   /*!
    * Returns true if this transformation has been defined by a singular
    * matrix. A singular matrix has zero or insignificant (with respect to the
    * machine epsilon for \c double) determinant and hence cannot be inverted.
    */
   bool IsSingularMatrix() const
   {
      return 1 + m_det == 1;
   }

   /*!
    * Returns the transformation of the specified point \a p.
    */
   DPoint Transform( const DPoint& p ) const
   {
      return DPoint( m_a00*p.x + m_a01*p.y + m_a02,
                     m_a10*p.x + m_a11*p.y + m_a12 );
   }

   /*!
    * Returns the transformation of the specified point \a p. This operator is
    * equivalent to the Transform() member function.
    */
   DPoint operator ()( const DPoint& p ) const
   {
      return Transform( p );
   }

   /*!
    * Returns the inverse transformation of the specified point \a p.
    *
    * If the transformation matrix is singular, this member function throws an
    * Error exception.
    */
   DPoint TransformInverse( const DPoint& p ) const
   {
      if ( IsSingularMatrix() )
         throw Error( "Invalid call to LinearTransformation::TransformInverse(): Singular matrix" );
      return DPoint( ( m_a11*p.x - m_a01*p.y + m_a01*m_a12 - m_a02*m_a11)/m_det,
                     (-m_a10*p.x + m_a00*p.y + m_a02*m_a10 - m_a00*m_a12)/m_det );
   }

   /*!
    * Returns the product of this transformation by a transformation \a T.
    */
   LinearTransformation Multiply( const LinearTransformation& T ) const
   {
      return LinearTransformation( m_a00*T.m_a00 + m_a01*T.m_a10,
                                   m_a00*T.m_a01 + m_a01*T.m_a11,
                                   m_a00*T.m_a02 + m_a01*T.m_a12 + m_a02,
                                   m_a10*T.m_a00 + m_a11*T.m_a10,
                                   m_a10*T.m_a01 + m_a11*T.m_a11,
                                   m_a10*T.m_a02 + m_a11*T.m_a12 + m_a12 );
   }

   /*!
    * Returns the inverse of this transformation.
    *
    * If the transformation matrix is singular, this member function throws an
    * Error exception.
    */
   LinearTransformation Inverse() const
   {
      if ( IsSingularMatrix() )
         throw Error( "Invalid call to LinearTransformation::Inverse(): Singular matrix" );
      return LinearTransformation(  m_a11/m_det,
                                   -m_a01/m_det,
                                   (m_a01*m_a12 - m_a02*m_a11)/m_det,
                                   -m_a10/m_det,
                                    m_a00/m_det,
                                   (m_a02*m_a10 - m_a00*m_a12)/m_det );
   }

   /*!
    * Returns the transformation coefficient at the first row, first column.
    */
   double A00() const
   {
      return m_a00;
   }

   /*!
    * Returns the transformation coefficient at the first row, second column.
    */
   double A01() const
   {
      return m_a01;
   }

   /*!
    * Returns the transformation coefficient at the first row, third column.
    */
   double A02() const
   {
      return m_a02;
   }

   /*!
    * Returns the transformation coefficient at the second row, first column.
    */
   double A10() const
   {
      return m_a10;
   }

   /*!
    * Returns the transformation coefficient at the second row, second column.
    */
   double A11() const
   {
      return m_a11;
   }

   /*!
    * Returns the transformation coefficient at the third row, third column.
    */
   double A12() const
   {
      return m_a12;
   }

   /*!
    * Returns a 2x3 matrix initialized with the transformation coefficients in
    * this object.
    */
   Matrix ToMatrix() const
   {
      Matrix M( 2, 3 );
      M[0][0] = m_a00; M[0][1] = m_a01; M[0][2] = m_a02;
      M[1][0] = m_a10; M[1][1] = m_a11; M[1][2] = m_a12;
      return M;
   }

   /*!
    * Returns a six-component vector initialized with the transformation
    * coefficients in this object.
    */
   Vector ToVector() const
   {
      Vector v( 6 );
      v[0] = m_a00; v[1] = m_a01; v[2] = m_a02;
      v[3] = m_a10; v[4] = m_a11; v[5] = m_a12;
      return v;
   }

   /*!
    * Returns a string representation of the transformation matrix. The
    * representation consists of two text lines, each with \a indent leading
    * spaces, separated by a newline character.
    */
   String ToString( int indent = 0 ) const
   {
      String text;
      if ( indent > 0 )
         text << String( ' ', indent );
      text.AppendFormat( "%+12g %+12g %+12g\n", m_a00, m_a01, m_a02 );
      if ( indent > 0 )
         text << String( ' ', indent );
      text.AppendFormat( "%+12g %+12g %+12g", m_a10, m_a11, m_a12 );
      return text;
   }

private:

   double m_a00 = 1, m_a01 = 0, m_a02 = 0,
          m_a10 = 0, m_a11 = 1, m_a12 = 0;
   double m_det = 1;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_LinearTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/LinearTransformation.h - Released 2017-06-17T10:55:43Z
