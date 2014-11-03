// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/Arguments.cpp - Released 2014/10/29 07:34:20 UTC
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

#include <pcl/Arguments.h>
#include <pcl/Exception.h>
#include <pcl/File.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>

#include <errno.h>
#include <stdlib.h> // for getenv()

namespace pcl
{

// ----------------------------------------------------------------------------

void Argument::Parse( const char16_type* argv )
{
   Initialize();

   String a( argv );
   a.Trim();

   size_type n = a.Length();
   if ( n == 0 )
      throw Error( "No argument identifier" );

   token = a;

   size_type nsep = a.Find( '=' );

   if ( nsep != String::notFound )
   {
      if ( n < nsep+2 )
         throw ParseError( "Expected argument value", a, int( nsep+1 ) );

      char16_type c = a[nsep+1];

      if ( c >= '0' && c <= '9' || c == '+' || c == '-' || c == '.' )
      {
         type = numeric_arg;

         char16_type* endptr = 0;

         errno = 0;

#ifdef __PCL_WINDOWS
         asNumeric = ::wcstod( reinterpret_cast<wchar_t*>( a.c_str() )+nsep+1,
                              &reinterpret_cast<wchar_t*&>( endptr ) );
#else
         Array<wchar_t> w = a.ToWCharArray( nsep+1 );

         wchar_t* wendptr = 0;
         asNumeric = ::wcstod( w.Begin(), &wendptr );

         if ( wendptr != 0 )
            endptr = a.c_str() + nsep + 1 + (wendptr - w.Begin());
#endif
         if ( errno == ERANGE )
            throw ParseError( "Numeric value out of representable range", a );

         if ( errno != 0 || (endptr != 0 && *endptr != 0) )
            throw ParseError( "Invalid immediate numeric expression", a,
                              (endptr != 0 && *endptr != 0) ? endptr - a.c_str() : -1 );
      }
      else
      {
         type = string_arg;

         size_type pos = nsep+1;

         if ( c == '\"' )
            ++pos;

         asString = a.SubString( pos );

         if ( c == '\"' )
         {
            size_type l = asString.Length();
            if ( l != 0 && asString[l-1] == '\"' )
               asString.Delete( l-1 );
         }

         if ( asString.IsEmpty() )
            throw ParseError( "Expected string value", a, int( pos ) );
      }

      id = a.Left( nsep );
   }
   else if ( a[n-1] == '+' )
   {
      type = switch_arg;
      asSwitch = true;
      id = a.Left( n-1 );
   }
   else if ( a[n-1] == '-' )
   {
      type = switch_arg;
      asSwitch = false;
      id = a.Left( n-1 );
   }
   else
   {
      type = literal_arg;
      id = a;
   }
}

// ----------------------------------------------------------------------------

static void SearchDirectory_Recursive( StringList& fileNames, const String& fileName, bool recursive )
{
   String fileDir = File::ExtractDrive( fileName ) + File::ExtractDirectory( fileName );
   String wildSpec = File::ExtractName( fileName ) + File::ExtractExtension( fileName );

   FindFileInfo info;

   for ( File::Find f( fileName ); f.NextItem( info ); )
      if ( !info.IsDirectory() )
         if ( MatchesWildSpecification( info.name, wildSpec ) ) // apply our own matching criteria
         {
            String path = fileDir + '/' + info.name;
            if ( !fileNames.Has( path ) )
               fileNames.Add( path );
         }

   if ( recursive )
   {
      StringList directories;

      for ( File::Find f( fileDir + "/*" ); f.NextItem( info ); )
         if ( info.IsDirectory() && info.name != "." && info.name != ".." )
            directories.Add( info.name );

      for ( StringList::const_iterator i = directories.Begin(); i != directories.End(); ++i )
         SearchDirectory_Recursive( fileNames, fileDir + '/' + *i + '/' + wildSpec, true );
   }
}

StringList SearchDirectory( const String& fileName, bool recursive )
{
   StringList fileNames;
   SearchDirectory_Recursive( fileNames, fileName, recursive );
   return fileNames;
}

// ----------------------------------------------------------------------------

static String s_recursiveSearchArg = "-r";

String RecursiveDirSearchArgument()
{
   return s_recursiveSearchArg;
}

void SetRecursiveDirSearchArgument( const String& argId )
{
   s_recursiveSearchArg = argId.IsEmpty() ? String( "-r" ) : argId;
}

// ----------------------------------------------------------------------------

static inline bool IsWild( const String& s )
{
   return s.Has( '*' ) || s.Has( '?' );
}

static void AddView( StringList& items, const View& v )
{
   String id( v.FullId() );
   if ( !items.Has( id ) )
      items.Add( id );
}

static void FindPreviews( StringList& items, const ImageWindow& w, const String& previewId )
{
   if ( IsWild( previewId ) )
   {
      Array<View> P = w.Previews();
      for ( size_type i = 0; i < P.Length(); ++i )
         if ( MatchesWildSpecification( String( P[i].Id() ), previewId ) )
            AddView( items, P[i] );
   }
   else
   {
      View p = w.PreviewById( IsoString( previewId ) );
      if ( p.IsNull() )
         throw ParseError( "Preview not found", previewId );
      AddView( items, p );
   }
}

static void FindPreviews( StringList& items, const String& imageId, const String& previewId )
{
   if ( IsWild( imageId ) )
   {
      Array<ImageWindow> W = ImageWindow::AllWindows();
      for ( size_type i = 0; i < W.Length(); ++i )
         if ( MatchesWildSpecification( String( W[i].MainView().Id() ), imageId ) )
            FindPreviews( items, W[i], previewId );
   }
   else
   {
      ImageWindow w = ImageWindow::WindowById( IsoString( imageId ) );
      if ( w.IsNull() )
         throw ParseError( "Image not found", imageId );
      FindPreviews( items, w, previewId );
   }
}

ArgumentList ExtractArguments( const StringList& argv, argument_item_mode mode, ArgumentOptions options )
{
   bool noItems             = mode == ArgumentItemMode::NoItems;
   bool itemsAsFiles        = mode == ArgumentItemMode::AsFiles;
   bool itemsAsViews        = mode == ArgumentItemMode::AsViews;
   bool allowWildcards      = !noItems && options.IsFlagSet( ArgumentOption::AllowWildcards );
   bool noPreviews          = itemsAsViews && options.IsFlagSet( ArgumentOption::NoPreviews );
   bool recursiveDirSearch  = itemsAsFiles && allowWildcards && options.IsFlagSet( ArgumentOption::RecursiveDirSearch );
   bool recursiveSearchArgs = recursiveDirSearch && options.IsFlagSet( ArgumentOption::RecursiveSearchArgs );

   // This is the recursive search mode flag, controlled by --r[+|-]
   bool recursiveSearch = false;

   // The list of existing view identifiers, in case itemsAsViews = true.
   SortedStringList imageIds;

   // The list of extracted arguments
   ArgumentList arguments;

   for ( StringList::const_iterator i = argv.Begin(); i != argv.End(); ++i )
   {
      if ( i->BeginsWith( '-' ) )
      {
         Argument arg( i->At( 1 ) );

         if ( recursiveSearchArgs && arg.Id() == s_recursiveSearchArg )
         {
            if ( arg.IsSwitch() )
               recursiveSearch = arg.SwitchState();
            else if ( arg.IsLiteral() )
               recursiveSearch = true;
            else
               arguments.Add( arg );
         }
         else
            arguments.Add( arg );
      }
      else
      {
         if ( noItems )
            throw ParseError( "Non-parametric arguments are not allowed", *i  );

         StringList items;

         if ( itemsAsFiles )
         {
            String fileName = *i;
            if ( fileName.BeginsWith( '\"' ) )
               fileName.Delete( 0 );
            if ( fileName.EndsWith( '\"' ) )
               fileName.Delete( fileName.UpperBound() );

            fileName.Trim();
            if ( fileName.IsEmpty() )
               throw ParseError( "Empty path specification", *i );

            fileName = File::FullPath( fileName );

            if ( IsWild( fileName ) ) // wild path ?
            {
               if ( !allowWildcards )
                  throw ParseError( "Wildcards not allowed", fileName );

               items = SearchDirectory( fileName, recursiveSearch );
            }
            else
               items.Add( fileName );
         }
         else if ( itemsAsViews )
         {
            String viewId = *i;

            if ( !allowWildcards && IsWild( viewId ) )
               throw ParseError( "Wildcards not allowed", viewId );

            size_type p = viewId.Find( "->" );

            if ( p != String::notFound )
            {
               if ( noPreviews )
                  throw ParseError( "Preview identifiers not allowed", viewId );

               String imageId = viewId.Left( p );
               if ( imageId.IsEmpty() )
                  throw ParseError( "Missing image identifier", viewId );

               String previewId = viewId.SubString( p+2 );
               if ( previewId.IsEmpty() )
                  throw ParseError( "Missing preview identifier", viewId );

               FindPreviews( items, imageId, previewId );
            }
            else
            {
               if ( IsWild( viewId ) )
               {
                  Array<ImageWindow> W = ImageWindow::AllWindows();
                  for ( size_type i = 0; i < W.Length(); ++i )
                  {
                     View v = W[i].MainView();
                     if ( MatchesWildSpecification( String( v.Id() ), viewId ) )
                        AddView( items, v );
                  }
               }
               else
               {
                  ImageWindow w = ImageWindow::WindowById( IsoString( viewId ) );
                  if ( w.IsNull() )
                     throw ParseError( "Image not found", viewId );
                  AddView( items, w.MainView() );
               }
            }
         }
         else
            items.Add( *i );

         Argument arg( *i, items );
         arguments.Add( arg );
      }
   }

   return arguments;
}

// ----------------------------------------------------------------------------

bool MatchesWildSpecification( const String& string, const String& pattern )
{
   if ( string.IsEmpty() )
      return pattern.IsEmpty();

   for ( String::const_iterator i = string.Begin(), j = pattern.Begin(); ; ++i, ++j )
   {
      if ( j == pattern.End() )
         return i == string.End();

      if ( *j == '*' )
      {
         do
         {
            if ( ++j == pattern.End() )
               return true;
         }
         while ( *j == '?' || *j == '*' );

         if ( i == string.End() )
            return false;

         for ( String::const_iterator i1 = string.End(); ; )
         {
            if ( *--i1 == *j )
            {
               i = i1;
               break;
            }

            if ( i1 == i )
               return false;
         }
      }
      else
      {
         if ( i == string.End() || *j != '?' && *i != *j )
            return false;
      }
   }
}

// ----------------------------------------------------------------------------

String ReplaceEnvironmentVariables( const String& s0 )
{
   String s( s0 );
   size_type p = 0;

   for ( ;; )
   {
      for ( ;; )
      {
         p = s.Find( '$', p );
         if ( p == String::notFound )
            return s;
         if ( p == 0 || s[p-1] != '\\' )
            break;
         s.Delete( p-1 );
      }

      size_type p1 = p;
      size_type n = s.Length();
      while ( ++p1 < n )
      {
         int c = int( s[p1] );
         if ( (c < 'A' || c > 'Z') && (c < 'a' || c > 'z') && (c < '0' || c > '9') && c != '_' )
            break;
      }

      String var( s.SubString( p+1, p1-p-1 ) );
      if ( !var.IsEmpty() )
      {
         IsoString v8( var );
         var = ReplaceEnvironmentVariables( String( ::getenv( v8.c_str() ) ) );
      }

      s.Replace( p, p1-p, var );

      p += var.Length();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/Arguments.cpp - Released 2014/10/29 07:34:20 UTC
