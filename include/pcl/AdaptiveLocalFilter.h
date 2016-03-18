//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/AdaptiveLocalFilter.h - Released 2016/02/21 20:22:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_ImageTransformation_h
#include <pcl/ImageTransformation.h>
#endif

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
class PCL_CLASS AdaptiveLocalFilter : public ImageTransformation
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
      ImageTransformation(),
      m_size( Max( 3, size|1 ) ), m_sigma( Max( 0.0, sigma ) ), m_useMAD( useMAD ),
      m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
   {
   }

   /*!
    * Copy constructor.
    */
   AdaptiveLocalFilter( const AdaptiveLocalFilter& x ) :
      ImageTransformation( x ),
      m_size( x.m_size ), m_sigma( x.m_sigma ), m_useMAD( x.m_useMAD ),
      m_parallel( x.m_parallel ), m_maxProcessors( x.m_maxProcessors )
   {
   }

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

   /*!
    * Returns true iff this object is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this instance of %AdaptiveLocalFilter.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance of %AdaptiveLocalFilter. If \a enable is false
    *                this parameter is ignored. A value <= 0 is ignored. The
    *                default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( enable && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this instance of %AdaptiveLocalFilter.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this instance of
    * %AdaptiveLocalFilter.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   int MaxProcessors() const
   {
      return m_maxProcessors;
   }

   /*!
    * Sets the maximum number of processors allowed for this instance of
    * %AdaptiveLocalFilter.
    *
    * In the current version of PCL, a module can use a maximum of 1023
    * processors. The term \e processor actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   void SetMaxProcessors( int maxProcessors )
   {
      m_maxProcessors = unsigned( Range( maxProcessors, 1, PCL_MAX_PROCESSORS ) );
   }

protected:

   int      m_size;              // Filter size in pixels.
   double   m_sigma;             // Standard deviation of the noise in the target image.
   bool     m_useMAD        : 1; // Use MAD instead of variance / standard deviation.
   bool     m_parallel      : 1; // Use multiple execution threads.
   unsigned m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT; // Maximum number of processors allowed

   /*
    * Adaptive local filter in the spatial domain.
    */
   virtual void Apply( pcl::Image& ) const;
   virtual void Apply( pcl::DImage& ) const;
   virtual void Apply( pcl::UInt8Image& ) const;
   virtual void Apply( pcl::UInt16Image& ) const;
   virtual void Apply( pcl::UInt32Image& ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_AdaptiveLocalFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/AdaptiveLocalFilter.h - Released 2016/02/21 20:22:12 UTC
