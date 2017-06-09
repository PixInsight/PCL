//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0837
// ----------------------------------------------------------------------------
// pcl/MercatorProjection.h - Released 2017-06-09T08:12:42Z
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

#ifndef __PCL_MercatorProjection_h
#define __PCL_MercatorProjection_h

/// \file pcl/MercatorProjection.h

#include <pcl/Defs.h>

#include <pcl/ProjectionBase.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MercatorProjection
 * \brief Mercator projection system
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS MercatorProjection : public ProjectionBase
{
public:

   /*!
    * Default constructor.
    */
   MercatorProjection() = default;

   /*!
    * Copy constructor.
    */
   MercatorProjection( const MercatorProjection& ) = default;

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   virtual ProjectionBase* Clone() const
   {
      return new MercatorProjection( *this );
   }

   /*!
    * Returns the WCS projection identifier for this projection system.
    */
   virtual IsoString ProjCode() const
   {
      return "MER";
   }

   /*!
    * Returns the readable name of this projection system.
    */
   virtual IsoString Name() const
   {
      return "Mercator";
   }

protected:

   virtual bool Project( DPoint& pW, const DPoint& pN ) const noexcept;

   virtual bool Unproject( DPoint& pN, const DPoint& pW ) const noexcept;

private:

   double m_r0 = Const<double>::deg();
   double m_x0 = 0;
   double m_y0 = 0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MercatorProjection_h

// ----------------------------------------------------------------------------
// EOF pcl/MercatorProjection.h - Released 2017-06-09T08:12:42Z
