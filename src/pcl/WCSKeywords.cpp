//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/WCSKeywords.cpp - Released 2019-01-21T12:06:21Z
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

#include <pcl/LinearTransformation.h>
#include <pcl/TimePoint.h>
#include <pcl/WCSKeywords.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

void WCSKeywords::Read( const FITSKeywordArray& keywords )
{
   for ( const FITSHeaderKeyword& key : keywords )
   {
      IsoString svalue = key.StripValueDelimiters();
      double nvalue;
      if ( key.name == "OBJCTRA" )
      {
         if ( svalue.TrySexagesimalToDouble( nvalue, Array<char>() << ' ' << ':' ) )
            if ( nvalue >= 0 )
            {
               nvalue *= 15;
               if ( nvalue <= 360 )
               {
                  if ( nvalue == 360 )
                     nvalue = 0;
                  objctra = nvalue;
               }
            }
      }
      else if ( key.name == "OBJCTDEC" )
      {
         if ( svalue.TrySexagesimalToDouble( nvalue, Array<char>() << ' ' << ':' ) )
            if ( nvalue >= -90 )
               if ( nvalue <= +90 )
                  objctdec = nvalue;
      }
      else if ( key.name == "FOCALLEN" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            focallen = nvalue;
      }
      else if ( key.name == "XPIXSZ" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            xpixsz = nvalue;
      }
      else if ( key.name == "DATE-OBS" )
      {
         TimePoint t;
         if ( TimePoint::TryFromString( t, svalue ) )
            dateobs = t.JD();
      }
      else if ( key.name == "CTYPE1" )
      {
         ctype1 = svalue;
      }
      else if ( key.name == "CTYPE2" )
      {
         ctype2 = svalue;
      }
      else if ( key.name == "CRVAL1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            crval1 = nvalue;
      }
      else if ( key.name == "CRVAL2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            crval2 = nvalue;
      }
      else if ( key.name == "CRPIX1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            crpix1 = nvalue;
      }
      else if ( key.name == "CRPIX2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            crpix2 = nvalue;
      }
      else if ( key.name == "CD1_1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            cd1_1 = nvalue;
      }
      else if ( key.name == "CD1_2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            cd1_2 = nvalue;
      }
      else if ( key.name == "CD2_1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            cd2_1 = nvalue;
      }
      else if ( key.name == "CD2_2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            cd2_2 = nvalue;
      }
      else if ( key.name == "CDELT1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            cdelt1 = nvalue;
      }
      else if ( key.name == "CDELT2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            cdelt2 = nvalue;
      }
      else if ( key.name == "CROTA1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            crota1 = nvalue;
      }
      else if ( key.name == "CROTA2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            crota2 = nvalue;
      }
      else if ( key.name == "PV1_1" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            pv1_1 = nvalue;
      }
      else if ( key.name == "PV1_2" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            pv1_2 = nvalue;
      }
      else if ( key.name == "PV1_3" || key.name == "LONPOLE" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            lonpole = nvalue;
      }
      else if ( key.name == "PV1_4" || key.name == "LATPOLE" )
      {
         if ( svalue.TryToDouble( nvalue ) )
            latpole = nvalue;
      }
      else if ( key.name == "REFSPLIN" || key.name == "REFSPLINE" )
      {
         // N.B. 9-char keyword name "REFSPLINE" written by old versions, not FITS-compliant.
         refSpline = svalue;
      }
   }
}

// ----------------------------------------------------------------------------

bool WCSKeywords::ExtractWorldTransformation( LinearTransformation& transIW, int imageHeight )
{
   /*
    * Transform pixel coordinates in FITS convention to World coordinates.
    */
   LinearTransformation transFW;
   if ( cd1_1.IsDefined() && cd1_2.IsDefined() && cd2_1.IsDefined() && cd2_2.IsDefined() )
   {
      transFW = LinearTransformation( cd1_1(), cd1_2(), -cd1_1()*crpix1() - cd1_2()*crpix2(),
                                      cd2_1(), cd2_2(), -cd2_1()*crpix1() - cd2_2()*crpix2() );
   }
   else if ( cdelt1.IsDefined() && cdelt2.IsDefined() )
   {
      if ( !crota2.IsDefined() )
         crota2 = 0;
      double sinr, cosr;
      SinCos( Rad( crota2() ), sinr, cosr );
      double cd1_1 =  cdelt1()*cosr;
      double cd1_2 = -cdelt2()*sinr;
      double cd2_1 =  cdelt1()*sinr;
      double cd2_2 =  cdelt2()*cosr;
      transFW = LinearTransformation( cd1_1, cd1_2, -cd1_1*crpix1() - cd1_2*crpix2(),
                                      cd2_1, cd2_2, -cd2_1*crpix1() - cd2_2*crpix2() );
   }
   else
      return false;

   /*
    * Transforms pixel coordinates between FITS and PixInsight conventions.
    */
   LinearTransformation ref_F_I( 1,  0,              -0.5,
                                 0, -1, imageHeight + 0.5 );
   transIW = transFW.Multiply( ref_F_I.Inverse() );
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/WCSKeywords.cpp - Released 2019-01-21T12:06:21Z
