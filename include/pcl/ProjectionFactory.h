//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/ProjectionFactory.h - Released 2018-12-12T09:24:21Z
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

#ifndef __PCL_ProjectionFactory_h
#define __PCL_ProjectionFactory_h

/// \file pcl/ProjectionFactory.h

#include <pcl/Defs.h>

#include <pcl/ProjectionBase.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ProjectionFactory
 * \brief Selectable projection system
 *
 * %ProjectionFactory implements a projection system that can be selected upon
 * construction among all of the projections implemented in the PixInsight/PCL
 * platform.
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS ProjectionFactory
{
public:

   /*!
    * Creates a new projection for the specified WCS projection \a code and
    * origin equatorial coordinates \a ra and \a dec in degrees.
    *
    * The following WCS projection codes are supported in current PCL versions:
    *
    * TAN\n
    * STG\n
    * CAR\n
    * MER\n
    * AIT\n
    * ZEA\n
    * SIN
    *
    * The specified WCS projection \a code is case insensitive and can contain
    * leading and trailing spaces.
    */
   static ProjectionBase* CreateByWCSCode( const IsoString& code, double ra, double dec );

   static ProjectionBase* CreateByWCSCode( const IsoString::ustring_base& code, double ra, double dec )
   {
      return CreateByWCSCode( IsoString( code ), ra, dec );
   }

   /*!
    * Creates a new projection for the specified projection \a name and origin
    * equatorial coordinates \a ra and \a dec in degrees.
    *
    * The following projection names are supported in current PCL versions:
    *
    * Gnomonic\n
    * Stereographic\n
    * PlateCarree\n
    * Mercator\n
    * HammerAitoff\n
    * ZenithalEqualArea\n
    * Orthographic
    *
    * The specified projection \a name is case insensitive and can contain
    * leading and trailing spaces.
    */
   static ProjectionBase* CreateByName( const IsoString& name, double ra, double dec );

   static ProjectionBase* CreateByName( const IsoString::ustring_base& name, double ra, double dec )
   {
      return CreateByName( IsoString( name ), ra, dec );
   }

   /*!
    * Creates a new projection with data acquired from the specified WCS
    * keywords.
    */
   static ProjectionBase* Create( const WCSKeywords& wcs );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ProjectionFactory_h

// ----------------------------------------------------------------------------
// EOF pcl/ProjectionFactory.h - Released 2018-12-12T09:24:21Z
