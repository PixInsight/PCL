//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/Font.cpp - Released 2015/07/30 17:15:31 UTC
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

#include <pcl/AutoLock.h>
#include <pcl/Font.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Font::Font( family f, double ptSize ) :
   UIObject( (*API->Font->CreateFontByFamily)( ModuleHandle(), f, ptSize ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateFontByFamily" );
}

Font::Font( const String& face, double ptSize ) :
   UIObject( (*API->Font->CreateFontByFace)( ModuleHandle(), face.c_str(), ptSize ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateFontByFace" );
}

// ----------------------------------------------------------------------------

Font& Font::Null()
{
   static Font* nullFont = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullFont == nullptr )
      nullFont = new Font( nullptr );
   return *nullFont;
}

// ----------------------------------------------------------------------------

String Font::Face() const
{
   if ( IsNull() )
      return String();

   size_type len = 0;
   (*API->Font->GetFontFace)( handle, 0, &len );
   if ( len == 0 )
      throw APIFunctionError( "GetFontFace" );

   String face;
   face.SetLength( len );
   if ( (*API->Font->GetFontFace)( handle, face.c_str(), &len ) == api_false )
      throw APIFunctionError( "GetFontFace" );
   face.ResizeToNullTerminated();
   return face;
}

// ----------------------------------------------------------------------------

void Font::SetFace( const String& face )
{
   SetUnique();
   (*API->Font->SetFontFace)( handle, face.c_str() );
}

// ----------------------------------------------------------------------------

bool Font::IsExactMatch() const
{
   return (*API->Font->GetFontExactMatch)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

int Font::PixelSize() const
{
   return (*API->Font->GetFontPixelSize)( handle );
}

// ----------------------------------------------------------------------------

void Font::SetPixelSize ( int pxSize )
{
   SetUnique();
   (*API->Font->SetFontPixelSize)( handle, pxSize );
}

// ----------------------------------------------------------------------------

double Font::PointSize() const
{
   double ptSize = 0.0;
   (*API->Font->GetFontPointSize)( handle, &ptSize );
   return ptSize;
}

// ----------------------------------------------------------------------------

void Font::SetPointSize( double ptSize )
{
   SetUnique();
   (*API->Font->SetFontPointSize)( handle, ptSize );
}

// ----------------------------------------------------------------------------

bool Font::IsFixedPitch() const
{
   return (*API->Font->GetFontFixedPitch)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Font::SetFixedPitch( bool enable )
{
   SetUnique();
   (*API->Font->SetFontFixedPitch)( handle, enable );
}

// ----------------------------------------------------------------------------

bool Font::IsKerningEnabled() const
{
   return (*API->Font->GetFontKerning)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Font::EnableKerning( bool enable )
{
   SetUnique();
   (*API->Font->SetFontKerning)( handle, enable );
}

// ----------------------------------------------------------------------------

void Font::DisableKerning( bool disable )
{
   SetUnique();
   (*API->Font->SetFontKerning)( handle, !disable );
}

// ----------------------------------------------------------------------------

int Font::StretchFactor() const
{
   return (*API->Font->GetFontStretchFactor)( handle );
}

// ----------------------------------------------------------------------------

void Font::SetStretchFactor( int stretch )
{
   SetUnique();
   (*API->Font->SetFontStretchFactor)( handle, stretch );
}

// ----------------------------------------------------------------------------

int Font::Weight() const
{
   return (*API->Font->GetFontWeight)( handle );
}

// ----------------------------------------------------------------------------

void Font::SetWeight( int weight )
{
   SetUnique();
   (*API->Font->SetFontWeight)( handle, weight );
}

// ----------------------------------------------------------------------------

bool Font::IsItalic() const
{
   return (*API->Font->GetFontItalic)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Font::SetItalic( bool enable )
{
   SetUnique();
   (*API->Font->SetFontItalic)( handle, enable );
}

// ----------------------------------------------------------------------------

bool Font::IsUnderline() const
{
   return (*API->Font->GetFontUnderline)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Font::SetUnderline( bool enable )
{
   SetUnique();
   (*API->Font->SetFontUnderline)( handle, enable );
}

// ----------------------------------------------------------------------------

bool Font::IsOverline() const
{
   return (*API->Font->GetFontOverline)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Font::SetOverline( bool enable )
{
   SetUnique();
   (*API->Font->SetFontOverline)( handle, enable );
}

// ----------------------------------------------------------------------------

bool Font::IsStrikeOut() const
{
   return (*API->Font->GetFontStrikeOut)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Font::SetStrikeOut( bool enable )
{
   SetUnique();
   (*API->Font->SetFontStrikeOut)( handle, enable );
}

// ----------------------------------------------------------------------------

int Font::Ascent() const
{
   return (*API->Font->GetFontAscent)( handle );
}

// ----------------------------------------------------------------------------

int Font::Descent() const
{
   return (*API->Font->GetFontDescent)( handle );
}

// ----------------------------------------------------------------------------

int Font::Height() const
{
   return (*API->Font->GetFontHeight)( handle );
}

// ----------------------------------------------------------------------------

int Font::LineSpacing() const
{
   return (*API->Font->GetFontLineSpacing)( handle );
}

// ----------------------------------------------------------------------------

bool Font::IsCharDefined( int c ) const
{
   return (*API->Font->GetFontCharDefined)( handle, c ) != api_false;
}

// ----------------------------------------------------------------------------

int Font::MaxWidth() const
{
   return (*API->Font->GetFontMaxWidth)( handle );
}

// ----------------------------------------------------------------------------

int Font::Width( const String& s ) const
{
   return (*API->Font->GetStringPixelWidth)( handle, s.c_str() );
}

// ----------------------------------------------------------------------------

int Font::Width( int c ) const
{
   return (*API->Font->GetCharPixelWidth)( handle, c );
}

// ----------------------------------------------------------------------------

Rect Font::BoundingRect( const String& s ) const
{
   Rect r;
   (*API->Font->GetStringPixelRect)( handle, s.c_str(), &r.x0, &r.y0, &r.x1, &r.y1, 0x00 );
   return r;
}

// ----------------------------------------------------------------------------

Rect Font::TightBoundingRect( const String& s ) const
{
   Rect r;
   (*API->Font->GetStringPixelRect)( handle, s.c_str(), &r.x0, &r.y0, &r.x1, &r.y1, 0x01 );
   return r;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class InternalFontEnumerator
{
public:

   static api_bool api_func FontCallback( const char16_type* item, void* ptrToStringList )
   {
      reinterpret_cast<StringList*>( ptrToStringList )->Add( String( item ) );
      return api_true;
   }

   static api_bool api_func PtSizeCallback( const double* ptSize, void* ptrToArray )
   {
      reinterpret_cast<Array<double>*>( ptrToArray )->Add( *ptSize );
      return api_true;
   }
};

// ----------------------------------------------------------------------------

StringList Font::AvailableFonts( const String& writingSystem )
{
   StringList faces;

   IsoString ws8( writingSystem );

   size_type len = 0;
   (*API->Font->EnumerateFonts)( 0, 0, &len, 0, ws8.c_str() );

   if ( len > 0 )
   {
      String fontFace;
      fontFace.Reserve( len );
      if ( (*API->Font->EnumerateFonts)( InternalFontEnumerator::FontCallback,
                        fontFace.c_str(), &len, &faces, ws8.c_str() ) == api_false )
      {
         throw APIFunctionError( "EnumerateFonts" );
      }
   }

   return faces;
}

// ----------------------------------------------------------------------------

StringList Font::AvailableFontWritingSystems( const String& font )
{
   StringList writingSystems;

   size_type len = 0;
   (*API->Font->EnumerateWritingSystems)( 0, 0, &len, 0, font.c_str() );

   if ( len > 0 )
   {
      String system;
      system.Reserve( len );
      if ( (*API->Font->EnumerateWritingSystems)( InternalFontEnumerator::FontCallback,
                        system.c_str(), &len, &writingSystems, font.c_str() ) == api_false )
      {
         throw APIFunctionError( "EnumerateWritingSystems" );
      }
   }

   return writingSystems;
}

// ----------------------------------------------------------------------------

StringList Font::AvailableFontStyles( const String& font )
{
   StringList styles;

   size_type len = 0;
   (*API->Font->EnumerateFontStyles)( 0, 0, &len, 0, font.c_str() );

   if ( len > 0 )
   {
      String style;
      style.Reserve( len );
      if ( (*API->Font->EnumerateFontStyles)( InternalFontEnumerator::FontCallback,
                        style.c_str(), &len, &styles, font.c_str() ) == api_false )
      {
         throw APIFunctionError( "EnumerateFontStyles" );
      }
   }

   return styles;
}

// ----------------------------------------------------------------------------

Array<double> Font::OptimalFontPointSizes( const String& font, const String& style )
{
   Array<double> optimalSizes;
   double ptSize;
   if ( (*API->Font->EnumerateOptimalFontPointSizes)( InternalFontEnumerator::PtSizeCallback,
                                 &ptSize, &optimalSizes, font.c_str(), style.c_str() ) == api_false )
   {
      throw APIFunctionError( "EnumerateOptimalFontPointSizes" );
   }
   return optimalSizes;
}

// ----------------------------------------------------------------------------

bool Font::IsScalableFont( const String& font, const String& style )
{
   return (*API->Font->GetFontScalable)( font.c_str(), style.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool Font::IsFixedPitchFont( const String& font, const String& style )
{
   return (*API->Font->GetNominalFontFixedPitch)( font.c_str(), style.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool Font::IsItalicFont( const String& font, const String& style )
{
   return (*API->Font->GetNominalFontItalic)( font.c_str(), style.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

int Font::FontWeight( const String& font, const String& style )
{
   return (*API->Font->GetNominalFontWeight)( font.c_str(), style.c_str() );
}

// ----------------------------------------------------------------------------

void* Font::CloneHandle() const
{
   return (*API->Font->CloneFont)( ModuleHandle(), handle );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace FontFamily
{

String PCL_FUNC FamilyToFace( value_type family )
{
   const char* face;

   switch ( family )
   {
   default:
   case pcl::FontFamily::Default:
#ifdef __PCL_X11
      face = "Sans";
#endif
#ifdef __PCL_MACOSX
      face = "Lucida Grande";
#endif
#ifdef __PCL_WINDOWS
      face = "MS Sans Serif";
#endif
      break;

   case pcl::FontFamily::SansSerif:
#ifdef __PCL_X11
      face = "Sans";
#endif
#ifdef __PCL_MACOSX
      face = "Lucida Grande";
#endif
#ifdef __PCL_WINDOWS
      face = "Arial";
#endif
      break;

   case pcl::FontFamily::Serif:
#ifdef __PCL_X11
      face = "Serif";
#endif
#ifdef __PCL_MACOSX
      face = "Times";
#endif
#ifdef __PCL_WINDOWS
      face = "Times New Roman";
#endif
      break;

   case pcl::FontFamily::Script:
      face = "Script";
      break;

   case pcl::FontFamily::TypeWriter:
#ifdef __PCL_X11
      face = "Monospace";
#endif
#ifdef __PCL_MACOSX
      face = "Courier";
#endif
#ifdef __PCL_WINDOWS
      face = "Courier New";
#endif
      break;

   case pcl::FontFamily::Decorative:
      face = "Bookman Old Style";
      break;

   case pcl::FontFamily::Symbol:
      face = "Symbol";
      break;
   }

   return face;
}

} // FontFamily

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Font.cpp - Released 2015/07/30 17:15:31 UTC
