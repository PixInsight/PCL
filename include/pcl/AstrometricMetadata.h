//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/AstrometricMetadata.h - Released 2017-07-18T16:13:52Z
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

class PCL_CLASS ImageWindow;
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
#ifndef _MSC_VER
   AstrometricMetadata( AstrometricMetadata&& ) = default;
#else
   AstrometricMetadata( AstrometricMetadata&& x ) :
      m_projection( x.m_projection.Release() ),
      m_transformWI( x.m_transformWI.Release() ),
      m_width( x.m_width ),
      m_height( x.m_height ),
      m_xpixsz( x.m_xpixsz ),
      m_dateobs( x.m_dateobs ),
      m_resolution( x.m_resolution ),
      m_focal( x.m_focal ),
      m_useFocal( x.m_useFocal )
   {
   }
#endif

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
   AstrometricMetadata( ProjectionBase* projection, WorldTransformation* worldTransformation, int width, int height ) :
      m_projection( projection ),
      m_transformWI( worldTransformation ),
      m_width( width ),
      m_height( height )
   {
      LinearTransformation linearTransIW = m_transformWI->ApproximateLinearTransform();
      double resx = Sqrt( linearTransIW.A00() * linearTransIW.A00() + linearTransIW.A01() * linearTransIW.A01() );
      double resy = Sqrt( linearTransIW.A10() * linearTransIW.A10() + linearTransIW.A11() * linearTransIW.A11() );
      m_resolution = (resx + resy)/2;
   }

   /*!
    * Constructs an %AstrometricMetadata object from the existing keywords and
    * properties of an image \a window.
    */
   AstrometricMetadata( const ImageWindow& window );

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
   AstrometricMetadata& operator =( AstrometricMetadata&& x )
   {
      m_projection  = x.m_projection.Release();
      m_transformWI = x.m_transformWI.Release();
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
    * Returns true iff this object is valid. A valid %AstrometricMetadata
    * instance defines a projection system and a world transformation.
    */
   bool IsValid() const
   {
      return !m_projection.IsNull() && !m_transformWI.IsNull();
   }

   /*!
    * Checks that this metadata is valid and can perform coherent coordinate
    * transformations.
    */
   void Validate() const;

   /*!
    * Returns true if this object uses a world transformation based on 2-D
    * surface splines, false if a WCS linear transformation is being used.
    */
   bool HasSplineWorldTransformation() const
   {
      return dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() ) != nullptr;
   }

   /*!
    * Updates the keywords and properties of an image \a window to store the
    * astrometric metadata represented by this object.
    *
    * The pixel dimensions of the image in th target \a window must be coherent
    * with this astrometric solution. If that is not true, this member function
    * will throw an Error exception.
    */
   void Write( ImageWindow& window, bool notify = true ) const;

   /*!
    * Updates the keywords and properties of the current image in an XISF
    * \a writer to store the astrometric metadata represented by this object.
    *
    * The caller must ensure that the pixel dimensions of the current image in
    * the target %XISF \a writer, that is, the width and height of the image
    * being generated, are coherent with this astrometric solution. This
    * condition cannot be verified by this member function.
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
    * Returns the projection system of this astrometric solution. Returns
    * \c nullptr if this object is not valid.
    */
   const ProjectionBase* Projection() const
   {
      return m_projection.Pointer();
   }

   /*!
    * Returns the world transformation of this astrometric solution. Returns
    * \c nullptr if this object is not valid.
    */
   const WorldTransformation* WorldTransform() const
   {
      return m_transformWI.Pointer();
   }

   /*!
    *
    */
   double ResolutionFromFocal( double focal ) const
   {
      return (focal > 0) ? m_xpixsz.OrElse( 0 )/focal * 0.18/Const<double>::pi() : 0.0;
   }

   /*!
    *
    */
   double FocalFromResolution( double resolution ) const
   {
      return ResolutionFromFocal( resolution );
   }

   /*!
    *
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
    * Returns the Julian day number of the observation.
    */
   Optional<double> DateObs() const
   {
      return m_dateobs;
   }

   /*!
    * Sets the julian date of the observation
    */
   void SetDateObs( double dateObsJD )
   {
      m_dateobs = dateObsJD;
   }

   /*!
    *
    */
   Optional<double> PixelSize() const
   {
      return m_xpixsz;
   }

   /*!
    *
    */
   void SetPixelSize( double pixelSize )
   {
      m_xpixsz = pixelSize;
      m_useFocal = false;
      m_focal = FocalFromResolution( m_resolution );
   }

   /*!
    * Transforms from image coordinates to celestial coordinates
    */
   bool ImageToCelestial( DPoint& pRD, const DPoint& pI ) const
   {
      if ( !IsValid() )
         throw Error( "Invalid call to AstrometricMetadata::ImageToCelestial(): No astrometric solution" );
      return m_projection->Inverse( pRD, m_transformWI->Inverse( pI ) );
   }

   /*!
    * Transforms from celestial coordinates to image coordinates.
    */
   bool CelestialToImage( DPoint& pI, const DPoint& pRD ) const
   {
      if ( !IsValid() )
         throw Error( "Invalid call to AstrometricMetadata::CelestialToImage(): No astrometric solution" );
      DPoint pW;
      if ( m_projection->Direct( pW, pRD ) )
      {
         pI = m_transformWI->Direct( pW );
         return true;
      }
      return false;
   }

   /*!
    * Returns a printable textual representation of the metadata properties and
    * parameters of the astrometric solution represented by this object.
    */
   String Summary() const;

private:

   AutoPointer<ProjectionBase>      m_projection;
   AutoPointer<WorldTransformation> m_transformWI;
   int                              m_width = 0;
   int                              m_height = 0;
   Optional<double>                 m_xpixsz;
   Optional<double>                 m_dateobs;
   double                           m_resolution = 0;   // deg/px
   Optional<double>                 m_focal;            // mm
   bool                             m_useFocal = false;

   void Build( const FITSKeywordArray&, const ByteArray&, int, int );
   void UpdateBasicKeywords( FITSKeywordArray& ) const;
   void UpdateWCSKeywords( FITSKeywordArray& ) const;
   WCSKeywords GetWCSvalues() const;
};

} //pcl

#endif // __AstrometricMetadata_h

// ----------------------------------------------------------------------------
// EOF pcl/AstrometricMetadata.h - Released 2017-07-18T16:13:52Z
