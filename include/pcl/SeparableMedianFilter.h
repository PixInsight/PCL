//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/SeparableMedianFilter.h - Released 2019-01-21T12:06:07Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ImageTransformation.h>
#include <pcl/Matrix.h> // PCL_VALID_KERNEL_SIZE()
#include <pcl/ParallelProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SeparableMedianFilter
 * \brief A two-dimensional separable filter that approximates a median filter.
 *
 * ### TODO: Write a detailed description for %SeparableMedianFilter.
 */
class PCL_CLASS SeparableMedianFilter : public ImageTransformation,
                                        public ParallelProcess
{
public:

   /*!
    * Constructs a %SeparableMedianFilter object with the specified size.
    *
    * \param size    Size of the separable median filter in pixels. Must be an
    *                odd integer &gt;= 3 (3, 5, 7, ...).
    */
   SeparableMedianFilter( int size = 3 ) :
      m_size( PCL_VALID_KERNEL_SIZE( size ) )
   {
      PCL_PRECONDITION( size >= 3 && size&1 == 1 )
   }

   /*!
    * Copy constructor.
    */
   SeparableMedianFilter( const SeparableMedianFilter& ) = default;

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
    * specified size must be an odd integer &gt; 3 (3, 5, 7, ...).
    */
   void SetSize( int size )
   {
      PCL_PRECONDITION( size >= 3 && size&1 == 1 )
      m_size = PCL_VALID_KERNEL_SIZE( size );
   }

protected:

   int  m_size; // filter size

   /*
    * In-place 2-D separable median approximation filter.
    */
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SeparableMedianFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/SeparableMedianFilter.h - Released 2019-01-21T12:06:07Z
