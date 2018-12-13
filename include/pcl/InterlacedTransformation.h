//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/InterlacedTransformation.h - Released 2018-12-12T09:24:21Z
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

#ifndef __PCL_InterlacedTransformation_h
#define __PCL_InterlacedTransformation_h

/// \file pcl/InterlacedTransformation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ImageTransformation.h>
#include <pcl/Utility.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class InterlacedTransformation
 * \brief Interlaced image transformation in the spatial domain.
 *
 * %InterlacedTransformation represents an <em>interlaced image
 * transformation</em> in PCL. An interlaced transformation can be applied to a
 * subset of the pixels in a target image by acting exclusively on selected
 * rows and columns of pixels, distributed at regular intervals.
 *
 * The distance between two consecutive rows or columns of transformed pixels
 * is the <em>interlacing distance</em> that characterizes an interlaced
 * transformation. Intermediate pixels falling between selected rows or columns
 * are ignored during the transformation.
 *
 * In PCL, convolutions in the spatial domain and morphological transformations
 * have been implemented as interlaced transformations. A good example of
 * interlaced image transformation algorithm is the starlet transform (see the
 * StarletTransform class), which uses interlaced convolutions to perform a
 * multiscale decomposition.
 */
class PCL_CLASS InterlacedTransformation : public virtual ImageTransformation
{
public:

   /*!
    * Constructs an %InterlacedTransformation object with the specified
    * <em>interlacing distance</em> \a n > 0 in pixels.
    */
   InterlacedTransformation( int n = 1 ) :
      m_distance( pcl::Max( 1, n ) )
   {
      PCL_PRECONDITION( n > 0 )
   }

   /*!
    * Copy constructor.
    */
   InterlacedTransformation( const InterlacedTransformation& ) = default;

   /*!
    * Destroys an %InterlacedTransformation object.
    */
   virtual ~InterlacedTransformation()
   {
   }

   /*!
    * Returns true iff this transformation is currently interlaced.
    *
    * The transformation is interlaced if the current interlacing distance is
    * greater than one pixel.
    *
    * \sa InterlacingDistance()
    */
   bool IsInterlaced() const
   {
      return m_distance > 1;
   }

   /*!
    * Returns the current interlacing distance of this transformation in
    * pixels.
    *
    * \sa SetInterlacingDistance(), IsInterlaced()
    */
   int InterlacingDistance() const
   {
      return m_distance;
   }

   /*!
    * Sets the current interlacing distance of this transformation to the
    * specified value \a n > 0 in pixels.
    *
    * The interlacing distance is the distance in pixels between two
    * consecutive rows or columns of transformed pixels. Intermediate pixels
    * falling between selected rows or columns are ignored.
    *
    * \sa InterlacingDistance(), DisableInterlacing(), IsInterlaced()
    */
   void SetInterlacingDistance( int n )
   {
      PCL_PRECONDITION( n > 0 )
      m_distance = pcl::Max( 1, n );
   }

   /*!
    * Disables interlacing for this transformation.
    *
    * This is a convenience function, equivalent to:
    *
    * \code SetInterlacingDistance( 1 ); \endcode
    */
   void DisableInterlacing()
   {
      SetInterlacingDistance( 1 );
   }

private:

   /*
    * Interlacing distance in pixels
    */
   int m_distance = 1;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_InterlacedTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/InterlacedTransformation.h - Released 2018-12-12T09:24:21Z
