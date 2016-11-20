//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.11.00.0344
// ----------------------------------------------------------------------------
// FileDataCache.h - Released 2016/11/13 17:30:54 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __FileDataCache_h
#define __FileDataCache_h

#include <pcl/File.h>
#include <pcl/IndirectSortedArray.h>
#include <pcl/Vector.h>
#include <pcl/StringList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS FileDataCacheItem
{
public:

   String   path;     // full file path
   unsigned lastUsed; // date of last usage, JD.0
   FileTime time;     // cached file time

   virtual ~FileDataCacheItem()
   {
   }

   void Assign( const FileDataCacheItem& i )
   {
      path     = i.path;
      lastUsed = i.lastUsed;
      time     = i.time;
   }

   bool operator ==( const FileDataCacheItem& i ) const
   {
      return path == i.path;
   }

   bool operator <( const FileDataCacheItem& i ) const
   {
      return path < i.path;
   }

   bool ModifiedSince( FileTime t ) const
   {
      if ( time.year != t.year )
         return time.year < t.year;
      if ( time.month != t.month )
         return time.month < t.month;
      if ( time.day != t.day )
         return time.day < t.day;
      if ( time.hour != t.hour )
         return time.hour < t.hour;
      if ( time.minute != t.minute )
         return time.minute < t.minute;
      if ( time.second != t.second )
         return time.second < t.second;
#if 0
      /*
       * File time milliseconds are only available on Windows, but they are
       * unreliable and generate wrong cache invalidations.
       */
      if ( time.milliseconds != t.milliseconds )
         return time.milliseconds < t.milliseconds;
#endif
      return false;
   }

   unsigned DaysSinceLastUsed() const;

protected:

   virtual void AssignData( const FileDataCacheItem& )
   {
   }

   virtual String DataAsString() const
   {
      return String();
   }

   virtual bool GetDataFromTokens( const StringList& )
   {
      return true;
   }

   virtual bool ValidateData() const
   {
      return true;
   }

   // Utility auxiliary routines
   static String VectorAsString( const DVector& );
   static bool GetVector( DVector&, StringList::const_iterator&, const StringList& );

   // Fake ctor. for searching items
   FileDataCacheItem( const String& p = String() ) : path( p )
   {
   }

   // Copy ctor.
   FileDataCacheItem( const FileDataCacheItem& item )
   {
      (void)operator =( item );
   }

private:

   String AsString() const;
   bool FromString( const String& s );

   bool Load( const IsoString& keyPrefix, int index );
   void Save( const IsoString& keyPrefix, int index ) const;

   friend class FileDataCache;
};

// ----------------------------------------------------------------------------

class PCL_CLASS FileDataCache
{
public:

   FileDataCache( const IsoString& key, int days = 30 ) :
   m_cache(), m_keyPrefix( key.Trimmed() ), m_durationDays( Max( 0, days ) ), m_enabled( true )
   {
      PCL_PRECONDITION( !m_keyPrefix.IsEmpty() )
      if ( m_keyPrefix.IsEmpty() )
         throw Error( "FileDataCache: Invalid key" );
      if ( !m_keyPrefix.StartsWith( '/' ) )
         m_keyPrefix.Prepend( '/' );
      if ( !m_keyPrefix.EndsWith( '/' ) )
         m_keyPrefix.Append( '/' );
   }

   virtual ~FileDataCache()
   {
      Clear();
   }

   virtual String CacheName() const
   {
      return "File Cache";
   }

   bool IsEnabled() const
   {
      return m_enabled;
   }

   void Enable( bool enable )
   {
      m_enabled = enable;
   }

   int Duration() const
   {
      return m_durationDays;
   }

   void SetDuration( int days )
   {
      m_durationDays = Max( 0, days );
   }

   bool NeverExpires() const
   {
      return m_durationDays <= 0;
   }

   size_type NumberOfItems() const
   {
      return m_cache.Length();
   }

   bool IsEmpty() const
   {
      return m_cache.IsEmpty();
   }

   const FileDataCacheItem* Find( const String& path ) const
   {
      cache_index::const_iterator i = m_cache.Search( FileDataCacheItem( path ) );
      return (i == m_cache.End()) ? 0 : *i;
   }

   void Add( const FileDataCacheItem& );
   bool Get( FileDataCacheItem&, const String& path );

   void Clear()
   {
      m_cache.Destroy();
   }

   virtual void Load();
   virtual void Save() const;
   virtual void Purge() const;

protected:

   virtual FileDataCacheItem* NewItem() const = 0;

private:

   typedef IndirectSortedArray<FileDataCacheItem> cache_index;

   cache_index m_cache;
   IsoString   m_keyPrefix;
   int         m_durationDays; // <= 0 -> never expires
   bool        m_enabled;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FileDataCache_h

// ----------------------------------------------------------------------------
// EOF FileDataCache.h - Released 2016/11/13 17:30:54 UTC
