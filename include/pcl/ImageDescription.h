//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/ImageDescription.h - Released 2015/10/08 11:24:12 UTC
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

#ifndef __PCL_ImageDescription_h
#define __PCL_ImageDescription_h

/// \file pcl/ImageDescription.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_ImageInfo_h
#include <pcl/ImageInfo.h>
#endif

#ifndef __PCL_ImageOptions_h
#include <pcl/ImageOptions.h>
#endif

#ifndef __PCL_Variant_h
#include <pcl/Variant.h>
#endif

#ifndef __PCL_Array_h
#include <pcl/Array.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ImageDescription
 * \brief A structure to transport basic information and options for an image
 *        stored in an image file.
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
    * Assignment operator. Returns a reference to this object.
    */
   ImageDescription& operator =( const ImageDescription& x )
   {
      info = x.info;
      options = x.options;
      id = x.id;
      return *this;
   }
};

/*!
 * \class pcl::ImageDescriptionArray
 * \brief Dynamic array of ImageDescription structures.
 *
 * %ImageDescriptionArray is a template instantiation of Array for
 * ImageDescription.
 */
typedef Array<ImageDescription>  ImageDescriptionArray;

// ----------------------------------------------------------------------------

/*!
 * \class ImagePropertyDescription
 * \brief A structure to describe a data property stored in an image file.
 *
 * This structure is used (instantiated in ImagePropertyDescriptionArray
 * containers) by the FileFormatImplementation and FileFormaInstance classes to
 * describe data properties stored in image files.
 *
 * \sa FileFormatImplementation, FileFormatInstance
 */
struct PCL_CLASS ImagePropertyDescription
{
   /*!
    * An enumeration of supported property data types.
    */
   typedef VariantType::value_type  data_type;

   IsoString id;   //!< Identifier of this property.
   data_type type; //!< Property data type.

   /*!
    * Default constructor. Constructs an %ImagePropertyDescription object with
    * an empty property identifier and unspecified (invalid) data type.
    */
   ImagePropertyDescription() : id(), type( VariantType::Invalid )
   {
   }

   /*!
    * Constructs an %ImagePropertyDescription object with the specified
    * property identifier and data type.
    */
   ImagePropertyDescription( const IsoString& s, data_type t = VariantType::Invalid ) : id( s ), type( t )
   {
   }

   /*!
    * Copy constructor.
    */
   ImagePropertyDescription( const ImagePropertyDescription& x ) : id( x.id ), type( x.type )
   {
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   ImagePropertyDescription& operator =( const ImagePropertyDescription& x )
   {
      id = x.id;
      type = x.type;
      return *this;
   }

   /*!
    * Returns true iff this object represents a valid data property.
    */
   bool IsValid() const
   {
      return type != VariantType::Invalid;
   }

   /*!
    * Less-than relational operator. Returns true if this object precedes the
    * specified object \a x; otherwise returns false.
    *
    * Since image property identifiers are unique (within an image file),
    * comparisons of %ImagePropertyDescription structures only take into
    * account the ImagePropertyDescription::id member.
    */
   bool operator <( const ImagePropertyDescription& x ) const
   {
      return id < x.id;
   }

   /*!
    * Equality operator. Returns true if this object refers to the same image
    * property as the specified object \a x; otherwise returns false.
    *
    * Since image property identifiers are unique (within an image file),
    * comparisons of %ImagePropertyDescription structures only take into
    * account the ImagePropertyDescription::id member.
    */
   bool operator ==( const ImagePropertyDescription& x ) const
   {
      return id == x.id;
   }
};

/*!
 * \class pcl::ImagePropertyDescriptionArray
 * \brief Dynamic array of ImagePropertyDescription structures.
 *
 * %ImagePropertyDescriptionArray is a template instantiation of Array for
 * ImagePropertyDescription.
 */
typedef Array<ImagePropertyDescription>  ImagePropertyDescriptionArray;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageDescription_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageDescription.h - Released 2015/10/08 11:24:12 UTC
