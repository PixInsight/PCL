//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/KeyValue.h - Released 2018-11-01T11:06:36Z
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

#ifndef __PCL_KeyValue_h
#define __PCL_KeyValue_h

/// \file pcl/KeyValue.h

#include <pcl/Defs.h>

#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class KeyValue
 * \brief Generic key-value association.
 */
template <typename T1, typename T2>
class PCL_CLASS KeyValue
{
public:

   T1 key;     //!< The key member of this key-value association
   T2 value;   //!< The value member of this key-value association

   /*!
    * Constructs a key-value association with default key and value members.
    */
   KeyValue() : key(), value()
   {
   }

   /*!
    * Constructs a key-value association with the specified key and value.
    */
   KeyValue( const T1& k, const T2& v ) : key( k ), value( v )
   {
   }

   /*!
    * Constructs a key-value association with the specified key and a
    * default-constructed value.
    */
   KeyValue( const T1& k ) : key( k ), value()
   {
   }

   /*!
    * Copy constructor.
    */
   KeyValue( const KeyValue& x ) : key( x.key ), value( x.value )
   {
   }

   /*!
    * Move constructor.
    */
   KeyValue( KeyValue&& x ) : key( std::move( x.key ) ), value( std::move( x.value ) )
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   KeyValue& operator =( const KeyValue& x )
   {
      key = x.key;
      value = x.value;
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   KeyValue& operator =( KeyValue&& x )
   {
      key = std::move( x.key );
      value = std::move( x.value );
      return *this;
   }

   /*!
    * Returns true iff this key-value association is equal to another object
    * \a x. key-value associations are equal if their respective key and value
    * members are equal.
    */
   bool operator ==( const KeyValue& x ) const
   {
      return key == x.key && value == x.value;
   }

   /*!
    * Returns true iff this key-value association precedes another object \a x.
    * In key-value comparisons, key members have precedence over value members.
    * The comparison algorithm is as follows:
    *
    * \code
    * if ( this->key != x.key )
    *    return this->key < x.key;
    * else
    *    return this->value < x.value;
    * \endcode
    *
    * The implementation of this operator only requires <em>less than</em>
    * semantics for the two types T1 and T2; it doesn't use equality operators.
    */
   bool operator <( const KeyValue& x ) const
   {
      return (key < x.key) ? true : ((x.key < key) ? false : value < x.value);
   }
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup string_key_value_associations String Key-Value Associations
 */

/*!
 * \class pcl::StringKeyValue
 * \ingroup string_key_value_associations
 * \brief Key-value association of two Unicode (UTF-16) strings.
 */
typedef KeyValue<String, String>       StringKeyValue;

/*!
 * \class pcl::IsoStringKeyValue
 * \ingroup string_key_value_associations
 * \brief Key-value association of two 8-bit strings.
 */
typedef KeyValue<IsoString, IsoString> IsoStringKeyValue;

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_KeyValue_h

// ----------------------------------------------------------------------------
// EOF pcl/KeyValue.h - Released 2018-11-01T11:06:36Z
