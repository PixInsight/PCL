//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/Color.cpp - Released 2017-06-28T11:58:42Z
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

#include <pcl/Array.h>
#include <pcl/Color.h>
#include <pcl/Relational.h>

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
   cssColors << CSSColor( "AliceBlue",              0xFFF0F8FF )
             << CSSColor( "AntiqueWhite",           0xFFFAEBD7 )
             << CSSColor( "Aqua",                   0xFF00FFFF )
             << CSSColor( "Aquamarine",             0xFF7FFFD4 )
             << CSSColor( "Azure",                  0xFFF0FFFF )
             << CSSColor( "Beige",                  0xFFF5F5DC )
             << CSSColor( "Bisque",                 0xFFFFE4C4 )
             << CSSColor( "Black",                  0xFF000000 )
             << CSSColor( "BlanchedAlmond",         0xFFFFEBCD )
             << CSSColor( "Blue",                   0xFF0000FF )
             << CSSColor( "BlueViolet",             0xFF8A2BE2 )
             << CSSColor( "Brown",                  0xFFA52A2A )
             << CSSColor( "BurlyWood",              0xFFDEB887 )
             << CSSColor( "CadetBlue",              0xFF5F9EA0 )
             << CSSColor( "Chartreuse",             0xFF7FFF00 )
             << CSSColor( "Chocolate",              0xFFD2691E )
             << CSSColor( "Coral",                  0xFFFF7F50 )
             << CSSColor( "CornflowerBlue",         0xFF6495ED )
             << CSSColor( "Cornsilk",               0xFFFFF8DC )
             << CSSColor( "Crimson",                0xFFDC143C )
             << CSSColor( "Cyan",                   0xFF00FFFF )
             << CSSColor( "DarkBlue",               0xFF00008B )
             << CSSColor( "DarkCyan",               0xFF008B8B )
             << CSSColor( "DarkGoldenRod",          0xFFB8860B )
             << CSSColor( "DarkGray",               0xFFA9A9A9 )
             << CSSColor( "DarkGreen",              0xFF006400 )
             << CSSColor( "DarkKhaki",              0xFFBDB76B )
             << CSSColor( "DarkMagenta",            0xFF8B008B )
             << CSSColor( "DarkOliveGreen",         0xFF556B2F )
             << CSSColor( "DarkOrange",             0xFFFF8C00 )
             << CSSColor( "DarkOrchid",             0xFF9932CC )
             << CSSColor( "DarkRed",                0xFF8B0000 )
             << CSSColor( "DarkSalmon",             0xFFE9967A )
             << CSSColor( "DarkSeaGreen",           0xFF8FBC8F )
             << CSSColor( "DarkSlateBlue",          0xFF483D8B )
             << CSSColor( "DarkSlateGray",          0xFF2F4F4F )
             << CSSColor( "DarkTurquoise",          0xFF00CED1 )
             << CSSColor( "DarkViolet",             0xFF9400D3 )
             << CSSColor( "DeepPink",               0xFFFF1493 )
             << CSSColor( "DeepSkyBlue",            0xFF00BFFF )
             << CSSColor( "DimGray",                0xFF696969 )
             << CSSColor( "DodgerBlue",             0xFF1E90FF )
             << CSSColor( "Feldspar",               0xFFD19275 )
             << CSSColor( "FireBrick",              0xFFB22222 )
             << CSSColor( "FloralWhite",            0xFFFFFAF0 )
             << CSSColor( "ForestGreen",            0xFF228B22 )
             << CSSColor( "Fuchsia",                0xFFFF00FF )
             << CSSColor( "Gainsboro",              0xFFDCDCDC )
             << CSSColor( "GhostWhite",             0xFFF8F8FF )
             << CSSColor( "Gold",                   0xFFFFD700 )
             << CSSColor( "GoldenRod",              0xFFDAA520 )
             << CSSColor( "Gray",                   0xFF808080 )
             << CSSColor( "Green",                  0xFF008000 )
             << CSSColor( "GreenYellow",            0xFFADFF2F )
             << CSSColor( "HoneyDew",               0xFFF0FFF0 )
             << CSSColor( "HotPink",                0xFFFF69B4 )
             << CSSColor( "IndianRed",              0xFFCD5C5C )
             << CSSColor( "Indigo",                 0xFF4B0082 )
             << CSSColor( "Ivory",                  0xFFFFFFF0 )
             << CSSColor( "Khaki",                  0xFFF0E68C )
             << CSSColor( "Lavender",               0xFFE6E6FA )
             << CSSColor( "LavenderBlush",          0xFFFFF0F5 )
             << CSSColor( "LawnGreen",              0xFF7CFC00 )
             << CSSColor( "LemonChiffon",           0xFFFFFACD )
             << CSSColor( "LightBlue",              0xFFADD8E6 )
             << CSSColor( "LightCoral",             0xFFF08080 )
             << CSSColor( "LightCyan",              0xFFE0FFFF )
             << CSSColor( "LightGoldenRodYellow",   0xFFFAFAD2 )
             << CSSColor( "LightGray",              0xFFD3D3D3 )
             << CSSColor( "LightGreen",             0xFF90EE90 )
             << CSSColor( "LightPink",              0xFFFFB6C1 )
             << CSSColor( "LightSalmon",            0xFFFFA07A )
             << CSSColor( "LightSeaGreen",          0xFF20B2AA )
             << CSSColor( "LightSkyBlue",           0xFF87CEFA )
             << CSSColor( "LightSlateBlue",         0xFF8470FF )
             << CSSColor( "LightSlateGray",         0xFF778899 )
             << CSSColor( "LightSteelBlue",         0xFFB0C4DE )
             << CSSColor( "LightYellow",            0xFFFFFFE0 )
             << CSSColor( "Lime",                   0xFF00FF00 )
             << CSSColor( "LimeGreen",              0xFF32CD32 )
             << CSSColor( "Linen",                  0xFFFAF0E6 )
             << CSSColor( "Magenta",                0xFFFF00FF )
             << CSSColor( "Maroon",                 0xFF800000 )
             << CSSColor( "MediumAquaMarine",       0xFF66CDAA )
             << CSSColor( "MediumBlue",             0xFF0000CD )
             << CSSColor( "MediumOrchid",           0xFFBA55D3 )
             << CSSColor( "MediumPurple",           0xFF9370D8 )
             << CSSColor( "MediumSeaGreen",         0xFF3CB371 )
             << CSSColor( "MediumSlateBlue",        0xFF7B68EE )
             << CSSColor( "MediumSpringGreen",      0xFF00FA9A )
             << CSSColor( "MediumTurquoise",        0xFF48D1CC )
             << CSSColor( "MediumVioletRed",        0xFFC71585 )
             << CSSColor( "MidnightBlue",           0xFF191970 )
             << CSSColor( "MintCream",              0xFFF5FFFA )
             << CSSColor( "MistyRose",              0xFFFFE4E1 )
             << CSSColor( "Moccasin",               0xFFFFE4B5 )
             << CSSColor( "NavajoWhite",            0xFFFFDEAD )
             << CSSColor( "Navy",                   0xFF000080 )
             << CSSColor( "OldLace",                0xFFFDF5E6 )
             << CSSColor( "Olive",                  0xFF808000 )
             << CSSColor( "OliveDrab",              0xFF6B8E23 )
             << CSSColor( "Orange",                 0xFFFFA500 )
             << CSSColor( "OrangeRed",              0xFFFF4500 )
             << CSSColor( "Orchid",                 0xFFDA70D6 )
             << CSSColor( "PaleGoldenRod",          0xFFEEE8AA )
             << CSSColor( "PaleGreen",              0xFF98FB98 )
             << CSSColor( "PaleTurquoise",          0xFFAFEEEE )
             << CSSColor( "PaleVioletRed",          0xFFD87093 )
             << CSSColor( "PapayaWhip",             0xFFFFEFD5 )
             << CSSColor( "PeachPuff",              0xFFFFDAB9 )
             << CSSColor( "Peru",                   0xFFCD853F )
             << CSSColor( "Pink",                   0xFFFFC0CB )
             << CSSColor( "Plum",                   0xFFDDA0DD )
             << CSSColor( "PowderBlue",             0xFFB0E0E6 )
             << CSSColor( "Purple",                 0xFF800080 )
             << CSSColor( "Red",                    0xFFFF0000 )
             << CSSColor( "RosyBrown",              0xFFBC8F8F )
             << CSSColor( "RoyalBlue",              0xFF4169E1 )
             << CSSColor( "SaddleBrown",            0xFF8B4513 )
             << CSSColor( "Salmon",                 0xFFFA8072 )
             << CSSColor( "SandyBrown",             0xFFF4A460 )
             << CSSColor( "SeaGreen",               0xFF2E8B57 )
             << CSSColor( "SeaShell",               0xFFFFF5EE )
             << CSSColor( "Sienna",                 0xFFA0522D )
             << CSSColor( "Silver",                 0xFFC0C0C0 )
             << CSSColor( "SkyBlue",                0xFF87CEEB )
             << CSSColor( "SlateBlue",              0xFF6A5ACD )
             << CSSColor( "SlateGray",              0xFF708090 )
             << CSSColor( "Snow",                   0xFFFFFAFA )
             << CSSColor( "SpringGreen",            0xFF00FF7F )
             << CSSColor( "SteelBlue",              0xFF4682B4 )
             << CSSColor( "Tan",                    0xFFD2B48C )
             << CSSColor( "Teal",                   0xFF008080 )
             << CSSColor( "Thistle",                0xFFD8BFD8 )
             << CSSColor( "Tomato",                 0xFFFF6347 )
             << CSSColor( "Transparent",            0x00000000 )
             << CSSColor( "Turquoise",              0xFF40E0D0 )
             << CSSColor( "Violet",                 0xFFEE82EE )
             << CSSColor( "VioletRed",              0xFFD02090 )
             << CSSColor( "Wheat",                  0xFFF5DEB3 )
             << CSSColor( "White",                  0xFFFFFFFF )
             << CSSColor( "WhiteSmoke",             0xFFF5F5F5 )
             << CSSColor( "Yellow",                 0xFFFFFF00 )
             << CSSColor( "YellowGreen",            0xFF9ACD32 );
}

// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------

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

   css_color_collection::const_iterator i = cssColors.Search( CSSColor( colorName.c_str(), 0 ) );
   return (i != cssColors.End()) ? (*i).value : 0;
}

// ----------------------------------------------------------------------------

IsoString CSSColorName( RGBA c )
{
   if ( c == 0 )
      return "Transparent";
   SetAlpha( c, 0xFF );
   for ( const CSSColor& color : cssColors )
      if ( color.value == c )
         return color.name;
   return IsoString();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Color.cpp - Released 2017-06-28T11:58:42Z
