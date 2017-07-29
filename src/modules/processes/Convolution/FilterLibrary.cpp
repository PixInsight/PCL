//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0226
// ----------------------------------------------------------------------------
// FilterLibrary.cpp - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
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

#include "FilterLibrary.h"

#include <pcl/File.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FilterLibrary TheFilterLibrary;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FilterLibrary::Load( const String& _filePath )
{
   filters = FilterParser( File::ReadLines( _filePath, ReadTextOption::TrimTrailingSpaces ) ).Filters();
   filePath = _filePath;
}

void FilterLibrary::LoadDefaultLibrary()
{
   String path = DefaultLibraryPath();
   if ( !File::Exists( path ) )
      throw Error( "Default filter library not found: " + path );
   Load( path );
}

void FilterLibrary::Save()
{
   if ( filePath.IsEmpty() )
      throw Error( "Internal error: Invalid call to FilterLibrary::Save() without a valid file path" );
   SaveAs( filePath );
}

void FilterLibrary::SaveAs( const String& _filePath )
{
   File f;
   f.CreateForWriting( _filePath );
   if ( !filters.IsEmpty() )
      for ( filter_list::const_iterator i = filters.Begin(); ; )
      {
         f.OutText( i->ToSource() );
         if ( ++i == filters.End() )
            break;
         f.OutTextLn( IsoString() );
      }
   f.Close();

   filePath = _filePath;
}

void FilterLibrary::New()
{
   filters.Clear();
   filePath.Clear();
}

bool FilterLibrary::IsWritable() const
{
   /*
    * If wheter this is a new library, or the current library file does not
    * exist, or it is a read-only file, then this library is not writable.
    */
   if ( filePath.IsEmpty() )
      return false;
   if ( !File::Exists( filePath ) )
      return false;
   if ( File::IsReadOnly( filePath ) )
      return false;

   /*
    * Do not allow rewriting the default filter collection.
    */
#ifdef __PCL_WINDOWS
   // Windows filesystem is case-insensitive
   if ( filePath.CompareIC( DefaultLibraryPath() ) == 0 )
#else
   if ( filePath == DefaultLibraryPath() )
#endif
      return false;

   /*
    * Attempt to create a file on the same directory where we have the current
    * filter collection file. If we can create a file, then the current filter
    * library is (should be) writable.
    */
   try
   {
      String dirPath = File::ExtractDrive( filePath ) + File::ExtractDirectory( filePath );
      if ( !File::DirectoryExists( dirPath ) ) // ?? cannot happen
         return false;

      if ( !dirPath.EndsWith( '/' ) )
         dirPath += '/';
      String testFileName = dirPath + "test_file";
      if ( File::Exists( testFileName ) )
      {
         String baseName = testFileName;
         unsigned i = 0;
         do
            testFileName = baseName + String( ++i );
         while ( File::Exists( testFileName ) );
      }
      File f;
      f.CreateForWriting( testFileName );
      f.Close();
      File::Remove( testFileName );
      return true;
   }
   catch ( ... )
   {
      return false;
   }
}

String FilterLibrary::DefaultLibraryPath()
{
   return PixInsightSettings::GlobalString( "Application/BaseDirectory" )
                                    + "/library/default.filters";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

IsoString Filter::ToSource() const
{
   IsoString s;
   if ( IsValid() )
   {
      if ( IsSeparable() )
      {
         s += "SeparableFilter {\n";
         s += "   name { " + Separable().Name().ToUTF8() + " }\n";
         s += "   row-vector { ";
         SeparableFilter::coefficient_vector H = Separable().RowFilter();
         for ( int i = 0; i < H.Length(); ++i )
            s.AppendFormat( ( H[i] < 0 ) ? "%.6f " :  " %.6f ", H[i] );
         s += "}\n";
         s += "   col-vector { ";
         SeparableFilter::coefficient_vector V = Separable().ColFilter();
         for ( int i = 0; i < V.Length(); ++i )
            s.AppendFormat( ( V[i] < 0 ) ? "%.6f " :  " %.6f ", V[i] );
         s += "}\n";
      }
      else
      {
         s += "KernelFilter {\n";
         s += "   name { " + Kernel().Name().ToUTF8() + " }\n";
         s += "   coefficients {\n";
         KernelFilter::coefficient_matrix M = Kernel().Coefficients();
         for ( int i = 0; i < M.Rows(); ++i )
         {
            s += "      ";
            for ( int j = 0; ; )
            {
               s.AppendFormat( ( M[i][j] < 0 ) ? "%.6f " :  " %.6f ", M[i][j] );
               if ( ++j == M.Cols() )
                  break;
               s += ' ';
            }
            s += '\n';
         }
         s += "   }\n";
      }
      s += "}\n";
   }

   return s;
}

// ----------------------------------------------------------------------------

Filter Filter::FromSource( const IsoString& src )
{
   IsoStringList lines;
   src.Break( lines, '\n' );
   FilterParser P( lines );
   if ( P.Filters().IsEmpty() )
      return Filter();
   return P.Filters()[0];
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define PARSE_ERROR( message, tokenPtr ) \
   throw SourceCodeError( message, tokenPtr->line+1, tokenPtr->column+1 )

FilterParser::token_list FilterParser::Tokenize( const IsoStringList& linesUTF8 )
{
   token_list tokenList;
   bool blockComment = false;
   int row = 0;
   for ( IsoStringList::const_iterator i = linesUTF8.Begin(); i < linesUTF8.End(); ++i, ++row )
   {
      IsoString token;
      int tokenCol = 0;
      for ( IsoString::const_iterator j = i->Begin(); j < i->End(); ++j )
      {
         if ( blockComment )
         {
            if ( *j == '*' )
               if ( ++j < i->End() )
                  if ( *j == '/' )
                  {
                     blockComment = false;
                     /*
                      * Uncomment the next line to turn block comments into
                      * separators.
                      *
                     tokenCol = j - i->Begin() + 1;
                      */
                  }
         }
         else
         {
            bool lineComment = false;
            switch ( *j )
            {
            case ' ':
            case '\t':
               if ( !token.IsEmpty() )
               {
                  tokenList.Add( Token( token, row, tokenCol ) );
                  token.Clear();
               }
               for ( IsoString::const_iterator k = j; ++k < i->End(); ++j )
                  if ( *k != ' ' && *k != '\t' )
                     break;
               tokenCol = j - i->Begin() + 1;
               break;
               /*
            case ',':
               if ( token.IsEmpty() )
                  throw SourceCodeError( "Expected a token before ','", row+1, int( j - i->Begin() + 1 ) );
               tokenList.Add( Token( token, row, tokenCol ) );
               token.Clear();
               tokenCol = j - i->Begin() + 1;
               break;
               */
            case '{':
            case '}':
               if ( !token.IsEmpty() )
               {
                  tokenList.Add( Token( token, row, tokenCol ) );
                  token.Clear();
               }
               tokenList.Add( Token( IsoString( *j ), row, int( j - i->Begin() ) ) );
               tokenCol = j - i->Begin() + 1;
               break;
            case '/':
               if ( ++j < i->End() )
               {
                  if ( *j == '/' )
                     lineComment = true;
                  else if ( *j == '*' )
                  {
                     blockComment = true;
                     /*
                      * Uncomment the next lines to turn block comments into
                      * separators.
                      *
                     if ( !token.IsEmpty() )
                     {
                        tokenList.Add( Token( token, row, tokenCol ) );
                        token.Clear();
                     }
                      */
                  }
                  else
                     token += *--j;
               }
               break;
            default:
               token += *j;
               break;
            }
            if ( lineComment )
               break;
         }
      }

      if ( !token.IsEmpty() )
         tokenList.Add( Token( token, row, tokenCol ) );
   }
   return tokenList;
}

// ----------------------------------------------------------------------------

void FilterParser::Parse( const token_list& tokens )
{
   filters.Clear();

   int filterType = UnknownFilterType;
   String filterName;
   int filterSize = 0;
   KernelFilter::coefficient_matrix    filterMatrix;
   SeparableFilter::coefficient_vector rowVector, colVector;

   int state = FilterTypeState;

   for ( token_list::const_iterator i = tokens.Begin(); i != tokens.End(); )
   {
      switch ( state )
      {
      case FilterTypeState:
         {
            if ( i->token == "KernelFilter" )
               filterType = KernelFilterType;
            else if ( i->token == "SeparableFilter" )
               filterType = SeparableFilterType;
            else if ( i->token == '{' || i->token == '}' )
               PARSE_ERROR( "Misplaced bracket", i );
            else if ( !i->token.IsValidIdentifier() )
               PARSE_ERROR( "Invalid filter type \'" + i->token + '\'', i );
            else
               PARSE_ERROR( "Unknown filter type \'" + i->token + '\'', i );

            if ( ++i == tokens.End() || i->token != '{' )
               PARSE_ERROR( "Expected left bracket", i );
            state = FilterParameterState;

            ++i;
         }
         break;

      case FilterParameterState:
         {
            if ( i->token == '}' )
            {
               if ( filterName.IsEmpty() )
                  PARSE_ERROR( "Missing filter name", i );
               if ( filterSize == 0 )
                  PARSE_ERROR( "Empty filter definition", i );

               if ( filterType == KernelFilterType )
               {
                  if ( filterMatrix.IsEmpty() )
                     PARSE_ERROR( "Missing kernel filter coefficients", i );

                  filters.Add( Filter( KernelFilter( filterMatrix, filterName ) ) );
               }
               else if ( filterType == SeparableFilterType )
               {
                  if ( rowVector.IsEmpty() || colVector.IsEmpty() )
                     PARSE_ERROR( "Missing separable filter coefficients", i );

                  filters.Add( Filter( SeparableFilter( rowVector, colVector, filterName ) ) );
               }
               else
                  PARSE_ERROR( "Internal parser error", i );

               filterName.Clear();
               filterSize = 0;
               filterMatrix = KernelFilter::coefficient_matrix();
               rowVector = colVector = SeparableFilter::coefficient_vector();

               state = FilterTypeState;
               ++i;
            }
            else
            {
               token_list::const_iterator j = i; ++j;
               token_list::const_iterator k = tokens.End();
               CaptureParameterValueTokens( j, k );
               int n = Distance( j, k );

               if ( i->token == "name" )
               {
                  if ( n == 0 )
                     PARSE_ERROR( "Expected a filter name", i );
                  if ( !filterName.IsEmpty() )
                     PARSE_ERROR( "Duplicate filter name", i );
                  filterName = j->token;
                  for ( ; ++j < k; )
                     filterName += ' ' + j->token;
               }
               else if ( i->token == "coefficients" )
               {
                  if ( filterType != KernelFilterType )
                     PARSE_ERROR( "Invalid kernel filter parameter", i );
                  if ( !filterMatrix.IsEmpty() )
                     PARSE_ERROR( "Duplicate kernel filter coefficients", i );

                  token_list::const_iterator p = j;
                  GenericVector<KernelFilter::coefficient> C( n );
                  for ( KernelFilter::coefficient* c = C.Begin(); c < C.End(); ++c, ++p )
                     *c = p->token.ToFloat();

                  int numberOfCoefficients = filterSize*filterSize;
                  if ( numberOfCoefficients != 0 )
                  {
                     if ( numberOfCoefficients != n )
                        PARSE_ERROR( "Incongruent kernel filter size; expected " + String( numberOfCoefficients ) + " coefficients", j );
                  }
                  else
                  {
                     filterSize = TruncI( Sqrt( double( n ) ) );
                     if ( filterSize*filterSize != n )
                        PARSE_ERROR( "Non-square kernel filter defined", j );
                     if ( filterSize < 3 )
                        PARSE_ERROR( "The kernel filter is too small - 3x3 is the minimum required", j );
                     if ( (filterSize & 0x01) == 0 )
                        PARSE_ERROR( "Invalid even kernel filter dimension (" + String( filterSize ) + ')', j );
                  }

                  filterMatrix = KernelFilter::coefficient_matrix( C.Begin(), filterSize, filterSize );
                  j = p;
               }
               else if ( i->token == "row-vector" )
               {
                  if ( filterType != SeparableFilterType )
                     PARSE_ERROR( "Invalid separable filter parameter", i );
                  if ( !rowVector.IsEmpty() )
                     PARSE_ERROR( "Duplicate row vector specification", i );

                  token_list::const_iterator p = j;
                  rowVector = SeparableFilter::coefficient_vector( n );
                  for ( SeparableFilter::coefficient* c = rowVector.Begin(); c < rowVector.End(); ++c, ++p )
                     *c = p->token.ToFloat();

                  if ( filterSize != 0 )
                  {
                     if ( filterSize != n )
                        PARSE_ERROR( "Incongruent separable row filter length; expected " + String( filterSize ) + " coefficients", j );
                  }
                  else
                  {
                     if ( n < 3 )
                        PARSE_ERROR( "Too few row filter coefficients specified - three or more coefficients are required", j );
                     if ( (n & 0x01) == 0 )
                        PARSE_ERROR( "Invalid even row filter length (" + String( n ) + ')', j );
                     filterSize = n;
                  }

                  j = p;
               }
               else if ( i->token == "col-vector" || i->token == "column-vector" )
               {
                  if ( filterType != SeparableFilterType )
                     PARSE_ERROR( "Invalid separable filter parameter", i );
                  if ( !colVector.IsEmpty() )
                     PARSE_ERROR( "Duplicate column vector specification", i );

                  token_list::const_iterator p = j;
                  colVector = SeparableFilter::coefficient_vector( n );
                  for ( SeparableFilter::coefficient* c = colVector.Begin(); c != colVector.End(); ++c, ++p )
                     *c = p->token.ToFloat();

                  if ( filterSize != 0 )
                  {
                     if ( filterSize != n )
                        PARSE_ERROR( "Incongruent separable column filter length; expected " + String( filterSize ) + " coefficients", j );
                  }
                  else
                  {
                     if ( n < 3 )
                        PARSE_ERROR( "Too few column filter coefficients specified - three or more coefficients are required", j );
                     if ( (n & 0x01) == 0 )
                        PARSE_ERROR( "Invalid even column filter length (" + String( n ) + ')', j );
                     filterSize = n;
                  }

                  j = p;
               }
               else
                  PARSE_ERROR( "Unknown filter parameter '" + i->token + '\'', i );

               int d = Distance( i, j );
               i = k;
               if ( d > 1 ) // j-i > 1 if and only if value is enclosed by brackets
                  ++i;
            }
         }
         break;

      default:
         PARSE_ERROR( "Internal parser error", i );
      }
   }

   if ( state != FilterTypeState )
      PARSE_ERROR( "Missing right bracket", tokens.At(tokens.UpperBound()));
}

// ----------------------------------------------------------------------------

void FilterParser::CaptureParameterValueTokens( token_list::const_iterator& i, token_list::const_iterator& j )
{
   if ( i->token == '{' )
   {
      for ( token_list::const_iterator k = ++i; k < j; ++k )
      {
         if ( k->token == '}' )
         {
            j = k;
            return;
         }
         if ( k->token == '{' )
            PARSE_ERROR( "Unexpected left bracket", k );
      }
      PARSE_ERROR( "Missing right bracket", i );
   }

   if ( i->token == '}' )
      PARSE_ERROR( "Unexpected right bracket", i );

   j = i;
   ++j;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FilterLibrary.cpp - Released 2017-05-02T09:43:00Z
