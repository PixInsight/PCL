//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0348
// ----------------------------------------------------------------------------
// FITSHeaderInstance.cpp - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "FITSHeaderInstance.h"

#include <pcl/View.h>
#include <pcl/ImageWindow.h> // for UndoFlags

namespace pcl
{

// ----------------------------------------------------------------------------

FITSHeaderInstance::FITSHeaderInstance( const MetaProcess* m ) : ProcessImplementation( m ), keywords()
{
}

FITSHeaderInstance::FITSHeaderInstance( const FITSHeaderInstance& x ) : ProcessImplementation( x ), keywords()
{
   keywords = x.keywords;
}

void FITSHeaderInstance::Assign( const ProcessImplementation& p )
{
   const FITSHeaderInstance* x = dynamic_cast<const FITSHeaderInstance*>( &p );
   if ( x != 0 )
      keywords = x->keywords;
}

UndoFlags FITSHeaderInstance::UndoMode( const View& ) const
{
   return UndoFlag::Keywords;
}

bool FITSHeaderInstance::IsMaskable( const View&, const ImageWindow& /*mask*/ ) const
{
   return false;
}

bool FITSHeaderInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( !v.IsMainView() )
   {
      whyNot = "FITSHeader can only be executed on main views, not on previews.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool FITSHeaderInstance::ExecuteOn( View& view )
{
   FITSKeywordArray K, K0;

   // Collect current reserved keywords
   view.Window().GetKeywords( K0 );
   for ( FITSKeywordArray::iterator i = K0.Begin(); i != K0.End(); ++i )
      if ( IsReservedKeyword( i->name ) )
         K.Add( *i );

   // Append new non-reserved keywords
   for ( keyword_list::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
      if ( !IsReservedKeyword( i->name ) )
         K.Add( FITSHeaderKeyword( IsoString( i->name ),
                                   IsoString( i->value ),
                                   IsoString( i->comment ) ) );

   // Update keyword set
   view.Window().SetKeywords( K );

   // Done Ok
   return true;
}

void* FITSHeaderInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheFITSKeywordNameParameter )
      return keywords[tableRow].name.c_str();
   if ( p == TheFITSKeywordValueParameter )
      return keywords[tableRow].value.c_str();
   if ( p == TheFITSKeywordCommentParameter )
      return keywords[tableRow].comment.c_str();
   return 0;
}

bool FITSHeaderInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheFITSKeywordNameParameter )
   {
      keywords[tableRow].name.Clear();
      if ( sizeOrLength > 0 )
         keywords[tableRow].name.SetLength( sizeOrLength );
   }
   else if ( p == TheFITSKeywordValueParameter )
   {
      keywords[tableRow].value.Clear();
      if ( sizeOrLength > 0 )
         keywords[tableRow].value.SetLength( sizeOrLength );
   }
   else if ( p == TheFITSKeywordCommentParameter )
   {
      keywords[tableRow].comment.Clear();
      if ( sizeOrLength > 0 )
         keywords[tableRow].comment.SetLength( sizeOrLength );
   }
   else if ( p == TheFITSKeywordTableParameter )
   {
      keywords.Clear();
      if ( sizeOrLength > 0 )
         keywords.Add( Keyword(), sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type FITSHeaderInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheFITSKeywordNameParameter )
      return keywords[tableRow].name.Length();
   if ( p == TheFITSKeywordValueParameter )
      return keywords[tableRow].value.Length();
   if ( p == TheFITSKeywordCommentParameter )
      return keywords[tableRow].comment.Length();
   if ( p == TheFITSKeywordTableParameter )
      return keywords.Length();
   return 0;
}

// ----------------------------------------------------------------------------

void FITSHeaderInstance::ImportKeywords( const ImageWindow& w )
{
   keywords.Clear();

   FITSKeywordArray K;
   w.GetKeywords( K );

   for ( FITSKeywordArray::const_iterator i = K.Begin(); i != K.End(); ++i )
      keywords.Add( Keyword( i->name, i->value, i->comment ) );
}

bool FITSHeaderInstance::IsReservedKeyword( const String& id )
{
   static const char* cReserved[] =
   {
      // Structural/Critical keywords
      "SIMPLE",
      "BITPIX",
      "NAXIS",
      "NAXIS1",
      "NAXIS2",
      "NAXIS3",
      "NAXIS4",
      "NAXIS5",
      "NAXIS6",
      "NAXIS7",
      "NAXIS8",
      "NAXIS9",
      "NAXIS10",
      "EXTEND",
      "BSCALE",
      "BZERO",
      "EXTNAME",

      // PixInsight reserved keywords
      "PINSIGHT",
      "COLORSPC",
      "ALPHACHN",
      "RESOUNIT",
      "RESOLUTN",
      "XRESOLTN",
      "YRESOLTN",
      "ICCPROFL",
      "THUMBIMG",

      0
   };

   static SortedStringList reserved;
   if ( reserved.IsEmpty() )
      for ( const char** i = cReserved; *i != 0; ++i )
         reserved.Add( *i );

   return reserved.Contains( id.Uppercase() );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSHeaderInstance.cpp - Released 2015/12/18 08:55:08 UTC
