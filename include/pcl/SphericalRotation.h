//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/SphericalRotation.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_SphericalRotation_h
#define __PCL_SphericalRotation_h

/// \file pcl/SphericalRotation.h

#include <pcl/Defs.h>

#include <pcl/Optional.h>
#include <pcl/Point.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SphericalRotation
 * \brief Rotation in spherical coordinates
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS SphericalRotation
{
public:

   constexpr static double Tolerance = 1.0e-05;

   /*!
    * Default constructor.
    */
   SphericalRotation() = default;

   /*!
    * Copy constructor.
    */
   SphericalRotation( const SphericalRotation& ) = default;

   /*!
    *
    */
   SphericalRotation( double lng0, double lat0, double phi0, double theta0, double phip,
                      const Optional<double>& latpole );

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SphericalRotation& operator =( const SphericalRotation& ) = default;

   /*!
    *
    */
   DPoint NativeToCelestial( const DPoint& np ) const;

   /*!
    *
    */
   DPoint CelestialToNative( const DPoint& cp ) const;

private:

   double m_latpole;
   double m_alphaP;
   double m_deltaP;
   double m_phiP;
   double m_cosdeltaP;
   double m_sindeltaP;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SphericalRotation_h

// ----------------------------------------------------------------------------
// EOF pcl/SphericalRotation.h - Released 2017-07-09T18:07:07Z
