//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/AstrometricMetadata.h - Released 2018-11-01T11:06:36Z
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

#ifndef __PCL_AstrometricMetadata_h
#define __PCL_AstrometricMetadata_h

/// \file pcl/AstrometricMetadata.h

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Optional.h>
#include <pcl/Point.h>
#include <pcl/ProjectionBase.h>
#include <pcl/SphericalRotation.h>
#include <pcl/WCSKeywords.h>
#include <pcl/WorldTransformation.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
namespace pi
{
   class ImageWindow;
}
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup astrometry_support Astrometry Support Classes
 *
 * This group of classes implement the core astrometry support on the
 * PixInsight platform.
 *
 * The original implementation of these classes has been contributed by Spanish
 * software developer and PTeam member Andrés del Pozo, who is also the author
 * of all astrometry scripts available in the standard PixInsight distribution,
 * including the ImageSolver and AnnotateImage scripts, among others.
 */

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
class PCL_CLASS ImageWindow;
#endif

class PCL_CLASS XISFReader;
class PCL_CLASS XISFWriter;

/*!
 * \class AstrometricMetadata
 * \brief Astrometric metadata
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS AstrometricMetadata
{
public:

   /*!
    * \struct pcl::AstrometricMetadata::DescriptionItems
    * \brief A collection of strings describing the properties and parameters
    * of an astrometric solution.
    */
   struct DescriptionItems
   {
      String referenceMatrix;
      String wcsTransformationType;
      String projectionName;
      String projectionOrigin;
      String resolution;
      String rotation;
      String transformationErrors;
      String observationDate;
      String focalDistance;
      String pixelSize;
      String fieldOfView;
      String centerCoordinates;
      String topLeftCoordinates;
      String topRightCoordinates;
      String bottomLeftCoordinates;
      String bottomRightCoordinates;
   };

   /*!
    * Default constructor. Constructs an uninitialized %AstrometricMetadata
    * object.
    */
   AstrometricMetadata() = default;

   /*!
    * Copy constructor.
    */
   AstrometricMetadata( const AstrometricMetadata& x ) :
      m_projection( x.m_projection->Clone() ),
      m_transformWI( x.m_transformWI->Clone() ),
      m_width( x.m_width ),
      m_height( x.m_height ),
      m_xpixsz( x.m_xpixsz ),
      m_dateobs( x.m_dateobs ),
      m_resolution( x.m_resolution ),
      m_focal( x.m_focal ),
      m_useFocal( x.m_useFocal )
   {
   }

   /*!
    * Move constructor.
    */
   AstrometricMetadata( AstrometricMetadata&& ) = default;

   /*!
    * Constructor from minimal data.
    *
    * \param projection          Pointer to a dynamically allocated projection
    *                            system.
    *
    * \param worldTransformation Pointer to a dynamically allocated world
    *                            transformation.
    *
    * \param width               Width in pixels of the image with which this
    *                            astrometric solution is associated.
    *
    * \param height              Height in pixels of the image with which this
    *                            astrometric solution is associated.
    *
    * Ownership of the specified \a projection and \a worldTransformation
    * objects will be transferred to this object, which will destroy and
    * deallocate them when appropriate.
    */
   AstrometricMetadata( ProjectionBase* projection, WorldTransformation* worldTransformation, int width, int height );

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   // Implemented in /core/Components/ImageWindow.cpp
   AstrometricMetadata( const pi::ImageWindow* );
#else
   /*!
    * Constructs an %AstrometricMetadata object from the existing keywords and
    * properties of an image \a window.
    */
   AstrometricMetadata( const ImageWindow& window );
#endif

   /*!
    * Constructs an %AstrometricMetadata object from the existing keywords and
    * properties of the currently selected image in an XISF \a reader.
    */
   AstrometricMetadata( XISFReader& reader );

   /*!
    * Virtual destructor.
    */
   virtual ~AstrometricMetadata()
   {
   }

   /*!
    * Copy constructor. Returns a reference to this object.
    */
   AstrometricMetadata& operator =( const AstrometricMetadata& x )
   {
      m_projection  = x.m_projection->Clone();
      m_transformWI = x.m_transformWI->Clone();
      m_width       = x.m_width;
      m_height      = x.m_height;
      m_xpixsz      = x.m_xpixsz;
      m_dateobs     = x.m_dateobs;
      m_resolution  = x.m_resolution;
      m_focal       = x.m_focal;
      m_useFocal    = x.m_useFocal;
      return *this;
   }

   /*!
    * Move constructor. Returns a reference to this object.
    */
   AstrometricMetadata& operator =( AstrometricMetadata&& ) = default;

   /*!
    * Returns true iff this object is valid. A valid %AstrometricMetadata
    * instance defines a projection system and a world transformation.
    */
   bool IsValid() const
   {
      return !m_projection.IsNull() && !m_transformWI.IsNull();
   }

   /*!
    * Checks that this astrometric solution is valid and can perform coherent
    * coordinate transformations, and returns the transformation errors
    * observed at the center of the image.
    *
    * \param[out] ex    Error on the X axis in pixels.
    *
    * \param[out] ey    Error on the Y axis in pixels.
    *
    * This routine performs two successive coordinate transformations, from
    * image to celestial and from celestial to image coordinates, computed for
    * the geometric center of the image. The reported values in the specified
    * \a ex and \a ey variables are the differences between the initial and
    * final image coordinates in pixels.
    *
    * This function throws an Error exception if the solution has not been
    * initialized, or if it cannot perform valid coordinate transformations.
    */
   void Verify( double& ex, double& ey ) const;

   /*!
    * Checks that this astrometric solution is valid and can perform coherent
    * coordinate transformations.
    *
    * \param tolerance     Maximum difference in pixels allowed for validation.
    *                      The default value is 0.01 pixels.
    *
    * This routine performs two successive coordinate transformations, from
    * image to celestial and from celestial to image coordinates, computed for
    * the geometric center of the image. If the difference between the initial
    * and final image coordinates is larger than the specified tolerance in
    * pixels, for one or both axes, an Error exception is thrown.
    *
    * This function also throws an Error exception if the solution has not been
    * initialized, or if it cannot perform valid coordinate transformations.
    */
   void Validate( double tolerance = 0.01 ) const;

   /*!
    * Returns true if this object uses a world transformation based on 2-D
    * surface splines (or <em>thin plates</em>), false if a WCS linear
    * transformation is being used.
    */
   bool HasSplineWorldTransformation() const
   {
      return dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() ) != nullptr;
   }

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   // Implemented in /core/Components/ImageWindow.cpp
   void Write( pi::ImageWindow* window, bool notify = true ) const;
#else
   /*!
    * Updates the keywords and properties of an image \a window to store the
    * astrometric metadata represented by this object.
    *
    * The pixel dimensions of the image in th target \a window must be coherent
    * with this astrometric solution. If that is not true, this member function
    * will throw an Error exception.
    */
   void Write( ImageWindow& window, bool notify = true ) const;
#endif

   /*!
    * Updates the keywords and properties of the current image in an XISF
    * \a writer to store the astrometric metadata represented by this object.
    *
    * The caller must ensure that the pixel dimensions of the current image in
    * the target %XISF \a writer, that is, the width and height of the image
    * being generated, are coherent with this astrometric solution. Currently
    * this condition cannot be verified by this member function.
    */
   void Write( XISFWriter& writer ) const;

   /*!
    * Returns the width in pixels of the image associated with the astrometric
    * solution represented by this object.
    */
   int Width() const
   {
      return m_width;
   }

   /*!
    * Returns the height in pixels of the image associated with the astrometric
    * solution represented by this object.
    */
   int Height() const
   {
      return m_height;
   }

   /*!
    * Returns a pointer to the projection system of this astrometric solution.
    * Returns \c nullptr if this object is not valid.
    */
   const ProjectionBase* Projection() const
   {
      return m_projection.Pointer();
   }

   /*!
    * Returns a pointer to the world coordinate transformation of this
    * astrometric solution. Returns \c nullptr if this object is not valid.
    */
   const WorldTransformation* WorldTransform() const
   {
      return m_transformWI.Pointer();
   }

   /*!
    * Returns the image resolution in seconds of arc per pixel, calculated from
    * the specified \a focal distance in millimeters.
    */
   double ResolutionFromFocal( double focal ) const
   {
      return (focal > 0) ? m_xpixsz.OrElse( 0 )/focal * 0.18/Const<double>::pi() : 0.0;
   }

   /*!
    * Returns the focal distance in millimeters, calculated from the specified
    * image \a resolution expressed in seconds of arc per pixel.
    */
   double FocalFromResolution( double resolution ) const
   {
      return ResolutionFromFocal( resolution );
   }

   /*!
    * Returns the angle of rotation of the Y axis of the image with respect to
    * the north direction, as represented on a flat projection of the celestial
    * sphere. A rotation angle of zero aligns north along the positive Y axis.
    * A positive rotation angle rotates the Y axis from north to east.
    *
    * The returned value is the rotation angle of the projection in degrees.
    * The \a flipped variable will be true iff the projection is mirrored on
    * the east-west direction.
    */
   double Rotation( bool& flipped ) const;

   /*!
    * Returns the resolution of the image in degrees per pixel.
    *
    * This value usually is an approximation since it changes across the image.
    * It should only be used for informative purposes.
    */
   double Resolution() const
   {
      return m_resolution;
   }

   /*!
    * Returns the Julian date of the observation.
    */
   Optional<double> DateObs() const
   {
      return m_dateobs;
   }

   /*!
    * Sets the Julian date of the observation.
    */
   void SetDateObs( double dateObsJD )
   {
      m_dateobs = dateObsJD;
   }

   /*!
    * Returns the physical pixel size in micrometers, if available.
    */
   Optional<double> PixelSize() const
   {
      return m_xpixsz;
   }

   /*!
    * Redefines the physical pixel size in micrometers. Recalculates the focal
    * distance as a function of the pixel size.
    */
   void SetPixelSize( double pixelSize )
   {
      m_xpixsz = pixelSize;
      m_useFocal = false;
      m_focal = FocalFromResolution( m_resolution );
   }

   /*!
    * Transformation from image coordinates to celestial coordinates.
    *
    * \param pRD  Reference to a point where the output equatorial spherical
    *             coordinates will be stored, expressed in degrees. \a pRD.x
    *             will be the right ascension and \a pRD.y the declination.
    *             Output right ascensions are constrained to the [0,360)
    *             range. Output declinations are in the range [-90,+90].
    *
    * \param pI   Input image coordinates in pixels. The specified location can
    *             legally lie outside of the image bounds defined by
    *             [0,0]-[Width(),Height()].
    *
    * Returns true iff the specified point \a pI can be projected on the
    * celestial sphere using this astrometric solution.
    */
   bool ImageToCelestial( DPoint& pRD, const DPoint& pI ) const
   {
      if ( !IsValid() )
         throw Error( "Invalid call to AstrometricMetadata::ImageToCelestial(): No astrometric solution." );
      bool valid = m_projection->Inverse( pRD, m_transformWI->Inverse( pI ) );
      if ( valid )
      {
         // Constrain right ascension to the [0,360) range.
         if ( pRD.x < 0 )
            pRD.x += 360;
         else if ( pRD.x >= 360 )
            pRD.x -= 360;
      }
      return valid;
   }

   /*!
    * Transformation from celestial coordinates to image coordinates.
    *
    * \param pI   Reference to a point where the output image coordinates will
    *             be stored.
    *
    * \param pRD  Input equatorial spherical coordinates expressed in degrees.
    *             \a pRD.x is the right ascension and \a pRD.y is declination.
    *
    * Returns true iff the specified celestial coordinates can be reprojected
    * on the image coordinate system. Note that the output image coordinates
    * can lie outside of the image bounds defined by [0,0]-[Width(),Height()].
    */
   bool CelestialToImage( DPoint& pI, const DPoint& pRD ) const
   {
      if ( !IsValid() )
         throw Error( "Invalid call to AstrometricMetadata::CelestialToImage(): No astrometric solution." );
      DPoint pW;
      if ( m_projection->Direct( pW, pRD ) )
      {
         pI = m_transformWI->Direct( pW );
         return true;
      }
      return false;
   }

   /*!
    * Regenerates the astrometric solution from standardized metadata.
    *
    * \param keywords         A list of FITS header keywords, which should
    *                         contain at least a minimal set of standard WCS
    *                         keywords to define a linear world transformation
    *                         from celestial to image coordinates.
    *
    * \param controlPoints    If not empty, this array must contain a list of
    *                         spline control points and generation parameters
    *                         serialized in raw binary format. See the
    *                         SplineWorldTransformation class for more
    *                         information.
    *
    * \param width            Width in pixels of the image with which this
    *                         astrometric solution is associated.
    *
    * \param height           Height in pixels of the image with which this
    *                         astrometric solution is associated.
    *
    * If the specified \a controlPoints array contains a valid serialization of
    * spline control points, the astrometric solution will use a high-precision
    * world transformation based on two-dimensional surface splines, also knwon
    * as <em>thin plates</em>, which is capable of modeling local image
    * distortions that are intractable with WCS linear transformations.
    *
    * If this object contains valid metadata before calling this function, it
    * will be disposed as appropriate, and a completely new astrometric
    * solution will be constructed.
    *
    * This member function can throw exceptions (of the Error class) if either
    * the specified \a controlPoints array is not empty and does not contain a
    * valid raw serialization of a spline-based transformation, or if the
    * generated coordinate transformations are not invalid (in the numerical or
    * geometric sense).
    */
   void Build( const FITSKeywordArray& keywords, const ByteArray& controlPoints, int width, int height );

   /*!
    * Updates the specified \a keywords array with basic astrometric FITS
    * header keywords: FOCALLEN, XPIXSZ, YPIXSZ, and the nonstandard but
    * customary keywords OBJCTRA and OBJCTDEC.
    */
   void UpdateBasicKeywords( FITSKeywordArray& keywords ) const;

   /*!
    * Updates the specified \a keywords array with the set of standard WCS FITS
    * header keywords: EQUINOX, CTYPE1, CTYPE2, CRPIX1, CRPIX2, CRVAL1, CRVAL2,
    * PV1_1, PV1_2, LONPOLE, LATPOLE, CD1_1, CD1_2, CD2_1, CD2_2, CDELT1,
    * CDELT2, CROTA1, CROTA2 and, if appropriate, a custom REFSPLINE keyword to
    * signal the availability of a spline-based astrometric solution.
    */
   void UpdateWCSKeywords( FITSKeywordArray& keywords ) const;

   /*!
    * Returns a printable textual representation of the metadata properties and
    * parameters of the astrometric solution represented by this object.
    */
   String Summary() const;

   /*!
    * Returns a collection of strings describing the metadata properties and
    * parameters of this astrometric solution.
    *
    * The returned object is a copy of an internal structure that is generated
    * automatically as necessary and cached between successive function calls.
    * This allows for efficient real-time representations of astrometric
    * metadata and parameters.
    */
   DescriptionItems Description() const
   {
      UpdateDescription();
      return m_description.IsNull() ? DescriptionItems() : *m_description;
   }

private:

   AutoPointer<ProjectionBase>      m_projection;
   AutoPointer<WorldTransformation> m_transformWI;
   int                              m_width = 0;
   int                              m_height = 0;
   Optional<double>                 m_xpixsz;
   Optional<double>                 m_dateobs;
   double                           m_resolution = 0; // deg/px
   Optional<double>                 m_focal;          // mm
   bool                             m_useFocal = false;
   mutable
   AutoPointer<DescriptionItems>    m_description;

   WCSKeywords GetWCSvalues() const;
   void UpdateDescription() const;
};

} //pcl

#endif // __AstrometricMetadata_h

// ----------------------------------------------------------------------------
// EOF pcl/AstrometricMetadata.h - Released 2018-11-01T11:06:36Z
