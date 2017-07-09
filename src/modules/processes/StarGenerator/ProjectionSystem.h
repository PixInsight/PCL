//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0285
// ----------------------------------------------------------------------------
// ProjectionSystem.h - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarGenerator PixInsight module.
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

#ifndef __ProjectionSystem_h
#define __ProjectionSystem_h

#include <pcl/Math.h>
#include <pcl/Constants.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

template <typename T> inline T InPiRange( T x )
{
   while ( x > Const<T>::pi() )
      x -= Const<T>::_2pi();
   while ( x < -Const<T>::pi() )
      x += Const<T>::_2pi();
   return x;
}

template <typename T> inline T In2PiRange( T x )
{
   if ( x > Const<T>::_2pi() )
   {
      do
         x -= Const<T>::_2pi();
      while ( x > Const<T>::_2pi() );
   }
   else if ( x < 0 )
   {
      do
         x += Const<T>::_2pi();
      while ( x < 0 );
   }
   return x;
}

// ----------------------------------------------------------------------------

class ProjectionSystem
{
public:

   ProjectionSystem( double lon, double lat ) : lon0( In2PiRange( lon ) ), lat0( lat )
   {
   }

   virtual ~ProjectionSystem()
   {
   }

   virtual bool IsConicProjection() const
   {
      return false;
   }

   virtual bool IsCylindricalProjection() const
   {
      return false;
   }

   virtual bool IsConformalProjection() const
   {
      return false;
   }

   virtual bool IsEqualAreaProjection() const
   {
      return false;
   }

   virtual String Name() const = 0;

   virtual void SphericalToRectangular( double& x, double& y, double lon, double lat ) const = 0;
   virtual void RectangularToSpherical( double& lon, double& lat, double x, double y ) const = 0;

   double CenterLongitude() const
   {
      return lon0;
   }

   double CenterLatitude() const
   {
      return lat0;
   }

protected:

   double lon0; // Center of projection, longitude in radians, [0,2pi[
   double lat0; // Center of projection, latitude in radians, [-pi/2,+pi/2]
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ProjectionSystem_h

// ----------------------------------------------------------------------------
// EOF ProjectionSystem.h - Released 2017-07-09T18:07:33Z
