//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.01.0016
// ----------------------------------------------------------------------------
// FileDataCache.cpp - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2018 Cameron Leger
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

#include "FileDataCache.h"

#include <pcl/AutoLock.h>
#include <pcl/AutoPointer.h>
#include <pcl/FileInfo.h>
#include <pcl/Math.h>
#include <pcl/Settings.h>

#include <time.h>

namespace pcl
{

// ----------------------------------------------------------------------------

bool FileDataCache::IsEmpty() const
{
   volatile AutoLock lock( m_mutex );
   return m_cache.IsEmpty();
}

// ----------------------------------------------------------------------------

const FileDataCacheItem* FileDataCache::Find( const String& path ) const
{
   volatile AutoLock lock( m_mutex );
   cache_index::const_iterator i = m_cache.Search( FileDataCacheItem( path ) );
   return (i == m_cache.End()) ? nullptr : i;
}

// ----------------------------------------------------------------------------

void FileDataCache::Clear()
{
   volatile AutoLock lock( m_mutex );
   m_cache.Destroy();
}

// ----------------------------------------------------------------------------

void FileDataCache::Add( const FileDataCacheItem& item )
{
   FileInfo info( item.path );
   if ( !info.Exists() || !info.IsFile() )
      throw Error( "FileDataCache::Add(): No such file: " + item.path );

   {
      volatile AutoLock lock( m_mutex );

      cache_index::const_iterator i = m_cache.Search( item );
      if ( i == m_cache.End() )
      {
         FileDataCacheItem* newItem = NewItem();
         newItem->path = item.path;
         m_cache << newItem;
         i = m_cache.Search( newItem );
      }

      FileDataCacheItem& newItem = *m_cache.MutableIterator( i );
      time_t t0 = ::time( 0 );
      const tm* t = ::gmtime( &t0 );
      newItem.lastUsed = unsigned( ComplexTimeToJD( t->tm_year+1900, t->tm_mon+1, t->tm_mday ) );
      newItem.time = info.LastModified();
      newItem.AssignData( item );
   }
}

// ----------------------------------------------------------------------------

bool FileDataCache::Get( FileDataCacheItem& item, const String& path )
{
   FileInfo info( path );
   bool badItem = !info.Exists() || !info.IsFile();

   {
      volatile AutoLock lock( m_mutex );

      cache_index::const_iterator i = m_cache.Search( FileDataCacheItem( path ) );
      if ( i != m_cache.End() )
      {
         if ( !badItem )
         {
            item.Assign( *i );
            item.AssignData( *i );
            if ( !item.ModifiedSince( info.LastModified() ) )
               return true;
         }

         m_cache.Destroy( m_cache.MutableIterator( i ) );
      }
   }

   if ( badItem )
      throw Error( "FileDataCache::Get(): No such file: " + path );

   return false;
}

// ----------------------------------------------------------------------------

void FileDataCache::Load()
{
   m_cache.Destroy();

   m_durationDays = 30;
   Settings::Read( m_keyPrefix + "Duration", m_durationDays );
   m_durationDays = Max( 0, m_durationDays );

   m_enabled = true;
   Settings::Read( m_keyPrefix + "Enabled", m_enabled );

   if ( IsEnabled() )
   {
      try
      {
         AutoPointer<FileDataCacheItem> item;
         for ( int i = 0; ; ++i )
         {
            item = NewItem();
            if ( !item->Load( m_keyPrefix, i ) )
               break;

            if ( m_durationDays > 0 && item->DaysSinceLastUsed() > unsigned( m_durationDays ) )
               item.Destroy();
            else
               m_cache << item.Release();
         }
      }
      catch ( ... )
      {
         m_cache.Destroy();
         throw Error( "FileDataCache::Load(): Corrupted cache data" );
      }
   }
}

// ----------------------------------------------------------------------------

void FileDataCache::Save() const
{
   if ( IsEnabled() )
   {
      Purge();
      int index = 0;
      for ( const FileDataCacheItem& item : m_cache )
         item.Save( m_keyPrefix, index++ );
   }

   // Make sure this is done _after_ Purge()
   Settings::Write( m_keyPrefix + "Duration", Duration() );
   Settings::Write( m_keyPrefix + "Enabled", IsEnabled() );
}

// ----------------------------------------------------------------------------

void FileDataCache::Purge() const
{
   IsoString key = m_keyPrefix;
   if ( key.EndsWith( '/' ) )
      key.DeleteRight( key.UpperBound() );
   Settings::Remove( key );
}

// ----------------------------------------------------------------------------

unsigned FileDataCacheItem::DaysSinceLastUsed() const
{
   time_t t0 = ::time( 0 );
   const tm* t = ::gmtime( &t0 );
   return unsigned( ComplexTimeToJD( t->tm_year+1900, t->tm_mon+1, t->tm_mday ) ) - lastUsed;
}

// ----------------------------------------------------------------------------

String FileDataCacheItem::AsString() const
{
   String s( "path\n" + path );
   s.AppendFormat( "\nlastUsed\n%u\ntime\n%u\n%u",
                   lastUsed,
                   unsigned( ComplexTimeToJD( time.year, time.month, time.day ) ),
                   ((time.hour*60 + time.minute)*60 + time.second)*1000 + time.milliseconds );
   String data = DataAsString();
   if ( !data.IsEmpty() )
   {
      s.Append( "\ndata\n" );
      s.Append( data );
   }
   return s;
}

// ----------------------------------------------------------------------------

bool FileDataCacheItem::FromString( const String& s )
{
   path.Clear();
   lastUsed = 0;
   time.year = 0;

   StringList tokens;
   s.Break( tokens, char16_type( '\n' ) );

   for ( StringList::const_iterator i = tokens.Begin(); i != tokens.End(); )
   {
      if ( *i == "path" )
      {
         if ( ++i == tokens.End() )
            return false;
         path = i->Trimmed();
         ++i;
      }
      else if ( *i == "lastUsed" )
      {
         if ( ++i == tokens.End() )
            return false;
         lastUsed = i->ToUInt();
         ++i;
      }
      else if ( *i == "time" )
      {
         if ( tokens.End() - i < 3 )
            return false;
         int y, m, d; double f;
         JDToComplexTime( y, m, d, f, i[1].ToUInt()+0.5 );
         unsigned t = i[2].ToUInt();
         time.year = y;
         time.month = m;
         time.day = d;
         time.milliseconds = t - 86399000;
         time.hour = TruncI( (t = TruncI( t/1000.0 ))/3600.0 );
         time.minute = TruncI( (t -= time.hour*3600)/60.0 );
         time.second = t - time.minute*60;
         i += 3;
      }
      else if ( *i == "data" )
      {
         if ( !GetDataFromTokens( tokens ) )
            return false;
         ++i;
      }
      else
      {
         ++i;
      }
   }

   return !path.IsEmpty() && lastUsed > 0 && time.year > 0 && ValidateData();
}

// ----------------------------------------------------------------------------

bool FileDataCacheItem::Load( const IsoString& keyPrefix, int index )
{
   String s;
   if ( !Settings::Read( keyPrefix + IsoString().Format( "%08d", index+1 ), s ) )
      return false;
   if ( s.IsEmpty() || !FromString( s ) )
      throw CaughtException();
   return true;
}

// ----------------------------------------------------------------------------

void FileDataCacheItem::Save( const IsoString& keyPrefix, int index ) const
{
   Settings::Write( keyPrefix + IsoString().Format( "%08d", index+1 ), AsString() );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FileDataCache.cpp - Released 2018-12-12T09:25:25Z
