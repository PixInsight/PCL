//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/WCSKeywords.h - Released 2017-08-01T14:23:31Z
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

#ifndef __PCL_WCSKeywords_h
#define __PCL_WCSKeywords_h

/// \file pcl/WCSKeywords.h

#include <pcl/Defs.h>

#include <pcl/FITSHeaderKeyword.h>
#include <pcl/Optional.h>
#include <pcl/String.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

class LinearTransformation;

/*!
 * \class WCSKeywords
 * \brief A structure for management of WCS FITS header keywords and associated
 * image properties
 *
 * \b References
 *
 * M. R. Calabretta, E. W. Greisen (2002) <em>Representations of World
 * Coordinates in FITS</em>, Astronomy & Astrophysics, 395, 1061-1075,
 * 1077-1122.
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS WCSKeywords
{
public:

   Optional<double> objctra;   //!< Right ascension coordinate of the center of the image in degrees.
   Optional<double> objctdec;  //!< Declination coordinate of the center of the image in degrees.
   Optional<double> focallen;  //!< Focal length in millimeters.
   Optional<double> xpixsz;    //!< Pixel size in micrometers.
   Optional<double> crval1;    //!< WCS coordinate at the reference pixel, X axis.
   Optional<double> crval2;    //!< WCS coordinate at the reference pixel, Y axis.
   Optional<double> crpix1;    //!< Image coordinate of the reference pixel on the X axis.
   Optional<double> crpix2;    //!< Image coordinate of the reference pixel on the Y axis.
   Optional<double> cd1_1;     //!< Rotation and scaling matrix: CDELT1 * cos(CROTA2)
   Optional<double> cd1_2;     //!< Rotation and scaling matrix: -CDELT2 * sin(CROTA2)
   Optional<double> cd2_1;     //!< Rotation and scaling matrix: CDELT1 * sin(CROTA2)
   Optional<double> cd2_2;     //!< Rotation and scaling matrix: CDELT2 * cos(CROTA2)
   Optional<double> cdelt1;    //!< Plate scale in degrees per pixel at the reference pixel, X axis.
   Optional<double> cdelt2;    //!< Plate scale in degrees per pixel at the reference pixel, Y axis.
   Optional<double> crota1;    //!< Rotation angle around the reference pixel in degrees. Same as CROTA2, unused.
   Optional<double> crota2;    //!< Rotation angle around the reference pixel in degrees.
   IsoString        ctype1;    //!< Coordinate type and projection.
   IsoString        ctype2;    //!< Coordinate type and projection.
   Optional<double> pv1_1;     //!< Native longitude of the reference point in degrees.
   Optional<double> pv1_2;     //!< Native latitude of the reference point in degrees.
   Optional<double> lonpole;   //!< Native longitude of the pole of the coordinate system in degrees.
   Optional<double> latpole;   //!< Native latitude of the pole of the coordinate system in degrees.
   Optional<double> dateobs;   //!< Observation time as a Julian day number.
   IsoString        refSpline; //!< If present, the astrometric solution uses 2-D surface splines (aka <em>thin plates</em>) instead of WCS polynomials.

   /*!
    * Default constructor. Constructs an uninitialized/undefined %WCSKeywords
    * object.
    */
   WCSKeywords() = default;

   /*!
    * Copy constructor.
    */
   WCSKeywords( const WCSKeywords& ) = default;

   /*!
    * Constructs a %WCSKeywords object by extracting data from the specified
    * array of \a keywords.
    */
   WCSKeywords( const FITSKeywordArray& keywords )
   {
      Read( keywords );
   }

   /*!
    * Extracts WCS metadata from the specified array of \a keywords. If some
    * metadata items are not included in the \a keywords, the previously
    * defined items, if any, are preserved.
    */
   void Read( const FITSKeywordArray& keywords );

   /*!
    *
    */
   bool ExtractWorldTransformation( LinearTransformation& transIW, int imageHeight );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_WCSKeywords_h

// ----------------------------------------------------------------------------
// EOF pcl/WCSKeywords.h - Released 2017-08-01T14:23:31Z
