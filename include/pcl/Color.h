//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/Color.h - Released 2015/10/08 11:24:12 UTC
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

#ifndef __PCL_Color_h
#define __PCL_Color_h

/// \file pcl/Color.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

#ifndef __PCL_Math_h
#include <pcl/Math.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * Defines a 32-bit RGBA pixel value.
 *
 * A %RGBA pixel value is encoded as follows:
 *
 * AARRGGBB
 *
 * where each letter represents a 4-bit hexadecimal digit (from 0 to F). Each
 * 8-bit pair represents a pixel component in the range from 0 to 255:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>AA</td>    <td>Alpha value, or <em>pixel transparency</em>: 0 means
 *                        completely transparent, 255 corresponds to an opaque
 *                        pixel.</td></tr>
 * <tr><td>RR</td>    <td>Red pixel color component.</td></tr>
 * <tr><td>GG</td>    <td>Green pixel color component.</td></tr>
 * <tr><td>BB</td>    <td>Blue pixel color component.</td></tr>
 * </table>
 */
typedef uint32 RGBA;

/*!
 * \defgroup rgba_utility_functions RGBA Color Utility Functions
 */

/*!
 * Returns the alpha (transparency) component of a RGBA pixel value.
 *
 * \ingroup rgba_utility_functions
 */
inline uint8 Alpha( RGBA rgba )
{
   return uint8( rgba >> 24 );
}

/*!
 * Returns the red color component of a RGBA pixel value.
 *
 * \ingroup rgba_utility_functions
 */
inline uint8 Red( RGBA rgba )
{
   return uint8( rgba >> 16 );
}

/*!
 * Returns the green color component of a RGBA pixel value.
 *
 * \ingroup rgba_utility_functions
 */
inline uint8 Green( RGBA rgba )
{
   return uint8( rgba >> 8 );
}

/*!
 * Returns the blue color component of a RGBA pixel value.
 *
 * \ingroup rgba_utility_functions
 */
inline uint8 Blue( RGBA rgba )
{
   return uint8( rgba );
}

/*!
 * Clears (sets to zero) the alpha (transparency) component of a RGBA pixel
 * value.
 *
 * \ingroup rgba_utility_functions
 */
inline void ClearAlpha( RGBA& rgba )
{
   rgba &= 0x00fffffful;
}

/*!
 * Clears (sets to zero) the red color component of a RGBA pixel value.
 *
 * \ingroup rgba_utility_functions
 */
inline void ClearRed( RGBA& rgba )
{
   rgba &= 0xff00fffful;
}

/*!
 * Clears (sets to zero) the green color component of a RGBA pixel value.
 *
 * \ingroup rgba_utility_functions
 */
inline void ClearGreen( RGBA& rgba )
{
   rgba &= 0xffff00fful;
}

/*!
 * Clears (sets to zero) the blue color component of a RGBA pixel value.
 *
 * \ingroup rgba_utility_functions
 */
inline void ClearBlue( RGBA& rgba )
{
   rgba &= 0xffffff00ul;
}

/*!
 * Sets the alpha (transparency) component of a RGBA pixel value.
 *
 * \ingroup rgba_utility_functions
 */
inline void SetAlpha( RGBA& rgba, uint8 a )
{
   ClearAlpha( rgba ); rgba |= uint32( a ) << 24;
}

/*!
 * Sets the red color component of a RGBA pixel value.
 *
 * \ingroup rgba_utility_functions
 */
inline void SetRed( RGBA& rgba, uint8 r )
{
   ClearRed( rgba ); rgba |= uint32( r ) << 16;
}

/*!
 * Sets the green color component of a RGBA pixel value.
 *
 * \ingroup rgba_utility_functions
 */
inline void SetGreen( RGBA& rgba, uint8 g )
{
   ClearGreen( rgba ); rgba |= uint32( g ) << 8;
}

/*!
 * Sets the blue color component of a RGBA pixel value.
 *
 * \ingroup rgba_utility_functions
 */
inline void SetBlue( RGBA& rgba, uint8 b )
{
   ClearBlue( rgba ); rgba |= uint32( b );
}

/*!
 * Returns a RGBA pixel value for the specified separate alpha and color
 * component integer values.
 *
 * \param r    Red color component
 * \param g    Green color component
 * \param b    Blue color component
 * \param a    Alpha  (transparency)
 *
 * All components must be in the range [0,255].
 *
 * \ingroup rgba_utility_functions
 */
inline RGBA RGBAColor( int r, int g, int b, int a )
{
   return (uint32( a ) << 24) | (uint32( r ) << 16) | (uint32( g ) << 8) | uint32( b );
}

/*!
 * Returns an opaque RGBA pixel value for the specified separate color
 * component integer values.
 *
 * \param r    Red color component
 * \param g    Green color component
 * \param b    Blue color component
 *
 * All components must be in the range [0,255]. The returned RGBA value has its
 * alpha component equal to 255 (opaque).
 *
 * \ingroup rgba_utility_functions
 */
inline RGBA RGBAColor( int r, int g, int b )
{
   return 0xff000000ul | (uint32( r ) << 16) | (uint32( g ) << 8) | uint32( b );
}

/*!
 * Returns a RGBA pixel value for the specified separate alpha and color
 * component real values.
 *
 * \param r    Red color component
 * \param g    Green color component
 * \param b    Blue color component
 * \param a    Alpha  (transparency)
 *
 * All components must be in the normalized range [0,1].
 *
 * \ingroup rgba_utility_functions
 */
inline RGBA RGBAColor( float r, float g, float b, float a )
{
   return RGBAColor( pcl::RoundI( 255*r ),
                     pcl::RoundI( 255*g ),
                     pcl::RoundI( 255*b ), pcl::RoundI( 255*a ) );
}

/*!
 * Returns an opaque RGBA pixel value for the specified separate color
 * component real values.
 *
 * \param r    Red color component
 * \param g    Green color component
 * \param b    Blue color component
 *
 * All components must be in the normalized range [0,1]. The returned RGBA
 * value has its alpha component equal to 255 (opaque).
 *
 * \ingroup rgba_utility_functions
 */
inline RGBA RGBAColor( float r, float g, float b )
{
   return RGBAColor( pcl::RoundI( 255*r ),
                     pcl::RoundI( 255*g ),
                     pcl::RoundI( 255*b ) );
}

/*!
 * Returns a RGBA pixel value corresponding to a CSS color specification
 * string.
 *
 * The string can be an hex-encoded color value in one of two valid formats:
 *
 * "#RRGGBB"
 * "#RRGGBBAA"
 *
 * or a valid CSS color name like "white", "black", "red", "orange", and so on.
 *
 * CSS color names are case-insensitive, and are returned with their alpha
 * components equal to 0xFF (opaque), except "transparent", which is returned
 * as 0x00000000 (also known as "transparent black").
 *
 * If the string cannot be converted to a valid color value, this function
 * silently ignores the error and returns zero.
 *
 * \ingroup rgba_utility_functions
 */
RGBA PCL_FUNC RGBAColor( const IsoString& );

template <class S>
inline RGBA PCL_FUNC RGBAColor( const S& s )
{
   return RGBAColor( IsoString( s ) );
}

/*!
 * An alias to RGBAColor( const IsoString& ).
 *
 * \ingroup rgba_utility_functions
 */
inline RGBA StringToRGBAColor( const IsoString& colorNameOrHex )
{
   return RGBAColor( colorNameOrHex );
}

template <class S>
inline RGBA StringToRGBAColor( const S& colorNameOrHex )
{
   return StringToRGBAColor( IsoString( colorNameOrHex ) );
}

/*!
 * Returns an hex-encoded string representation of a RGBA pixel value, ignoring
 * the alpha component.
 *
 * The returned string has the format "#RRGGBB".
 *
 * \ingroup rgba_utility_functions
 */
inline IsoString RGBColorToHexString( RGBA c )
{
   IsoString s;
   s.Format( "#%02X%02X%02X", Red( c ), Green( c ), Blue( c ) );
   return s;
}

/*!
 * Returns an hex-encoded string representation of a RGBA pixel value.
 *
 * The returned string has the format "#RRGGBBAA".
 *
 * \ingroup rgba_utility_functions
 */
inline IsoString RGBAColorToHexString( RGBA c )
{
   IsoString s;
   s.Format( "#%02X%02X%02X%02X", Red( c ), Green( c ), Blue( c ), Alpha( c ) );
   return s;
}

/*!
 * Returns the CSS color name corresponding to a RGBA pixel value.
 *
 * The alpha component is ignored, except for the special value 0x00000000,
 * which is returned as "Transparent". If the value does not have a CSS named
 * color counterpart, this function returns an empty string.
 *
 * \ingroup rgba_utility_functions
 */
IsoString PCL_FUNC CSSColorName( RGBA );

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Color_h

// ----------------------------------------------------------------------------
// EOF pcl/Color.h - Released 2015/10/08 11:24:12 UTC
