// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/ColorComboBox.cpp - Released 2014/11/14 17:17:01 UTC
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

#include <pcl/ColorComboBox.h>
#include <pcl/Bitmap.h>
#include <pcl/Graphics.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define ICONSIZE   16
#define ICONMARGIN  2

struct ComboColor
{
   RGBA   value;
   String name;
   Bitmap icon;

   ComboColor( RGBA, const String& = String() );

   ComboColor( const ComboColor& c ) : value( c.value ), name( c.name ), icon( c.icon )
   {
   }

   double H() const
   {
      int r = Red( value );
      int g = Green( value );
      int b = Blue( value );

      if ( r == g && r == b ) // if this is a gray color
         return -1;

      int max = Max( Max( r, g ), b );
      double delta = 255.0*(max - Min( Min( r, g ), b ));
      double h;

      if ( r == max )
         h = 60*(g - b)/delta;       // between yellow & magenta
      else if ( g == max )
         h = 120 + 60*(b - r)/delta; // between cyan & yellow
      else
         h = 240 + 60*(r - g)/delta; // between magenta & cyan

      if ( h < 0 )
         h += 360;

      return h;
   }

   int V() const
   {
      return Red( value ) + Green( value ) + Blue( value );
   }

   bool operator ==( const ComboColor& c ) const
   {
      return value == c.value;
   }

   bool operator <( const ComboColor& c ) const
   {
      double h = H();
      double ch = c.H();
      if ( h != ch )
         return h < ch;

      int v = V();
      int cv = c.V();
      if ( v != cv )
         return v < cv;

      return value < c.value;
   }
};

ComboColor::ComboColor( RGBA v, const String& n ) : value( v ), name( n ), icon( ICONSIZE, ICONSIZE )
{
   name = n;
   name.AppendFormat( " (%d,%d,%d)", Red( value ), Green( value ), Blue( value ) );

   // Make transparent
   icon.Fill( 0 );

   Graphics G( icon );
   G.SetPen( 0xFF000000 ); // Opaque black pen

   if ( value != 0 )
   {
      G.SetBrush( value );
      G.DrawRect( ICONMARGIN, ICONMARGIN, ICONSIZE-ICONMARGIN-1, ICONSIZE-ICONMARGIN-1 );
   }
   else
   {
      G.SetBrush( 0xFFFFFFFF );
      G.DrawRect( ICONMARGIN, ICONMARGIN, ICONSIZE-ICONMARGIN-1, ICONSIZE-ICONMARGIN-1 );
      G.DrawLine( ICONMARGIN, ICONMARGIN, ICONSIZE-ICONMARGIN, ICONSIZE-ICONMARGIN );
      G.DrawLine( ICONSIZE-ICONMARGIN+1, ICONMARGIN, ICONMARGIN+1, ICONSIZE-ICONMARGIN );
   }
}

typedef IndirectSortedArray<ComboColor>  combo_color_collection;

static combo_color_collection    comboColors;
static size_type                 numberOfColorComboBoxes = 0;

static void __InitComboColors()
{
   /* === This is an alternative short color list ===
   comboColors.Add( ComboColor( 0x00000000, "Default"   ) );
   comboColors.Add( ComboColor( 0xFF00FFFF, "Aqua"      ) );
   comboColors.Add( ComboColor( 0xFF000000, "Black"     ) );
   comboColors.Add( ComboColor( 0xFF0000FF, "Blue"      ) );
   comboColors.Add( ComboColor( 0xFF00FFFF, "Cyan"      ) );
   comboColors.Add( ComboColor( 0xFF808080, "Gray"      ) );
   comboColors.Add( ComboColor( 0xFF008000, "Green"     ) );
   comboColors.Add( ComboColor( 0xFFADD8E6, "LightBlue" ) );
   comboColors.Add( ComboColor( 0xFF00FF00, "Lime"      ) );
   comboColors.Add( ComboColor( 0xFFFF00FF, "Magenta"   ) );
   comboColors.Add( ComboColor( 0xFF800000, "Maroon"    ) );
   comboColors.Add( ComboColor( 0xFF000080, "Navy"      ) );
   comboColors.Add( ComboColor( 0xFF808000, "Olive"     ) );
   comboColors.Add( ComboColor( 0xFFFFA500, "Orange"    ) );
   comboColors.Add( ComboColor( 0xFF800080, "Purple"    ) );
   comboColors.Add( ComboColor( 0xFFFF0000, "Red"       ) );
   comboColors.Add( ComboColor( 0xFFC0C0C0, "Silver"    ) );
   comboColors.Add( ComboColor( 0xFF008080, "Teal"      ) );
   comboColors.Add( ComboColor( 0xFFEE82EE, "Violet"    ) );
   comboColors.Add( ComboColor( 0xFFFFFFFF, "White"     ) );
   comboColors.Add( ComboColor( 0xFFFFFF00, "Yellow"    ) );
   */

   comboColors.Add( new ComboColor( 0xFFF0F8FF, "AliceBlue"            ) );
   comboColors.Add( new ComboColor( 0xFFFAEBD7, "AntiqueWhite"         ) );
   comboColors.Add( new ComboColor( 0xFF00FFFF, "Aqua"                 ) );
   comboColors.Add( new ComboColor( 0xFF7FFFD4, "Aquamarine"           ) );
   comboColors.Add( new ComboColor( 0xFFF0FFFF, "Azure"                ) );
   comboColors.Add( new ComboColor( 0xFFF5F5DC, "Beige"                ) );
   comboColors.Add( new ComboColor( 0xFFFFE4C4, "Bisque"               ) );
   comboColors.Add( new ComboColor( 0xFF000000, "Black"                ) );
   comboColors.Add( new ComboColor( 0xFFFFEBCD, "BlanchedAlmond"       ) );
   comboColors.Add( new ComboColor( 0xFF0000FF, "Blue"                 ) );
   comboColors.Add( new ComboColor( 0xFF8A2BE2, "BlueViolet"           ) );
   comboColors.Add( new ComboColor( 0xFFA52A2A, "Brown"                ) );
   comboColors.Add( new ComboColor( 0xFFDEB887, "BurlyWood"            ) );
   comboColors.Add( new ComboColor( 0xFF5F9EA0, "CadetBlue"            ) );
   comboColors.Add( new ComboColor( 0xFF7FFF00, "Chartreuse"           ) );
   comboColors.Add( new ComboColor( 0xFFD2691E, "Chocolate"            ) );
   comboColors.Add( new ComboColor( 0xFFFF7F50, "Coral"                ) );
   comboColors.Add( new ComboColor( 0xFF6495ED, "CornflowerBlue"       ) );
   comboColors.Add( new ComboColor( 0xFFFFF8DC, "Cornsilk"             ) );
   comboColors.Add( new ComboColor( 0xFFDC143C, "Crimson"              ) );
   comboColors.Add( new ComboColor( 0xFF00FFFF, "Cyan"                 ) );
   comboColors.Add( new ComboColor( 0xFF00008B, "DarkBlue"             ) );
   comboColors.Add( new ComboColor( 0xFF008B8B, "DarkCyan"             ) );
   comboColors.Add( new ComboColor( 0xFFB8860B, "DarkGoldenRod"        ) );
   comboColors.Add( new ComboColor( 0xFFA9A9A9, "DarkGray"             ) );
   comboColors.Add( new ComboColor( 0xFF006400, "DarkGreen"            ) );
   comboColors.Add( new ComboColor( 0xFFBDB76B, "DarkKhaki"            ) );
   comboColors.Add( new ComboColor( 0xFF8B008B, "DarkMagenta"          ) );
   comboColors.Add( new ComboColor( 0xFF556B2F, "DarkOliveGreen"       ) );
   comboColors.Add( new ComboColor( 0xFFFF8C00, "DarkOrange"           ) );
   comboColors.Add( new ComboColor( 0xFF9932CC, "DarkOrchid"           ) );
   comboColors.Add( new ComboColor( 0xFF8B0000, "DarkRed"              ) );
   comboColors.Add( new ComboColor( 0xFFE9967A, "DarkSalmon"           ) );
   comboColors.Add( new ComboColor( 0xFF8FBC8F, "DarkSeaGreen"         ) );
   comboColors.Add( new ComboColor( 0xFF483D8B, "DarkSlateBlue"        ) );
   comboColors.Add( new ComboColor( 0xFF2F4F4F, "DarkSlateGray"        ) );
   comboColors.Add( new ComboColor( 0xFF00CED1, "DarkTurquoise"        ) );
   comboColors.Add( new ComboColor( 0xFF9400D3, "DarkViolet"           ) );
   comboColors.Add( new ComboColor( 0xFFFF1493, "DeepPink"             ) );
   comboColors.Add( new ComboColor( 0xFF00BFFF, "DeepSkyBlue"          ) );
   comboColors.Add( new ComboColor( 0xFF696969, "DimGray"              ) );
   comboColors.Add( new ComboColor( 0xFF1E90FF, "DodgerBlue"           ) );
   comboColors.Add( new ComboColor( 0xFFD19275, "Feldspar"             ) );
   comboColors.Add( new ComboColor( 0xFFB22222, "FireBrick"            ) );
   comboColors.Add( new ComboColor( 0xFFFFFAF0, "FloralWhite"          ) );
   comboColors.Add( new ComboColor( 0xFF228B22, "ForestGreen"          ) );
   comboColors.Add( new ComboColor( 0xFFFF00FF, "Fuchsia"              ) );
   comboColors.Add( new ComboColor( 0xFFDCDCDC, "Gainsboro"            ) );
   comboColors.Add( new ComboColor( 0xFFF8F8FF, "GhostWhite"           ) );
   comboColors.Add( new ComboColor( 0xFFFFD700, "Gold"                 ) );
   comboColors.Add( new ComboColor( 0xFFDAA520, "GoldenRod"            ) );
   comboColors.Add( new ComboColor( 0xFF808080, "Gray"                 ) );
   comboColors.Add( new ComboColor( 0xFF008000, "Green"                ) );
   comboColors.Add( new ComboColor( 0xFFADFF2F, "GreenYellow"          ) );
   comboColors.Add( new ComboColor( 0xFFF0FFF0, "HoneyDew"             ) );
   comboColors.Add( new ComboColor( 0xFFFF69B4, "HotPink"              ) );
   comboColors.Add( new ComboColor( 0xFFCD5C5C, "IndianRed"            ) );
   comboColors.Add( new ComboColor( 0xFF4B0082, "Indigo"               ) );
   comboColors.Add( new ComboColor( 0xFFFFFFF0, "Ivory"                ) );
   comboColors.Add( new ComboColor( 0xFFF0E68C, "Khaki"                ) );
   comboColors.Add( new ComboColor( 0xFFE6E6FA, "Lavender"             ) );
   comboColors.Add( new ComboColor( 0xFFFFF0F5, "LavenderBlush"        ) );
   comboColors.Add( new ComboColor( 0xFF7CFC00, "LawnGreen"            ) );
   comboColors.Add( new ComboColor( 0xFFFFFACD, "LemonChiffon"         ) );
   comboColors.Add( new ComboColor( 0xFFADD8E6, "LightBlue"            ) );
   comboColors.Add( new ComboColor( 0xFFF08080, "LightCoral"           ) );
   comboColors.Add( new ComboColor( 0xFFE0FFFF, "LightCyan"            ) );
   comboColors.Add( new ComboColor( 0xFFFAFAD2, "LightGoldenRodYellow" ) );
   comboColors.Add( new ComboColor( 0xFFD3D3D3, "LightGrey"            ) );
   comboColors.Add( new ComboColor( 0xFF90EE90, "LightGreen"           ) );
   comboColors.Add( new ComboColor( 0xFFFFB6C1, "LightPink"            ) );
   comboColors.Add( new ComboColor( 0xFFFFA07A, "LightSalmon"          ) );
   comboColors.Add( new ComboColor( 0xFF20B2AA, "LightSeaGreen"        ) );
   comboColors.Add( new ComboColor( 0xFF87CEFA, "LightSkyBlue"         ) );
   comboColors.Add( new ComboColor( 0xFF8470FF, "LightSlateBlue"       ) );
   comboColors.Add( new ComboColor( 0xFF778899, "LightSlateGray"       ) );
   comboColors.Add( new ComboColor( 0xFFB0C4DE, "LightSteelBlue"       ) );
   comboColors.Add( new ComboColor( 0xFFFFFFE0, "LightYellow"          ) );
   comboColors.Add( new ComboColor( 0xFF00FF00, "Lime"                 ) );
   comboColors.Add( new ComboColor( 0xFF32CD32, "LimeGreen"            ) );
   comboColors.Add( new ComboColor( 0xFFFAF0E6, "Linen"                ) );
   comboColors.Add( new ComboColor( 0xFFFF00FF, "Magenta"              ) );
   comboColors.Add( new ComboColor( 0xFF800000, "Maroon"               ) );
   comboColors.Add( new ComboColor( 0xFF66CDAA, "MediumAquaMarine"     ) );
   comboColors.Add( new ComboColor( 0xFF0000CD, "MediumBlue"           ) );
   comboColors.Add( new ComboColor( 0xFFBA55D3, "MediumOrchid"         ) );
   comboColors.Add( new ComboColor( 0xFF9370D8, "MediumPurple"         ) );
   comboColors.Add( new ComboColor( 0xFF3CB371, "MediumSeaGreen"       ) );
   comboColors.Add( new ComboColor( 0xFF7B68EE, "MediumSlateBlue"      ) );
   comboColors.Add( new ComboColor( 0xFF00FA9A, "MediumSpringGreen"    ) );
   comboColors.Add( new ComboColor( 0xFF48D1CC, "MediumTurquoise"      ) );
   comboColors.Add( new ComboColor( 0xFFC71585, "MediumVioletRed"      ) );
   comboColors.Add( new ComboColor( 0xFF191970, "MidnightBlue"         ) );
   comboColors.Add( new ComboColor( 0xFFF5FFFA, "MintCream"            ) );
   comboColors.Add( new ComboColor( 0xFFFFE4E1, "MistyRose"            ) );
   comboColors.Add( new ComboColor( 0xFFFFE4B5, "Moccasin"             ) );
   comboColors.Add( new ComboColor( 0xFFFFDEAD, "NavajoWhite"          ) );
   comboColors.Add( new ComboColor( 0xFF000080, "Navy"                 ) );
   comboColors.Add( new ComboColor( 0xFFFDF5E6, "OldLace"              ) );
   comboColors.Add( new ComboColor( 0xFF808000, "Olive"                ) );
   comboColors.Add( new ComboColor( 0xFF6B8E23, "OliveDrab"            ) );
   comboColors.Add( new ComboColor( 0xFFFFA500, "Orange"               ) );
   comboColors.Add( new ComboColor( 0xFFFF4500, "OrangeRed"            ) );
   comboColors.Add( new ComboColor( 0xFFDA70D6, "Orchid"               ) );
   comboColors.Add( new ComboColor( 0xFFEEE8AA, "PaleGoldenRod"        ) );
   comboColors.Add( new ComboColor( 0xFF98FB98, "PaleGreen"            ) );
   comboColors.Add( new ComboColor( 0xFFAFEEEE, "PaleTurquoise"        ) );
   comboColors.Add( new ComboColor( 0xFFD87093, "PaleVioletRed"        ) );
   comboColors.Add( new ComboColor( 0xFFFFEFD5, "PapayaWhip"           ) );
   comboColors.Add( new ComboColor( 0xFFFFDAB9, "PeachPuff"            ) );
   comboColors.Add( new ComboColor( 0xFFCD853F, "Peru"                 ) );
   comboColors.Add( new ComboColor( 0xFFFFC0CB, "Pink"                 ) );
   comboColors.Add( new ComboColor( 0xFFDDA0DD, "Plum"                 ) );
   comboColors.Add( new ComboColor( 0xFFB0E0E6, "PowderBlue"           ) );
   comboColors.Add( new ComboColor( 0xFF800080, "Purple"               ) );
   comboColors.Add( new ComboColor( 0xFFFF0000, "Red"                  ) );
   comboColors.Add( new ComboColor( 0xFFBC8F8F, "RosyBrown"            ) );
   comboColors.Add( new ComboColor( 0xFF4169E1, "RoyalBlue"            ) );
   comboColors.Add( new ComboColor( 0xFF8B4513, "SaddleBrown"          ) );
   comboColors.Add( new ComboColor( 0xFFFA8072, "Salmon"               ) );
   comboColors.Add( new ComboColor( 0xFFF4A460, "SandyBrown"           ) );
   comboColors.Add( new ComboColor( 0xFF2E8B57, "SeaGreen"             ) );
   comboColors.Add( new ComboColor( 0xFFFFF5EE, "SeaShell"             ) );
   comboColors.Add( new ComboColor( 0xFFA0522D, "Sienna"               ) );
   comboColors.Add( new ComboColor( 0xFFC0C0C0, "Silver"               ) );
   comboColors.Add( new ComboColor( 0xFF87CEEB, "SkyBlue"              ) );
   comboColors.Add( new ComboColor( 0xFF6A5ACD, "SlateBlue"            ) );
   comboColors.Add( new ComboColor( 0xFF708090, "SlateGray"            ) );
   comboColors.Add( new ComboColor( 0xFFFFFAFA, "Snow"                 ) );
   comboColors.Add( new ComboColor( 0xFF00FF7F, "SpringGreen"          ) );
   comboColors.Add( new ComboColor( 0xFF4682B4, "SteelBlue"            ) );
   comboColors.Add( new ComboColor( 0xFFD2B48C, "Tan"                  ) );
   comboColors.Add( new ComboColor( 0xFF008080, "Teal"                 ) );
   comboColors.Add( new ComboColor( 0xFFD8BFD8, "Thistle"              ) );
   comboColors.Add( new ComboColor( 0xFFFF6347, "Tomato"               ) );
   comboColors.Add( new ComboColor( 0xFF40E0D0, "Turquoise"            ) );
   comboColors.Add( new ComboColor( 0xFFEE82EE, "Violet"               ) );
   comboColors.Add( new ComboColor( 0xFFD02090, "VioletRed"            ) );
   comboColors.Add( new ComboColor( 0xFFF5DEB3, "Wheat"                ) );
   comboColors.Add( new ComboColor( 0xFFFFFFFF, "White"                ) );
   comboColors.Add( new ComboColor( 0xFFF5F5F5, "WhiteSmoke"           ) );
   comboColors.Add( new ComboColor( 0xFFFFFF00, "Yellow"               ) );
   comboColors.Add( new ComboColor( 0xFF9ACD32, "YellowGreen"          ) );
}

// ----------------------------------------------------------------------------

ColorComboBox::ColorComboBox( Control& parent ) :
ComboBox( parent ),
onColorSelected( 0 ), onColorSelectedReceiver( 0 ),
onColorHighlighted( 0 ), onColorHighlightedReceiver( 0 ),
onCustomColorDefined( 0 ), onCustomColorDefinedReceiver( 0 ),
customColor( 0 )
{
   ++numberOfColorComboBoxes;

   if ( comboColors.IsEmpty() )
      __InitComboColors();

   SetIconSize( ICONSIZE, ICONSIZE );

   for ( combo_color_collection::const_iterator i = comboColors.Begin(); i != comboColors.End(); ++i )
      AddItem( (*i)->name, (*i)->icon );

   OnItemSelected( (ComboBox::item_event_handler)&ColorComboBox::__ItemSelected, *this );
   OnItemHighlighted( (ComboBox::item_event_handler)&ColorComboBox::__ItemHighlighted, *this );
}

ColorComboBox::~ColorComboBox()
{
   if ( --numberOfColorComboBoxes == 0 )
      comboColors.Destroy();
}

RGBA ColorComboBox::CurrentColor() const
{
   int i = CurrentItem();
   if ( i >= NumberOfItems() )
      return customColor;
   return comboColors[i]->value;
}

void ColorComboBox::SetCurrentColor( RGBA color )
{
   color |= 0xff000000; // We only deal with opaque colors

   combo_color_collection::const_iterator i = comboColors.Search( ComboColor( color ) );
   if ( i != comboColors.End() )
      SetCurrentItem( i - comboColors.Begin() );
   else
   {
      if ( color != customColor )
      {
         if ( NumberOfItems() > int( comboColors.Length() ) )
            RemoveItem( NumberOfItems() - 1 );

         customColor = color;

         ComboColor C( customColor, "Custom" );
         AddItem( C.name, C.icon );

         if ( onCustomColorDefined != 0 )
            (onCustomColorDefinedReceiver->*onCustomColorDefined)( *this, customColor );
      }

      SetCurrentItem( NumberOfItems() - 1 );
   }
}

void ColorComboBox::OnColorSelected( color_event_handler f, Control& c )
{
   if ( f == 0 || c.IsNull() )
   {
      onColorSelected = 0;
      onColorSelectedReceiver = 0;
   }
   else
   {
      onColorSelected = f;
      onColorSelectedReceiver = &c;
   }
}

void ColorComboBox::OnColorHighlighted( color_event_handler f, Control& c )
{
   if ( f == 0 || c.IsNull() )
   {
      onColorHighlighted = 0;
      onColorHighlightedReceiver = 0;
   }
   else
   {
      onColorHighlighted = f;
      onColorHighlightedReceiver = &c;
   }
}

void ColorComboBox::OnCustomColorDefined( color_event_handler f, Control& c )
{
   if ( f == 0 || c.IsNull() )
   {
      onCustomColorDefined = 0;
      onCustomColorDefinedReceiver = 0;
   }
   else
   {
      onCustomColorDefined = f;
      onCustomColorDefinedReceiver = &c;
   }
}

void ColorComboBox::__ItemSelected( ComboBox& sender, int index )
{
   if ( onColorSelected != 0 )
      (onColorSelectedReceiver->*onColorSelected)( *this,
            (index < int( comboColors.Length() )) ? comboColors[index]->value : customColor );
}

void ColorComboBox::__ItemHighlighted( ComboBox& sender, int index )
{
   if ( onColorHighlighted != 0 )
      (onColorHighlightedReceiver->*onColorHighlighted)( *this,
            (index < int( comboColors.Length() )) ? comboColors[index]->value : customColor );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/ColorComboBox.cpp - Released 2014/11/14 17:17:01 UTC
