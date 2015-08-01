//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.09.04.0274
// ----------------------------------------------------------------------------
// IntegrationCache.h - Released 2015/07/31 11:49:48 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#ifndef __IntegrationCache_h
#define __IntegrationCache_h

#include "FileDataCache.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class IntegrationCacheItem : public FileDataCacheItem
{
public:

   DVector mean;
   DVector median;
   DVector stdDev;
   DVector avgDev;
   DVector mad;
   DVector bwmv;
   DVector pbmv;
   DVector sn;
   DVector qn;
   DVector ikss;
   DVector iksl;
   DVector noise;
   float   pedestal;

   IntegrationCacheItem( const String& path = String() ) :
   FileDataCacheItem( path ), pedestal( -1 )
   {
   }

   virtual ~IntegrationCacheItem()
   {
   }

private:

   virtual void AssignData( const FileDataCacheItem& item );
   virtual String DataAsString() const;
   virtual bool GetDataFromTokens( const StringList& tokens );
};

// ----------------------------------------------------------------------------

class IntegrationCache : public FileDataCache
{
public:

   IntegrationCache();
   virtual ~IntegrationCache();

   virtual String CacheName() const
   {
      return "ImageIntegration Cache";
   }

private:

   virtual FileDataCacheItem* NewItem() const
   {
      return new IntegrationCacheItem;
   }
};

extern IntegrationCache* TheIntegrationCache;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __IntegrationCache_h

// ----------------------------------------------------------------------------
// EOF IntegrationCache.h - Released 2015/07/31 11:49:48 UTC
