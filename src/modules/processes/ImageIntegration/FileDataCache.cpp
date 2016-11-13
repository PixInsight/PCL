//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.11.00.0344
// ----------------------------------------------------------------------------
// FileDataCache.cpp - Released 2016/11/13 17:30:54 UTC
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

#include "FileDataCache.h"

#include <pcl/FileInfo.h>
#include <pcl/Settings.h>
#include <pcl/Math.h>

#include <time.h>

namespace pcl
{

// ----------------------------------------------------------------------------

void FileDataCache::Add( const FileDataCacheItem& item )
{
   FileInfo info( item.path );
   if ( !info.Exists() || !info.IsFile() )
      throw Error( "FileDataCache::Add(): No such file: " + item.path );

   cache_index::const_iterator i = m_cache.Search( item );
   if ( i == m_cache.End() )
   {
      FileDataCacheItem* newItem = NewItem();
      newItem->path = item.path;
      m_cache.Add( newItem );
      i = m_cache.Search( newItem );
   }

#define newItem (*const_cast<cache_index::iterator>( i ))
   time_t t0 = ::time( 0 );
   const tm* t = ::gmtime( &t0 );
   newItem->lastUsed = unsigned( ComplexTimeToJD( t->tm_year+1900, t->tm_mon+1, t->tm_mday ) );
   newItem->time = info.LastModified();
   newItem->AssignData( item );
#undef newItem
}

bool FileDataCache::Get( FileDataCacheItem& item, const String& path )
{
   cache_index::const_iterator i = m_cache.Search( FileDataCacheItem( path ) );
   if ( i == m_cache.End() )
      return false;

   FileInfo info( path );
   if ( !info.Exists() || !info.IsFile() )
      throw Error( "FileDataCache::Get(): No such file: " + path );

   item.Assign( **i );
   item.AssignData( **i );
   if ( !item.ModifiedSince( info.LastModified() ) )
      return true;

   m_cache.Destroy( m_cache.MutableIterator( i ) );
   return false;
}

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
      FileDataCacheItem* item = 0;

      try
      {
         for ( int i = 0; ; ++i )
         {
            item = NewItem();

            if ( !item->Load( m_keyPrefix, i ) )
            {
               delete item, item = 0;
               break;
            }

            if ( m_durationDays > 0 && item->DaysSinceLastUsed() > unsigned( m_durationDays ) )
               delete item;
            else
               m_cache.Add( item );

            item = 0;
         }
      }
      catch ( ... )
      {
         if ( item != 0 )
            delete item;
         m_cache.Destroy();
         throw Error( "FileDataCache::Load(): Corrupted cache data" );
      }
   }
}

void FileDataCache::Save() const
{
   if ( IsEnabled() )
   {
      Purge();
      int index = 0;
      for ( cache_index::const_iterator i = m_cache.Begin(); i != m_cache.End(); ++i )
         (*i)->Save( m_keyPrefix, index++ );
   }

   // Make sure this is done after Purge()
   Settings::Write( m_keyPrefix + "Duration", Duration() );
   Settings::Write( m_keyPrefix + "Enabled", IsEnabled() );
}

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

String FileDataCacheItem::VectorAsString( const DVector& v )
{
   String s = String().Format( "\n%d", v.Length() );
   for ( int i = 0; i < v.Length(); ++i )
      s.AppendFormat( "\n%.8e", v[i] );
   return s;
}

bool FileDataCacheItem::GetVector( DVector& v, StringList::const_iterator& i, const StringList& s )
{
   if ( i == s.End() )
      return false;
   int n = i->ToInt();
   if ( n < 0 || s.End() - i <= n )
      return false;
   ++i;
   v = DVector( n );
   for ( int j = 0; j < n; ++j, ++i )
      v[j] = i->ToDouble();
   return true;
}

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

bool FileDataCacheItem::Load( const IsoString& keyPrefix, int index )
{
   String s;
   if ( !Settings::Read( keyPrefix + IsoString().Format( "%08d", index+1 ), s ) )
      return false;
   if ( s.IsEmpty() || !FromString( s ) )
      throw CatchedException();
   return true;
}

void FileDataCacheItem::Save( const IsoString& keyPrefix, int index ) const
{
   Settings::Write( keyPrefix + IsoString().Format( "%08d", index+1 ), AsString() );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FileDataCache.cpp - Released 2016/11/13 17:30:54 UTC
