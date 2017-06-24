//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/FontComboBox.h - Released 2017-06-21T11:36:33Z
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

#ifndef __PCL_FontComboBox_h
#define __PCL_FontComboBox_h

/// \file pcl/FontComboBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/ComboBox.h>
#include <pcl/Font.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class FontComboBox
 * \brief A %ComboBox descendant class to select font faces.
 *
 * %FontComboBox is a utility class that provides a simple list with all the
 * installed fonts on the system, plus the set of <em>standard font
 * families</em> - see the FontFamily namespace for the supported set of
 * standard font families in the PixInsight/PCL platform.
 *
 * Optionally, %FontComboBox can restrict the list of font items to include
 * only scalable, bitmapped or fixed-pitch fonts. The fonts list can also be
 * restricted as a function of the available writing systems (Latin, Greek,
 * Cyrillic, and so on).
 *
 * \sa Font, ComboBox, FontFamily
 */
class PCL_CLASS FontComboBox : public ComboBox
{
public:

   /*!
    * Constructs a %FontComboBox as a child control of \a parent.
    *
    * A default set of %ComboBox items is created for the full set of fonts
    * currently installed on the system.
    */
   FontComboBox( Control& parent = Control::Null() );

   /*!
    * Destroys a %FontComboBox control.
    */
   virtual ~FontComboBox()
   {
   }

   /*!
    * Returns the face name of the font that corresponds to the currently
    * selected %ComboBox item, or an empty string if no item is currently
    * selected.
    *
    * \sa CurrentFont(), SetCurrentFont( const String& ),
    * SetCurrentFont( const Font& )
    */
   String CurrentFontFace() const;

   /*!
    * Returns a Font object that corresponds to the currently selected
    * %ComboBox item, or a null font object (Font::Null()) if no item is
    * currently selected.
    *
    * \param size    The size in points of the returned font.
    *
    * This is a convenience member function, equivalent to:
    * Font( CurrentFontFace(), size ).
    *
    * \sa CurrentFontFace(), SetCurrentFont( const String& ),
    * SetCurrentFont( const Font& )
    */
   pcl::Font CurrentFont( double size = 12.0 ) const
   {
      return pcl::Font( CurrentFontFace(), size );
   }

   /*!
    * Selects the %ComboBox item corresponding to the specified font \a face.
    *
    * If the specified \a face doesn't correspond to an installed or standard
    * font family, or if an empty string is specified, this member function
    * leaves the %ComboBox control with no selection, i.e. with its selected
    * index equal to -1.
    *
    * \sa SetCurrentFont( const Font& ), CurrentFont(), CurrentFontFace()
    */
   void SetCurrentFont( const String& face );

   /*!
    * Selects the %ComboBox item corresponding to the specified \a font.
    *
    * If the specified \a font doesn't correspond to an installed or standard
    * font family, or if \a font is Font::Null(), this member function leaves
    * the %ComboBox control with no selection, i.e. with its selected index
    * equal to -1.
    *
    * This is a convenience member function, equivalent to:
    * SetCurrentFont( font.Face() ).
    *
    * \sa SetCurrentFont( const String& ), CurrentFont(), CurrentFontFace()
    */
   void SetCurrentFont( const pcl::Font& font )
   {
      SetCurrentFont( font.Face() );
   }

   /*!
    * Resets this %FontComboBox control to its default state: Replaces the
    * current list of %ComboBox items with a new set corresponding to the
    * standard font families available on the PixInsight/PCL platform, plus all
    * installed fonts on the system.
    *
    * \sa AddStandardFontFamilies(), AddScalableFonts(), AddFixedPitchFonts(),
    * AddItalicFonts(), AddWritingSystem()
    */
   void ResetFonts();

   /*!
    * Adds a list of %ComboBox items corresponding to the standard font
    * families available on the PixInsight/PCL platform. This includes the
    * following fonts: Default, SansSerif, Serif, Script, Monospace,
    * Decorative, and Symbol.
    *
    * \note The list of standard font families is always prepended at the
    * beginning of the existing set of items.
    *
    * \sa RemoveStandardFontFamilies()
    */
   void AddStandardFontFamilies();

   /*!
    * Removes all existing %ComboBox items that correspond to standard font
    * families available on the PixInsight/PCL platform. See
    * AddStandardFontFamilies() for more information.
    *
    * \sa AddStandardFontFamilies()
    */
   void RemoveStandardFontFamilies();

   /*!
    * Adds a list of %ComboBox items corresponding to all scalable fonts
    * currently installed on the system.
    *
    * \sa RemoveScalableFonts()
    */
   void AddScalableFonts();

   /*!
    * Removes all existing %ComboBox items that correspond to scalable fonts.
    *
    * \sa AddScalableFonts()
    */
   void RemoveScalableFonts();

   /*!
    * Adds a list of %ComboBox items corresponding to all fixed-pitch fonts
    * currently installed on the system.
    *
    * \sa RemoveFixedPitchFonts()
    */
   void AddFixedPitchFonts();

   /*!
    * Removes all existing %ComboBox items that correspond to fixed-pitch
    * fonts.
    *
    * \sa AddFixedPitchFonts()
    */
   void RemoveFixedPitchFonts();

   /*!
    * Adds a list of %ComboBox items corresponding to all italic fonts
    * currently installed on the system.
    *
    * \sa RemoveItalicFonts()
    */
   void AddItalicFonts();

   /*!
    * Removes all existing %ComboBox items that correspond to italic fonts.
    *
    * \sa AddItalicFonts()
    */
   void RemoveItalicFonts();

   /*!
    * Adds a list of %ComboBox items corresponding to all installed fonts that
    * support the specified writing system.
    *
    * \sa RemoveWritingSystem()
    */
   void AddWritingSystem( const String& writingSystem );

   /*!
    * Removes all existing %ComboBox items that correspond to installed fonts
    * supporting the specified writing system.
    *
    * \sa AddWritingSystem()
    */
   void RemoveWritingSystem( const String& writingSystem );

   /*!
    * Adds items for all installed fonts that support the Latin writing system.
    *
    * This is a convenience member function, equivalent to:
    * AddWritingSystem( "Latin" ).
    *
    * \sa RemoveLatinFonts(), AddGreekFonts()
    */
   void AddLatinFonts()
   {
      AddWritingSystem( "Latin" );
   }

   /*!
    * Removes all existing %ComboBox items that correspond to installed fonts
    * supporting the Latin writing system.
    *
    * \sa AddLatinFonts(), RemoveGreekFonts()
    */
   void RemoveLatinFonts()
   {
      RemoveWritingSystem( "Latin" );
   }

   /*!
    * Adds items for all installed fonts that support the Greek writing system.
    *
    * This is a convenience member function, equivalent to:
    * AddWritingSystem( "Greek" ).
    *
    * \sa RemoveGreekFonts(), AddLatinFonts()
    */
   void AddGreekFonts()
   {
      AddWritingSystem( "Greek" );
   }

   /*!
    * Removes all existing %ComboBox items that correspond to installed fonts
    * supporting the Greek writing system.
    *
    * \sa AddGreekFonts(), RemoveLatinFonts()
    */
   void RemoveGreekFonts()
   {
      RemoveWritingSystem( "Greek" );
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnFontSelected( FontComboBox& sender, const String& font );
   // void OnFontHighlighted( FontComboBox& sender, const String& font );

   /*!
    * \defgroup fontcombobox_event_handlers FontComboBox Event Handlers
    */

   /*!
    * Defines the prototype of a <em>font combo box event handler</em>.
    *
    * A font combo box event is generated when an item is selected or
    * highlighted in a font combo box.
    *
    * \param sender  The %FontComboBox control that sends a font combo box
    *                event.
    *
    * \param font    The face name of the font that has been selected or
    *                highlighted in the \a sender font combo box.
    *
    * \ingroup fontcombobox_event_handlers
    */
   typedef void (Control::*font_event_handler)( FontComboBox& sender, const String& font );

   /*!
    * Sets the <em>font selected</em> event handler for this font combo box.
    *
    * \param handler    The font combo box event handler. Must be a member
    *                   function of the receiver object's class. This handler
    *                   will be called whenever a font is selected in this
    *                   font combo box control.
    *
    * \param receiver   The control that will receive <em>font selected</em>
    *                   events from this font combo box.
    *
    * \ingroup fontcombobox_event_handlers
    */
   void OnFontSelected( font_event_handler handler, Control& receiver );

   /*!
    * Sets the <em>font highlighted</em> event handler for this font combo
    * box control.
    *
    * \param handler    The font combo box event handler. Must be a member
    *                   function of the receiver object's class. This handler
    *                   will be called whenever a font is highlighted in this
    *                   font combo box control.
    *
    * \param receiver   The control that will receive <em>font
    *                   highlighted</em> events from this font combo box.
    *
    * \ingroup fontcombobox_event_handlers
    */
   void OnFontHighlighted( font_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      font_event_handler onFontSelected            = nullptr;
      Control*           onFontSelectedReceiver    = nullptr;

      font_event_handler onFontHighlighted         = nullptr;
      Control*           onFontHighlightedReceiver = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   void ItemSelected( ComboBox&, int );
   void ItemHighlighted( ComboBox&, int );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_FontComboBox_h

// ----------------------------------------------------------------------------
// EOF pcl/FontComboBox.h - Released 2017-06-21T11:36:33Z
