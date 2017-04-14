//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/ImageDescription.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_ImageDescription_h
#define __PCL_ImageDescription_h

/// \file pcl/ImageDescription.h

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/ImageInfo.h>
#include <pcl/ImageOptions.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ImageDescription
 * \brief A structure to transport basic information and options for an image
 *        stored in an image file
 *
 * This structure is used (instantiated in ImageDescriptionArray containers) by
 * the FileFormatImplementation and FileFormaInstance classes to describe
 * individual images stored in image files.
 *
 * \sa ImageInfo, ImageOptions, FileFormatImplementation, FileFormatInstance
 */
struct PCL_CLASS ImageDescription
{
   ImageInfo    info;    //!< Basic image information: dimensions and color space.
   ImageOptions options; //!< Format-independent image file options.
   IsoString    id;      //!< Identifier of this image. Usually a running counter starting from zero.

   /*!
    * Default constructor.
    */
   ImageDescription() = default;

   /*!
    * Constructs an %ImageDescription object from the specified ImageInfo,
    * ImageOptions and image identifier.
    */
   ImageDescription( const ImageInfo& i, const ImageOptions& o, const IsoString& s = IsoString() ) :
      info( i ), options( o ), id( s )
   {
   }

   ImageDescription( const ImageInfo& i, const ImageOptions& o, const String& s ) :
      ImageDescription( i, o, IsoString( s ) )
   {
   }

   /*!
    * Constructs an %ImageDescription object from the specified ImageInfo
    * structure with default format-independent options and an empty image
    * identifier.
    */
   ImageDescription( const ImageInfo& i ) : info( i ), options(), id()
   {
   }

   /*!
    * Copy constructor.
    */
   ImageDescription( const ImageDescription& ) = default;

   /*!
    * Cop assignment operator. Returns a reference to this object.
    */
   ImageDescription& operator =( const ImageDescription& ) = default;
};

/*!
 * \class pcl::ImageDescriptionArray
 * \brief Dynamic array of ImageDescription structures
 *
 * %ImageDescriptionArray is a template instantiation of Array for
 * ImageDescription.
 */
typedef Array<ImageDescription>  ImageDescriptionArray;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageDescription_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageDescription.h - Released 2017-04-14T23:04:40Z
