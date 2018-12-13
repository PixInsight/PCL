//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/ZenithalProjections.h - Released 2018-12-12T09:24:21Z
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

#ifndef __PCL_ZenithalProjections_h
#define __PCL_ZenithalProjections_h

/// \file pcl/ZenithalProjections.h

#include <pcl/Defs.h>

#include <pcl/ProjectionBase.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ZenithalProjectionBase
 * \brief Base class of zenithal projection systems
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS ZenithalProjectionBase : public ProjectionBase
{
protected:

   /*!
    * Default constructor.
    */
   ZenithalProjectionBase()
   {
      m_theta0 = 90;
   }

   /*!
    * Copy constructor.
    */
   ZenithalProjectionBase( const ZenithalProjectionBase& ) = default;

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   virtual ProjectionBase* Clone() const override = 0;

   /*!
    * Returns the WCS projection identifier for this projection system.
    */
   virtual IsoString ProjCode() const override = 0;

   /*!
    * Returns the readable name of this projection system.
    */
   virtual IsoString Name() const override = 0;

protected:

   /*!
    * Transforms from world coordinates to native spherical coordinates.
    */
   bool Project( DPoint& pW, const DPoint& pN ) const noexcept override
   {
      double rTheta = GetRTheta( pN );
      double sinTheta, cosTheta;
      SinCos( Rad( pN.x ), sinTheta, cosTheta );
      pW.x =  rTheta * sinTheta;
      pW.y = -rTheta * cosTheta;
      return true;
   }

   /*!
    * Transforms from native spherical coordinates to world coordinates.
    */
   bool Unproject( DPoint& pN, const DPoint& pW ) const noexcept override
   {
      pN.x = Deg( ArcTan( pW.x, -pW.y ) );
      pN.y = GetTheta( Sqrt( pW.x*pW.x + pW.y*pW.y ) );
      return true;
   }

   virtual double GetRTheta( const DPoint& np ) const = 0;
   virtual double GetTheta( double rTheta ) const = 0;
};

// ----------------------------------------------------------------------------

/*!
 * \class ZenithalEqualAreaProjection
 * \brief Zenithal equal-area projection system
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS ZenithalEqualAreaProjection: public ZenithalProjectionBase
{
public:

   /*!
    * Default constructor.
    */
   ZenithalEqualAreaProjection() = default;

   /*!
    * Copy constructor.
    */
   ZenithalEqualAreaProjection( const ZenithalEqualAreaProjection& ) = default;

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   ProjectionBase* Clone() const override
   {
      return new ZenithalEqualAreaProjection( *this );
   }

   /*!
    * Returns the WCS projection identifier for this projection system.
    */
   IsoString ProjCode() const override
   {
      return "ZEA";
   }

   /*!
    * Returns the readable name of this projection system.
    */
   IsoString Name() const override
   {
      return "Zenithal Equal Area";
   }

   /*!
    *
    */
   bool CheckBrokenLine( const DPoint& cp1, const DPoint& cp2 ) const noexcept override
   {
      DPoint np1 = m_sph.CelestialToNative( cp1 );
      DPoint np2 = m_sph.CelestialToNative( cp2 );
      return Min( Mod( Abs( np1.x - np2.x - 360 ), 360.0 ),
                  Mod( Abs( np1.x - np2.x + 360 ), 360.0 ) ) < 180*Sin( Rad( 45 + (np1.y + np2.y)/2/2 ) );
   }

private:

   double GetRTheta( const DPoint& np ) const override
   {
      return 2 * Deg( Sin( Rad( (90 - np.y)/2 ) ) );
   }

   double GetTheta( double rTheta ) const override
   {
      return 90 - 2*Deg( ArcSin( Rad( rTheta )/2 ) );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class StereographicProjection
 * \brief Stereographic projection system
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS StereographicProjection: public ZenithalProjectionBase
{
public:

   /*!
    * Default constructor.
    */
   StereographicProjection() = default;

   /*!
    * Copy constructor.
    */
   StereographicProjection( const StereographicProjection& ) = default;

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   ProjectionBase* Clone() const override
   {
      return new StereographicProjection( *this );
   }

   /*!
    * Returns the WCS projection identifier for this projection system.
    */
   IsoString ProjCode() const override
   {
      return "STG";
   }

   /*!
    * Returns the readable name of this projection system.
    */
   IsoString Name() const override
   {
      return "Stereographic";
   }

   /*!
    *
    */
   bool CheckBrokenLine(const DPoint& cp1, const DPoint& cp2) const noexcept override
   {
      return true;
   }

private:

   double GetRTheta( const DPoint& np ) const override
   {
      return 2 * Deg( Tan( Rad( (90 - np.y)/2 ) ) );
   }

   double GetTheta( double rTheta ) const override
   {
      return 90 - 2*Deg( ArcTan( Rad( rTheta )/2 ) );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ZenithalProjections_h

// ----------------------------------------------------------------------------
// EOF pcl/ZenithalProjections.h - Released 2018-12-12T09:24:21Z
