//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/SeparableMedianFilter.h - Released 2015/07/30 17:15:18 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_SeparableMedianFilter_h
#define __PCL_SeparableMedianFilter_h

/// \file pcl/SeparableMedianFilter.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_ImageTransformation_h
#include <pcl/ImageTransformation.h>
#endif

#ifndef __PCL_Matrix_h
#include <pcl/Matrix.h> // for PCL_VALID_KERNEL_SIZE()
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SeparableMedianFilter
 * \brief A two-dimensional separable filter that approximates a median filter.
 *
 * ### TODO: Write a detailed description for %SeparableMedianFilter.
 */
class PCL_CLASS SeparableMedianFilter : public ImageTransformation
{
public:

   /*!
    * Constructs a %SeparableMedianFilter object with the specified size.
    *
    * \param size    Size of the separable median filter in pixels. Must be an
    *                odd integer &gt;= 3 (3, 5, 7, ...).
    */
   SeparableMedianFilter( int size = 3 ) :
      ImageTransformation(),
      m_size( PCL_VALID_KERNEL_SIZE( size ) ), m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
   {
      PCL_PRECONDITION( size >= 3 && size&1 == 1 )
   }

   /*!
    * Copy constructor.
    */
   SeparableMedianFilter( const SeparableMedianFilter& x ) :
      ImageTransformation( x ),
      m_size( x.m_size ), m_parallel( x.m_parallel ), m_maxProcessors( x.m_maxProcessors )
   {
   }

   /*!
    * Destroys this %SeparableMedianFilter object.
    */
   virtual ~SeparableMedianFilter()
   {
   }

   /*!
    * Returns the current size in pixels of this separable median filter.
    */
   int Size() const
   {
      return m_size;
   }

   /*!
    * Sets a new \a size in pixels for this separable median filter object. The
    * specified size must be an odd integer &gt;= 3 (3, 5, 7, ...).
    */
   void SetSize( int size )
   {
      PCL_PRECONDITION( size >= 3 && size&1 == 1 )
      m_size = PCL_VALID_KERNEL_SIZE( size );
   }

   /*!
    * Returns true if this object is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this instance of %SeparableMedianFilter.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance of %SeparableMedianFilter. If \a enable is false
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
    * Disables parallel processing for this instance of %SeparableMedianFilter.
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
    * %SeparableMedianFilter.
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
    * %SeparableMedianFilter.
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

   int      m_size;              // filter size
   bool     m_parallel      : 1; // use multiple threads
   unsigned m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT; // Maximum number of processors allowed

   /*
    * In-place 2-D separable median approximation filter.
    */
   virtual void Apply( pcl::Image& ) const;
   virtual void Apply( pcl::DImage& ) const;
   virtual void Apply( pcl::UInt8Image& ) const;
   virtual void Apply( pcl::UInt16Image& ) const;
   virtual void Apply( pcl::UInt32Image& ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SeparableMedianFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/SeparableMedianFilter.h - Released 2015/07/30 17:15:18 UTC
