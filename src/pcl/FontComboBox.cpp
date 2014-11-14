// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/FontComboBox.cpp - Released 2014/11/14 17:17:01 UTC
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

#include <pcl/FontComboBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static const char* s_standardFaces[] =
{ "Default", "SansSerif", "Serif", "Script", "Monospace", "Decorative", "Symbol" };

// ----------------------------------------------------------------------------

static SortedStringList GetFaceList( const FontComboBox* combo )
{
   SortedStringList faces;
   for ( int i = 0, n = combo->NumberOfItems(); i < n; ++i )
   {
      String face = combo->ItemText( i );
      face.Trim();
      faces.Add( face );
   }
   return faces;
}

template <class C>
static void SetFaceList( FontComboBox* combo, const C& faces )
{
   combo->DisableUpdates();
   combo->Clear();
   for ( SortedStringList::const_iterator i = faces.Begin(); i != faces.End(); ++i )
      combo->AddItem( ' ' + *i + ' ' );
   combo->EnableUpdates();
}

// ----------------------------------------------------------------------------

FontComboBox::FontComboBox( Control& parent ) :
ComboBox( parent ),
onFontSelected( 0 ), onFontSelectedReceiver( 0 ),
onFontHighlighted( 0 ), onFontHighlightedReceiver( 0 )
{
   ResetFonts();
   OnItemSelected( (ComboBox::item_event_handler)&FontComboBox::__ItemSelected, *this );
   OnItemHighlighted( (ComboBox::item_event_handler)&FontComboBox::__ItemHighlighted, *this );
}

FontComboBox::~FontComboBox()
{
}

String FontComboBox::CurrentFontFace() const
{
   int index = CurrentItem();
   if ( index < 0 )
      return String();
   String face = ItemText( index );
   face.Trim();
   return face;
}

void FontComboBox::SetCurrentFont( const String& face )
{
   String findFace = face;
   findFace.Trim();
   if ( findFace.IsEmpty() )
      findFace = "Default";
   SetCurrentItem( FindItem( ' ' + findFace + ' ', 0, true ) ); // find exact match, case insensitive
}

void FontComboBox::ResetFonts()
{
   // ### TODO: Add item icons for standard, scalable, bitmapped and fixed-pitch fonts.

   StringList faces = Font::AvailableFonts();
   for ( size_type i = 0; i < ItemsInArray( s_standardFaces ); ++i )
      if ( !faces.Has( s_standardFaces[i] ) )
         faces.Add( s_standardFaces[i] );
   faces.Sort();
   SetFaceList( this, faces );
}

void FontComboBox::AddStandardFontFamilies()
{
   SortedStringList faces = GetFaceList( this );
   for ( size_type i = 0; i < ItemsInArray( s_standardFaces ); ++i )
      if ( !faces.Has( s_standardFaces[i] ) )
         faces.Add( s_standardFaces[i] );
   SetFaceList( this, faces );
}

void FontComboBox::RemoveStandardFontFamilies()
{
   SortedStringList faces = GetFaceList( this );
   for ( size_type i = 0; i < ItemsInArray( s_standardFaces ); ++i )
      faces.Remove( s_standardFaces[i] );
   SetFaceList( this, faces );
}

void FontComboBox::AddScalableFonts()
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( StringList::const_iterator i = installedFaces.Begin(); i != installedFaces.End(); ++i )
      if ( !faces.Has( *i ) )
         if ( Font::IsScalableFont( *i ) )
            faces.Add( *i );
   SetFaceList( this, faces );
}

void FontComboBox::RemoveScalableFonts()
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( StringList::const_iterator i = installedFaces.Begin(); i != installedFaces.End(); ++i )
      if ( Font::IsScalableFont( *i ) )
         faces.Remove( *i );
   SetFaceList( this, faces );
}

void FontComboBox::AddFixedPitchFonts()
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces =  GetFaceList( this );
   for ( StringList::const_iterator i = installedFaces.Begin(); i != installedFaces.End(); ++i )
      if ( !faces.Has( *i ) )
         if ( Font::IsFixedPitchFont( *i ) )
            faces.Add( *i );
   SetFaceList( this, faces );
}

void FontComboBox::RemoveFixedPitchFonts()
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( StringList::const_iterator i = installedFaces.Begin(); i != installedFaces.End(); ++i )
      if ( Font::IsFixedPitchFont( *i ) )
         faces.Remove( *i );
   SetFaceList( this, faces );
}

void FontComboBox::AddItalicFonts()
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( StringList::const_iterator i = installedFaces.Begin(); i != installedFaces.End(); ++i )
      if ( !faces.Has( *i ) )
         if ( Font::IsItalicFont( *i ) )
            faces.Add( *i );
   SetFaceList( this, faces );
}

void FontComboBox::RemoveItalicFonts()
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( StringList::const_iterator i = installedFaces.Begin(); i != installedFaces.End(); ++i )
      if ( Font::IsItalicFont( *i ) )
         faces.Remove( *i );
   SetFaceList( this, faces );
}

void FontComboBox::AddWritingSystem( const String& writingSystem )
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( StringList::const_iterator i = installedFaces.Begin(); i != installedFaces.End(); ++i )
      if ( !faces.Has( *i ) )
      {
         StringList supportedSystems = Font::AvailableFontWritingSystems( *i );
         if ( supportedSystems.Has( writingSystem ) )
            faces.Add( *i );
      }
   SetFaceList( this, faces );
}

void FontComboBox::RemoveWritingSystem( const String& writingSystem )
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( StringList::const_iterator i = installedFaces.Begin(); i != installedFaces.End(); ++i )
   {
      StringList supportedSystems = Font::AvailableFontWritingSystems( *i );
      if ( supportedSystems.Has( writingSystem ) )
         faces.Remove( *i );
   }
   SetFaceList( this, faces );
}

void FontComboBox::OnFontSelected( font_event_handler f, Control& c )
{
   if ( f == 0 || c.IsNull() )
   {
      onFontSelected = 0;
      onFontSelectedReceiver = 0;
   }
   else
   {
      onFontSelected = f;
      onFontSelectedReceiver = &c;
   }
}

void FontComboBox::OnFontHighlighted( font_event_handler f, Control& c )
{
   if ( f == 0 || c.IsNull() )
   {
      onFontHighlighted = 0;
      onFontHighlightedReceiver = 0;
   }
   else
   {
      onFontHighlighted = f;
      onFontHighlightedReceiver = &c;
   }
}

void FontComboBox::__ItemSelected( ComboBox& sender, int index )
{
   if ( onFontSelected != 0 )
   {
      String face = (index < 0) ? String() : ItemText( index );
      face.Trim();
      (onFontSelectedReceiver->*onFontSelected)( *this, face );
   }
}

void FontComboBox::__ItemHighlighted( ComboBox& sender, int index )
{
   if ( onFontHighlighted != 0 )
   {
      String face = (index < 0) ? String() : ItemText( index );
      face.Trim();
      (onFontHighlightedReceiver->*onFontHighlighted)( *this, face );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/FontComboBox.cpp - Released 2014/11/14 17:17:01 UTC
