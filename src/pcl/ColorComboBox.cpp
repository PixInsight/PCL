//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/ColorComboBox.cpp - Released 2019-01-21T12:06:21Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/Bitmap.h>
#include <pcl/ColorComboBox.h>
#include <pcl/Graphics.h>
#include <pcl/ReferenceSortedArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define ICONSIZE   16
#define ICONMARGIN  2

struct ComboColor
{
   RGBA   value;
   String name;

   ComboColor( RGBA v, const String& n = String() ) :
      value( v ),
      name( n )
   {
   }

   ComboColor( const ComboColor& ) = delete;
   ComboColor& operator =( const ComboColor& ) = delete;

   Bitmap Icon( int size, int margin ) const
   {
      Bitmap icon( size, size );
      icon.Fill( 0 ); // make transparent
      Graphics g( icon );
      g.EnableAntialiasing();
      g.SetPen( 0xFF000000, margin/2.0 ); // opaque black pen
      if ( value != 0 )
      {
         g.SetBrush( value );
         g.DrawRect( margin, margin, size-margin-1, size-margin-1 );
      }
      else
      {
         g.SetBrush( 0xFFFFFFFF );
         g.DrawRect( margin, margin, size-margin-1, size-margin-1 );
         g.DrawLine( margin, margin, size-margin, size-margin );
         g.DrawLine( size-margin+1, margin, margin+1, size-margin );
      }
      g.EndPaint();

      return icon;
   }

   String Title() const
   {
      return name;
      // + String().Format( " (%d,%d,%d)", Red( value ), Green( value ), Blue( value ) );
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

typedef ReferenceSortedArray<ComboColor> combo_color_collection;

static combo_color_collection s_comboColors;

static void InitializeComboColors()
{
   s_comboColors.Clear();

   /*
    * This is an alternative short color list
    *
   s_comboColors << new ComboColor( 0x00000000, "Default"   )
                 << new ComboColor( 0xFF00FFFF, "Aqua"      )
                 << new ComboColor( 0xFF000000, "Black"     )
                 << new ComboColor( 0xFF0000FF, "Blue"      )
                 << new ComboColor( 0xFF00FFFF, "Cyan"      )
                 << new ComboColor( 0xFF808080, "Gray"      )
                 << new ComboColor( 0xFF008000, "Green"     )
                 << new ComboColor( 0xFFADD8E6, "LightBlue" )
                 << new ComboColor( 0xFF00FF00, "Lime"      )
                 << new ComboColor( 0xFFFF00FF, "Magenta"   )
                 << new ComboColor( 0xFF800000, "Maroon"    )
                 << new ComboColor( 0xFF000080, "Navy"      )
                 << new ComboColor( 0xFF808000, "Olive"     )
                 << new ComboColor( 0xFFFFA500, "Orange"    )
                 << new ComboColor( 0xFF800080, "Purple"    )
                 << new ComboColor( 0xFFFF0000, "Red"       )
                 << new ComboColor( 0xFFC0C0C0, "Silver"    )
                 << new ComboColor( 0xFF008080, "Teal"      )
                 << new ComboColor( 0xFFEE82EE, "Violet"    )
                 << new ComboColor( 0xFFFFFFFF, "White"     )
                 << new ComboColor( 0xFFFFFF00, "Yellow"    );
    */

   s_comboColors << new ComboColor( 0xFFF0F8FF, "AliceBlue"            )
                 << new ComboColor( 0xFFFAEBD7, "AntiqueWhite"         )
                 << new ComboColor( 0xFF00FFFF, "Aqua"                 )
                 << new ComboColor( 0xFF7FFFD4, "Aquamarine"           )
                 << new ComboColor( 0xFFF0FFFF, "Azure"                )
                 << new ComboColor( 0xFFF5F5DC, "Beige"                )
                 << new ComboColor( 0xFFFFE4C4, "Bisque"               )
                 << new ComboColor( 0xFF000000, "Black"                )
                 << new ComboColor( 0xFFFFEBCD, "BlanchedAlmond"       )
                 << new ComboColor( 0xFF0000FF, "Blue"                 )
                 << new ComboColor( 0xFF8A2BE2, "BlueViolet"           )
                 << new ComboColor( 0xFFA52A2A, "Brown"                )
                 << new ComboColor( 0xFFDEB887, "BurlyWood"            )
                 << new ComboColor( 0xFF5F9EA0, "CadetBlue"            )
                 << new ComboColor( 0xFF7FFF00, "Chartreuse"           )
                 << new ComboColor( 0xFFD2691E, "Chocolate"            )
                 << new ComboColor( 0xFFFF7F50, "Coral"                )
                 << new ComboColor( 0xFF6495ED, "CornflowerBlue"       )
                 << new ComboColor( 0xFFFFF8DC, "Cornsilk"             )
                 << new ComboColor( 0xFFDC143C, "Crimson"              )
                 << new ComboColor( 0xFF00FFFF, "Cyan"                 )
                 << new ComboColor( 0xFF00008B, "DarkBlue"             )
                 << new ComboColor( 0xFF008B8B, "DarkCyan"             )
                 << new ComboColor( 0xFFB8860B, "DarkGoldenRod"        )
                 << new ComboColor( 0xFFA9A9A9, "DarkGray"             )
                 << new ComboColor( 0xFF006400, "DarkGreen"            )
                 << new ComboColor( 0xFFBDB76B, "DarkKhaki"            )
                 << new ComboColor( 0xFF8B008B, "DarkMagenta"          )
                 << new ComboColor( 0xFF556B2F, "DarkOliveGreen"       )
                 << new ComboColor( 0xFFFF8C00, "DarkOrange"           )
                 << new ComboColor( 0xFF9932CC, "DarkOrchid"           )
                 << new ComboColor( 0xFF8B0000, "DarkRed"              )
                 << new ComboColor( 0xFFE9967A, "DarkSalmon"           )
                 << new ComboColor( 0xFF8FBC8F, "DarkSeaGreen"         )
                 << new ComboColor( 0xFF483D8B, "DarkSlateBlue"        )
                 << new ComboColor( 0xFF2F4F4F, "DarkSlateGray"        )
                 << new ComboColor( 0xFF00CED1, "DarkTurquoise"        )
                 << new ComboColor( 0xFF9400D3, "DarkViolet"           )
                 << new ComboColor( 0xFFFF1493, "DeepPink"             )
                 << new ComboColor( 0xFF00BFFF, "DeepSkyBlue"          )
                 << new ComboColor( 0xFF696969, "DimGray"              )
                 << new ComboColor( 0xFF1E90FF, "DodgerBlue"           )
                 << new ComboColor( 0xFFD19275, "Feldspar"             )
                 << new ComboColor( 0xFFB22222, "FireBrick"            )
                 << new ComboColor( 0xFFFFFAF0, "FloralWhite"          )
                 << new ComboColor( 0xFF228B22, "ForestGreen"          )
                 << new ComboColor( 0xFFFF00FF, "Fuchsia"              )
                 << new ComboColor( 0xFFDCDCDC, "Gainsboro"            )
                 << new ComboColor( 0xFFF8F8FF, "GhostWhite"           )
                 << new ComboColor( 0xFFFFD700, "Gold"                 )
                 << new ComboColor( 0xFFDAA520, "GoldenRod"            )
                 << new ComboColor( 0xFF808080, "Gray"                 )
                 << new ComboColor( 0xFF008000, "Green"                )
                 << new ComboColor( 0xFFADFF2F, "GreenYellow"          )
                 << new ComboColor( 0xFFF0FFF0, "HoneyDew"             )
                 << new ComboColor( 0xFFFF69B4, "HotPink"              )
                 << new ComboColor( 0xFFCD5C5C, "IndianRed"            )
                 << new ComboColor( 0xFF4B0082, "Indigo"               )
                 << new ComboColor( 0xFFFFFFF0, "Ivory"                )
                 << new ComboColor( 0xFFF0E68C, "Khaki"                )
                 << new ComboColor( 0xFFE6E6FA, "Lavender"             )
                 << new ComboColor( 0xFFFFF0F5, "LavenderBlush"        )
                 << new ComboColor( 0xFF7CFC00, "LawnGreen"            )
                 << new ComboColor( 0xFFFFFACD, "LemonChiffon"         )
                 << new ComboColor( 0xFFADD8E6, "LightBlue"            )
                 << new ComboColor( 0xFFF08080, "LightCoral"           )
                 << new ComboColor( 0xFFE0FFFF, "LightCyan"            )
                 << new ComboColor( 0xFFFAFAD2, "LightGoldenRodYellow" )
                 << new ComboColor( 0xFFD3D3D3, "LightGrey"            )
                 << new ComboColor( 0xFF90EE90, "LightGreen"           )
                 << new ComboColor( 0xFFFFB6C1, "LightPink"            )
                 << new ComboColor( 0xFFFFA07A, "LightSalmon"          )
                 << new ComboColor( 0xFF20B2AA, "LightSeaGreen"        )
                 << new ComboColor( 0xFF87CEFA, "LightSkyBlue"         )
                 << new ComboColor( 0xFF8470FF, "LightSlateBlue"       )
                 << new ComboColor( 0xFF778899, "LightSlateGray"       )
                 << new ComboColor( 0xFFB0C4DE, "LightSteelBlue"       )
                 << new ComboColor( 0xFFFFFFE0, "LightYellow"          )
                 << new ComboColor( 0xFF00FF00, "Lime"                 )
                 << new ComboColor( 0xFF32CD32, "LimeGreen"            )
                 << new ComboColor( 0xFFFAF0E6, "Linen"                )
                 << new ComboColor( 0xFFFF00FF, "Magenta"              )
                 << new ComboColor( 0xFF800000, "Maroon"               )
                 << new ComboColor( 0xFF66CDAA, "MediumAquaMarine"     )
                 << new ComboColor( 0xFF0000CD, "MediumBlue"           )
                 << new ComboColor( 0xFFBA55D3, "MediumOrchid"         )
                 << new ComboColor( 0xFF9370D8, "MediumPurple"         )
                 << new ComboColor( 0xFF3CB371, "MediumSeaGreen"       )
                 << new ComboColor( 0xFF7B68EE, "MediumSlateBlue"      )
                 << new ComboColor( 0xFF00FA9A, "MediumSpringGreen"    )
                 << new ComboColor( 0xFF48D1CC, "MediumTurquoise"      )
                 << new ComboColor( 0xFFC71585, "MediumVioletRed"      )
                 << new ComboColor( 0xFF191970, "MidnightBlue"         )
                 << new ComboColor( 0xFFF5FFFA, "MintCream"            )
                 << new ComboColor( 0xFFFFE4E1, "MistyRose"            )
                 << new ComboColor( 0xFFFFE4B5, "Moccasin"             )
                 << new ComboColor( 0xFFFFDEAD, "NavajoWhite"          )
                 << new ComboColor( 0xFF000080, "Navy"                 )
                 << new ComboColor( 0xFFFDF5E6, "OldLace"              )
                 << new ComboColor( 0xFF808000, "Olive"                )
                 << new ComboColor( 0xFF6B8E23, "OliveDrab"            )
                 << new ComboColor( 0xFFFFA500, "Orange"               )
                 << new ComboColor( 0xFFFF4500, "OrangeRed"            )
                 << new ComboColor( 0xFFDA70D6, "Orchid"               )
                 << new ComboColor( 0xFFEEE8AA, "PaleGoldenRod"        )
                 << new ComboColor( 0xFF98FB98, "PaleGreen"            )
                 << new ComboColor( 0xFFAFEEEE, "PaleTurquoise"        )
                 << new ComboColor( 0xFFD87093, "PaleVioletRed"        )
                 << new ComboColor( 0xFFFFEFD5, "PapayaWhip"           )
                 << new ComboColor( 0xFFFFDAB9, "PeachPuff"            )
                 << new ComboColor( 0xFFCD853F, "Peru"                 )
                 << new ComboColor( 0xFFFFC0CB, "Pink"                 )
                 << new ComboColor( 0xFFDDA0DD, "Plum"                 )
                 << new ComboColor( 0xFFB0E0E6, "PowderBlue"           )
                 << new ComboColor( 0xFF800080, "Purple"               )
                 << new ComboColor( 0xFFFF0000, "Red"                  )
                 << new ComboColor( 0xFFBC8F8F, "RosyBrown"            )
                 << new ComboColor( 0xFF4169E1, "RoyalBlue"            )
                 << new ComboColor( 0xFF8B4513, "SaddleBrown"          )
                 << new ComboColor( 0xFFFA8072, "Salmon"               )
                 << new ComboColor( 0xFFF4A460, "SandyBrown"           )
                 << new ComboColor( 0xFF2E8B57, "SeaGreen"             )
                 << new ComboColor( 0xFFFFF5EE, "SeaShell"             )
                 << new ComboColor( 0xFFA0522D, "Sienna"               )
                 << new ComboColor( 0xFFC0C0C0, "Silver"               )
                 << new ComboColor( 0xFF87CEEB, "SkyBlue"              )
                 << new ComboColor( 0xFF6A5ACD, "SlateBlue"            )
                 << new ComboColor( 0xFF708090, "SlateGray"            )
                 << new ComboColor( 0xFFFFFAFA, "Snow"                 )
                 << new ComboColor( 0xFF00FF7F, "SpringGreen"          )
                 << new ComboColor( 0xFF4682B4, "SteelBlue"            )
                 << new ComboColor( 0xFFD2B48C, "Tan"                  )
                 << new ComboColor( 0xFF008080, "Teal"                 )
                 << new ComboColor( 0xFFD8BFD8, "Thistle"              )
                 << new ComboColor( 0xFFFF6347, "Tomato"               )
                 << new ComboColor( 0xFF40E0D0, "Turquoise"            )
                 << new ComboColor( 0xFFEE82EE, "Violet"               )
                 << new ComboColor( 0xFFD02090, "VioletRed"            )
                 << new ComboColor( 0xFFF5DEB3, "Wheat"                )
                 << new ComboColor( 0xFFFFFFFF, "White"                )
                 << new ComboColor( 0xFFF5F5F5, "WhiteSmoke"           )
                 << new ComboColor( 0xFFFFFF00, "Yellow"               )
                 << new ComboColor( 0xFF9ACD32, "YellowGreen"          );
}

// ----------------------------------------------------------------------------

ColorComboBox::ColorComboBox( Control& parent ) :
   ComboBox( parent ),
   m_customColor( 0 )
{
   if ( s_comboColors.IsEmpty() )
      InitializeComboColors();

   double f = DisplayPixelRatio();
   int size = RoundInt( f*ICONSIZE );
   int margin = RoundInt( f*ICONMARGIN );
   SetIconSize( size );
   for ( const ComboColor& color : s_comboColors )
      AddItem( color.Title(), color.Icon( size, margin ) );

   OnItemSelected( (ComboBox::item_event_handler)&ColorComboBox::ItemSelected, *this );
   OnItemHighlighted( (ComboBox::item_event_handler)&ColorComboBox::ItemHighlighted, *this );
}

// ----------------------------------------------------------------------------

RGBA ColorComboBox::CurrentColor() const
{
   int i = CurrentItem();
   if ( i >= NumberOfItems() )
      return m_customColor;
   return s_comboColors[i].value;
}

// ----------------------------------------------------------------------------

void ColorComboBox::SetCurrentColor( RGBA color )
{
   color |= 0xff000000; // We only deal with opaque colors

   combo_color_collection::const_iterator i = s_comboColors.Search( ComboColor( color ) );
   if ( i != s_comboColors.End() )
      SetCurrentItem( i - s_comboColors.Begin() );
   else
   {
      if ( color != m_customColor )
      {
         if ( NumberOfItems() > int( s_comboColors.Length() ) )
            RemoveItem( NumberOfItems()-1 );

         m_customColor = color;

         double f = DisplayPixelRatio();
         ComboColor C( m_customColor, String().Format( "Custom (%d,%d,%d)",
                           Red( m_customColor ), Green( m_customColor ), Blue( m_customColor ) ) );
         AddItem( C.Title(), C.Icon( RoundInt( f*ICONSIZE ), RoundInt( f*ICONMARGIN ) ) );

         if ( !m_handlers.IsNull() )
            if ( m_handlers->onCustomColorDefined != nullptr )
               (m_handlers->onCustomColorDefinedReceiver->*m_handlers->onCustomColorDefined)( *this, m_customColor );
      }

      SetCurrentItem( NumberOfItems()-1 );
   }
}

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

// ----------------------------------------------------------------------------

void ColorComboBox::OnColorSelected( color_event_handler f, Control& c )
{
   if ( f == nullptr || c.IsNull() )
   {
      if ( !m_handlers.IsNull() )
      {
         m_handlers->onColorSelected = nullptr;
         m_handlers->onColorSelectedReceiver = nullptr;
      }
   }
   else
   {
      INIT_EVENT_HANDLERS();
      m_handlers->onColorSelected = f;
      m_handlers->onColorSelectedReceiver = &c;
   }
}

// ----------------------------------------------------------------------------

void ColorComboBox::OnColorHighlighted( color_event_handler f, Control& c )
{
   if ( f == nullptr || c.IsNull() )
   {
      if ( !m_handlers.IsNull() )
      {
         m_handlers->onColorHighlighted = nullptr;
         m_handlers->onColorHighlightedReceiver = nullptr;
      }
   }
   else
   {
      INIT_EVENT_HANDLERS();
      m_handlers->onColorHighlighted = f;
      m_handlers->onColorHighlightedReceiver = &c;
   }
}

// ----------------------------------------------------------------------------

void ColorComboBox::OnCustomColorDefined( color_event_handler f, Control& c )
{
   if ( f == nullptr || c.IsNull() )
   {
      if ( !m_handlers.IsNull() )
      {
         m_handlers->onCustomColorDefined = nullptr;
         m_handlers->onCustomColorDefinedReceiver = nullptr;
      }
   }
   else
   {
      INIT_EVENT_HANDLERS();
      m_handlers->onCustomColorDefined = f;
      m_handlers->onCustomColorDefinedReceiver = &c;
   }
}

// ----------------------------------------------------------------------------

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

void ColorComboBox::ItemSelected( ComboBox& sender, int index )
{
   if ( !m_handlers.IsNull() )
      if ( m_handlers->onColorSelected != nullptr )
         (m_handlers->onColorSelectedReceiver->*m_handlers->onColorSelected)( *this,
               (index < int( s_comboColors.Length() )) ? s_comboColors[index].value : m_customColor );
}

// ----------------------------------------------------------------------------

void ColorComboBox::ItemHighlighted( ComboBox& sender, int index )
{
   if ( !m_handlers.IsNull() )
      if ( m_handlers->onColorHighlighted != nullptr )
         (m_handlers->onColorHighlightedReceiver->*m_handlers->onColorHighlighted)( *this,
               (index < int( s_comboColors.Length() )) ? s_comboColors[index].value : m_customColor );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ColorComboBox.cpp - Released 2019-01-21T12:06:21Z
