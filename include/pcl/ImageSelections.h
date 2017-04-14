//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/ImageSelections.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_ImageSelections_h
#define __PCL_ImageSelections_h

/// \file pcl/ImageSelections.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Array.h>
#include <pcl/Point.h>
#include <pcl/Rectangle.h>
#include <pcl/StatusMonitor.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \struct ImageSelections
 * \brief A structure used to store rectangular image selections, channel
 *        ranges, anchor points, and clipping ranges.
 *
 * %ImageSelections Defines a rectangular area, a channel range and an anchor
 * point that can be used as a selection for 2D images.
 *
 * %ImageSelections also defines a clipping range in the normalized [0,1] range.
 * Range clipping is used for calculation of statistics such as mean, standard
 * deviation, average deviation, etc.
 */
struct PCL_CLASS ImageSelections
{
   int    channel;       //!< First selected channel
   int    lastChannel;   //!< Last selected channel
   Point  point;         //!< Current anchor point
   Rect   rectangle;     //!< Current rectangular selection
   double clipLow;       //!< Clipping range, lower bound
   double clipHigh;      //!< Clipping range, upper bound
   bool   clipped : 1;   //!< Use range clipping

   /*!
    * Constructs a default %ImageSelections instance:
    *
    * \li Only the first channel selected: channel = lastChannel = 0.
    * \li Empty rectangular selection: rectangle = Rect( 0 ).
    * \li Anchor point at the origin of image coordinates: point = Point( 0 ).
    * \li Clipping range lower bound = 0.0
    * \li Clipping range upper bound = 1.0
    * \li Range clipping disabled
    */
   ImageSelections() :
   channel( 0 ), lastChannel( 0 ), point( 0 ), rectangle( 0 ), clipLow( 0 ), clipHigh( 1 ), clipped( false )
   {
   }

   /*!
    * Copy constructor.
    */
   ImageSelections( const ImageSelections& x ) :
   channel( x.channel ), lastChannel( x.lastChannel ), point( x.point ), rectangle( x.rectangle ),
   clipLow( x.clipLow ), clipHigh( x.clipHigh ), clipped( x.clipped )
   {
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   ImageSelections& operator =( const ImageSelections& x )
   {
      channel = x.channel;
      lastChannel = x.lastChannel;
      point = x.point;
      rectangle = x.rectangle;
      clipLow = x.clipLow;
      clipHigh = x.clipHigh;
      clipped = x.clipped;
      return *this;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageSelections_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageSelections.h - Released 2017-04-14T23:04:40Z
