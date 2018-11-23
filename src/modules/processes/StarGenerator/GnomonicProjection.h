//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0312
// ----------------------------------------------------------------------------
// GnomonicProjection.h - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarGenerator PixInsight module.
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

#ifndef __GnomonicProjection_h
#define __GnomonicProjection_h

#include "ProjectionSystem.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class GnomonicProjection : public ProjectionSystem
{
public:

   GnomonicProjection( double lon, double lat ) : ProjectionSystem( lon, lat )
   {
      SinCos( lat0, s0, c0 );
   }

   virtual ~GnomonicProjection()
   {
   }

   virtual String Name() const
   {
      return "Gnomonic Projection";
   }

   virtual void SphericalToRectangular( double& x, double& y, double lon, double lat ) const
   {
      double sdl, cdl; SinCos( InPiRange( lon - lon0 ), sdl, cdl );
      double s1, c1; SinCos( lat, s1, c1 );
      double cc = s0*s1 + c0*c1*cdl;
      x = c1*sdl/cc;
      y = (c0*s1 - s0*c1*cdl)/cc;
   }

   virtual void RectangularToSpherical( double& lon, double& lat, double x, double y ) const
   {
      double rho = Sqrt( x*x + y*y );
      if ( rho == 0 )
      {
         lon = lon0;
         lat = lat0;
      }
      else
      {
         double sc, cc; SinCos( ArcTan( rho ), sc, cc );
         lon = In2PiRange( lon0 + ArcTan( x*sc, rho*c0*cc - y*s0*sc ) );
         lat = ArcSin( cc*s0 + y*sc*c0/rho );
      }
   }

private:

   double s0, c0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GnomonicProjection_h

// ----------------------------------------------------------------------------
// EOF GnomonicProjection.h - Released 2018-11-23T18:45:58Z
