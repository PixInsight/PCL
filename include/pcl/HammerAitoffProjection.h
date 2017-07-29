//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/HammerAitoffProjection.h - Released 2017-06-28T11:58:36Z
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

#ifndef __PCL_HammerAitoffProjection_h
#define __PCL_HammerAitoffProjection_h

/// \file pcl/HammerAitoffProjection.h

#include <pcl/Defs.h>

#include <pcl/ProjectionBase.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class HammerAitoffProjection
 * \brief Hammer-Aitoff projection system
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS HammerAitoffProjection : public ProjectionBase
{
public:

   /*!
    * Default constructor.
    */
   HammerAitoffProjection() = default;

   /*!
    * Copy constructor.
    */
   HammerAitoffProjection( const HammerAitoffProjection& ) = default;

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   virtual ProjectionBase* Clone() const
   {
      return new HammerAitoffProjection( *this );
   }

   /*!
    * Returns the WCS projection identifier for this projection system.
    */
   virtual IsoString ProjCode() const
   {
      return "AIT";
   }

   /*!
    * Returns the readable name of this projection system.
    */
   virtual IsoString Name() const
   {
      return "Hammer-Aitoff";
   }

protected:

   virtual bool Project( DPoint& pW, const DPoint& pN ) const noexcept;

   virtual bool Unproject( DPoint& pN, const DPoint& pW ) const noexcept;

private:

   double m_Zmin = Const<double>::_1_sqrt2();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_HammerAitoffProjection_h

// ----------------------------------------------------------------------------
// EOF pcl/HammerAitoffProjection.h - Released 2017-06-28T11:58:36Z
