// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/FileFormat.cpp - Released 2014/11/14 17:17:01 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include <pcl/FileFormat.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FileFormatPrivate
{
public:

   meta_format_handle handle;
   api_format_caps    capabilities;

   FileFormatPrivate() : handle( 0 ), capabilities()
   {
      ZeroCaps();
   }

   FileFormatPrivate( const FileFormatPrivate* x ) : handle( 0 ), capabilities()
   {
      if ( x != 0 )
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

   // Strict aliasing safe code:
   void ZeroCaps()
   {
      //*reinterpret_cast<uint32*>( &capabilities ) = 0;
      union { api_format_caps capabilities; uint32 u; } v;
      v.u = 0u;
      capabilities = v.capabilities;
   }
};

// ----------------------------------------------------------------------------

FileFormat::FileFormat( const String& nameExtOrMime, bool toRead, bool toWrite ) :
FileFormatBase(), data( new FileFormatPrivate )
{
   if ( nameExtOrMime.IsEmpty() )
      throw Error( "FileFormat: Empty format name, file extension or MIME type specified" );

   if ( nameExtOrMime.Has( '/' ) )
   {
      IsoString mimeType( nameExtOrMime );
      data->handle = (*API->FileFormat->GetFileFormatByMimeType)( ModuleHandle(), mimeType.c_str(), toRead, toWrite );
      if ( data->handle == 0 )
         throw Error( "FileFormat: No installed image file format was found "
                      "for the specified MIME type \'" + nameExtOrMime + "\' and access conditions" );
   }
   else if ( nameExtOrMime.BeginsWith( '.' ) )
   {
      data->handle = (*API->FileFormat->GetFileFormatByFileExtension)( ModuleHandle(), nameExtOrMime.c_str(), toRead, toWrite );
      if ( data->handle == 0 )
         throw Error( "FileFormat: No installed image file format was found "
                      "for the specified file extension \'" + nameExtOrMime + "\'and access conditions" );
   }
   else
   {
      IsoString id( nameExtOrMime );
      data->handle = (*API->FileFormat->GetFileFormatByName)( ModuleHandle(), id.c_str() );
      if ( data->handle == 0 )
         throw Error( "FileFormat: No installed image file format was found "
                      "with the specified identifier \'" + nameExtOrMime + '\'' );
   }

   data->GetCapabilities();
}

FileFormat::FileFormat( const FileFormat& fmt ) : FileFormatBase(), data( new FileFormatPrivate( fmt.data ) )
{
}

FileFormat::FileFormat( const void* handle ) : FileFormatBase(), data( new FileFormatPrivate )
{
   if ( handle == 0 )
      throw Error( "FileFormat: Null file format handle" );
   data->handle = handle;
   data->GetCapabilities();
}

FileFormat::~FileFormat()
{
   if ( data != 0 )
   {
      delete data;
      data = 0;
   }
}

// ----------------------------------------------------------------------------

IsoString FileFormat::Name() const
{
   size_type len = 0;
   (*API->FileFormat->GetFileFormatName)( data->handle, 0, &len );

   IsoString name;

   if ( len != 0 )
   {
      name.Reserve( len );
      if ( (*API->FileFormat->GetFileFormatName)( data->handle, name.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetFileFormatName" );
   }

   return name;
}

// ----------------------------------------------------------------------------

StringList FileFormat::FileExtensions() const
{
   size_type count = 0;
   size_type maxLen = 0;
   (*API->FileFormat->GetFileFormatFileExtensions)( data->handle, 0, &count, &maxLen );

   StringList extensions( count );

   if ( count != 0 )
   {
      Array<char16_type*> ptrs;
      for ( size_type i = 0; i < count; ++i )
      {
         extensions[i].Reserve( maxLen );
         ptrs.Add( extensions[i].c_str() );
      }

      if ( (*API->FileFormat->GetFileFormatFileExtensions)( data->handle, ptrs.Begin(), &count, &maxLen ) == api_false )
         throw APIFunctionError( "GetFileFormatFileExtensions" );
   }

   return extensions;
}

// ----------------------------------------------------------------------------

IsoStringList FileFormat::MimeTypes() const
{
   size_type count = 0;
   size_type maxLen = 0;
   (*API->FileFormat->GetFileFormatMimeTypes)( data->handle, 0, &count, &maxLen );

   IsoStringList mimeTypes( count );

   if ( count != 0 )
   {
      Array<char*> ptrs;
      for ( size_type i = 0; i < count; ++i )
      {
         mimeTypes[i].Reserve( maxLen );
         ptrs.Add( mimeTypes[i].c_str() );
      }

      if ( (*API->FileFormat->GetFileFormatMimeTypes)( data->handle, ptrs.Begin(), &count, &maxLen ) == api_false )
         throw APIFunctionError( "GetFileFormatMimeTypes" );
   }

   return mimeTypes;
}

// ----------------------------------------------------------------------------

uint32 FileFormat::Version() const
{
   return (*API->FileFormat->GetFileFormatVersion)( data->handle );
}

// ----------------------------------------------------------------------------

String FileFormat::Description() const
{
   size_type len = 0;
   (*API->FileFormat->GetFileFormatDescription)( data->handle, 0, &len );

   String description;

   if ( len != 0 )
   {
      description.Reserve( len );

      if ( (*API->FileFormat->GetFileFormatDescription)( data->handle, description.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetFileFormatDescription" );
   }

   return description;
}

// ----------------------------------------------------------------------------

String FileFormat::Implementation() const
{
   size_type len = 0;
   (*API->FileFormat->GetFileFormatImplementation)( data->handle, 0, &len );

   String implementation;

   if ( len != 0 )
   {
      implementation.Reserve( len );

      if ( (*API->FileFormat->GetFileFormatImplementation)( data->handle, implementation.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetFileFormatImplementation" );
   }

   return implementation;
}

// ----------------------------------------------------------------------------

Bitmap FileFormat::Icon() const
{
   return Bitmap( (*API->FileFormat->GetFileFormatIcon)( data->handle ) );
}

// ----------------------------------------------------------------------------

Bitmap FileFormat::SmallIcon() const
{
   return Bitmap( (*API->FileFormat->GetFileFormatSmallIcon)( data->handle ) );
}

// ----------------------------------------------------------------------------

bool FileFormat::CanRead() const
{
   return data->capabilities.canRead;
}

bool FileFormat::CanWrite() const
{
   return data->capabilities.canWrite;
}

bool FileFormat::CanReadIncrementally() const
{
   return data->capabilities.canReadIncrementally;
}

bool FileFormat::CanWriteIncrementally() const
{
   return data->capabilities.canWriteIncrementally;
}

bool FileFormat::CanStore8Bit() const
{
   return data->capabilities.canStore8bit;
}

bool FileFormat::CanStore16Bit() const
{
   return data->capabilities.canStore16bit;
}

bool FileFormat::CanStore32Bit() const
{
   return data->capabilities.canStore32bit;
}

bool FileFormat::CanStore64Bit() const
{
   return data->capabilities.canStore64bit;
}

bool FileFormat::CanStoreFloat() const
{
   return data->capabilities.canStoreFloat;
}

bool FileFormat::CanStoreDouble() const
{
   return data->capabilities.canStoreDouble;
}

bool FileFormat::CanStoreComplex() const
{
   return data->capabilities.canStoreComplex;
}

bool FileFormat::CanStoreDComplex() const
{
   return data->capabilities.canStoreDComplex;
}

bool FileFormat::CanStoreGrayscale() const
{
   return data->capabilities.canStoreGrayscale;
}

bool FileFormat::CanStoreRGBColor() const
{
   return data->capabilities.canStoreRGBColor;
}

bool FileFormat::CanStoreAlphaChannels() const
{
   return data->capabilities.canStoreAlphaChannels;
}

bool FileFormat::CanStoreResolution() const
{
   return data->capabilities.canStoreResolution;
}

bool FileFormat::CanStoreKeywords() const
{
   return data->capabilities.canStoreKeywords;
}

bool FileFormat::CanStoreICCProfiles() const
{
   return data->capabilities.canStoreICCProfiles;
}

bool FileFormat::CanStoreMetadata() const
{
   return data->capabilities.canStoreMetaData;
}

bool FileFormat::CanStoreThumbnails() const
{
   return data->capabilities.canStoreThumbnails;
}

bool FileFormat::CanStoreProperties() const
{
   return data->capabilities.canStoreProperties;
}

bool FileFormat::SupportsCompression() const
{
   return data->capabilities.supportsCompression;
}

bool FileFormat::SupportsMultipleImages() const
{
   return data->capabilities.supportsMultipleImages;
}

bool FileFormat::CanEditPreferences() const
{
   return data->capabilities.canEditPreferences;
}

bool FileFormat::UsesFormatSpecificData() const
{
   return data->capabilities.usesFormatSpecificData;
}

// ----------------------------------------------------------------------------

bool FileFormat::ValidateFormatSpecificData( const void* block ) const
{
   return (*API->FileFormat->ValidateFormatSpecificData)( data->handle, block ) != api_false;
}

void FileFormat::DisposeFormatSpecificData( void* block ) const
{
   (*API->FileFormat->DisposeFormatSpecificData)( data->handle, block );
}

// ----------------------------------------------------------------------------

bool FileFormat::EditPreferences() const
{
   return (*API->FileFormat->EditFileFormatPreferences)( data->handle ) != api_false;
}

// ----------------------------------------------------------------------------

const void* FileFormat::Handle() const
{
   return data->handle;
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

} // pcl

// ****************************************************************************
// EOF pcl/FileFormat.cpp - Released 2014/11/14 17:17:01 UTC
