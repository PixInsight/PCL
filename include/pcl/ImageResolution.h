//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/ImageResolution.h - Released 2018-12-12T09:24:21Z
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

#ifndef __PCL_ImageResolution_h
#define __PCL_ImageResolution_h

/// \file pcl/ImageResolution.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ImageResolution
 * \brief %Image resolution data
 *
 * ### TODO: Write a detailed description for %ImageResolution.
 */
class PCL_CLASS ImageResolution
{
public:

   /*!
    * Constructs a default %ImageResolution object.
    */
   ImageResolution() : xResolution( 72 ), yResolution( 72 ), metric( false )
   {
   }

   /*!
    * Copy constructor.
    */
   ImageResolution( const ImageResolution& x )
   {
      (void)operator =( x );
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   ImageResolution& operator =( const ImageResolution& x )
   {
      xResolution = x.xResolution;
      yResolution = x.yResolution;
      metric      = x.metric;
      return *this;
   }

   /*!
    * Gets resolution values for both \a x and \a y axes in pixels per
    * resolution unit.
    *
    * \sa HorizontalResolution(), VerticalResolution()
    */
   void GetResolution( double& x, double& y )
   {
      x = xResolution; y = yResolution;
   }

   /*!
    * Returns the horizontal (X-axis) resolution in pixels per resolution unit.
    *
    * \sa VerticalResolution(), GetResolution()
    */
   double HorizontalResolution() const
   {
      return xResolution;
   }

   /*!
    * Returns the vertical (Y-axis) resolution in pixels per resolution unit.
    *
    * \sa HorizontalResolution(), GetResolution()
    */
   double VerticalResolution() const
   {
      return yResolution;
   }

   /*!
    * Sets resolution values for both \a x and \a y axes in pixels per
    * resolution unit.
    *
    * \sa GetResolution(), SetResolution( double )
    */
   void SetResolution( double x, double y )
   {
      PCL_PRECONDITION( x >= 0 )
      PCL_PRECONDITION( y >= 0 )
      xResolution = pcl::Max( 0.0, x ); yResolution = pcl::Max( 0.0, y );
   }

   /*!
    * Returns the resolution for both axes, in pixels per resolution unit.
    *
    * This member function is more meaningful for contexts where separate
    * horizontal and vertical resolutions are not supported.
    *
    * \sa GetResolution(), SetResolution( double ), SetResolution( double, double )
    */
   double Resolution() const
   {
      return xResolution;
   }

   /*!
    * Sets the resolution for both axes, in pixels per resolution unit.
    *
    * This member function is more meaningful for contexts where separate
    * horizontal and vertical resolutions are not supported.
    *
    * \sa Resolution(), HorizontalResolution(), VerticalResolution(), GetResolution()
    */
   void SetResolution( double r )
   {
      PCL_PRECONDITION( r >= 0 )
      xResolution = yResolution = pcl::Max( 0.0, r );
   }

   /*!
    * Returns true iff this object express resolution in pixels per centimeter;
    * false if it express resolution in pixels per inch.
    *
    * \sa SetMetricResolution()
    */
   bool IsMetricResolution() const
   {
      return metric;
   }

   /*!
    * Sets the resolution unit for this %ImageResolution object.
    *
    * \param m    If true, enables resolution in pixels per centimeter (metric
    *             resolution). If false, enables resolution in pixels per inch.
    *
    * \sa IsMetricResolution()
    */
   void SetMetricResolution( bool m )
   {
      metric = m;
   }

protected:

   /*
    * Resolution in pixels per resolution unit (centimeters or inches).
    */
   double xResolution;
   double yResolution;

   /*
    * Flag true when resolution values are expressed in pixels per centimeter.
    * Pixels per inch otherwise.
    */
   bool   metric;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageResolution_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageResolution.h - Released 2018-12-12T09:24:21Z
