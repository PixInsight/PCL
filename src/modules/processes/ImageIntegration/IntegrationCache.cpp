//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.15.00.0398
// ----------------------------------------------------------------------------
// IntegrationCache.cpp - Released 2017-05-05T08:37:32Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#include "IntegrationCache.h"

#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IntegrationCache* TheIntegrationCache = nullptr;

// ----------------------------------------------------------------------------

void IntegrationCacheItem::AssignData( const FileDataCacheItem& item )
{
#define src static_cast<const IntegrationCacheItem&>( item )
   mean     = src.mean;
   median   = src.median;
   stdDev   = src.stdDev;
   avgDev   = src.avgDev;
   mad      = src.mad;
   bwmv     = src.bwmv;
   pbmv     = src.pbmv;
   sn       = src.sn;
   qn       = src.qn;
   ikss     = src.ikss;
   iksl     = src.iksl;
   noise    = src.noise;
   metadata = src.metadata;
#undef src
}

// ----------------------------------------------------------------------------

String IntegrationCacheItem::DataAsString() const
{
   StringList tokens;
   if ( !mean.IsEmpty() )
      tokens.Append( "mean" + VectorAsString( mean ) );
   if ( !median.IsEmpty() )
      tokens.Append( "median" + VectorAsString( median ) );
   if ( !stdDev.IsEmpty() )
      tokens.Append( "stdDev" + VectorAsString( stdDev ) );
   if ( !avgDev.IsEmpty() )
      tokens.Append( "avgDev" + VectorAsString( avgDev ) );
   if ( !mad.IsEmpty() )
      tokens.Append( "mad" + VectorAsString( mad ) );
   if ( !bwmv.IsEmpty() )
      tokens.Append( "bwmv" + VectorAsString( bwmv ) );
   if ( !pbmv.IsEmpty() )
      tokens.Append( "pbmv" + VectorAsString( pbmv ) );
   if ( !sn.IsEmpty() )
      tokens.Append( "sn" + VectorAsString( sn ) );
   if ( !qn.IsEmpty() )
      tokens.Append( "qn" + VectorAsString( qn ) );
   if ( !ikss.IsEmpty() )
      tokens.Append( "ikss" + VectorAsString( ikss ) );
   if ( !iksl.IsEmpty() )
      tokens.Append( "iksl" + VectorAsString( iksl ) );
   if ( !noise.IsEmpty() )
      tokens.Append( "noise" + VectorAsString( noise ) );
   if ( !metadata.IsEmpty() )
      tokens.Append( "metadata\n" + metadata );

   return String().ToNewLineSeparated( tokens );
}

// ----------------------------------------------------------------------------

bool IntegrationCacheItem::GetDataFromTokens( const StringList& tokens )
{
   AssignData( IntegrationCacheItem() );

   for ( StringList::const_iterator i = tokens.Begin(); i != tokens.End(); )
      if ( *i == "mean" )
      {
         if ( !GetVector( mean, ++i, tokens ) )
            return false;
      }
      else if ( *i == "median" )
      {
         if ( !GetVector( median, ++i, tokens ) )
            return false;
      }
      else if ( *i == "stdDev" )
      {
         if ( !GetVector( stdDev, ++i, tokens ) )
            return false;
      }
      else if ( *i == "avgDev" )
      {
         if ( !GetVector( avgDev, ++i, tokens ) )
            return false;
      }
      else if ( *i == "mad" )
      {
         if ( !GetVector( mad, ++i, tokens ) )
            return false;
      }
      else if ( *i == "bwmv" )
      {
         if ( !GetVector( bwmv, ++i, tokens ) )
            return false;
      }
      else if ( *i == "pbmv" )
      {
         if ( !GetVector( pbmv, ++i, tokens ) )
            return false;
      }
      else if ( *i == "sn" )
      {
         if ( !GetVector( sn, ++i, tokens ) )
            return false;
      }
      else if ( *i == "qn" )
      {
         if ( !GetVector( qn, ++i, tokens ) )
            return false;
      }
      else if ( *i == "ikss" )
      {
         if ( !GetVector( ikss, ++i, tokens ) )
            return false;
      }
      else if ( *i == "iksl" )
      {
         if ( !GetVector( iksl, ++i, tokens ) )
            return false;
      }
      else if ( *i == "noise" )
      {
         if ( !GetVector( noise, ++i, tokens ) )
            return false;
      }
      else if ( *i == "metadata" )
      {
         if ( ++i == tokens.End() )
            return false;
         metadata = *i;
      }
      else
      {
         ++i;
      }

   return true;
}

// ----------------------------------------------------------------------------

IntegrationCache::IntegrationCache() : FileDataCache( "/ImageIntegration/Cache" )
{
   if ( TheIntegrationCache == nullptr )
      TheIntegrationCache = this;
   Load();
}

// ----------------------------------------------------------------------------

IntegrationCache::~IntegrationCache()
{
   if ( TheIntegrationCache == this )
      TheIntegrationCache = nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegrationCache.cpp - Released 2017-05-05T08:37:32Z
