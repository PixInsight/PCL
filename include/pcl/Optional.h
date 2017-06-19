//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/Optional.h - Released 2017-06-17T10:55:43Z
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

#ifndef __PCL_Optional_h
#define __PCL_Optional_h

/// \file pcl/Optional.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Optional
 * \brief An object that can be in a defined or undefined state
 *
 * %Optional stores an instance of the template argument T along with a
 * <em>defined state</em> Boolean flag. %Optional objects update their defined
 * state automatically as they are created, copied and assigned.
 *
 * This class is useful to deal with objects that can be defined optionally,
 * when it is inconvenient to manage the object's value and its defined state
 * as two separate items. %Optional offers the power of encapsulation in
 * object-oriented languages to solve these problems efficiently.
 */
template <class T>
class PCL_CLASS Optional
{
public:

   /*!
    * Constructs an undefined %Optional object.
    */
   Optional() = default;

   /*!
    * Copy constructor.
    */
   Optional( const Optional& ) = default;

   /*
    * Move constructor.
    */
#ifndef _MSC_VER
   Optional( Optional&& ) = default;
#else
   Optional( Optional&& x ) :
      m_value( std::move( x.m_value ) ),
      m_defined( x.m_defined )
   {
   }
#endif

   /*!
    * Constructs a defined %Optional object with the specified \a value.
    */
   Optional( const T& value ) :
      m_value( value ),
      m_defined( true )
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   Optional& operator =( const Optional& x )
   {
      if ( bool( m_defined = x.m_defined ) )
         m_value = x.m_value;
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   Optional& operator =( Optional&& x )
   {
      if ( bool( m_defined = x.m_defined ) )
         m_value = std::move( x.m_value );
      return *this;
   }

   /*!
    * Assigns the specified \a value to this object. Returns a reference to
    * this object. After assigning a value, an %Optional object will be in
    * defined state.
    */
   Optional& operator =( const T& value )
   {
      m_value = value;
      m_defined = true;
      return *this;
   }

   /*!
    * Type conversion operator. Returns a reference to the value stored in this
    * %Optional object.
    *
    * If this object is undefined, the returned value may be unpredictable,
    * depending on construction semantics for the type T.
    */
   operator const T&() const
   {
      return m_value;
   }

   /*!
    * Returns a reference to the value stored in this %Optional object.
    *
    * If this object is undefined, the returned value may be unpredictable,
    * depending on construction semantics for the type T.
    *
    * This function performs the same role as operator const T&(). It is
    * required for cases where an automatic type conversion is not applicable.
    */
   const T& operator ()() const
   {
      return m_value;
   }

   /*!
    * Returns true if this %Optional object has been defined.
    */
   bool IsDefined() const
   {
      return m_defined;
   }

   /*!
    * Returns the value stored in this object if it has been defined. Otherwise
    * returns the specified \a value.
    */
   const T& OrElse( const T& value ) const
   {
      return m_defined ? m_value : value;
   }

   /*!
    * Returns the value stored in this object if it has been defined. Otherwise
    * throws the specified \a exception.
    */
   template <class E>
   const T& OrElseThrow( const Exception& exception ) const
   {
      if ( m_defined )
         return m_value;
      throw exception;
   }

   /*!
    * Returns the value stored in this object if it has been defined. Otherwise
    * throws an Error exception with the specified \a message.
    */
   const T& OrElseThrow( const String& message ) const
   {
      if ( m_defined )
         return m_value;
      throw Error( message );
   }

   /*!
    * Equality operator. Returns true iff one of the following condition holds:
    *
    * \li This object has been defined, \a x has been defined, and the value in
    * this object equals the value in \a x.
    *
    * \li Neither this object nor \a x have been defined.
    */
   bool operator ==( const Optional& x ) const
   {
      return m_defined ? (x.m_defined ? m_value == x.m_value : false) : !x.m_defined;
   }

   /*!
    * Value equality operator. Returns true iff this object has been defined
    * and its value equals the specified \a value.
    */
   bool operator ==( const T& value ) const
   {
      return m_defined && m_value == value;
   }

   /*!
    * Less than relational operator. Returns true iff this object has been
    * defined and either \a x has not been defined (defined objects always
    * precede undefined ones), or the value in this object precedes the value
    * in \a x.
    */
   bool operator <( const Optional& x ) const
   {
      return m_defined && (!x.m_defined || m_value < x.m_value);
   }

   /*!
    * Less-than-value relational operator. Returns true iff this object has
    * been defined and its value precedes the specified \a value.
    */
   bool operator <( const T& value ) const
   {
      return m_defined && m_value < value;
   }

private:

   T    m_value;
   bool m_defined = false;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Optional_h

// ----------------------------------------------------------------------------
// EOF pcl/Optional.h - Released 2017-06-17T10:55:43Z
