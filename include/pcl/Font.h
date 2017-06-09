//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0837
// ----------------------------------------------------------------------------
// pcl/Font.h - Released 2017-06-09T08:12:42Z
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

#ifndef __PCL_Font_h
#define __PCL_Font_h

/// \file pcl/Font.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Rectangle.h>
#include <pcl/StringList.h>
#include <pcl/UIObject.h>

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::FontFamily
 * \brief Platform-independent, standard font families
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>FontFamily::Default</td>    <td>Default font</td></tr>
 * <tr><td>FontFamily::SansSerif</td>  <td>Sans serif font: Helvetica, Swiss, Arial</td></tr>
 * <tr><td>FontFamily::Helvetica</td>  <td>Equivalent to SansSerif</td></tr>
 * <tr><td>FontFamily::Swiss</td>      <td>Equivalent to SansSerif</td></tr>
 * <tr><td>FontFamily::Serif</td>      <td>Serif font: Times, Garamond</td></tr>
 * <tr><td>FontFamily::Times</td>      <td>Equivalent to Serif</td></tr>
 * <tr><td>FontFamily::Script</td>     <td>Handwriting font: Script, Comic</td></tr>
 * <tr><td>FontFamily::Monospace</td>  <td>Fixed-pitch font: Courier, Vera Sans Mono</td></tr>
 * <tr><td>FontFamily::TypeWriter</td> <td>Equivalent to Monospace</td></tr>
 * <tr><td>FontFamily::Courier</td>    <td>Equivalent to Monospace</td></tr>
 * <tr><td>FontFamily::Teletype</td>   <td>Equivalent to Monospace</td></tr>
 * <tr><td>FontFamily::Decorative</td> <td>Decorative font: OldEnglish</td></tr>
 * <tr><td>FontFamily::OldEnglish</td> <td>Equivalent to Decorative</td></tr>
 * <tr><td>FontFamily::Symbol</td>     <td>Math, greek, etc: Symbol</td></tr>
 * </table>
 */
namespace FontFamily
{
   enum value_type
   {
      Default,       // default font
      SansSerif,     // sans serif font: Helvetica, Swiss, Arial
      Helvetica  = SansSerif,
      Swiss      = SansSerif,
      Serif,         // serif font: Times, Garamond
      Times      = Serif,
      Script,        // handwriting font: Script, Comic
      Monospace,     // fixed-pitch font: Courier, Vera Sans Mono
      TypeWriter = Monospace,
      Courier    = Monospace,
      Teletype   = Monospace,
      Decorative,    // decorative font: OldEnglish,
      OldEnglish = Decorative,
      Symbol         // math, greek, etc.: Symbol
   };

   String PCL_FUNC FamilyToFace( value_type family );
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::FontWeight
 * \brief Standard font weights
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>FontWeight::Thin</td>       <td>100</td></tr>
 * <tr><td>FontWeight::ExtraLight</td> <td>200</td></tr>
 * <tr><td>FontWeight::UltraLight</td> <td>= ExtraLight</td></tr>
 * <tr><td>FontWeight::Light</td>      <td>300</td></tr>
 * <tr><td>FontWeight::Normal</td>     <td>400</td></tr>
 * <tr><td>FontWeight::Regular</td>    <td>= Normal</td></tr>
 * <tr><td>FontWeight::Medium</td>     <td>500</td></tr>
 * <tr><td>FontWeight::SemiBold</td>   <td>600</td></tr>
 * <tr><td>FontWeight::DemiBold</td>   <td>= SemiBold</td></tr>
 * <tr><td>FontWeight::Bold</td>       <td>700</td></tr>
 * <tr><td>FontWeight::ExtraBold</td>  <td>800</td></tr>
 * <tr><td>FontWeight::UltraBold</td>  <td>= ExtraBold</td></tr>
 * <tr><td>FontWeight::Heavy</td>      <td>850</td></tr>
 * <tr><td>FontWeight::Black</td>      <td>900</td></tr>
 * </table>
 */
namespace FontWeight
{
   enum value_type
   {
      Thin           =  100,
      ExtraLight     =  200,
      UltraLight     =  ExtraLight,
      Light          =  300,
      Normal         =  400,
      Regular        =  Normal,
      Medium         =  500,
      SemiBold       =  600,
      DemiBold       =  SemiBold,
      Bold           =  700,
      ExtraBold      =  800,
      UltraBold      =  ExtraBold,
      Heavy          =  850,
      Black          =  900
   };
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::FontStretch
 * \brief Standard font stretchs
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>FontStretch::UltraCondensed</td> <td>50</td></tr>
 * <tr><td>FontStretch::ExtraCondensed</td> <td>62</td></tr>
 * <tr><td>FontStretch::Condensed</td>      <td>75</td></tr>
 * <tr><td>FontStretch::SemiCondensed</td>  <td>87</td></tr>
 * <tr><td>FontStretch::Unstretched</td>    <td>100</td></tr>
 * <tr><td>FontStretch::SemiExpanded</td>   <td>112</td></tr>
 * <tr><td>FontStretch::Expanded</td>       <td>125</td></tr>
 * <tr><td>FontStretch::ExtraExpanded</td>  <td>150</td></tr>
 * <tr><td>FontStretch::UltraExpanded</td>  <td>200</td></tr>
 * </table>
 */
namespace FontStretch
{
   enum value_type
   {
      UltraCondensed =  50,
      ExtraCondensed =  62,
      Condensed      =  75,
      SemiCondensed  =  87,
      Unstretched    = 100,
      SemiExpanded   = 112,
      Expanded       = 125,
      ExtraExpanded  = 150,
      UltraExpanded  = 200
   };
}

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

// ----------------------------------------------------------------------------

/*!
 * \class Font
 * \brief Client-side interface to a PixInsight %Font object
 *
 * ### TODO: Write a detailed description for %Font.
 */
class PCL_CLASS Font : public UIObject
{
public:

   /*!
    * Represents a standard font family.
    */
   typedef FontFamily::value_type      family;

   /*!
    * Represents a standard font weight.
    */
   typedef FontWeight::value_type      std_weight;

   /*!
    * Represents a standard font stretch.
    */
   typedef FontStretch::value_type     std_stretch;

   /*!
    * Constructs a %Font object with the specified family and \a size in
    * points.
    *
    * ### TODO: Elaborate on font families, font matching, etc.
    */
   Font( family f = FontFamily::Default, double size = 12.0 );

   /*!
    * Constructs a %Font object with the specified font \a face and \a size
    * in points.
    *
    * ### TODO: Elaborate on font faces, font matching, etc.
    */
   Font( const String& face, double size = 12.0 );

   /*!
    * Copy constructor. This object will reference the same server-side font
    * as the specified instance \a f.
    */
   Font( const Font& f ) : UIObject( f )
   {
   }

   /*!
    * Move constructor.
    */
   Font( Font&& x ) : UIObject( std::move( x ) )
   {
   }

   /*!
    * Destroys a %Font object. If this object references an existing font in
    * the PixInsight core application, its reference count is decremented. If
    * it becomes unreferenced, it will be garbage-collected.
    */
   virtual ~Font()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    *
    * Makes this object reference the same server-side font as the specified
    * instance \a f. If the previous font becomes unreferenced, it will be
    * garbage-collected by the PixInsight core application.
    */
   Font& operator =( const Font& f )
   {
      SetHandle( f.handle );
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   Font& operator =( Font&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Returns a reference to a <em>null font</em>. A null %Font object does not
    * correspond to an existing font object in the PixInsight core application.
    */
   static Font& Null();

   /*! #
    */
   String Face() const;

   /*! #
    */
   void SetFace( const String& );

   /*! #
    */
   bool IsExactMatch() const;

   /*! #
    */
   StringList AvailableWritingSystems() const
   {
      return AvailableFontWritingSystems( Face() );
   }

   /*! #
    */
   StringList AvailableStyles() const
   {
      return AvailableFontStyles( Face() );
   }

   /*! #
    */
   bool IsScalable( const String& style = String() ) const
   {
      return IsScalableFont( Face(), style );
   }

   /*! #
    */
   Array<double> OptimalPointSizes( const String& style = String() ) const
   {
      return OptimalFontPointSizes( Face(), style );
   }

   /*! #
    */
   int PixelSize() const;

   /*! #
    */
   void SetPixelSize ( int );

   /*! #
    */
   double PointSize() const;

   /*! #
    */
   void SetPointSize( double );

   /*! #
    */
   bool IsFixedPitch() const;

   /*! #
    */
   bool IsFixedPitch( const String& style ) const
   {
      return IsFixedPitchFont( Face(), style );
   }

   /*! #
    */
   void SetFixedPitch( bool = true );

   /*! #
    */
   bool IsKerningEnabled() const;

   /*! #
    */
   void EnableKerning( bool = true );

   /*! #
    */
   void DisableKerning( bool = true );

   /*! #
    */
   int StretchFactor() const;

   /*! #
    */
   void SetStretchFactor( int );

   /*! #
    */
   int Weight() const;

   /*! #
    */
   int Weight( const String& style ) const
   {
      return FontWeight( Face(), style );
   }

   /*! #
    */
   void SetWeight( int );

   /*! #
    */
   bool IsBold() const
   {
      return Weight() >= FontWeight::Bold;
   }

   /*! #
    */
   void SetBold( bool b = true )
   {
      SetWeight( b ? FontWeight::Bold : FontWeight::Normal );
   }

   /*! #
    */
   bool IsItalic() const;

   /*! #
    */
   bool IsItalic( const String& style ) const
   {
      return IsItalicFont( Face(), style );
   }

   /*! #
    */
   void SetItalic( bool = true );

   /*! #
    */
   bool IsUnderline() const;

   /*! #
    */
   void SetUnderline( bool = true );

   /*! #
    */
   bool IsOverline() const;

   /*! #
    */
   void SetOverline( bool = true );

   /*! #
    */
   bool IsStrikeOut() const;

   /*! #
    */
   void SetStrikeOut( bool = true );

   /*! #
    */
   int Ascent() const;

   /*! #
    */
   int Descent() const;

   /*! #
    */
   int Height() const;

   /*! #
    */
   int LineSpacing() const;

   /*! #
    */
   bool IsCharDefined( int ) const;

   /*! #
    */
   int MaxWidth() const;

   /*! #
    */
   int Width( const String& ) const;

   /*! #
    */
   int Width( int ch ) const;

   /*! #
    */
   Rect BoundingRect( const String& ) const;

   /*! #
    */
   Rect TightBoundingRect( const String& ) const;

   /*! #
    */
   static StringList AvailableFonts( const String& writingSystem = String() );

   /*! #
    */
   static StringList AvailableFontWritingSystems( const String& font );

   /*! #
    */
   static StringList AvailableFontStyles( const String& font );

   /*! #
    */
   static Array<double> OptimalFontPointSizes( const String& font, const String& style = String() );

   /*! #
    */
   static bool IsScalableFont( const String& font, const String& style = String() );

   /*! #
    */
   static bool IsFixedPitchFont( const String& font, const String& style = String() );

   /*! #
    */
   static bool IsItalicFont( const String& font, const String& style = String() );

   /*! #
    */
   static int FontWeight( const String& font, const String& style = String() );

private:

   Font( void* h ) : UIObject( h )
   {
   }

   virtual void* CloneHandle() const;

   friend class InternalFontEnumerator;
   friend class GraphicsContextBase;
   friend class Control;
   friend class TreeBox;
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Font_h

// ----------------------------------------------------------------------------
// EOF pcl/Font.h - Released 2017-06-09T08:12:42Z
