//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/ProjectionFactory.cpp - Released 2018-11-01T11:06:52Z
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

#include <pcl/GnomonicProjection.h>
#include <pcl/HammerAitoffProjection.h>
#include <pcl/MercatorProjection.h>
#include <pcl/OrthographicProjection.h>
#include <pcl/PlateCarreeProjection.h>
#include <pcl/ProjectionFactory.h>
#include <pcl/ZenithalProjections.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

static ProjectionBase* CreateProjectionByWCSCode( bool& needsInitialization, const IsoString& wcsCode, double ra, double dec )
{
   needsInitialization = true;
   switch ( wcsCode.Trimmed().CaseFolded().Hash32() )
   {
   case 0x5e1ebd44: // "TAN"
      needsInitialization = false;
      return new GnomonicProjection( ra, dec );
   case 0x1cafa6e6: // "STG"
      return new StereographicProjection();
   case 0x9f2b1ebc: // "CAR"
      return new PlateCarreeProjection();
   case 0xecd36fef: // "MER"
      return new MercatorProjection();
   case 0xaa395586: // "AIT"
      return new HammerAitoffProjection();
   case 0x97c16049: // "ZEA"
      return new ZenithalEqualAreaProjection();
   case 0x6e069940: // "SIN"
      return new OrthographicProjection();
   default:
      throw Error( "ProjectionFactory: Invalid/unsupported WCS projection code \'" + wcsCode + '\'' );
   }
}

ProjectionBase* ProjectionFactory::CreateByWCSCode( const IsoString& code, double ra, double dec )
{
   bool needsInitialization;
   ProjectionBase* projection = CreateProjectionByWCSCode( needsInitialization, code, ra, dec );
   if ( needsInitialization )
      projection->InitFromRefPoint( ra, dec );
   return projection;
}

// ----------------------------------------------------------------------------

static ProjectionBase* CreateProjectionByName( bool& needsInitialization, const IsoString& name, double ra, double dec )
{
   needsInitialization = true;
   switch ( name.Trimmed().CaseFolded().Hash32() )
   {
   case 0xd91ba2fe: // Gnomonic
      needsInitialization = false;
      return new GnomonicProjection( ra, dec );
   case 0xf75bc292: // Stereographic
      return new StereographicProjection();
   case 0xcf20742f: // PlateCarree
      return new PlateCarreeProjection();
   case 0xde9d75b3: // Mercator
      return new MercatorProjection();
   case 0x164deada: // HammerAitoff
      return new HammerAitoffProjection();
   case 0xdee97130: // ZenithalEqualArea
      return new ZenithalEqualAreaProjection();
   case 0x9463e32f: // Orthographic
      return new OrthographicProjection();
   default:
      throw Error( "ProjectionFactory: Invalid/unsupported projection name \'" + name + '\'' );
   }
}

ProjectionBase* ProjectionFactory::CreateByName( const IsoString& name, double ra, double dec )
{
   bool needsInitialization;
   ProjectionBase* projection = CreateProjectionByName( needsInitialization, name, ra, dec );
   if ( needsInitialization )
      projection->InitFromRefPoint( ra, dec );
   return projection;
}

// ----------------------------------------------------------------------------

ProjectionBase* ProjectionFactory::Create( const WCSKeywords& wcs )
{
   IsoString ptype1 = wcs.ctype1.Substring( 5, 3 );
   IsoString ptype2 = wcs.ctype2.Substring( 5, 3 );
   if ( ptype1 != ptype2 )
      throw Error( "ProjectionFactory::Create(): Unsupported WCS coordinates: Axes with different projections." );
   bool needsInitialization;
   ProjectionBase* projection = CreateProjectionByWCSCode( needsInitialization, ptype1, wcs.crval1, wcs.crval2 );
   if ( needsInitialization )
      projection->InitFromWCS( wcs );
   return projection;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ProjectionFactory.cpp - Released 2018-11-01T11:06:52Z
