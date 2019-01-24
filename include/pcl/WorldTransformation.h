//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/WorldTransformation.h - Released 2019-01-21T12:06:07Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_WorldTransformation_h
#define __PCL_WorldTransformation_h

/// \file pcl/WorldTransformation.h

#include <pcl/Defs.h>

#include <pcl/LinearTransformation.h>
#include <pcl/SurfaceSplines.h>
#include <pcl/WCSKeywords.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class WorldTransformation
 * \brief Abstract base class of world coordinate transformations
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS WorldTransformation
{
public:

   /*!
    * Default constructor.
    */
   WorldTransformation() = default;

   /*!
    * Copy constructor.
    */
   WorldTransformation( const WorldTransformation& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~WorldTransformation()
   {
   }

   /*!
    * Returns true iff this transformation is empty (uninitialized, invalid).
    */
   virtual bool IsEmpty() const
   {
      return false;
   }

   /*!
    * Returns a dynamically allocated copy of this object.
    */
   virtual WorldTransformation* Clone() const = 0;

   /*!
    * Transforms from world coordinates to image coordinates.
    */
   virtual DPoint Direct( const DPoint& p ) const = 0;

   /*!
    * Transforms from image coordinates to world coordinates.
    */
   virtual DPoint Inverse( const DPoint& p ) const = 0;

   /*!
    * Returns an approximate linear transformation from Image to World
    * coordinates.
    */
   virtual const LinearTransformation& ApproximateLinearTransform() const = 0;
};

// ----------------------------------------------------------------------------

/*!
 * \class LinearWorldTransformation
 * \brief WCS linear world coordinate transformation
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS LinearWorldTransformation : public WorldTransformation
{
public:

   /*!
    * Constructor from a linear transformation. The specified transformation
    * \a transIW must transform from image coordinates to world coordinates.
    */
   LinearWorldTransformation( const LinearTransformation& transIW ) :
      m_transWI( transIW.Inverse() ),
      m_transIW( transIW )
   {
   }

   /*!
    * Copy constructor.
    */
   LinearWorldTransformation( const LinearWorldTransformation& ) = default;

   /*!
    * Move constructor.
    */
   LinearWorldTransformation( LinearWorldTransformation&& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~LinearWorldTransformation()
   {
   }

   /*!
    * Returns false, since a linear WCS transformation cannot be empty.
    */
   bool IsEmpty() const override
   {
      return false;
   }

   /*!
    * Returns a dynamically allocated copy of this object.
    */
   WorldTransformation* Clone() const override
   {
      return new LinearWorldTransformation( *this );
   }

   /*!
    * Transforms from world coordinates to image coordinates.
    */
   DPoint Direct( const DPoint& p ) const override
   {
      return m_transWI.Transform( p );
   }

   /*!
    * Transforms from image coordinates to world coordinates.
    */
   DPoint Inverse( const DPoint& p ) const override
   {
      return m_transIW.Transform( p );
   }

   /*!
    * Returns a reference to the internal linear transformation (from image to
    * world coordinates).
    */
   const LinearTransformation& ApproximateLinearTransform() const override
   {
      return m_transIW;
   }

private:

   LinearTransformation m_transWI; // world -> image
   LinearTransformation m_transIW; // image -> world
};

// ----------------------------------------------------------------------------

/*!
 * \class SplineWorldTransformation
 * \brief Surface spline world coordinate transformation
 *
 * %SplineWorldTransformation implements a world coordinate transform based on
 * 2-D interpolating/approximating surface splines (also known as <em>thin
 * plates</em>). These mathematical modeling devices allow for accurate
 * representations of coordinate systems subject to arbitrary local
 * distortions, which are impossible to achieve with linear transformations.
 *
 * The underlying implementation of this class uses the SurfaceSpline,
 * PointSurfaceSpline and PointGridInterpolation template classes.
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS SplineWorldTransformation : public WorldTransformation
{
public:

   /*!
    * Constructs a 2-D spline based world coordinate transformation.
    *
    * \param controlPointsW   Array of world control points. Each point in this
    *                         array must contain equatorial coordinates in
    *                         degrees. For a given point p, p.x is the right
    *                         ascension coordinate and p.y is the declination.
    *
    * \param controlPointsI   Array of image control points. Each point in this
    *                         array must contain the coordinates on the X and Y
    *                         axes of the image plane corresponding to the same
    *                         point in the \a controlPointsW array. In other
    *                         words, there must be a one-to-one correspondence
    *                         between world and image control points.
    *
    * \param smoothness       When this parameter is greater than zero,
    *                         approximating splines will be generated instead
    *                         of interpolating splines. The higher this value,
    *                         the closest will be the 2-D approximating surface
    *                         to the reference plane of the image.
    *                         Approximating surface splines are robust to
    *                         outlier control points and hence recommended in
    *                         virtually all cases. The default smoothness value
    *                         is 0.05.
    *
    * \param weights          When the \a smoothness parameter is greater than
    *                         zero and this vector is not empty, it must define
    *                         a positive weight greater than zero for each
    *                         point in the \a controlPointsW and
    *                         \a controlPointsI arrays. If \a smoothness is
    *                         zero or negative, this parameter will be ignored.
    *                         See the PointSurfaceSpline::Initialize() member
    *                         function for detailed information on 2-D spline
    *                         node weights.
    *
    * \param order            Derivative order of continuity. The default value
    *                         is 2, which is also recommended for most
    *                         applications. Higher orders may improve
    *                         adaptability to complex distortions, but may also
    *                         lead to ill-conditioned system matrices and hence
    *                         to errors while building the surface splines.
    *
    * After surface spline initialization, an approximate linear transformation
    * will be calculated automatically.
    *
    * Newly constructed instances are guaranteed to be valid (in the structural
    * and numerical senses). In the event of invalid input data or spline
    * initialization problems, this constructor will throw an Error exception.
    */
   SplineWorldTransformation( const Array<DPoint>& controlPointsW,
                              const Array<DPoint>& controlPointsI,
                              float smoothness = 0.05, const FVector& weights = FVector(), int order = 2 ) :
      m_controlPointsW( controlPointsW ),
      m_controlPointsI( controlPointsI ),
      m_order( order ),
      m_smoothness( smoothness ),
      m_weights( weights )
   {
      InitializeSplines();
      CalculateLinearApproximation();
   }

   /*!
    * Constructs a %SplineWorldTransformation instance by deserializing the
    * specified raw \a data.
    *
    * An approximate linear transformation will be calculated automatically.
    */
   SplineWorldTransformation( const ByteArray& data )
   {
      Deserialize( data );
      InitializeSplines();
      CalculateLinearApproximation();
   }

   /*!
    * Constructs a %SplineWorldTransformation instance by deserializing the
    * specified raw \a data, with a prescribed approximate linear
    * transformation \a linearTransIW.
    */
   SplineWorldTransformation( const ByteArray& data, const LinearTransformation& linearTransIW )
   {
      Deserialize( data );
      InitializeSplines();
      m_linearIW = linearTransIW;
   }

   /*!
    * Copy constructor.
    */
   SplineWorldTransformation( const SplineWorldTransformation& ) = default;

   /*!
    * Move constructor.
    */
   SplineWorldTransformation( SplineWorldTransformation&& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~SplineWorldTransformation()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SplineWorldTransformation& operator =( const SplineWorldTransformation& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   SplineWorldTransformation& operator =( SplineWorldTransformation&& ) = default;

   /*!
    * Returns true iff this object has no working data.
    *
    * With the restrictions imposed by class constructors, this can only happen
    * if this object is an xvalue after move construction or assignment.
    */
   bool IsEmpty() const override
   {
      return m_controlPointsW.IsEmpty() || m_controlPointsI.IsEmpty();
   }

   /*!
    * Returns a dynamically allocated copy of this object.
    */
   WorldTransformation* Clone() const override
   {
      return new SplineWorldTransformation( *this );
   }

   /*!
    * Transforms from world coordinates to image coordinates.
    *
    * If grid interpolations have been initialized for this object, they will
    * be used transparently by this member function. Otherwise the surface
    * splines will be interpolated directly.
    *
    * If grid interpolation is being used, the specified coordinates should not
    * lie outside the reference rectangle used for grid initialization.
    *
    * \sa Inverse(), InitializeGridInterpolations()
    */
   DPoint Direct( const DPoint& p ) const override
   {
      if ( m_gridWI.IsValid() )
         return m_gridWI( p );
      return m_splineWI( p );
   }

   /*!
    * Transforms from image coordinates to world coordinates.
    *
    * If grid interpolations have been initialized for this object, they will
    * be used transparently by this member function. Otherwise the surface
    * splines will be interpolated directly.
    *
    * If grid interpolation is being used, the specified coordinates should not
    * lie outside the reference rectangle used for grid initialization.
    *
    * \sa Direct(), InitializeGridInterpolations()
    */
   DPoint Inverse( const DPoint& p ) const override
   {
      if ( m_gridIW.IsValid() )
         return m_gridIW( p );
      return m_splineIW(p);
   }

   /*!
    * Returns an approximate linear transformation from Image to World
    * coordinates, computed from the internal point surface splines.
    */
   const LinearTransformation& ApproximateLinearTransform() const override
   {
      return m_linearIW;
   }

   /*!
    * Initializes the internal grid interpolation devices for the specified
    * reference rectangular region \a rect and grid distance \a delta.
    *
    * See GridInterpolation::Initialize() for detailed information on spline
    * grid interpolation and its parameters.
    */
   void InitializeGridInterpolations( const Rect& rect, int delta = 8 )
   {
      m_gridWI.Initialize( rect, delta, m_splineWI );
      m_gridIW.Initialize( rect, delta, m_splineIW );
   }

   /*!
    * Serializes this %SplineWorldTransformation instance in raw binary format
    * and stores the result in the specified \a data array.
    */
   void Serialize( ByteArray& data ) const;

private:

   Array<DPoint>              m_controlPointsW;
   Array<DPoint>              m_controlPointsI;
   int                        m_order = 2;
   float                      m_smoothness = 0;
   FVector                    m_weights;
   PointSurfaceSpline<DPoint> m_splineWI;
   PointSurfaceSpline<DPoint> m_splineIW;
   PointGridInterpolation     m_gridWI;
   PointGridInterpolation     m_gridIW;
   LinearTransformation       m_linearIW;

   void Deserialize( const ByteArray& );
   void InitializeSplines();
   void CalculateLinearApproximation();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_WorldTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/WorldTransformation.h - Released 2019-01-21T12:06:07Z
