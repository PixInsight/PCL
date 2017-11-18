//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.02.01.0002
// ----------------------------------------------------------------------------
// SubframeSelectorCache.cpp - Released 2017-11-05T16:00:00Z
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

#include "SubframeSelectorCache.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorCache* TheSubframeSelectorCache = nullptr;

// ----------------------------------------------------------------------------

void SubframeSelectorCacheItem::AssignData( const FileDataCacheItem& item )
{
#define src static_cast<const SubframeSelectorCacheItem&>( item )
   cacheVersion        = src.cacheVersion;
   fwhm                = src.fwhm;
   fwhmMeanDev         = src.fwhmMeanDev;
   eccentricity        = src.eccentricity;
   eccentricityMeanDev = src.eccentricityMeanDev;
   snrWeight           = src.snrWeight;
   median              = src.median;
   medianMeanDev       = src.medianMeanDev;
   noise               = src.noise;
   noiseRatio          = src.noiseRatio;
   stars               = src.stars;
   starResidual        = src.starResidual;
   starResidualMeanDev = src.starResidualMeanDev;
#undef src
}

// ----------------------------------------------------------------------------

String SubframeSelectorCacheItem::DataAsString() const
{
   StringList tokens;
   tokens.Append( String().Format( "cacheVersion\n%i", cacheVersion ) );
   tokens.Append( String().Format( "fwhm\n%.8f", fwhm ) );
   tokens.Append( String().Format( "fwhmMeanDev\n%.8f", fwhmMeanDev ) );
   tokens.Append( String().Format( "eccentricity\n%.8f", eccentricity ) );
   tokens.Append( String().Format( "eccentricityMeanDev\n%.8f", eccentricityMeanDev ) );
   tokens.Append( String().Format( "snrWeight\n%.8f", snrWeight ) );
   tokens.Append( String().Format( "median\n%.8f", median ) );
   tokens.Append( String().Format( "medianMeanDev\n%.8f", medianMeanDev ) );
   tokens.Append( String().Format( "noise\n%.8f", noise ) );
   tokens.Append( String().Format( "noiseRatio\n%.8f", noiseRatio ) );
   tokens.Append( String().Format( "stars\n%u", stars ) );
   tokens.Append( String().Format( "starResidual\n%.8f", starResidual ) );
   tokens.Append( String().Format( "starResidualMeanDev\n%.8f", starResidualMeanDev ) );
   return String().ToNewLineSeparated( tokens );
}

// ----------------------------------------------------------------------------

bool SubframeSelectorCacheItem::GetDataFromTokens( const StringList& tokens )
{
   AssignData( SubframeSelectorCacheItem() );

   for ( StringList::const_iterator i = tokens.Begin(); i != tokens.End(); )
      if ( *i == "cacheVersion" )
      {
         if ( !(++i)->TryToInt( cacheVersion ) )
            return false;
      }
      else if ( *i == "fwhm" )
      {
         if ( !(++i)->TryToDouble( fwhm ) )
            return false;
      }
      else if ( *i == "fwhmMeanDev" )
      {
         if ( !(++i)->TryToDouble( fwhmMeanDev ) )
            return false;
      }
      else if ( *i == "eccentricity" )
      {
         if ( !(++i)->TryToDouble( eccentricity ) )
            return false;
      }
      else if ( *i == "eccentricityMeanDev" )
      {
         if ( !(++i)->TryToDouble( eccentricityMeanDev ) )
            return false;
      }
      else if ( *i == "snrWeight" )
      {
         if ( !(++i)->TryToDouble( snrWeight ) )
            return false;
      }
      else if ( *i == "median" )
      {
         if ( !(++i)->TryToDouble( median ) )
            return false;
      }
      else if ( *i == "medianMeanDev" )
      {
         if ( !(++i)->TryToDouble( medianMeanDev ) )
            return false;
      }
      else if ( *i == "noise" )
      {
         if ( !(++i)->TryToDouble( noise ) )
            return false;
      }
      else if ( *i == "noiseRatio" )
      {
         if ( !(++i)->TryToDouble( noiseRatio ) )
            return false;
      }
      else if ( *i == "stars" )
      {
         unsigned int s;
         if ( !(++i)->TryToUInt( s ) )
            return false;
         stars = static_cast<uint16>(s);
      }
      else if ( *i == "starResidual" )
      {
         if ( !(++i)->TryToDouble( starResidual ) )
            return false;
      }
      else if ( *i == "starResidualMeanDev" )
      {
         if ( !(++i)->TryToDouble( starResidualMeanDev ) )
            return false;
      }
      else
      {
         ++i;
      }

   return true;
}

// ----------------------------------------------------------------------------

SubframeSelectorCache::SubframeSelectorCache() : FileDataCache( "/SubframeSelector/Cache" )
{
   if ( TheSubframeSelectorCache == nullptr )
      TheSubframeSelectorCache = this;
   Load();
}

// ----------------------------------------------------------------------------

SubframeSelectorCache::~SubframeSelectorCache()
{
   if ( TheSubframeSelectorCache == this )
      TheSubframeSelectorCache = nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorCache.cpp - Released 2017-11-05T16:00:00Z
