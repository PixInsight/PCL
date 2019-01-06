//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/AdaptiveLocalFilter.h - Released 2018-12-12T09:24:21Z
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

#ifndef __PCL_AdaptiveLocalFilter_h
#define __PCL_AdaptiveLocalFilter_h

/// \file pcl/AdaptiveLocalFilter.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ImageTransformation.h>
#include <pcl/ParallelProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class AdaptiveLocalFilter
 * \brief Adaptive, local noise reduction filter in the spatial domain.
 *
 * Implementation of the adaptive, local noise reduction filter in the spatial
 * domain as described by González and Woods.
 *
 * <b>References</b>
 *
 * R.C. González and R.E. Woods, <em>Digital %Image Processing, Third
 * Edition</em>, Pearson / Prentice Hall, 2008. pp 330-332.
 */
class PCL_CLASS AdaptiveLocalFilter : public ImageTransformation,
                                      public ParallelProcess
{
public:

   /*!
    * Constructs an %AdaptiveLocalFilter instance.
    *
    * \param sigma   Standard deviation or median absolute deviation from the
    *                median (MAD) of the noise (see the \a useMAD parameter).
    *                If the MAD is used, the specified \a sigma value must be
    *                consistent with the standard deviation of a normal
    *                distribution (typically, the computed MAD estimate must be
    *                multiplied by 1.4826).
    *
    * \param size    Filter size in pixels. This is the length of a side of the
    *                square pixel neighborhood used by the adaptive filter. The
    *                filter size must be an odd number greater than or equal to
    *                three. If an even size is specified, the smallest odd size
    *                greater than the specified value will be used. The default
    *                size is five pixels.
    *
    * \param useMAD  If true, use the median absolute deviation from the median
    *                (MAD) instead of variance / standard deviation for noise
    *                estimates. Note that this changes the meaning of the
    *                \a sigma parameter. The default value is false (variance
    *                is used by default).
    */
   AdaptiveLocalFilter( double sigma, int size = 5, bool useMAD = false ) :
      m_size( Max( 3, size|1 ) ),
      m_sigma( Max( 0.0, sigma ) ),
      m_useMAD( useMAD )
   {
   }

   /*!
    * Copy constructor.
    */
   AdaptiveLocalFilter( const AdaptiveLocalFilter& ) = default;

   /*!
    * Destroys this %AdaptiveLocalFilter object.
    */
   virtual ~AdaptiveLocalFilter()
   {
   }

   /*!
    * Returns the filter size in pixels.
    */
   int Size() const
   {
      return m_size;
   }

   /*!
    * Sets the filter \a size in pixels.
    */
   void SetSize( int size )
   {
      PCL_PRECONDITION( size >= 3 )
      PCL_PRECONDITION( (size & 1) != 0 )
      m_size = Max( 3, size|1 );
   }

   /*!
    * Returns the standard deviation or median absolute deviation from the
    * median (MAD) of the noise in this adaptive filter.
    *
    * The meaning of the returned value depends on whether this filter uses
    * variance or MAD for noise estimates --see the UsingMAD() member function
    * and the class constructor for more information.
    */
   double Sigma() const
   {
      return m_sigma;
   }

   /*!
    * Sets the standard deviation or median absolute deviation from the median
    * (MAD) of the noise in this adaptive filter. The specified \a sigma must
    * be a positive, nonzero value (a zero noise value is legal, but the filter
    * will obviously have no effect).
    *
    * The meaning of the specified \a sigma value depends on whether this
    * filter uses variance or MAD for noise estimates--see the UsingMAD()
    * member function and the class constructor for more information.
    */
   void SetSigma( double sigma )
   {
      PCL_PRECONDITION( sigma >= 0 )
      m_sigma = Max( 0.0, sigma );
   }

   /*!
    * Returns true iff this adaptive filter uses median absolute deviation (MAD)
    * instead of variance / standard deviation to interpret noise estimates.
    */
   bool UsingMAD() const
   {
      return m_useMAD;
   }

   /*!
    * Sets the way this adaptive filter interprets noise estimates: either as
    * the median absolute deviation from the median (MAD) or as the standard
    * deviation of the noise in the target image.
    */
   void UseMAD( bool useMAD = true )
   {
      m_useMAD = useMAD;
   }

protected:

   int    m_size;            // filter size in pixels.
   double m_sigma = 5;       // standard deviation of the noise in the target image.
   bool   m_useMAD = false;  // use MAD instead of variance / standard deviation.

   /*
    * Adaptive local filter in the spatial domain.
    */
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_AdaptiveLocalFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/AdaptiveLocalFilter.h - Released 2018-12-12T09:24:21Z
