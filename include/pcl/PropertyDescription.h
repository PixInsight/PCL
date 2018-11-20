//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/PropertyDescription.h - Released 2018-11-01T11:06:36Z
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

#ifndef __PCL_PropertyDescription_h
#define __PCL_PropertyDescription_h

/// \file pcl/PropertyDescription.h

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/Variant.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PropertyDescription
 * \brief A structure to describe a data property
 *
 * This structure is used (instantiated in PropertyDescriptionArray
 * containers) by the FileFormatImplementation and FileFormaInstance classes to
 * describe data properties stored in image files, among other classes.
 *
 * \sa FileFormatImplementation, FileFormatInstance
 */
struct PCL_CLASS PropertyDescription
{
   /*!
    * An enumeration of supported property data types.
    */
   typedef VariantType::value_type  data_type;

   IsoString id;                          //!< Identifier of this property.
   data_type type = VariantType::Invalid; //!< Property data type.

   /*!
    * Default constructor. Constructs an %PropertyDescription object with
    * an empty property identifier and unspecified (invalid) data type.
    */
   PropertyDescription() = default;

   /*!
    * Constructs an %PropertyDescription object with the specified
    * property identifier and data type.
    */
   PropertyDescription( const IsoString& s, data_type t = VariantType::Invalid ) :
      id( s ), type( t )
   {
   }

   /*!
    * Copy constructor.
    */
   PropertyDescription( const PropertyDescription& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   PropertyDescription& operator =( const PropertyDescription& ) = default;

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
    * Since property identifiers are unique within a property's scope (within
    * an image file, for example), comparisons of %PropertyDescription
    * structures only take into account the PropertyDescription::id member.
    */
   bool operator <( const PropertyDescription& x ) const
   {
      return id < x.id;
   }

   /*!
    * Equality operator. Returns true if this object refers to the same
    * property as the specified object \a x; otherwise returns false.
    *
    * Since property identifiers are unique within a property's scope (within
    * an image file, for example), comparisons of %PropertyDescription
    * structures only take into account the PropertyDescription::id member.
    */
   bool operator ==( const PropertyDescription& x ) const
   {
      return id == x.id;
   }
};

/*!
 * \class pcl::PropertyDescriptionArray
 * \brief Dynamic array of PropertyDescription structures
 *
 * %PropertyDescriptionArray is a template instantiation of Array for
 * PropertyDescription.
 */
typedef Array<PropertyDescription>  PropertyDescriptionArray;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PropertyDescription_h

// ----------------------------------------------------------------------------
// EOF pcl/PropertyDescription.h - Released 2018-11-01T11:06:36Z
