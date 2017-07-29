//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/Settings.cpp - Released 2017-07-18T16:14:00Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/Settings.h>
#include <pcl/MetaModule.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#ifdef ERROR
#undef ERROR
#endif

#define ERROR( what, key ) throw APIError( String( what ) + ": " + key )

// ----------------------------------------------------------------------------

static bool s_lastReadOK = false;

// ----------------------------------------------------------------------------

bool Settings::Read( const IsoString& key, ByteArray& a )
{
   uint8* p = 0;
   size_type n = 0;
   s_lastReadOK = (*API->Global->ReadSettingsBlock)( ModuleHandle(), (void**)&p, &n, key.c_str(), api_false ) != api_false;

   if ( s_lastReadOK && p != 0 )
   {
      if ( n != 0 )
         a.Assign( p, p+n );
      else
         a.Clear();

      Module->Deallocate( p );
   }

   return s_lastReadOK;
}

bool Settings::ReadGlobal( const IsoString& key, ByteArray& a )
{
   uint8* p = 0;
   size_type n = 0;
   s_lastReadOK = (*API->Global->ReadSettingsBlock)( ModuleHandle(), (void**)&p, &n, key.c_str(), api_true ) != api_false;

   if ( s_lastReadOK && p != 0 )
   {
      if ( n != 0 )
         a.Assign( p, p+n );
      else
         a.Clear();

      Module->Deallocate( p );
   }

   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

bool Settings::Read( const IsoString& key, String& s )
{
   char16_type* p = 0;
   s_lastReadOK = (*API->Global->ReadSettingsString)( ModuleHandle(), &p, key.c_str(), api_false ) != api_false;

   if ( s_lastReadOK && p != 0 )
   {
      s = String( p );
      Module->Deallocate( p );
   }

   return s_lastReadOK;
}

bool Settings::ReadGlobal( const IsoString& key, String& s )
{
   char16_type* p = 0;
   s_lastReadOK = (*API->Global->ReadSettingsString)( ModuleHandle(), &p, key.c_str(), api_true ) != api_false;

   if ( s_lastReadOK && p != 0 )
   {
      s = String( p );
      Module->Deallocate( p );
   }

   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

bool Settings::Read( const IsoString& key, bool& b )
{
   api_bool ab;
   s_lastReadOK = (*API->Global->ReadSettingsFlag)( ModuleHandle(), &ab, key.c_str(), api_false ) != api_false;
   if ( s_lastReadOK )
      b = ab != api_false;
   return s_lastReadOK;
}

bool Settings::ReadGlobal( const IsoString& key, bool& b )
{
   api_bool ab;
   s_lastReadOK = (*API->Global->ReadSettingsFlag)( ModuleHandle(), &ab, key.c_str(), api_true ) != api_false;
   if ( s_lastReadOK )
      b = ab != api_false;
   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

bool Settings::Read( const IsoString& key, int& i )
{
   int32 ai;
   s_lastReadOK = (*API->Global->ReadSettingsInteger)( ModuleHandle(), &ai, key.c_str(), api_false ) != api_false;
   if ( s_lastReadOK )
      i = ai;
   return s_lastReadOK;
}

bool Settings::ReadGlobal( const IsoString& key, int& i )
{
   int32 ai;
   s_lastReadOK = (*API->Global->ReadSettingsInteger)( ModuleHandle(), &ai, key.c_str(), api_true ) != api_false;
   if ( s_lastReadOK )
      i = ai;
   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

bool Settings::Read( const IsoString& key, unsigned& u )
{
   uint32 au;
   s_lastReadOK = (*API->Global->ReadSettingsUnsignedInteger)( ModuleHandle(), &au, key.c_str(), api_false ) != api_false;
   if ( s_lastReadOK )
      u = au;
   return s_lastReadOK;
}

bool Settings::ReadGlobal( const IsoString& key, unsigned& u )
{
   uint32 au;
   s_lastReadOK = (*API->Global->ReadSettingsUnsignedInteger)( ModuleHandle(), &au, key.c_str(), api_true ) != api_false;
   if ( s_lastReadOK )
      u = au;
   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

bool Settings::Read( const IsoString& key, double& d )
{
   double ad;
   s_lastReadOK = (*API->Global->ReadSettingsReal)( ModuleHandle(), &ad, key.c_str(), api_false ) != api_false;
   if ( s_lastReadOK )
      d = ad;
   return s_lastReadOK;
}

bool Settings::ReadGlobal( const IsoString& key, double& d )
{
   double ad;
   s_lastReadOK = (*API->Global->ReadSettingsReal)( ModuleHandle(), &ad, key.c_str(), api_true ) != api_false;
   if ( s_lastReadOK )
      d = ad;
   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

bool Settings::LastReadOK()
{
   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

void Settings::Write( const IsoString& key, const ByteArray& a )
{
   if ( (*API->Global->WriteSettingsBlock)( ModuleHandle(), a.Begin(), a.Length(), key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error writing local ByteArray data", key );
}

void Settings::WriteGlobal( const IsoString& key, const ByteArray& a )
{
   if ( (*API->Global->WriteSettingsBlock)( ModuleHandle(), a.Begin(), a.Length(), key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error writing global ByteArray data", key );
}

// ----------------------------------------------------------------------------

void Settings::Write( const IsoString& key, const pcl::String& s )
{
   if ( (*API->Global->WriteSettingsString)( ModuleHandle(), s.c_str(), key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error writing local String data", key );
}

void Settings::WriteGlobal( const IsoString& key, const pcl::String& s )
{
   if ( (*API->Global->WriteSettingsString)( ModuleHandle(), s.c_str(), key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error writing global String data", key );
}

// ----------------------------------------------------------------------------

void Settings::Write( const IsoString& key, bool b )
{
   if ( (*API->Global->WriteSettingsFlag)( ModuleHandle(), b, key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error writing local boolean data", key );
}

void Settings::WriteGlobal( const IsoString& key, bool b )
{
   if ( (*API->Global->WriteSettingsFlag)( ModuleHandle(), b, key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error writing global boolean data", key );
}

// ----------------------------------------------------------------------------

void Settings::Write( const IsoString& key, int i )
{
   if ( (*API->Global->WriteSettingsInteger)( ModuleHandle(), i, key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error writing local integer data", key );
}

void Settings::WriteGlobal( const IsoString& key, int i )
{
   if ( (*API->Global->WriteSettingsInteger)( ModuleHandle(), i, key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error writing global integer data", key );
}

// ----------------------------------------------------------------------------

void Settings::Write( const IsoString& key, unsigned u )
{
   if ( (*API->Global->WriteSettingsUnsignedInteger)( ModuleHandle(), u, key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error writing local unsigned integer data", key );
}

void Settings::WriteGlobal( const IsoString& key, unsigned u )
{
   if ( (*API->Global->WriteSettingsUnsignedInteger)( ModuleHandle(), u, key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error writing global unsigned integer data", key );
}

// ----------------------------------------------------------------------------

void Settings::Write( const IsoString& key, double d )
{
   if ( (*API->Global->WriteSettingsReal)( ModuleHandle(), d, key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error writing local floating point data", key );
}

void Settings::WriteGlobal( const IsoString& key, double d )
{
   if ( (*API->Global->WriteSettingsReal)( ModuleHandle(), d, key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error writing global floating point data", key );
}

// ----------------------------------------------------------------------------

void Settings::Remove( const IsoString& key )
{
   if ( (*API->Global->DeleteSettingsItem)( ModuleHandle(), key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error removing local settings key", key );
}

void Settings::RemoveGlobal( const IsoString& key )
{
   if ( (*API->Global->DeleteSettingsItem)( ModuleHandle(), key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error removing global settings key", key );
}

// ----------------------------------------------------------------------------

bool Settings::CanReadGlobal( const IsoString& key )
{
   return ((*API->Global->GetSettingsItemGlobalAccess)( ModuleHandle(), key.c_str() ) & 0x02) != 0;
}

bool Settings::CanWriteGlobal( const IsoString& key )
{
   return ((*API->Global->GetSettingsItemGlobalAccess)( ModuleHandle(), key.c_str() ) & 0x01) != 0;
}

void Settings::SetGlobalKeyAccess( const IsoString& key, bool allowRead, bool allowWrite )
{
   uint32 flags = 0;
   if ( allowRead ) flags |= 0x02;
   if ( allowWrite ) flags |= 0x01;
   if ( (*API->Global->SetSettingsItemGlobalAccess)( ModuleHandle(), key.c_str(), flags ) == api_false )
      ERROR( "Settings: Error setting global key access", key );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Settings.cpp - Released 2017-07-18T16:14:00Z
