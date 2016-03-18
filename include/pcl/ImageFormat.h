//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/ImageFormat.h - Released 2016/02/21 20:22:12 UTC
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

#ifndef __PCL_ImageFormat_h
#define __PCL_ImageFormat_h

/// \file pcl/ImageFormat.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

#ifndef __PCL_StringList_h
#include <pcl/StringList.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ImageFormat
 * \brief Characterization of built-in image file formats
 *
 * ### TODO: Write a detailed description for %ImageFormat.
 */
class PCL_CLASS ImageFormat
{
public:

   /*!
    * Constructs an %ImageFormat object.
    */
   ImageFormat()
   {
   }

   /*!
    * Destroys an %ImageFormat object.
    */
   virtual ~ImageFormat()
   {
   }

   /*!
    * Returns an identifying name for this image file format.
    */
   virtual String Name() const = 0;

   /*!
    * Returns a brief description of this image file format.
    */
   virtual String Description() const = 0;

   /*!
    * Returns a brief description of this image file format implementation.
    */
   virtual String Implementation() const = 0;

   /*!
    * Returns a list of the file extensions associated to this file format.
    * Each item in the returned list must be a valid file extension starting
    * with the standard dot extension separator.
    */
   virtual StringList FileExtensions() const = 0;

   /*!
    * Returns true iff this file format implementation can read an entire image
    * in a single operation.
    */
   virtual bool CanRead() const
   {
      return true;
   }

   /*!
    * Returns true iff this file format implementation can write an entire image
    * in a single operation.
    */
   virtual bool CanWrite() const
   {
      return true;
   }

   /*!
    * Returns true iff this file format implementation supports <em>incremental
    * read</em> operations on image files.
    *
    * Incremental read allows loading images by successive pixel rows.
    */
   virtual bool CanReadIncrementally() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation supports <em>incremental
    * write</em> operations on image files.
    *
    * Incremental write allows writing images by successive pixel rows.
    */
   virtual bool CanWriteIncrementally() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation can read/write 8-bit
    * integer images
    */
   virtual bool CanStore8Bit() const
   {
      return true;
   }

   /*!
    * Returns true iff this file format implementation can read/write 16-bit
    * integer images
    */
   virtual bool CanStore16Bit() const
   {
      return true;
   }

   /*!
    * Returns true iff this file format implementation can read/write 32-bit
    * integer images
    */
   virtual bool CanStore32Bit() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation can read/write 64-bit
    * integer images
    */
   virtual bool CanStore64Bit() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation can read/write 32-bit
    * floating point images (IEEE 754 32-bit <em>single precision</em> format
    * for pixel sample values).
    */
   virtual bool CanStoreFloat() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation can read/write 64-bit
    * floating point images (IEEE 754 64-bit <em>double precision</em> format
    * for pixel sample values).
    */
   virtual bool CanStoreDouble() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation can read/write 32-bit
    * complex floating point images (IEEE 754 32-bit <em>single precision</em>
    * format for components of complex pixel sample values).
    */
   virtual bool CanStoreComplex() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation can read/write 64-bit
    * complex floating point images (IEEE 754 64-bit <em>double precision</em>
    * format for components of complex pixel sample values).
    */
   virtual bool CanStoreDComplex() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation can read/write grayscale
    * pixel data.
    */
   virtual bool CanStoreGrayscale() const
   {
      return true;
   }

   /*!
    * Returns true iff this file format implementation can read/write RGB color
    * pixel data.
    */
   virtual bool CanStoreRGBColor() const
   {
      return true;
   }

   /*!
    * Returns true iff this file format implementation supports alpha channels.
    */
   virtual bool CanStoreAlphaChannels() const
   {
      return true;
   }

   /*!
    * Returns true iff this file format implementation can store/retrieve image
    * resolution data.
    */
   virtual bool CanStoreResolution() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation can embed/extract FITS
    * header keyword collections.
    */
   virtual bool CanStoreKeywords() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation can embed/extract ICC
    * color profiles.
    */
   virtual bool CanStoreICCProfiles() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation can embed/extract
    * thumbnail images.
    */
   virtual bool CanStoreThumbnails() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation supports compression of
    * pixel data.
    *
    * This refers to compression of \e source pixels, not to native compression
    * schemes used by some file formats.
    *
    * For examle, the compression schemes employed in the JPEG and JPEG2000
    * formats must \e not cause this member function to return true. The
    * optional ZIP and LZW compressions used in TIFF are the exact kind of
    * compressions that must cause this member function to return true.
    */
   virtual bool SupportsCompression() const
   {
      return false;
   }

   /*!
    * Returns true iff this file format implementation supports multiple images
    * stored in a single file.
    *
    * For example, multiple images (e.g., taken with different filters) can be
    * stored in FITS files by means of FITS image extensions, forming a
    * <em>data cube</em> of several images with the same dimensions, or even a
    * collection of independent images.
    */
   virtual bool SupportsMultipleImages() const
   {
      return false;
   }

private:

   ImageFormat( const ImageFormat& ) = delete;
   void operator =( const ImageFormat& ) = delete;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageFormat_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageFormat.h - Released 2016/02/21 20:22:12 UTC
