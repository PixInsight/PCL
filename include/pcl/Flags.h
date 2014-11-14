// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/Flags.h - Released 2014/11/14 17:16:34 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef __PCL_Flags_h
#define __PCL_Flags_h

/// \file pcl/Flags.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
namespace pi
{
class GlobalSettings;
}
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Flags
 * \brief A type-safe collection of enumerated flags
 *
 * ### TODO: Write a detailed description for %Flags.
 */
template <typename E>
class PCL_CLASS Flags
{
public:

   /*!
    * Represents the enumerated type that defines individual flags for this
    * class.
    */
   typedef E enum_type;

   /*!
    * Constructs an empty (zero) %Flags instance.
    */
   Flags() : m_flags( 0 )
   {
   }

   /*!
    * Copy constructor.
    */
   Flags( const Flags& f ) : m_flags( f.m_flags )
   {
   }

   /*!
    * Constructs a %Flags instance from an enumerated flag value \a e.
    */
   Flags( E e ) : m_flags( e )
   {
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   Flags& operator =( Flags f )
   {
      m_flags = f.m_flags;
      return *this;
   }

   /*!
    * Assigns an enumerated flag value \a e to this object. Returns a reference
    * to this object.
    */
   Flags& operator =( E e )
   {
      m_flags = e;
      return *this;
   }

   /*!
    * Performs a bitwise AND operation with other flags \a f and stores the
    * resulting flags in this object. Returns a reference to this object.
    */
   Flags& operator &=( Flags f )
   {
      m_flags &= f.m_flags;
      return *this;
   }

   /*!
    * Performs a bitwise AND operation with an enumerated flag value \a e and
    * stores the resulting flags in this object. Returns a reference to this
    * object.
    */
   Flags& operator &=( E e )
   {
      m_flags &= unsigned( e );
      return *this;
   }

   /*!
    * Performs a bitwise AND operation with a mask value \a m and stores the
    * resulting flags in this object. Returns a reference to this object.
    */
   Flags& operator &=( unsigned m )
   {
      m_flags &= m;
      return *this;
   }

   /*!
    * Performs a bitwise inclusive OR operation with other flags \a f and
    * stores the resulting flags in this object. Returns a reference to this
    * object.
    */
   Flags& operator |=( Flags f )
   {
      m_flags |= f.m_flags;
      return *this;
   }

   /*!
    * Performs a bitwise inclusive OR operation with an enumerated flag value
    * \a e and stores the resulting flags in this object. Returns a reference
    * to this object.
    */
   Flags& operator |=( E e )
   {
      m_flags |= unsigned( e );
      return *this;
   }

   /*!
    * Performs a bitwise inclusive OR operation with a mask value \a m and
    * stores the resulting flags in this object. Returns a reference to this
    * object.
    */
   Flags& operator |=( unsigned m )
   {
      m_flags |= m;
      return *this;
   }

   /*!
    * Performs a bitwise exclusive OR (XOR) operation with other flags \a f and
    * stores the resulting flags in this object. Returns a reference to this
    * object.
    */
   Flags& operator ^=( Flags f )
   {
      m_flags ^= f.m_flags;
      return *this;
   }

   /*!
    * Performs a bitwise exclusive OR (XOR) operation with an enumerated flag
    * value \a e and stores the resulting flags in this object. Returns a
    * reference to this object.
    */
   Flags& operator ^=( E e )
   {
      m_flags ^= unsigned( e );
      return *this;
   }

   /*!
    * Performs a bitwise exclusive OR (XOR) operation with a mask value \a m
    * and stores the resulting flags in this object. Returns a reference to
    * this object.
    */
   Flags& operator ^=( unsigned m )
   {
      m_flags ^= m;
      return *this;
   }

   /*!
    * Returns true if the specified flag \a e is set in this %Flags object.
    */
   bool IsFlagSet( E e ) const
   {
      return (m_flags & e) != 0;
   }

   /*!
    * Sets or clears the specified flag \a e in this %Flags object.
    */
   void SetFlag( E e, bool on = true )
   {
      if ( on )
         m_flags |= e;
      else
         m_flags &= ~e;
   }

   /*!
    * Clears the specified flag \a e.
    * This is a convenience member function, equivalent to SetFlag( e, false ).
    */
   void ClearFlag( E e )
   {
      SetFlag( e, false );
   }

   /*!
    * Clears all flags in this object.
    */
   void Clear()
   {
      m_flags = 0u;
   }

   /*!
    * Sets the specified flag \a e in this object. Returns a reference to this
    * object.
    */
   Flags& operator <<( E e )
   {
      SetFlag( e );
      return *this;
   }

   /*!
    * Returns true if all flags in this object are zero.
    */
   bool operator !() const
   {
      return m_flags == 0;
   }

   /*!
    * Converts this %Flags object to an unsigned integer.
    */
   operator unsigned() const
   {
      return m_flags;
   }

   /*!
    * Returns a %Flags object whose value is the bitwise negation of the flags
    * stored in this object.
    */
   Flags operator ~() const
   {
      Flags f; f.m_flags = ~m_flags; return f;
   }

private:

   unsigned m_flags;

#ifndef __PCL_NO_FLAGS_SETTINGS_IO
   friend class Settings;
#endif

#ifndef __PCL_NO_FLAGS_FILE_IO
   friend class File;
#endif

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   friend class pi::GlobalSettings;
#endif
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup flags_bitwise_operators Bitwise Flags Operators
 */

/*!
 * Bitwise AND between two Flags objects \a a and \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator &( Flags<E> a, Flags<E> b )
{
   Flags<E> f( a ); f &= b; return f;
}

/*!
 * Bitwise AND between a Flags object \a a and an enumerated value \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator &( Flags<E> a, E b )
{
   Flags<E> f( a ); f &= b; return f;
}

/*!
 * Bitwise AND between an enumerated value \a a and a Flags object \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator &( E a, Flags<E> b )
{
   return Flags<E>( a ) & b;
}

/*!
 * Bitwise AND between a Flags object \a a and a flags mask \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator &( Flags<E> a, unsigned b )
{
   Flags<E> f( a ); f &= b; return f;
}

/*!
 * Bitwise AND between a flags mask \a a and a Flags object \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator &( unsigned a, Flags<E> b )
{
   return Flags<E>( b ) & a;
}

/*!
 * Bitwise OR between two Flags objects \a a and \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator |( Flags<E> a, Flags<E> b )
{
   Flags<E> f( a ); f |= b; return f;
}

/*!
 * Bitwise OR between a Flags object \a a and an enumerated value \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator |( Flags<E> a, E b )
{
   Flags<E> f( a ); f |= b; return f;
}

/*!
 * Bitwise OR between an enumerated value \a a and a Flags object \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator |( E a, Flags<E> b )
{
   return Flags<E>( a ) | b;
}

/*!
 * Bitwise OR between a Flags object \a a and a flags mask \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator |( Flags<E> a, unsigned b )
{
   Flags<E> f( a ); f |= b; return f;
}

/*!
 * Bitwise OR between a flags mask \a a and a Flags object \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator |( unsigned a, Flags<E> b )
{
   return Flags<E>( b ) | a;
}

/*!
 * Bitwise OR between two enumerated flags \a a and \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator |( E a, E b )
{
   return Flags<E>( a ) | b;
}

/*!
 * Bitwise XOR between two Flags objects \a a and \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator ^( Flags<E> a, Flags<E> b )
{
   Flags<E> f( a ); f ^= b; return f;
}

/*!
 * Bitwise XOR between a Flags object \a a and an enumerated value \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator ^( Flags<E> a, E b )
{
   Flags<E> f( a ); f ^= b; return f;
}

/*!
 * Bitwise XOR between an enumerated value \a a and a Flags object \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator ^( E a, Flags<E> b )
{
   return Flags<E>( a ) ^ b;
}

/*!
 * Bitwise XOR between a Flags object \a a and a flags mask \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator ^( Flags<E> a, unsigned b )
{
   Flags<E> f( a ); f ^= b; return f;
}

/*!
 * Bitwise XOR between a flags mask \a a and a Flags object \a b.
 * \ingroup flags_bitwise_operators
 */
template <class E>
inline Flags<E> operator ^( unsigned a, Flags<E> b )
{
   return Flags<E>( b ) ^ a;
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Flags_h

// ****************************************************************************
// EOF pcl/Flags.h - Released 2014/11/14 17:16:34 UTC
