//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/FontComboBox.cpp - Released 2017-06-28T11:58:42Z
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

#include <pcl/Exception.h>
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
      faces.Add( combo->ItemText( i ).Trimmed() );
   return faces;
}

template <class C>
static void SetFaceList( FontComboBox* combo, const C& faces )
{
   combo->DisableUpdates();
   combo->Clear();
   for ( auto face : faces )
      combo->AddItem( face );
   combo->EnableUpdates();
}

// ----------------------------------------------------------------------------

FontComboBox::FontComboBox( Control& parent ) :
   ComboBox( parent )
{
   ResetFonts();
   OnItemSelected( (ComboBox::item_event_handler)&FontComboBox::ItemSelected, *this );
   OnItemHighlighted( (ComboBox::item_event_handler)&FontComboBox::ItemHighlighted, *this );
}

String FontComboBox::CurrentFontFace() const
{
   int index = CurrentItem();
   return (index < 0) ? String() : ItemText( index ).Trimmed();
}

void FontComboBox::SetCurrentFont( const String& face )
{
   String findFace = face.Trimmed();
   if ( findFace.IsEmpty() )
      findFace = "Default";
   SetCurrentItem( FindItem( findFace, 0/*fromIdx*/, true/*exactMatch*/ ) );
}

void FontComboBox::ResetFonts()
{
   // ### TODO: Add item icons for standard, scalable, bitmapped and fixed-pitch fonts.
   StringList faces = Font::AvailableFonts();
   for ( size_type i = 0; i < ItemsInArray( s_standardFaces ); ++i )
      if ( !faces.Contains( s_standardFaces[i] ) )
         faces.Add( s_standardFaces[i] );
   faces.Sort();
   SetFaceList( this, faces );
}

void FontComboBox::AddStandardFontFamilies()
{
   SortedStringList faces = GetFaceList( this );
   for ( size_type i = 0; i < ItemsInArray( s_standardFaces ); ++i )
      if ( !faces.Contains( s_standardFaces[i] ) )
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
   for ( const String& face : installedFaces )
      if ( !faces.Contains( face ) )
         if ( Font::IsScalableFont( face ) )
            faces.Add( face );
   SetFaceList( this, faces );
}

void FontComboBox::RemoveScalableFonts()
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( const String& face : installedFaces )
      if ( Font::IsScalableFont( face ) )
         faces.Remove( face );
   SetFaceList( this, faces );
}

void FontComboBox::AddFixedPitchFonts()
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces =  GetFaceList( this );
   for ( const String& face : installedFaces )
      if ( !faces.Contains( face ) )
         if ( Font::IsFixedPitchFont( face ) )
            faces.Add( face );
   SetFaceList( this, faces );
}

void FontComboBox::RemoveFixedPitchFonts()
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( const String& face : installedFaces )
      if ( Font::IsFixedPitchFont( face ) )
         faces.Remove( face );
   SetFaceList( this, faces );
}

void FontComboBox::AddItalicFonts()
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( const String& face : installedFaces )
      if ( !faces.Contains( face ) )
         if ( Font::IsItalicFont( face ) )
            faces.Add( face );
   SetFaceList( this, faces );
}

void FontComboBox::RemoveItalicFonts()
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( const String& face : installedFaces )
      if ( Font::IsItalicFont( face ) )
         faces.Remove( face );
   SetFaceList( this, faces );
}

void FontComboBox::AddWritingSystem( const String& writingSystem )
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( const String& face : installedFaces )
      if ( !faces.Contains( face ) )
      {
         StringList supportedSystems = Font::AvailableFontWritingSystems( face );
         if ( supportedSystems.Contains( writingSystem ) )
            faces.Add( face );
      }
   SetFaceList( this, faces );
}

void FontComboBox::RemoveWritingSystem( const String& writingSystem )
{
   StringList installedFaces = Font::AvailableFonts();
   SortedStringList faces = GetFaceList( this );
   for ( const String& face : installedFaces )
   {
      StringList supportedSystems = Font::AvailableFontWritingSystems( face );
      if ( supportedSystems.Contains( writingSystem ) )
         faces.Remove( face );
   }
   SetFaceList( this, faces );
}

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void FontComboBox::OnFontSelected( font_event_handler f, Control& c )
{
   if ( f == nullptr || c.IsNull() )
   {
      if ( !m_handlers.IsNull() )
      {
         m_handlers->onFontSelected = nullptr;
         m_handlers->onFontSelectedReceiver = nullptr;
      }
   }
   else
   {
      INIT_EVENT_HANDLERS();
      m_handlers->onFontSelected = f;
      m_handlers->onFontSelectedReceiver = &c;
   }
}

void FontComboBox::OnFontHighlighted( font_event_handler f, Control& c )
{
   if ( f == nullptr || c.IsNull() )
   {
      if ( !m_handlers.IsNull() )
      {
         m_handlers->onFontHighlighted = nullptr;
         m_handlers->onFontHighlightedReceiver = nullptr;
      }
   }
   else
   {
      INIT_EVENT_HANDLERS();
      m_handlers->onFontHighlighted = f;
      m_handlers->onFontHighlightedReceiver = &c;
   }
}

#undef INIT_EVENT_HANDLERS

void FontComboBox::ItemSelected( ComboBox& sender, int index )
{
   if ( !m_handlers.IsNull() )
      if ( m_handlers->onFontSelected != nullptr )
      {
         String face = (index < 0) ? String() : ItemText( index ).Trimmed();
         (m_handlers->onFontSelectedReceiver->*m_handlers->onFontSelected)( *this, face );
      }
}

void FontComboBox::ItemHighlighted( ComboBox& sender, int index )
{
   if ( !m_handlers.IsNull() )
      if ( m_handlers->onFontHighlighted != nullptr )
      {
         String face = (index < 0) ? String() : ItemText( index ).Trimmed();
         (m_handlers->onFontHighlightedReceiver->*m_handlers->onFontHighlighted)( *this, face );
      }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FontComboBox.cpp - Released 2017-06-28T11:58:42Z
