//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/Flags.h - Released 2018-11-23T16:14:19Z
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

#ifndef __PCL_Flags_h
#define __PCL_Flags_h

/// \file pcl/Flags.h

#include <pcl/Defs.h>

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
namespace pi
{
class GlobalSettings;
}
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

template <bool> struct FlagType {};
template <>     struct FlagType<true>  { typedef unsigned type; };
template <>     struct FlagType<false> { typedef int      type; };

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

   static_assert( sizeof( E ) <= sizeof( int ),
                  "Invalid sizeof( Flags::enum_type ): Must not be larger than sizeof( int )" );

   /*!
    * Represents the enumerated type that defines individual flags.
    */
   typedef                          E enum_type;

   /*!
    * Represents the integral type used to store flags.
    */
   typedef typename FlagType<std::is_unsigned<enum_type>::value>::type
                                    flag_type;

   /*!
    * Constructs an empty (zero) %Flags instance.
    */
   Flags() : m_flags( 0 )
   {
   }

   /*!
    * Constructs a %Flags instance from an enumerated flag value \a e.
    */
   constexpr
   Flags( enum_type e ) : m_flags( int( e ) )
   {
   }

   /*!
    * Constructs a %Flags instance from a mask \a m.
    */
   constexpr
   Flags( flag_type m ) : m_flags( int( m ) )
   {
   }

   /*!
    * Copy constructor.
    */
   Flags( const Flags& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   Flags& operator =( const Flags& ) = default;

   /*!
    * Assigns an enumerated flag value \a e to this object. Returns a reference
    * to this object.
    */
   Flags& operator =( enum_type e )
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
   Flags& operator &=( enum_type e )
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
   Flags& operator |=( enum_type e )
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
   Flags& operator ^=( enum_type e )
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
    * Returns true iff the specified flag \a e is set in this %Flags object.
    */
   constexpr bool IsFlagSet( enum_type e ) const
   {
      return (m_flags & e) != 0;
   }

   /*!
    * Sets or clears the specified flag \a e in this %Flags object.
    */
   void SetFlag( enum_type e, bool on = true )
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
   void ClearFlag( enum_type e )
   {
      SetFlag( e, false );
   }

   /*!
    * Clears all flags in this object.
    */
   void Clear()
   {
      m_flags = 0;
   }

   /*!
    * Sets the specified flag \a e in this object. Returns a reference to this
    * object.
    */
   Flags& operator <<( enum_type e )
   {
      SetFlag( e );
      return *this;
   }

   /*!
    * Returns true iff all flags in this object are zero.
    */
   constexpr bool operator !() const
   {
      return m_flags == 0;
   }

   /*!
    * Converts this %Flags object to an integer.
    */
   constexpr operator flag_type() const
   {
      return m_flags;
   }

   /*!
    * Returns a %Flags object whose value is the bitwise negation of the flags
    * stored in this object.
    */
   constexpr Flags operator ~() const
   {
      return Flags( enum_type( ~m_flags ) );
   }

   /*!
    * Bitwise AND between two %Flags objects.
    */
   constexpr Flags operator &( Flags f ) const
   {
      return Flags( enum_type( m_flags & f.m_flags ) );
   }

   /*!
    * Bitwise AND between a Flags object and an enumerated value.
    */
   constexpr Flags operator &( enum_type e ) const
   {
      return Flags( enum_type( m_flags & flag_type( e ) ) );
   }

   /*!
    * Bitwise AND between a %Flags object and a mask.
    */
   constexpr Flags operator &( unsigned m ) const
   {
      return Flags( enum_type( m_flags & m ) );
   }

   /*!
    * Bitwise OR between two %Flags objects.
    */
   constexpr Flags operator |( Flags f ) const
   {
      return Flags( enum_type( m_flags | f.m_flags ) );
   }

   /*!
    * Bitwise OR between a Flags object and an enumerated value.
    */
   constexpr Flags operator |( enum_type e ) const
   {
      return Flags( enum_type( m_flags | flag_type( e ) ) );
   }

   /*!
    * Bitwise OR between a %Flags object and a mask.
    */
   constexpr Flags operator |( unsigned m ) const
   {
      return Flags( enum_type( m_flags | m ) );
   }

   /*!
    * Bitwise XOR between two %Flags objects.
    */
   constexpr Flags operator ^( Flags f ) const
   {
      return Flags( enum_type( m_flags ^ f.m_flags ) );
   }

   /*!
    * Bitwise XOR between a Flags object and an enumerated value.
    */
   constexpr Flags operator ^( enum_type e ) const
   {
      return Flags( enum_type( m_flags ^ flag_type( e ) ) );
   }

   /*!
    * Bitwise XOR between a %Flags object and a mask.
    */
   constexpr Flags operator ^( unsigned m ) const
   {
      return Flags( enum_type( m_flags ^ m ) );
   }

private:

   flag_type m_flags;

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

} // pcl

#endif   // __PCL_Flags_h

// ----------------------------------------------------------------------------
// EOF pcl/Flags.h - Released 2018-11-23T16:14:19Z
