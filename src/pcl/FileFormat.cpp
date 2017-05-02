//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// pcl/FileFormat.cpp - Released 2017-05-02T10:39:13Z
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

#include <pcl/File.h>
#include <pcl/FileFormat.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FileFormatPrivate
{
public:

   meta_format_handle      handle;
   api_format_capabilities capabilities;

   FileFormatPrivate() :
      handle( nullptr )
   {
      ZeroCaps();
   }

   FileFormatPrivate( const FileFormatPrivate* x ) :
      handle( nullptr ),
      capabilities()
   {
      if ( x != nullptr )
      {
         handle = x->handle;
         capabilities = x->capabilities;
      }
      else
         ZeroCaps();
   }

   void GetCapabilities()
   {
      if ( !(*API->FileFormat->GetFileFormatCapabilities)( handle, &capabilities ) )
         throw APIFunctionError( "GetFileFormatCapabilities" );
   }

   static api_bool EnumerationCallback( meta_format_handle handle, void* data )
   {
#define formats   reinterpret_cast<Array<FileFormat>*>( data )
      try
      {
         formats->Add( FileFormat( handle ) );
         return api_true;
      }
      catch ( ... )
      {
         return api_false;
      }
#undef formats
   }

private:

   void ZeroCaps()
   {
      // Strict aliasing safe code
#ifndef _MSC_VER
      // ### FIXME - VC++ 2015 says that api_format_capabilities is larger than 8 bytes !?
      static_assert( sizeof( api_format_capabilities ) <= sizeof( uint64 ), "Invalid sizeof( api_format_capabilities )" );
#endif
      union { api_format_capabilities capabilities; uint64 u; } v;
      v.u = 0u;
      capabilities = v.capabilities;
   }
};

// ----------------------------------------------------------------------------

FileFormat::FileFormat( const String& nameExtOrMime, bool toRead, bool toWrite ) :
   FileFormatBase()
{
   if ( nameExtOrMime.IsEmpty() )
      throw Error( "FileFormat: Empty format name, file extension or MIME type specified" );

   m_data = new FileFormatPrivate;

   if ( nameExtOrMime.Contains( '/' ) )
   {
      IsoString mimeType( nameExtOrMime );
      m_data->handle = (*API->FileFormat->GetFileFormatByMimeType)( ModuleHandle(), mimeType.c_str(), toRead, toWrite );
      if ( m_data->handle == nullptr )
         throw Error( "FileFormat: No installed image file format was found "
                      "for the specified MIME type \'" + nameExtOrMime + "\' and access conditions" );
   }
   else if ( nameExtOrMime.StartsWith( '.' ) )
   {
      m_data->handle = (*API->FileFormat->GetFileFormatByFileExtension)( ModuleHandle(), nameExtOrMime.c_str(), toRead, toWrite );
      if ( m_data->handle == nullptr )
         throw Error( "FileFormat: No installed image file format was found "
                      "for the specified file extension \'" + nameExtOrMime + "\'and access conditions" );
   }
   else
   {
      IsoString id( nameExtOrMime );
      m_data->handle = (*API->FileFormat->GetFileFormatByName)( ModuleHandle(), id.c_str() );
      if ( m_data->handle == nullptr )
         throw Error( "FileFormat: No installed image file format was found "
                      "with the specified identifier \'" + nameExtOrMime + '\'' );
   }

   m_data->GetCapabilities();
}

FileFormat::FileFormat( const FileFormat& fmt ) :
   FileFormatBase()
{
   m_data = new FileFormatPrivate( fmt.m_data );
}

FileFormat::FileFormat( const void* handle ) :
   FileFormatBase()
{
   if ( handle == nullptr )
      throw Error( "FileFormat: Null file format handle" );
   m_data = new FileFormatPrivate;
   m_data->handle = handle;
   m_data->GetCapabilities();
}

FileFormat::~FileFormat()
{
}

// ----------------------------------------------------------------------------

IsoString FileFormat::Name() const
{
   size_type len = 0;
   (*API->FileFormat->GetFileFormatName)( m_data->handle, 0, &len );

   IsoString name;
   if ( len > 0 )
   {
      name.SetLength( len );
      if ( (*API->FileFormat->GetFileFormatName)( m_data->handle, name.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetFileFormatName" );
      name.ResizeToNullTerminated();
   }
   return name;
}

// ----------------------------------------------------------------------------

StringList FileFormat::FileExtensions() const
{
   size_type count = 0;
   size_type maxLen = 0;
   (*API->FileFormat->GetFileFormatFileExtensions)( m_data->handle, 0, &count, &maxLen );

   StringList extensions( count );
   if ( count > 0 )
   {
      Array<char16_type*> ptrs;
      for ( size_type i = 0; i < count; ++i )
      {
         extensions[i].Reserve( maxLen );
         ptrs.Add( extensions[i].Begin() );
      }

      if ( (*API->FileFormat->GetFileFormatFileExtensions)( m_data->handle, ptrs.Begin(), &count, &maxLen ) == api_false )
         throw APIFunctionError( "GetFileFormatFileExtensions" );

      for ( String& ext : extensions )
         ext.ResizeToNullTerminated();
   }
   return extensions;
}

// ----------------------------------------------------------------------------

IsoStringList FileFormat::MimeTypes() const
{
   size_type count = 0;
   size_type maxLen = 0;
   (*API->FileFormat->GetFileFormatMimeTypes)( m_data->handle, 0, &count, &maxLen );

   IsoStringList mimeTypes( count );
   if ( count > 0 )
   {
      Array<char*> ptrs;
      for ( size_type i = 0; i < count; ++i )
      {
         mimeTypes[i].Reserve( maxLen );
         ptrs.Add( mimeTypes[i].Begin() );
      }

      if ( (*API->FileFormat->GetFileFormatMimeTypes)( m_data->handle, ptrs.Begin(), &count, &maxLen ) == api_false )
         throw APIFunctionError( "GetFileFormatMimeTypes" );

      for ( IsoString& mimeType : mimeTypes )
         mimeType.ResizeToNullTerminated();
   }
   return mimeTypes;
}

// ----------------------------------------------------------------------------

uint32 FileFormat::Version() const
{
   return (*API->FileFormat->GetFileFormatVersion)( m_data->handle );
}

// ----------------------------------------------------------------------------

String FileFormat::Description() const
{
   size_type len = 0;
   (*API->FileFormat->GetFileFormatDescription)( m_data->handle, 0, &len );

   String description;
   if ( len > 0 )
   {
      description.SetLength( len );
      if ( (*API->FileFormat->GetFileFormatDescription)( m_data->handle, description.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetFileFormatDescription" );
      description.ResizeToNullTerminated();
   }
   return description;
}

// ----------------------------------------------------------------------------

String FileFormat::Implementation() const
{
   size_type len = 0;
   (*API->FileFormat->GetFileFormatImplementation)( m_data->handle, 0, &len );

   String implementation;
   if ( len > 0 )
   {
      implementation.SetLength( len );
      if ( (*API->FileFormat->GetFileFormatImplementation)( m_data->handle, implementation.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetFileFormatImplementation" );
      implementation.ResizeToNullTerminated();
   }
   return implementation;
}

// ----------------------------------------------------------------------------

String FileFormat::Status() const
{
   size_type len = 0;
   (*API->FileFormat->GetFileFormatStatus)( m_data->handle, 0, &len, 0/*reserved*/ );

   String status;
   if ( len > 0 )
   {
      status.SetLength( len );
      if ( (*API->FileFormat->GetFileFormatStatus)( m_data->handle, status.Begin(), &len, 0/*reserved*/ ) == api_false )
         throw APIFunctionError( "GetFileFormatStatus" );
      status.ResizeToNullTerminated();
   }
   return status;
}

// ----------------------------------------------------------------------------

Bitmap FileFormat::Icon() const
{
   return Bitmap( (*API->FileFormat->GetFileFormatIcon)( m_data->handle ) );
}

// ----------------------------------------------------------------------------

Bitmap FileFormat::SmallIcon() const
{
   return Bitmap( (*API->FileFormat->GetFileFormatSmallIcon)( m_data->handle ) );
}

// ----------------------------------------------------------------------------

bool FileFormat::CanRead() const
{
   return m_data->capabilities.canRead;
}

bool FileFormat::CanWrite() const
{
   return m_data->capabilities.canWrite;
}

bool FileFormat::CanReadIncrementally() const
{
   return m_data->capabilities.canReadIncrementally;
}

bool FileFormat::CanWriteIncrementally() const
{
   return m_data->capabilities.canWriteIncrementally;
}

bool FileFormat::CanStore8Bit() const
{
   return m_data->capabilities.canStore8bit;
}

bool FileFormat::CanStore16Bit() const
{
   return m_data->capabilities.canStore16bit;
}

bool FileFormat::CanStore32Bit() const
{
   return m_data->capabilities.canStore32bit;
}

bool FileFormat::CanStore64Bit() const
{
   return m_data->capabilities.canStore64bit;
}

bool FileFormat::CanStoreFloat() const
{
   return m_data->capabilities.canStoreFloat;
}

bool FileFormat::CanStoreDouble() const
{
   return m_data->capabilities.canStoreDouble;
}

bool FileFormat::CanStoreComplex() const
{
   return m_data->capabilities.canStoreComplex;
}

bool FileFormat::CanStoreDComplex() const
{
   return m_data->capabilities.canStoreDComplex;
}

bool FileFormat::CanStoreGrayscale() const
{
   return m_data->capabilities.canStoreGrayscale;
}

bool FileFormat::CanStoreRGBColor() const
{
   return m_data->capabilities.canStoreRGBColor;
}

bool FileFormat::CanStoreAlphaChannels() const
{
   return m_data->capabilities.canStoreAlphaChannels;
}

bool FileFormat::CanStoreResolution() const
{
   return m_data->capabilities.canStoreResolution;
}

bool FileFormat::CanStoreKeywords() const
{
   return m_data->capabilities.canStoreKeywords;
}

bool FileFormat::CanStoreICCProfiles() const
{
   return m_data->capabilities.canStoreICCProfiles;
}

bool FileFormat::CanStoreThumbnails() const
{
   return m_data->capabilities.canStoreThumbnails;
}

bool FileFormat::CanStoreProperties() const
{
   return m_data->capabilities.canStoreProperties;
}

bool FileFormat::CanStoreImageProperties() const
{
   return m_data->capabilities.canStoreImageProperties;
}

bool FileFormat::CanStoreRGBWS() const
{
   return m_data->capabilities.canStoreRGBWS;
}

bool FileFormat::CanStoreDisplayFunctions() const
{
   return m_data->capabilities.canStoreDisplayFunctions;
}

bool FileFormat::CanStoreColorFilterArrays() const
{
   return m_data->capabilities.canStoreColorFilterArrays;
}

bool FileFormat::SupportsCompression() const
{
   return m_data->capabilities.supportsCompression;
}

bool FileFormat::SupportsMultipleImages() const
{
   return m_data->capabilities.supportsMultipleImages;
}

bool FileFormat::SupportsViewProperties() const
{
   return m_data->capabilities.supportsViewProperties;
}

bool FileFormat::CanEditPreferences() const
{
   return m_data->capabilities.canEditPreferences;
}

bool FileFormat::UsesFormatSpecificData() const
{
   return m_data->capabilities.usesFormatSpecificData;
}

bool FileFormat::IsDeprecated() const
{
   return m_data->capabilities.deprecated;
}

// ----------------------------------------------------------------------------

bool FileFormat::ValidateFormatSpecificData( const void* block ) const
{
   return (*API->FileFormat->ValidateFormatSpecificData)( m_data->handle, block ) != api_false;
}

void FileFormat::DisposeFormatSpecificData( void* block ) const
{
   (*API->FileFormat->DisposeFormatSpecificData)( m_data->handle, block );
}

// ----------------------------------------------------------------------------

bool FileFormat::EditPreferences() const
{
   return (*API->FileFormat->EditFileFormatPreferences)( m_data->handle ) != api_false;
}

// ----------------------------------------------------------------------------

const void* FileFormat::Handle() const
{
   return m_data->handle;
}

// ----------------------------------------------------------------------------

Array<FileFormat> FileFormat::AllFormats()
{
   Array<FileFormat> formats;
   if ( (*API->FileFormat->EnumerateFileFormats)( FileFormatPrivate::EnumerationCallback, &formats ) == api_false )
      throw APIFunctionError( "EnumerateFileFormats" );
   return formats;
}

// ----------------------------------------------------------------------------

bool FileFormat::IsSupportedFileFormatBySuffix( const String& path, bool toRead, bool toWrite )
{
   String suffix = File::ExtractSuffix( path ).Trimmed().CaseFolded();
   if ( suffix.IsEmpty() )
      return false;
   return (*API->FileFormat->GetFileFormatByFileExtension)( ModuleHandle(), suffix.c_str(), toRead, toWrite ) != nullptr;
}

// ----------------------------------------------------------------------------

static void
FindSupportedImageFiles( StringList& list, const String& dirPath, const String& rootPath,
                         bool toRead, bool toWrite, bool recursive, bool followLinks )
{
   /*
    * Secure search: Block any attempts to escape from the initial subtree
    * (e.g., through symbolic links).
    */
   if ( !dirPath.StartsWith( rootPath ) )
      return;

   pcl::FindFileInfo info;
   StringList directories;
   for ( File::Find f( dirPath + "/*" ); f.NextItem( info ); )
#ifndef __PCL_WINDOWS
      if ( followLinks || !info.attributes.IsFlagSet( FileAttribute::SymbolicLink ) )
#endif
         if ( info.IsDirectory() )
         {
            if ( recursive )
               if ( info.name != "." )
                  if ( info.name != ".." )
                     directories << info.name;
         }
         else
         {
            {
               String ext = File::ExtractSuffix( info.name );
               if ( (*API->FileFormat->GetFileFormatByFileExtension)( ModuleHandle(), ext.c_str(), toRead, toWrite ) != nullptr )
                  list << File::FullPath( dirPath + '/' + info.name );
            }
         }

   for ( const String& dir : directories )
      FindSupportedImageFiles( list, File::FullPath( dirPath + '/' + dir ), rootPath, toRead, toWrite, recursive, followLinks );
}

StringList FileFormat::SupportedImageFiles( const String& dirPath, bool toRead, bool toWrite, bool recursive, bool followLinks )
{
   StringList list;

#ifdef __PCL_WINDOWS
   String path = File::WindowsPathToUnix( dirPath.Trimmed() );
#else
   String path = dirPath.Trimmed();
#endif
   if ( !path.IsEmpty() )
   {
      // Strip away a trailing slash, except for root directories.
      if ( path.Length() > 1 && path.EndsWith( '/' )
#ifdef __PCL_WINDOWS
        && !(path.Length() == 3 && path[1] == ':')
#endif
         )
         path.Delete( path.UpperBound() );

      path = File::FullPath( path );

      FindSupportedImageFiles( list, path, path, toRead, toWrite, recursive, followLinks );
   }

   return list;
}

// ----------------------------------------------------------------------------

static void
FindDrizzleFiles( StringList& list, const String& dirPath, const String& rootPath, bool recursive, bool followLinks )
{
   /*
    * Secure search: Block any attempts to escape from the initial subtree
    * (e.g., through symbolic links).
    */
   if ( !dirPath.StartsWith( rootPath ) )
      return;

   pcl::FindFileInfo info;
   StringList directories;
   for ( File::Find f( dirPath + "/*" ); f.NextItem( info ); )
#ifndef __PCL_WINDOWS
      if ( followLinks || !info.attributes.IsFlagSet( FileAttribute::SymbolicLink ) )
#endif
         if ( info.IsDirectory() )
         {
            if ( recursive )
               if ( info.name != "." )
                  if ( info.name != ".." )
                     directories << info.name;
         }
         else
         {
            {
               String ext = File::ExtractSuffix( info.name ).CaseFolded();
               if ( ext == ".xdrz" || ext == ".drz" )
                  list << File::FullPath( dirPath + '/' + info.name );
            }
         }

   for ( const String& dir : directories )
      FindDrizzleFiles( list, File::FullPath( dirPath + '/' + dir ), rootPath, recursive, followLinks );
}

StringList FileFormat::DrizzleFiles( const String& dirPath, bool recursive, bool followLinks )
{
   StringList list;

#ifdef __PCL_WINDOWS
   String path = File::WindowsPathToUnix( dirPath.Trimmed() );
#else
   String path = dirPath.Trimmed();
#endif
   if ( !path.IsEmpty() )
   {
      // Strip away a trailing slash, except for root directories.
      if ( path.Length() > 1 && path.EndsWith( '/' )
#ifdef __PCL_WINDOWS
        && !(path.Length() == 3 && path[1] == ':')
#endif
         )
         path.Delete( path.UpperBound() );

      path = File::FullPath( path );

      FindDrizzleFiles( list, path, path, recursive, followLinks );
   }

   return list;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FileFormat.cpp - Released 2017-05-02T10:39:13Z
