//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// pcl/ICCProfile.cpp - Released 2015/11/26 15:59:45 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/EndianConversions.h>
#include <pcl/File.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ICCProfile.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * ICC profile header structure, 4.2.0 specification.
 */
struct ICCHeader
{
   uint32   size;
   uint32   CMMType;
   uint32   version;
   uint32   profileClass;
   uint32   dataColorSpace;
   uint32   connectionSpace;
   uint8    creationDateTime[ 12 ];
   uint32   signature;
   uint32   primaryPlatform;
   uint32   flags;
   uint32   deviceManufacturer;
   uint32   deviceModel;
   uint64   deviceAttributes;
   uint32   renderingIntent;
   uint32   illuminantX;
   uint32   illuminantY;
   uint32   illuminantZ;
   uint32   creatorSignature;
   uint8    profileID[ 16 ];
   uint8    reserved[ 28 ];

   size_type Size() const
   {
      return size_type( BigToLittleEndian( size ) );
   }

   uint32 Signature() const
   {
      return BigToLittleEndian( signature );
   }
};

// ----------------------------------------------------------------------------

#define header reinterpret_cast<const ICCHeader*>( data.Begin() )

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define ICC_MAGIC_NUMBER   0x61637370 // 'acsp'

static bool IsValidHeader( const ICCHeader* h )
{
   return h != nullptr &&
          h->Size() > sizeof( ICCHeader ) &&
          h->Signature() == ICC_MAGIC_NUMBER;
}

static bool IsValidHeader( const ByteArray& x )
{
   return IsValidHeader( reinterpret_cast<const ICCHeader*>( x.Begin() ) );
}

// ----------------------------------------------------------------------------

static void ThrowError( const String& message )
{
   size_type len;
   (void)(*API->ColorManagement->GetLastErrorMessage)( 0, &len );

   String apiMessage;
   if ( len > 0 )
   {
      apiMessage.SetLength( len );
      if ( (*API->ColorManagement->GetLastErrorMessage)( apiMessage.c_str(), &len ) == api_false )
         apiMessage.Clear();
      else
         apiMessage.ResizeToNullTerminated();
   }

   if ( apiMessage.IsEmpty() )
      apiMessage = '.';
   else
      apiMessage.Prepend( ": " );

   throw Error( message + apiMessage );
}

// ----------------------------------------------------------------------------

bool ICCProfile::IsSameProfile( const ICCProfile& icc ) const
{
#define iccHeader reinterpret_cast<const ICCHeader*>( icc.data.Begin() )

   if ( data.IsEmpty() )
      return icc.data.IsEmpty();
   if ( icc.data.IsEmpty() )
      return data.IsEmpty();
   size_type profileSize = header->Size();
   if ( iccHeader->Size() != profileSize )
      return false;
   return ::memcmp( data.Begin(), icc.data.Begin(), profileSize ) == 0;

#undef iccHeader
}

// ----------------------------------------------------------------------------

size_type ICCProfile::ProfileSize() const
{
   return data.IsEmpty() ? size_type( 0 ) : header->Size();
}

// ----------------------------------------------------------------------------

String ICCProfile::Description() const
{
   if ( data.IsEmpty() )
      return String();

   icc_profile_handle h = (*API->ColorManagement->OpenProfile)( data.Begin() );
   if ( h == 0 )
      ThrowError( "Unable to retrieve ICC profile description" );

   String description = Description( h );

   (*API->ColorManagement->CloseProfile)( h );

   return description;
}

// ----------------------------------------------------------------------------

ICCProfile::color_space ICCProfile::ColorSpace() const
{
   if ( data.IsEmpty() )
      return ICCColorSpace::Unknown;

   icc_profile_handle h = (*API->ColorManagement->OpenProfile)( data.Begin() );
   if ( h == 0 )
      ThrowError( "Unable to retrieve ICC profile color space" );

   ICCProfile::color_space cs =
         ICCProfile::color_space( (*API->ColorManagement->GetProfileColorSpace)( h ) );

   (*API->ColorManagement->CloseProfile)( h );

   return cs;
}

// ----------------------------------------------------------------------------

bool ICCProfile::IsEmbedded() const
{
   return !data.IsEmpty() && (data[44] & 1) != 0;
}

void ICCProfile::SetEmbeddedFlag( bool on )
{
   if ( !data.IsEmpty() )
   {
      data.EnsureUnique();
      if ( on )
         data[44] |= uint8( 1 );
      else
         data[44] &= ~uint8( 1 );
   }
}

// ----------------------------------------------------------------------------

void ICCProfile::Load( const String& profilePath )
{
   Clear();

   File f;

   try
   {
      path = profilePath.Trimmed();
      if ( path.IsEmpty() )
         throw Error( "Empty profile file path." );
      if ( !File::Exists( path ) )
         throw Error( "The profile file does not exist: " + path );

      f.Open( path, FileMode::Read|FileMode::Open|FileMode::ShareRead );

      fsize_type size = f.Size();
      if ( size_type( size ) < sizeof( ICCHeader ) )
         throw Error( "Invalid or corrupted ICC profile: " + path );

      data = ByteArray( size );
      f.Read( data.Begin(), size );

      if ( !IsValidHeader( data ) )
         throw Error( "Invalid or corrupted ICC profile: " + path );

      f.Close();
   }
   catch ( ... )
   {
      f.Close();
      Clear();
      throw;
   }
}

// ----------------------------------------------------------------------------

void ICCProfile::Set( const ByteArray& icc )
{
   Clear();

   if ( !icc.IsEmpty() )
   {
      if ( !IsValidHeader( icc ) )
         throw Error( String().Format( "Invalid or corrupted ICC profile data at %p", icc.Begin() ) );
      data = icc;
   }
}

// ----------------------------------------------------------------------------

void ICCProfile::Set( const void* icc )
{
#define iccHeader reinterpret_cast<const ICCHeader*>( icc )

   Clear();

   if ( !IsValidHeader( iccHeader ) )
      throw Error( String().Format( "Invalid or corrupted ICC profile data at %p", icc ) );
   const uint8* iccBegin = reinterpret_cast<const uint8*>( icc );
   data = ByteArray( iccBegin, iccBegin + iccHeader->Size() );

#undef iccHeader
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ICCProfile::handle ICCProfile::Open( const String& path )
{
   icc_profile_handle h = (*API->ColorManagement->OpenProfileFile)( path.c_str() );
   if ( h == 0 )
      ThrowError( "Cannot open ICC profile: " + path );
   return h;
}

// ----------------------------------------------------------------------------

ICCProfile::handle ICCProfile::Open( const void* data )
{
   icc_profile_handle h = (*API->ColorManagement->OpenProfile)( data );
   if ( h == 0 )
      ThrowError( "Cannot open ICC profile" );
   return h;
}

// ----------------------------------------------------------------------------

void ICCProfile::Close( ICCProfile::handle h )
{
   if ( (*API->ColorManagement->CloseProfile)( h ) == api_false )
      ThrowError( "Cannot close ICC profile" );
}

// ----------------------------------------------------------------------------

bool ICCProfile::IsValidHandle( ICCProfile::handle h )
{
   return (*API->ColorManagement->IsValidProfileHandle)( h ) != api_false;
}

// ----------------------------------------------------------------------------

bool ICCProfile::IsValidFile( const String& path )
{
   return (*API->ColorManagement->IsValidProfileFile)( path.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool ICCProfile::IsValid( const void* data )
{
   return (*API->ColorManagement->IsValidProfile)( data ) != api_false;
}

// ----------------------------------------------------------------------------

String ICCProfile::Description( handle h, const char* language, const char* country )
{
   if ( language == nullptr || *language == '\0' )
      language = "en";
   if ( country == nullptr || *country == '\0' )
      country = "US";

   size_type len;
   (void)(*API->ColorManagement->GetProfileInformation)( h, ::ColorManagementContext::Description,
                                                         language, country, 0, &len );
   String description;
   if ( len > 0 )
   {
      description.SetLength( len );
      if ( (*API->ColorManagement->GetProfileInformation)( h, ::ColorManagementContext::Description,
                                       language, country, description.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetProfileInformation" );
      description.ResizeToNullTerminated();
      description.Trim();
   }
   return description;
}

// ----------------------------------------------------------------------------

StringList ICCProfile::ProfileDirectories()
{
   StringList directories;

   for ( int i = 0; ; ++i )
   {
      size_type len = 0;
      (*API->Global->GetProfilesDirectory)( i, 0, &len );
      if ( len == 0 )
         break;

      String path;
      path.SetLength( len );
      if ( (*API->Global->GetProfilesDirectory)( i, path.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetProfilesDirectory" );
      path.ResizeToNullTerminated();

      directories.Add( path );
   }

   return directories;
}

// ----------------------------------------------------------------------------

static String FindProfileRecursively( const String& description, const String& dirPath, bool exactMatch )
{
   pcl::FindFileInfo info;

   for ( File::Find f( dirPath + "/*" ); f.NextItem( info ); )
      if ( !info.IsDirectory() && !info.attributes.IsFlagSet( FileAttribute::SymbolicLink ) )
      {
         /*
          * ### Don't restrict the search to .icc or .icm files because on the
          *     Mac (and maybe on other platforms too) profiles can have no
          *     file extensions, or even arbitrary extensions.
          */
         //String ext = File::ExtractExtension( info.name );
         //if ( !ext.CompareIC( ".icc" ) || !ext.CompareIC( ".icm" ) )

         String filePath = File::FullPath( dirPath + "/" + info.name );

         ICCProfile::handle h = 0;

         try
         {
            if ( (h = ICCProfile::Open( filePath )) != 0 )
            {
               String desc = ICCProfile::Description( h );
               ICCProfile::Close( h );
               if ( exactMatch ? (desc == description) : desc.ContainsIC( description ) )
                  return filePath;
            }
         }
         catch ( ... )
         {
            // Do not propagate exceptions here.
            if ( h != 0 )
               ICCProfile::Close( h );
         }
      }

   StringList directories;

   for ( File::Find f( dirPath + "/*" ); f.NextItem( info ); )
      if ( info.IsDirectory() && info.name != "." && info.name != ".." )
         directories.Add( info.name );

   for ( StringList::const_iterator i = directories.Begin(); i != directories.End(); ++i )
   {
      String filePath = FindProfileRecursively( description, dirPath + '/' + *i, exactMatch );
      if ( !filePath.IsEmpty() )
         return filePath;
   }

   return String();
}

String ICCProfile::FindInstalledProfile( const String& description, bool exactMatch )
{
   StringList dirs = ProfileDirectories();
   for ( StringList::const_iterator i = dirs.Begin(); i != dirs.End(); ++i )
   {
      String filePath = FindProfileRecursively( description, *i, exactMatch );
      if ( !filePath.IsEmpty() )
         return filePath;
   }

   return String();
}

// ----------------------------------------------------------------------------

static void FindProfilesRecursively( StringList& list, const String& dirPath )
{
   pcl::FindFileInfo info;

   for ( File::Find f( dirPath + "/*" ); f.NextItem( info ); )
      if ( !info.IsDirectory() && !info.attributes.IsFlagSet( FileAttribute::SymbolicLink ) )
      {
         /*
          * ### Don't restrict the search to .icc or .icm files because on the
          *     Mac (and maybe on other platforms too) profiles can have no
          *     file extensions, or even arbitrary extensions.
          */
         //String ext = File::ExtractExtension( info.name );
         //if ( !ext.CompareIC( ".icc" ) || !ext.CompareIC( ".icm" ) )

         String filePath = File::FullPath( dirPath + "/" + info.name );

         File f;

         try
         {
            ICCHeader hdr;
            f.Open( filePath, FileMode::Read|FileMode::Open|FileMode::ShareRead );
            f.Read( &hdr, sizeof( ICCHeader ) );
            f.Close();
            if ( IsValidHeader( &hdr ) )
               list.Add( filePath );
         }
         catch ( ... )
         {
            // Do not propagate exceptions here.
            f.Close();
         }
      }

   StringList directories;

   for ( File::Find f( dirPath + "/*" ); f.NextItem( info ); )
      if ( info.IsDirectory() && info.name != "." && info.name != ".." )
         directories.Add( info.name );

   for ( StringList::const_iterator i = directories.Begin(); i != directories.End(); ++i )
      FindProfilesRecursively( list, dirPath + '/' + *i );
}

StringList ICCProfile::FindProfiles( const String& dirPath )
{
   StringList list;

#ifdef __PCL_WINDOWS
   String path = File::WindowsPathToUnix( dirPath );
#else
   String path = dirPath;
#endif

   path.Trim();

   if ( path.IsEmpty() )
   {
      StringList dirs = ProfileDirectories();
      for ( StringList::const_iterator i = dirs.Begin(); i != dirs.End(); ++i )
         FindProfilesRecursively( list, *i );
   }
   else
   {
      // Strip away last slash except for root directories
      if ( path.Length() > 1 && path.EndsWith( '/' )
#ifdef __PCL_WINDOWS
        && !(path.Length() == 3 && path[1] == ':')
#endif
         )
         path.Delete( path.UpperBound() );

      FindProfilesRecursively( list, path );
   }

   return list;
}

// ----------------------------------------------------------------------------

/*
 * Extract a subset of profile paths and their descriptions from a previously
 * retrieved list of full paths to ICC profile disk files.
 *
 * selectedDescriptionsList   [output] List of ICC profile descriptions for
 *                            selected ICC profiles.
 *
 * selectedPathsList          [output] List of ICC profile full file paths for
 *                            selected ICC profiles.
 *
 * pathList                   Source list of ICC profile full file paths. This
 *                            can be the result of a previous call to
 *                            ICCProfile::Find().
 *
 * colorSpace                 Color spaces (or'ed ICCColorSpace constants) of
 *                            profiles to extract. Zero means any color space.
 *
 * profileClass               Profile classes (or'ed ICCProfileClass constants)
 *                            of profiles to extract. Zero means any class.
 */
void ICCProfile::ExtractProfileList( StringList& selectedDescriptionsList,
                                     StringList& selectedPathsList,
                                     const StringList& pathList,
                                     ICCColorSpaces colorSpace,
                                     ICCProfileClasses profileClass )
{
   for ( StringList::const_iterator i = pathList.Begin(); i != pathList.End(); ++i )
   {
      icc_profile_handle h = (*API->ColorManagement->OpenProfileFile)( i->c_str() );
      if ( h != 0 )
      {
         if ( !profileClass ||
               profileClass.IsFlagSet( ICCProfileClasses::enum_type(
                                          (*API->ColorManagement->GetProfileDeviceClass)( h ) ) ) )
         {
            if ( !colorSpace ||
                  colorSpace.IsFlagSet( ICCColorSpaces::enum_type(
                                             (*API->ColorManagement->GetProfileColorSpace)( h ) ) ) )
            {
               try
               {
                  String desc = Description( handle( h ) );
                  if ( !desc.IsEmpty() )
                  {
                     selectedDescriptionsList.Add( desc );
                     selectedPathsList.Add( *i );
                  }
               }
               catch ( ... )
               {
                  // Do not propagate exceptions here.
               }
            }
         }

         (void)(*API->ColorManagement->CloseProfile)( h );
      }
   }
}

// ----------------------------------------------------------------------------

ICCProfile::profile_list ICCProfile::FindProfilesByColorSpace( ICCColorSpaces colorSpaces )
{
   StringList all = FindProfiles();

   StringList descriptions;
   StringList paths;
   ExtractProfileList( descriptions, paths, all, colorSpaces );

   profile_list rawList;
   for ( StringList::const_iterator i = descriptions.Begin(), j = paths.Begin(); i != descriptions.End(); ++i, ++j )
      rawList.Add( Info( *i, *j ) );

   profile_list profiles;

   // Remove duplicates
   for ( profile_list::const_iterator i = rawList.Begin(), j = i; i != rawList.End(); ++i )
      if ( j == i || *i != *j )
      {
         j = i;
         profiles.Add( *i );
      }

   return profiles;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ICCProfile.cpp - Released 2015/11/26 15:59:45 UTC
