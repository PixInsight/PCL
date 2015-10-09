//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/SharedPixelData.h - Released 2015/10/08 11:24:12 UTC
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

#ifndef __PCL_SharedPixelData_h
#define __PCL_SharedPixelData_h

/// \file pcl/SharedPixelData.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_ColorSpace_h
#include <pcl/ColorSpace.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SharedPixelData
 * \brief Handles transparent, type-independent allocation of local and shared
 *        pixel data blocks.
 *
 * \internal This is a low-level class internally used by PCL classes and
 * functions. It is not intended to be used directly by user-level modules.
 * Unless you are a PixInsight Core or PCL maintainer, you should not need or
 * attempt to use or instantiate this class.
 */
class PCL_CLASS SharedPixelData
{
public:

   typedef ColorSpace::value_type   color_space;

   /*!
    * Constructs a %SharedPixelData object that represents a local image.
    */
   SharedPixelData() : m_handle( nullptr )
   {
   }

   /*!
    * References an existing shared image with the specified handle and sample
    * data type.
    *
    * \param handle           Handle to a shared image.
    *
    * \param bitsPerSample    Number of bits per pixel sample.
    *
    * \param floatSample      Whether this image stores floating point pixel
    *                         data, or integer pixel data.
    *
    * \param complexSample    Whether this image stores complex pixel data or
    *                         real pixel data.
    *
    * \note This constructor throws an Error exception if the specified handle
    * does not correspond to an existing shared image, or if the existing
    * shared image does not have the specified sample data type.
    */
   SharedPixelData( void* handle, int bitsPerSample, bool floatSample, bool complexSample );

   /*!
    * Creates a new shared image with the specified geometry, sample data type
    * and color space.
    *
    * \param width            %Image width in pixels.
    *
    * \param height           %Image height in pixels.
    *
    * \param numberOfChannels Number of channels, including nominal channels or
    *                         color components and alpha channels.
    *
    * \param bitsPerSample    Number of bits per pixel sample.
    *
    * \param floatSample      Whether this image stores floating point pixel
    *                         data, or integer pixel data.
    *
    * \param colorSpace       Color space where pixels are represented.
    */
   SharedPixelData( int width, int height, int numberOfChannels,
                    int bitsPerSample, bool floatSample, int colorSpace );

   /*!
    * Copy constructor.
    *
    * If the argument object \a x represents a shared image, this constructor
    * references the same shared image. If \a x is a local image, this function
    * just copies the null handle and has no further effect.
    */
   SharedPixelData( const SharedPixelData& x ) : m_handle( x.m_handle )
   {
      Attach();
   }

   /*!
    * Virtual destructor.
    *
    * If this object represents a shared image, it is dereferenced. If the
    * shared image becomes unreferenced, it will be garbage-collected and
    * eventually destroyed by the PixInsight core application.
    *
    * If this object represents a local image, this destructor has no effect.
    */
   virtual ~SharedPixelData()
   {
      Detach();
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    *
    * If this object represents a shared image, it is dereferenced. If the
    * specified object \a x represents a shared image, then this object will
    * reference it; otherwhise this operator just copies the null handle
    * without further effect.
    */
   SharedPixelData& operator =( const SharedPixelData& x )
   {
      if ( m_handle != x.m_handle )
      {
         Detach();
         m_handle = x.m_handle;
         Attach();
      }
      return *this;
   }

   /*!
    * Returns true iff this object represents the same shared image as another
    * object \a x, or if both objects represent local images.
    */
   bool operator ==( const SharedPixelData& x ) const
   {
      return m_handle == x.m_handle;
   }

   /*!
    * Returns the handle to the shared image represented by this object, or
    * zero (a null pointer) if this object represents a local image.
    */
   void* Handle() const
   {
      return m_handle;
   }

   /*!
    * Returns true if this object represents a shared image; false if it
    * represents a local image. Shared images live in the PixInsight core
    * application. Local images live in the calling module.
    */
   bool IsShared() const
   {
      return m_handle != nullptr;
   }

   /*!
    * Returns true iff the shared image represented by this object is not
    * uniquely referenced, i.e. if there exist other %SharedPixelData objects
    * representing the same shared image. Returns false if the shared image is
    * only referenced by this object, or if this object represents a local
    * image.
    */
   bool IsAliased() const;

   /*!
    * Returns true iff this object is the owner of the represented shared image.
    * Returns false if this object does not own the shared image, or if this
    * object does not represent a shared image.
    */
   bool IsOwner() const;

private:

   void* m_handle;

   void Attach();
   void Detach();

protected:

   void* Allocate( size_type sizeInBytes ) const;
   void Deallocate( void* ) const;

   void** GetSharedData() const;
   void SetSharedData( void** ptrToShared );

   void GetSharedGeometry( int& width, int& height, int& numberOfChannels );
   void SetSharedGeometry( int width, int height, int numberOfChannels );

   void GetSharedColor( color_space& colorSpace, RGBColorSystem& RGBWS );
   void SetSharedColor( color_space colorSpace, const RGBColorSystem& RGBWS );

   template <class P> friend class GenericImage;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SharedPixelData_h

// ----------------------------------------------------------------------------
// EOF pcl/SharedPixelData.h - Released 2015/10/08 11:24:12 UTC
