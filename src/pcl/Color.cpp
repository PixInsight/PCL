//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// pcl/Color.cpp - Released 2015/11/26 15:59:45 UTC
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

#include <pcl/Color.h>
#include <pcl/Relational.h>
#include <pcl/Array.h>

#ifdef __PCL_WINDOWS
#define STRCASECMP   ::stricmp
#else
#define STRCASECMP   ::strcasecmp
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

struct CSSColor
{
   const char* name;
   RGBA        value;

   CSSColor( const char* n, RGBA v ) : name( n ), value( v )
   {
   }

   CSSColor( const CSSColor& c ) : name( c.name ), value( c.value )
   {
   }

   bool operator ==( const CSSColor& c ) const
   {
      return STRCASECMP( name, c.name ) == 0;
   }

   bool operator <( const CSSColor& c ) const
   {
      return STRCASECMP( name, c.name ) < 0;
   }
};

typedef SortedArray<CSSColor> css_color_collection;

static css_color_collection   cssColors;

static void InitCSSColors()
{
   cssColors.Add( CSSColor( "AliceBlue",              0xFFF0F8FF ) );
   cssColors.Add( CSSColor( "AntiqueWhite",           0xFFFAEBD7 ) );
   cssColors.Add( CSSColor( "Aqua",                   0xFF00FFFF ) );
   cssColors.Add( CSSColor( "Aquamarine",             0xFF7FFFD4 ) );
   cssColors.Add( CSSColor( "Azure",                  0xFFF0FFFF ) );
   cssColors.Add( CSSColor( "Beige",                  0xFFF5F5DC ) );
   cssColors.Add( CSSColor( "Bisque",                 0xFFFFE4C4 ) );
   cssColors.Add( CSSColor( "Black",                  0xFF000000 ) );
   cssColors.Add( CSSColor( "BlanchedAlmond",         0xFFFFEBCD ) );
   cssColors.Add( CSSColor( "Blue",                   0xFF0000FF ) );
   cssColors.Add( CSSColor( "BlueViolet",             0xFF8A2BE2 ) );
   cssColors.Add( CSSColor( "Brown",                  0xFFA52A2A ) );
   cssColors.Add( CSSColor( "BurlyWood",              0xFFDEB887 ) );
   cssColors.Add( CSSColor( "CadetBlue",              0xFF5F9EA0 ) );
   cssColors.Add( CSSColor( "Chartreuse",             0xFF7FFF00 ) );
   cssColors.Add( CSSColor( "Chocolate",              0xFFD2691E ) );
   cssColors.Add( CSSColor( "Coral",                  0xFFFF7F50 ) );
   cssColors.Add( CSSColor( "CornflowerBlue",         0xFF6495ED ) );
   cssColors.Add( CSSColor( "Cornsilk",               0xFFFFF8DC ) );
   cssColors.Add( CSSColor( "Crimson",                0xFFDC143C ) );
   cssColors.Add( CSSColor( "Cyan",                   0xFF00FFFF ) );
   cssColors.Add( CSSColor( "DarkBlue",               0xFF00008B ) );
   cssColors.Add( CSSColor( "DarkCyan",               0xFF008B8B ) );
   cssColors.Add( CSSColor( "DarkGoldenRod",          0xFFB8860B ) );
   cssColors.Add( CSSColor( "DarkGray",               0xFFA9A9A9 ) );
   cssColors.Add( CSSColor( "DarkGreen",              0xFF006400 ) );
   cssColors.Add( CSSColor( "DarkKhaki",              0xFFBDB76B ) );
   cssColors.Add( CSSColor( "DarkMagenta",            0xFF8B008B ) );
   cssColors.Add( CSSColor( "DarkOliveGreen",         0xFF556B2F ) );
   cssColors.Add( CSSColor( "DarkOrange",             0xFFFF8C00 ) );
   cssColors.Add( CSSColor( "DarkOrchid",             0xFF9932CC ) );
   cssColors.Add( CSSColor( "DarkRed",                0xFF8B0000 ) );
   cssColors.Add( CSSColor( "DarkSalmon",             0xFFE9967A ) );
   cssColors.Add( CSSColor( "DarkSeaGreen",           0xFF8FBC8F ) );
   cssColors.Add( CSSColor( "DarkSlateBlue",          0xFF483D8B ) );
   cssColors.Add( CSSColor( "DarkSlateGray",          0xFF2F4F4F ) );
   cssColors.Add( CSSColor( "DarkTurquoise",          0xFF00CED1 ) );
   cssColors.Add( CSSColor( "DarkViolet",             0xFF9400D3 ) );
   cssColors.Add( CSSColor( "DeepPink",               0xFFFF1493 ) );
   cssColors.Add( CSSColor( "DeepSkyBlue",            0xFF00BFFF ) );
   cssColors.Add( CSSColor( "DimGray",                0xFF696969 ) );
   cssColors.Add( CSSColor( "DodgerBlue",             0xFF1E90FF ) );
   cssColors.Add( CSSColor( "Feldspar",               0xFFD19275 ) );
   cssColors.Add( CSSColor( "FireBrick",              0xFFB22222 ) );
   cssColors.Add( CSSColor( "FloralWhite",            0xFFFFFAF0 ) );
   cssColors.Add( CSSColor( "ForestGreen",            0xFF228B22 ) );
   cssColors.Add( CSSColor( "Fuchsia",                0xFFFF00FF ) );
   cssColors.Add( CSSColor( "Gainsboro",              0xFFDCDCDC ) );
   cssColors.Add( CSSColor( "GhostWhite",             0xFFF8F8FF ) );
   cssColors.Add( CSSColor( "Gold",                   0xFFFFD700 ) );
   cssColors.Add( CSSColor( "GoldenRod",              0xFFDAA520 ) );
   cssColors.Add( CSSColor( "Gray",                   0xFF808080 ) );
   cssColors.Add( CSSColor( "Green",                  0xFF008000 ) );
   cssColors.Add( CSSColor( "GreenYellow",            0xFFADFF2F ) );
   cssColors.Add( CSSColor( "HoneyDew",               0xFFF0FFF0 ) );
   cssColors.Add( CSSColor( "HotPink",                0xFFFF69B4 ) );
   cssColors.Add( CSSColor( "IndianRed",              0xFFCD5C5C ) );
   cssColors.Add( CSSColor( "Indigo",                 0xFF4B0082 ) );
   cssColors.Add( CSSColor( "Ivory",                  0xFFFFFFF0 ) );
   cssColors.Add( CSSColor( "Khaki",                  0xFFF0E68C ) );
   cssColors.Add( CSSColor( "Lavender",               0xFFE6E6FA ) );
   cssColors.Add( CSSColor( "LavenderBlush",          0xFFFFF0F5 ) );
   cssColors.Add( CSSColor( "LawnGreen",              0xFF7CFC00 ) );
   cssColors.Add( CSSColor( "LemonChiffon",           0xFFFFFACD ) );
   cssColors.Add( CSSColor( "LightBlue",              0xFFADD8E6 ) );
   cssColors.Add( CSSColor( "LightCoral",             0xFFF08080 ) );
   cssColors.Add( CSSColor( "LightCyan",              0xFFE0FFFF ) );
   cssColors.Add( CSSColor( "LightGoldenRodYellow",   0xFFFAFAD2 ) );
   cssColors.Add( CSSColor( "LightGray",              0xFFD3D3D3 ) );
   cssColors.Add( CSSColor( "LightGreen",             0xFF90EE90 ) );
   cssColors.Add( CSSColor( "LightPink",              0xFFFFB6C1 ) );
   cssColors.Add( CSSColor( "LightSalmon",            0xFFFFA07A ) );
   cssColors.Add( CSSColor( "LightSeaGreen",          0xFF20B2AA ) );
   cssColors.Add( CSSColor( "LightSkyBlue",           0xFF87CEFA ) );
   cssColors.Add( CSSColor( "LightSlateBlue",         0xFF8470FF ) );
   cssColors.Add( CSSColor( "LightSlateGray",         0xFF778899 ) );
   cssColors.Add( CSSColor( "LightSteelBlue",         0xFFB0C4DE ) );
   cssColors.Add( CSSColor( "LightYellow",            0xFFFFFFE0 ) );
   cssColors.Add( CSSColor( "Lime",                   0xFF00FF00 ) );
   cssColors.Add( CSSColor( "LimeGreen",              0xFF32CD32 ) );
   cssColors.Add( CSSColor( "Linen",                  0xFFFAF0E6 ) );
   cssColors.Add( CSSColor( "Magenta",                0xFFFF00FF ) );
   cssColors.Add( CSSColor( "Maroon",                 0xFF800000 ) );
   cssColors.Add( CSSColor( "MediumAquaMarine",       0xFF66CDAA ) );
   cssColors.Add( CSSColor( "MediumBlue",             0xFF0000CD ) );
   cssColors.Add( CSSColor( "MediumOrchid",           0xFFBA55D3 ) );
   cssColors.Add( CSSColor( "MediumPurple",           0xFF9370D8 ) );
   cssColors.Add( CSSColor( "MediumSeaGreen",         0xFF3CB371 ) );
   cssColors.Add( CSSColor( "MediumSlateBlue",        0xFF7B68EE ) );
   cssColors.Add( CSSColor( "MediumSpringGreen",      0xFF00FA9A ) );
   cssColors.Add( CSSColor( "MediumTurquoise",        0xFF48D1CC ) );
   cssColors.Add( CSSColor( "MediumVioletRed",        0xFFC71585 ) );
   cssColors.Add( CSSColor( "MidnightBlue",           0xFF191970 ) );
   cssColors.Add( CSSColor( "MintCream",              0xFFF5FFFA ) );
   cssColors.Add( CSSColor( "MistyRose",              0xFFFFE4E1 ) );
   cssColors.Add( CSSColor( "Moccasin",               0xFFFFE4B5 ) );
   cssColors.Add( CSSColor( "NavajoWhite",            0xFFFFDEAD ) );
   cssColors.Add( CSSColor( "Navy",                   0xFF000080 ) );
   cssColors.Add( CSSColor( "OldLace",                0xFFFDF5E6 ) );
   cssColors.Add( CSSColor( "Olive",                  0xFF808000 ) );
   cssColors.Add( CSSColor( "OliveDrab",              0xFF6B8E23 ) );
   cssColors.Add( CSSColor( "Orange",                 0xFFFFA500 ) );
   cssColors.Add( CSSColor( "OrangeRed",              0xFFFF4500 ) );
   cssColors.Add( CSSColor( "Orchid",                 0xFFDA70D6 ) );
   cssColors.Add( CSSColor( "PaleGoldenRod",          0xFFEEE8AA ) );
   cssColors.Add( CSSColor( "PaleGreen",              0xFF98FB98 ) );
   cssColors.Add( CSSColor( "PaleTurquoise",          0xFFAFEEEE ) );
   cssColors.Add( CSSColor( "PaleVioletRed",          0xFFD87093 ) );
   cssColors.Add( CSSColor( "PapayaWhip",             0xFFFFEFD5 ) );
   cssColors.Add( CSSColor( "PeachPuff",              0xFFFFDAB9 ) );
   cssColors.Add( CSSColor( "Peru",                   0xFFCD853F ) );
   cssColors.Add( CSSColor( "Pink",                   0xFFFFC0CB ) );
   cssColors.Add( CSSColor( "Plum",                   0xFFDDA0DD ) );
   cssColors.Add( CSSColor( "PowderBlue",             0xFFB0E0E6 ) );
   cssColors.Add( CSSColor( "Purple",                 0xFF800080 ) );
   cssColors.Add( CSSColor( "Red",                    0xFFFF0000 ) );
   cssColors.Add( CSSColor( "RosyBrown",              0xFFBC8F8F ) );
   cssColors.Add( CSSColor( "RoyalBlue",              0xFF4169E1 ) );
   cssColors.Add( CSSColor( "SaddleBrown",            0xFF8B4513 ) );
   cssColors.Add( CSSColor( "Salmon",                 0xFFFA8072 ) );
   cssColors.Add( CSSColor( "SandyBrown",             0xFFF4A460 ) );
   cssColors.Add( CSSColor( "SeaGreen",               0xFF2E8B57 ) );
   cssColors.Add( CSSColor( "SeaShell",               0xFFFFF5EE ) );
   cssColors.Add( CSSColor( "Sienna",                 0xFFA0522D ) );
   cssColors.Add( CSSColor( "Silver",                 0xFFC0C0C0 ) );
   cssColors.Add( CSSColor( "SkyBlue",                0xFF87CEEB ) );
   cssColors.Add( CSSColor( "SlateBlue",              0xFF6A5ACD ) );
   cssColors.Add( CSSColor( "SlateGray",              0xFF708090 ) );
   cssColors.Add( CSSColor( "Snow",                   0xFFFFFAFA ) );
   cssColors.Add( CSSColor( "SpringGreen",            0xFF00FF7F ) );
   cssColors.Add( CSSColor( "SteelBlue",              0xFF4682B4 ) );
   cssColors.Add( CSSColor( "Tan",                    0xFFD2B48C ) );
   cssColors.Add( CSSColor( "Teal",                   0xFF008080 ) );
   cssColors.Add( CSSColor( "Thistle",                0xFFD8BFD8 ) );
   cssColors.Add( CSSColor( "Tomato",                 0xFFFF6347 ) );
   cssColors.Add( CSSColor( "Transparent",            0x00000000 ) );
   cssColors.Add( CSSColor( "Turquoise",              0xFF40E0D0 ) );
   cssColors.Add( CSSColor( "Violet",                 0xFFEE82EE ) );
   cssColors.Add( CSSColor( "VioletRed",              0xFFD02090 ) );
   cssColors.Add( CSSColor( "Wheat",                  0xFFF5DEB3 ) );
   cssColors.Add( CSSColor( "White",                  0xFFFFFFFF ) );
   cssColors.Add( CSSColor( "WhiteSmoke",             0xFFF5F5F5 ) );
   cssColors.Add( CSSColor( "Yellow",                 0xFFFFFF00 ) );
   cssColors.Add( CSSColor( "YellowGreen",            0xFF9ACD32 ) );
}

static inline int HexDigitValue( char c )
{
   if ( c >= '0' && c <= '9' )
      return c - '0';
   if ( c >= 'A' && c <= 'F' )
      return c - 'A' + 10;
   if ( c >= 'a' && c <= 'f' )
      return c - 'a' + 10;
   return -1;
}

static inline int HexNibbleValue( const char* c )
{
   int h = HexDigitValue( c[0] );
   if ( h < 0 )
      return -1;

   int l = HexDigitValue( c[1] );
   if ( l < 0 )
      return -1;

   return (h << 4) + l;
}

RGBA RGBAColor( const IsoString& colorName )
{
   if ( colorName.StartsWith( '#' ) )
   {
      size_type n = colorName.Length();

      int r, g, b, a;

      if ( n == 9 )        // "#RRGGBBAA" format
      {
         r = HexNibbleValue( colorName.Begin()+1 );
         g = HexNibbleValue( colorName.Begin()+3 );
         b = HexNibbleValue( colorName.Begin()+5 );
         a = HexNibbleValue( colorName.Begin()+7 );
      }
      else if ( n == 7 )   // "#RRGGBB" format
      {
         r = HexNibbleValue( colorName.Begin()+1 );
         g = HexNibbleValue( colorName.Begin()+3 );
         b = HexNibbleValue( colorName.Begin()+5 );
         a = 0xFF;
      }
      else
         return 0;

      if ( r < 0 || g < 0 || b < 0 || a < 0 )
         return 0;

      return RGBAColor( r, g, b, a );
   }

   if ( cssColors.IsEmpty() )
      InitCSSColors();

   css_color_collection::const_iterator i =
      cssColors.Search( CSSColor( colorName.c_str(), 0 ) );

   return (i != cssColors.End()) ? (*i).value : 0;
}

// ----------------------------------------------------------------------------

IsoString CSSColorName( RGBA c )
{
   if ( c == 0 )
      return "Transparent";

   SetAlpha( c, 0xFF );

   for ( css_color_collection::const_iterator i = cssColors.Begin();
         i != cssColors.End();
         ++i )
   {
      if ( (*i).value == c )
         return (*i).name;
   }

   return IsoString();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Color.cpp - Released 2015/11/26 15:59:45 UTC
