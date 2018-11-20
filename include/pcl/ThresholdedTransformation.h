//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/ThresholdedTransformation.h - Released 2018-11-01T11:06:36Z
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

#ifndef __PCL_ThresholdedTransformation_h
#define __PCL_ThresholdedTransformation_h

/// \file pcl/ThresholdedTransformation.h

#include <pcl/Defs.h>

#include <pcl/ImageTransformation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ThresholdedTransformation
 * \brief Thresholded image transformation.
 *
 * %ThresholdedTransformation represents a <em>thresholded image
 * transformation</em> in PCL. A thresholded transformation modifies its effect
 * when applied to pixels within a given range of pixel sample values.
 *
 * Instead of defining a fixed range of sample values, a thresholded
 * transformation defines two \e threshold values in PCL. These thresholds are
 * normalized \e distances measured from a \e floating reference value that can
 * be specified on a per-pixel basis.
 *
 * In PCL, convolutions in the spatial domain and morphological transformations
 * have been implemented as thresholded transformations.
 */
class PCL_CLASS ThresholdedTransformation : public virtual ImageTransformation
{
public:

   /*!
    * Constructs a %ThresholdedTransformation object with the specified \a low
    * and \a high threshold values in the normalized real range [0,1].
    */
   ThresholdedTransformation( double low = 0, double high = 0 ) :
      m_lowThreshold( low ), m_highThreshold( high )
   {
   }

   /*!
    * Copy constructor.
    */
   ThresholdedTransformation( const ThresholdedTransformation& ) = default;

   /*!
    * Destroys a %ThresholdedTransformation object.
    */
   virtual ~ThresholdedTransformation()
   {
   }

   /*!
    * Returns true iff this transformation is currently thresholded.
    *
    * The transformation is thresholded when either threshold (low or high) is
    * nonzero. When both thresholds are zero, the transformation applies no
    * thresholding at all.
    */
   bool IsThresholded() const
   {
      return 1 + m_lowThreshold != 1 || 1 + m_highThreshold != 1;
   }

   /*!
    * Returns the current low threshold in the normalized real range.
    */
   double LowThreshold() const
   {
      return m_lowThreshold;
   }

   /*!
    * Sets the low threshold to the specified sample value \a t in the
    * normalized real range.
    */
   void SetLowThreshold( double t )
   {
      m_lowThreshold = t;
   }

   /*!
    * Returns the current high threshold in the normalized real range.
    */
   double HighThreshold() const
   {
      return m_highThreshold;
   }

   /*!
    * Sets the high threshold to the specified sample value \a t in the
    * normalized real range.
    */
   void SetHighThreshold( double t )
   {
      m_highThreshold = t;
   }

private:

   /*
    * Normalized threshold values.
    */
   double m_lowThreshold = 0;
   double m_highThreshold = 0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ThresholdedTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/ThresholdedTransformation.h - Released 2018-11-01T11:06:36Z
