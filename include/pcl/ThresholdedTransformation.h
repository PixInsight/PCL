// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/ThresholdedTransformation.h - Released 2014/10/29 07:34:07 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef __PCL_ThresholdedTransformation_h
#define __PCL_ThresholdedTransformation_h

/// \file pcl/ThresholdedTransformation.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_ImageTransformation_h
#include <pcl/ImageTransformation.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ThresholdedTransformation
 * \brief Thresholded image transformation.
 *
 * %ThresholdedTransformation represents a <em>thresholded image
 * transformation</em> in the PCL. A thresholded transformation modifies its
 * effect when applied to pixels within a given range of pixel sample values.
 *
 * Instead of defining a fixed range of sample values, a thresholded
 * transformation defines two \e threshold values in the PCL. These thresholds
 * are normalized \e distances measured from a \e floating reference value that
 * can be specified on a per-pixel basis.
 *
 * In the PCL, convolutions in the spatial domain and morphological
 * transformations have been implemented as thresholded transformations.
 */
class PCL_CLASS ThresholdedTransformation : public virtual ImageTransformation
{
public:

   /*!
    * Constructs a %ThresholdedTransformation object with the specified low and
    * high threshold values, \a th0 and \a th1 respectively, in the normalized
    * real range [0,1].
    */
   ThresholdedTransformation( double t0 = 0, double t1 = 0 ) :
   ImageTransformation(), th0( t0 ), th1( t1 )
   {
   }

   /*!
    * Constructs a %ThresholdedTransformation object as a copy of an existing
    * instance.
    */
   ThresholdedTransformation( const ThresholdedTransformation& x ) :
   ImageTransformation( x ), th0( x.th0 ), th1( x.th1 )
   {
   }

   /*!
    * Destroys a %ThresholdedTransformation object.
    */
   virtual ~ThresholdedTransformation()
   {
   }

   /*!
    * Assigns an existing %ThresholdedTransformation instance to this object.
    * Returns a reference to this object.
    */
   ThresholdedTransformation& operator =( const ThresholdedTransformation& x )
   {
      th0 = x.th0; th1 = x.th1;
      return *this;
   }

   /*!
    * Returns true if this transformation is currently thresholded.
    *
    * The transformation is thresholded when either threshold (low or high) is
    * nonzero. When both thresholds are zero, the transformation applies no
    * thresholding at all.
    */
   bool IsThresholded() const
   {
      return 1 + th0 != 1 || 1 + th1 != 1;
   }

   /*!
    * Returns the current low threshold in the normalized real range.
    */
   double LowThreshold() const
   {
      return th0;
   }

   /*!
    * Sets the low threshold to the specified sample value \a t in the
    * normalized real range.
    */
   void SetLowThreshold( double t )
   {
      th0 = t;
   }

   /*!
    * Returns the current high threshold in the normalized real range.
    */
   double HighThreshold() const
   {
      return th1;
   }

   /*!
    * Sets the high threshold to the specified sample value \a t in the
    * normalized real range.
    */
   void SetHighThreshold( double t )
   {
      th1 = t;
   }

private:

   /*
    * Low and high threshold values.
    */
   double th0;
   double th1;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ThresholdedTransformation_h

// ****************************************************************************
// EOF pcl/ThresholdedTransformation.h - Released 2014/10/29 07:34:07 UTC
