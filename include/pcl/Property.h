//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.04.0827
// ----------------------------------------------------------------------------
// pcl/Property.h - Released 2017-05-28T08:28:50Z
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

#ifndef __PCL_Property_h
#define __PCL_Property_h

/// \file pcl/Property.h

#include <pcl/Defs.h>

#include <pcl/PropertyDescription.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Property
 * \brief An identifier-value association
 *
 * %Property is a simple identifier-value association, useful to transport
 * properties associated with views and images on the PixInsight platform.
 *
 * The View and FileFormatInstance classes include member functions to generate
 * PropertyArray containers, namely View::GetProperties(),
 * FileFormatInstance::ReadProperties() and
 * FileFormatInstance::ReadImageProperties().
 */
class PCL_CLASS Property
{
public:

   /*!
    * Represents the string class used to transport property identifiers.
    */
   typedef IsoString                identifier_type;

   /*!
    * Represents the class used to transport property values.
    */
   typedef Variant                  value_type;

   /*!
    * Represents the data type of a property value.
    */
   typedef value_type::data_type    data_type;

   /*!
    * Default constructor. Constructs an invalid %Property object with an empty
    * property identifier and an invalid property value.
    */
   Property() = default;

   /*!
    * Copy constructor.
    */
   Property( const Property& ) = default;

   /*!
    * Move constructor.
    */
#ifndef _MSC_VER
   Property( Property&& ) = default;
#else
   Property( Property&& x ) :
      m_id( std::move( x.m_id ) ),
      m_value( std::move( x.m_value ) )
   {
   }
#endif

   /*!
    * Constructs a %Property object with the specified property \a identifier
    * and property \a value.
    */
   Property( const identifier_type& identifier, const value_type& value ) :
      m_id( identifier ),
      m_value( value )
   {
   }

   /*!
    * Constructs a %Property object with the specified property \a identifier
    * and an undefined (invalid) property value.
    */
   Property( const identifier_type& identifier ) :
      m_id( identifier )
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   Property& operator =( const Property& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
#ifndef _MSC_VER
   Property& operator =( Property&& ) = default;
#else
   Property& operator =( Property&& x )
   {
      m_id = std::move( x.m_id );
      m_value = std::move( x.m_value );
      return *this;
   }
#endif

   /*!
    * Virtual destructor.
    */
   virtual ~Property()
   {
   }

   /*!
    * Returns the property identifier.
    */
   const identifier_type& Identifier() const
   {
      return m_id;
   }

   /*!
    * Returns the property identifier. This function is a convenience synonym
    * for Identifier().
    */
   const identifier_type& Id() const
   {
      return Identifier();
   }

   /*!
    * Returns the property value.
    */
   const value_type& Value() const
   {
      return m_value;
   }

   /*!
    * Returns the data type of this property, which is the data type of its
    * value.
    */
   data_type Type() const
   {
      return m_value.Type();
   }

   /*!
    * Returns a description of this property, including its identifier and
    * value data type.
    */
   PropertyDescription Description() const
   {
      return PropertyDescription( m_id, m_value.Type() );
   }

   /*!
    * Returns true iff the specified string \a id is a valid property
    * identifier. Refer to XISF::IsValidPropertyId() for property identifier
    * syntax rules.
    */
   template <class S>
   static bool IsValidIdentifier( const S& id )
   {
      if ( id.IsEmpty() )
         return false;
      Array<S> tokens;
      id.Break( tokens, ':' );
      for ( const S& token : tokens )
         if ( !token.IsValidIdentifier() )
            return false;
      return true;
   }

   static bool IsValidIdentifier( const char* id )
   {
      return IsValidIdentifier( IsoString( id ) );
   }

   /*!
    * Returns true iff this property is valid. A valid property has a valid
    * identifier and a valid value.
    */
   bool IsValid() const
   {
      return IsValidIdentifier( m_id ) && m_value.IsValid();
   }

   /*!
    * Equality operator. Returns true iff this property is equal to \a other
    * property.
    *
    * Two %Property objects are equal if their identifiers are equal. Keep in
    * mind that this operator compares properties, not their values.
    */
   bool operator ==( const Property& other ) const
   {
      return m_id == other.m_id;
   }

   /*!
    * Less than operator. Returns true iff this property precedes \a other
    * property.
    *
    * For comparisons of %Property objects, only their identifiers are taken
    * into account. Keep in mind that this operator compares properties, not
    * their values.
    */
   bool operator <( const Property& other ) const
   {
      return m_id < other.m_id;
   }

protected:

   identifier_type m_id;
   value_type      m_value;
};

/*!
 * \class pcl::PropertyArray
 * \brief Dynamic array of Property objects
 *
 * %PropertyArray is a template instantiation of Array for Property.
 */
typedef Array<Property> PropertyArray;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Property_h

// ----------------------------------------------------------------------------
// EOF pcl/Property.h - Released 2017-05-28T08:28:50Z
