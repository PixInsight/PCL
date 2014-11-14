// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/MetaFileFormat.cpp - Released 2014/11/14 17:17:01 UTC
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

#include <pcl/ErrorHandler.h>
#include <pcl/FileFormatImplementation.h>
#include <pcl/MetaFileFormat.h>
#include <pcl/MetaModule.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static void APIImageInfoToPCL( ImageInfo& info, const api_image_info& i )
{
   info.width            = int( i.width );
   info.height           = int( i.height );
   info.numberOfChannels = int( i.numberOfChannels );
   info.colorSpace       = int( i.colorSpace );
   info.supported        = i.supported;
}

static void APIImageOptionsToPCL( ImageOptions& opt, const api_image_options& o )
{
   opt.bitsPerSample      = o.bitsPerSample;
   opt.ieeefpSampleFormat = o.ieeefpSampleFormat;
   opt.complexSample      = o.complexSample;
   opt.signedIntegers     = o.signedIntegers;
   opt.metricResolution   = o.metricResolution;
   opt.readNormalized     = o.readNormalized;
   opt.embedICCProfile    = o.embedICCProfile;
   opt.embedMetadata      = o.embedMetadata;
   opt.embedThumbnail     = o.embedThumbnail;
   opt.embedProperties    = o.embedProperties;
   opt.lowerRange         = o.lowerRange;
   opt.upperRange         = o.upperRange;
   opt.xResolution        = o.xResolution;
   opt.yResolution        = o.yResolution;
   opt.isoSpeed           = o.isoSpeed;
   opt.exposure           = o.exposure;
   opt.aperture           = o.aperture;
   opt.focalLength        = o.focalLength;
   opt.cfaType            = o.cfaType;
}

static void PCLImageInfoToAPI( api_image_info& i, const ImageInfo& info )
{
   i.width            = uint32( info.width );
   i.height           = uint32( info.height );
   i.numberOfChannels = uint32( info.numberOfChannels );
   i.colorSpace       = uint32( info.colorSpace );
   i.supported        = info.supported;
}

static void PCLImageOptionsToAPI( api_image_options& o, const ImageOptions& opt )
{
   o.bitsPerSample      = opt.bitsPerSample;
   o.ieeefpSampleFormat = opt.ieeefpSampleFormat;
   o.complexSample      = opt.complexSample;
   o.signedIntegers     = opt.signedIntegers;
   o.metricResolution   = opt.metricResolution;
   o.readNormalized     = opt.readNormalized;
   o.embedICCProfile    = opt.embedICCProfile;
   o.embedMetadata      = opt.embedMetadata;
   o.embedThumbnail     = opt.embedThumbnail;
   o.embedProperties    = opt.embedProperties;
   o.lowerRange         = opt.lowerRange;
   o.upperRange         = opt.upperRange;
   o.xResolution        = opt.xResolution;
   o.yResolution        = opt.yResolution;
   o.isoSpeed           = opt.isoSpeed;
   o.exposure           = opt.exposure;
   o.aperture           = opt.aperture;
   o.focalLength        = opt.focalLength;
   o.cfaType            = opt.cfaType;
}

// ----------------------------------------------------------------------------

#define MANDATORY( funcName )       \
   __Mandatory( Name(), funcName )

static void __Mandatory( const IsoString& formatName, const char* funcName )
{
   throw Error( String( formatName ) + "MetaFileFormat: " +
                funcName + "() must be reimplemented in descendant class" );
}

// ----------------------------------------------------------------------------

MetaFileFormat::MetaFileFormat() : MetaObject( Module ), FileFormatBase()
{
   if ( Module == 0 )
      throw Error( "MetaFileFormat: Module not initialized - illegal MetaFileFormat instantiation" );
}

// ----------------------------------------------------------------------------

uint32 MetaFileFormat::Version() const
{
   MANDATORY( "Version" );
   return 0;
}

// ----------------------------------------------------------------------------

bool MetaFileFormat::ValidateFormatSpecificData( const void* ) const
{
   MANDATORY( "ValidateFormatSpecificData" );
   return false;
}

// ----------------------------------------------------------------------------

void MetaFileFormat::DisposeFormatSpecificData( void* ) const
{
   MANDATORY( "DisposeFormatSpecificData" );
}

// ----------------------------------------------------------------------------

bool MetaFileFormat::EditPreferences() const
{
   // Only called if CanEditPreferences() is reimplemented to return true.
   MANDATORY( "EditPreferences" );
   return false;
}

// ----------------------------------------------------------------------------

Bitmap MetaFileFormat::Icon() const
{
   const char** xpm = IconImageXPM();
   if ( xpm != 0 )
      return Bitmap( xpm );

   String filePath = IconImageFile();
   if ( !filePath.IsEmpty() )
      return Bitmap( filePath );

   return Bitmap();
}

Bitmap MetaFileFormat::SmallIcon() const
{
   const char** xpm = SmallIconImageXPM();
   if ( xpm != 0 )
      return Bitmap( xpm );

   String filePath = SmallIconImageFile();
   if ( !filePath.IsEmpty() )
      return Bitmap( filePath );

   return Bitmap();
}

// ----------------------------------------------------------------------------
// File Format Context
// ----------------------------------------------------------------------------

#define format          reinterpret_cast<const pcl::MetaFileFormat*>( hF )
#define instance        reinterpret_cast<FileFormatImplementation*>( hf )
#define constInstance   reinterpret_cast<const FileFormatImplementation*>( hf )

class FileFormatDispatcher
{
public:

   static file_format_handle api_func CreateFileFormat( meta_format_handle hF )
   {
      try
      {
         return format->Create();
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static void api_func DestroyFileFormat( file_format_handle hf )
   {
      try
      {
         if ( hf != 0 )
            delete instance;
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateFormatSpecificData( meta_format_handle hF, const void* data )
   {
      try
      {
         return format->ValidateFormatSpecificData( data );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func DisposeFormatSpecificData( meta_format_handle hF, const void* data )
   {
      try
      {
         format->DisposeFormatSpecificData( const_cast<void*>( data ) );
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func EditFileFormatPreferences( meta_format_handle hF )
   {
      try
      {
         return (api_bool)format->EditPreferences();
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func Open( file_format_handle hf, const char16_type* filePath, const char* hints, uint32/*flags*/ )
   {
      try
      {
         FileFormatImplementation* i = instance;
         i->description = i->Open( filePath, hints );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   static uint32 api_func GetImageCount( const_file_format_handle hf )
   {
      return uint32( constInstance->description.Length() );
   }

   static api_bool api_func GetImageId( const_file_format_handle hf, char* id, uint32 n, uint32 index )
   {
      try
      {
         if ( index < constInstance->description.Length() )
         {
            constInstance->description[index].id.c_copy( id, n );
            return api_true;
         }

         *id = '\0';
         return api_false;
      }
      ERROR_HANDLER
      *id = '\0';
      return api_false;
   }

   static api_bool api_func GetImageDescription( const_file_format_handle hf, api_image_info* info, api_image_options* options, uint32 index )
   {
      try
      {
         if ( index < constInstance->description.Length() )
         {
            PCLImageInfoToAPI( *info, constInstance->description[index].info );
            PCLImageOptionsToAPI( *options, constInstance->description[index].options );
            return api_true;
         }

         return api_false;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func Close( file_format_handle hf )
   {
      try
      {
         instance->Close();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func IsOpen( const_file_format_handle hf )
   {
      try
      {
         return (api_bool)constInstance->IsOpen();
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static const char16_type* api_func FilePath( const_file_format_handle hf )
   {
      try
      {
         String s = constInstance->FilePath();
         s.SetUnique();
         return s.Release(); // the PI app. will invoke the module deallocation routine
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func SelectImage( file_format_handle hf, uint32 index )
   {
      try
      {
         instance->SelectImage( int( index ) );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static uint32 api_func SelectedImageIndex( const_file_format_handle hf )
   {
      try
      {
         return uint32( constInstance->SelectedImageIndex() );
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func SetFormatSpecificData( file_format_handle hf, const void* data )
   {
      try
      {
         instance->SetFormatSpecificData( data );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void* api_func GetFormatSpecificData( const_file_format_handle hf )
   {
      try
      {
         return constInstance->FormatSpecificData();
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static const char16_type* api_func GetImageProperties( const_file_format_handle hf )
   {
      try
      {
         String s = constInstance->ImageProperties();
         s.SetUnique();
         return s.Release(); // the PI app. will invoke the module deallocation routine
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeginKeywordExtraction( file_format_handle hf )
   {
      try
      {
         instance->BeginKeywordExtraction();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static size_type api_func GetKeywordCount( file_format_handle hf )
   {
      try
      {
         return instance->NumberOfKeywords();
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func GetNextKeyword( file_format_handle hf, char* name, char* value, char* comment, uint32 n )
   {
      try
      {
         pcl::FITSHeaderKeyword k;
         if ( !instance->GetNextKeyword( k ) )
            return api_false;
         k.Trim();
         k.name.c_copy( name, n );
         k.value.c_copy( value, n );
         k.comment.c_copy( comment, n );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func EndKeywordExtraction( file_format_handle hf )
   {
      try
      {
         instance->EndKeywordExtraction();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeginICCProfileExtraction( file_format_handle hf )
   {
      try
      {
         instance->BeginICCProfileExtraction();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static const void* api_func GetICCProfile( file_format_handle hf )
   {
      try
      {
         return instance->GetICCProfile().ProfileData().Begin();
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static void api_func EndICCProfileExtraction( file_format_handle hf )
   {
      try
      {
         instance->EndICCProfileExtraction();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeginMetadataExtraction( file_format_handle hf )
   {
      try
      {
         instance->BeginMetadataExtraction();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static const void* api_func GetMetadata( file_format_handle hf, uint32* size )
   {
      try
      {
         size_type sz = 0;
         const void* data = instance->GetMetadata( sz );
         if ( size != 0 )
            *size = uint32( sz );
         return data;
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static void api_func EndMetadataExtraction( file_format_handle hf )
   {
      try
      {
         instance->EndMetadataExtraction();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeginThumbnailExtraction( file_format_handle hf )
   {
      try
      {
         instance->BeginThumbnailExtraction();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func GetThumbnail( file_format_handle hf, image_handle hImage )
   {
      try
      {
         const UInt8Image& thumbnail = instance->GetThumbnail();

         uint32 bitsPerSample;
         api_bool isFloat;
         if ( !(*API->SharedImage->GetImageFormat)( hImage, &bitsPerSample, &isFloat ) )
            throw 0;

         if ( isFloat )
            switch ( bitsPerSample )
            {
            case 32 :
               pcl::Image( hImage ).Assign( thumbnail );
               break;
            case 64 :
               pcl::DImage( hImage ).Assign( thumbnail );
               break;
            default :
               return api_false;
            }
         else
            switch ( bitsPerSample )
            {
            case  8 :
               UInt8Image( hImage ).Assign( thumbnail );
               break;
            case 16 :
               UInt16Image( hImage ).Assign( thumbnail );
               break;
            case 32 :
               UInt32Image( hImage ).Assign( thumbnail );
               break;
            default :
               return api_false;
            }

         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func EndThumbnailExtraction( file_format_handle hf )
   {
      try
      {
         instance->EndThumbnailExtraction();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func EnumerateImageProperties( file_format_handle hf,
                                                      property_enumeration_callback cb,
                                                      char* id, size_type* len, void* data )
   {
      try
      {
         if ( len == 0 )
            return api_false;

         ImagePropertyDescriptionArray properties = instance->Properties();
         size_type n = 0;
         for ( ImagePropertyDescriptionArray::const_iterator i = properties.Begin(); i != properties.End(); ++i )
            if ( i->id.Length() > n )
               n = i->id.Length();
         if ( id != 0 && *len >= n )
         {
            if ( cb != 0 )
               for ( ImagePropertyDescriptionArray::const_iterator i = properties.Begin(); i != properties.End(); ++i )
               {
                  i->id.c_copy( id, *len+1 );
                  if ( !(*cb)( id, uint64( i->type ), data ) )
                     return api_false;
               }
            return api_true;
         }

         *len = n;
         return api_false;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeginPropertyExtraction( file_format_handle hf )
   {
      try
      {
         instance->BeginPropertyExtraction();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func GetImageProperty( file_format_handle hf, const char* id, api_property_value* apiValue )
   {
      try
      {
         if ( id == 0 )
         {
            if ( apiValue != 0 )
               apiValue->type = VTYPE_INVALID;
            return api_false;
         }

         const Variant& value = instance->GetImageProperty( id );
         if ( !value.IsValid() )
         {
            if ( apiValue != 0 )
               apiValue->type = VTYPE_INVALID;
            return api_false;
         }

         if ( apiValue == 0 ) // just verifying property existence?
            return api_true;

         APIPropertyValueFromVariant( *apiValue, value );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func EndPropertyExtraction( file_format_handle hf )
   {
      try
      {
         instance->EndPropertyExtraction();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeginPropertyEmbedding( file_format_handle hf )
   {
      try
      {
         instance->BeginPropertyEmbedding();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func SetImageProperty( file_format_handle hf, const char* id, const api_property_value* apiValue )
   {
      try
      {
         if ( apiValue == 0 )
            return api_false;
         instance->SetImageProperty( id, VariantFromAPIPropertyValue( *apiValue ) );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func EndPropertyEmbedding( file_format_handle hf )
   {
      try
      {
         instance->EndPropertyEmbedding();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ReadImage( file_format_handle hf, image_handle hImage )
   {
      try
      {
         uint32 bitsPerSample;
         api_bool isFloat;
         if ( !(*API->SharedImage->GetImageFormat)( hImage, &bitsPerSample, &isFloat ) )
            throw 0;

         if ( isFloat )
            switch ( bitsPerSample )
            {
            case 32 :
               {
                  pcl::Image image( hImage );
                  instance->ReadImage( image );
               }
               break;
            case 64 :
               {
                  pcl::DImage image( hImage );
                  instance->ReadImage( image );
               }
               break;
            default :
               return api_false;
            }
         else
            switch ( bitsPerSample )
            {
            case  8 :
               {
                  UInt8Image image( hImage );
                  instance->ReadImage( image );
               }
               break;
            case 16 :
               {
                  UInt16Image image( hImage );
                  instance->ReadImage( image );
               }
               break;
            case 32 :
               {
                  UInt32Image image( hImage );
                  instance->ReadImage( image );
               }
               break;
            default :
               return api_false;
            }

         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ReadPixels( file_format_handle hf, void* destination,
      uint32 startRow, uint32 numRows, uint32 channel, uint32 bitsPerSample, api_bool isFloat, api_bool isComplex )
   {
      try
      {
         if ( isComplex )
            return api_false;

         if ( isFloat )
            switch ( bitsPerSample )
            {
            case 32 :
               instance->Read( (float*)destination, startRow, numRows, channel );
               break;
            case 64 :
               instance->Read( (double*)destination, startRow, numRows, channel );
               break;
            default :
               return api_false;
            }
         else
            switch ( bitsPerSample )
            {
            case  8 :
               instance->Read( (uint8*)destination, startRow, numRows, channel );
               break;
            case 16 :
               instance->Read( (uint16*)destination, startRow, numRows, channel );
               break;
            case 32 :
               instance->Read( (uint32*)destination, startRow, numRows, channel );
               break;
            default :
               return api_false;
            }

         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func QueryImageFileOptions(
      file_format_handle hf, api_image_options* options, const void** formatOptions, uint32 n )
   {
      try
      {
         Array<ImageOptions> o;
         if ( options != 0 )
            for ( uint32 i = 0; i < n; ++i )
            {
               ImageOptions oi;
               APIImageOptionsToPCL( oi, options[i] );
               o.Add( oi );
            }

         Array<void*> f;
         if ( formatOptions != 0 )
            for ( uint32 i = 0; i < n; ++i )
               f.Add( const_cast<void*>( formatOptions[i] ) );

         if ( !instance->QueryOptions( o, f ) )
            return api_false;

         if ( options != 0 )
            for ( uint32 i = 0, n1 = Min( n, uint32( o.Length() ) ); i < n1; ++i )
               PCLImageOptionsToAPI( options[i], o[i] );

         if ( formatOptions != 0 )
            for ( uint32 i = 0, n1 = Min( n, uint32( f.Length() ) ); i < n1; ++i )
               formatOptions[i] = f[i];

         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func Create( file_format_handle hf, const char16_type* path, int32 count, const char* hints, uint32/*flags*/ )
   {
      try
      {
         instance->Create( path, count, hints );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func SetImageId( file_format_handle hf, const char* id )
   {
      try
      {
         instance->SetId( id );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func SetImageOptions( file_format_handle hf, const api_image_options* options )
   {
      try
      {
         ImageOptions o;
         APIImageOptionsToPCL( o, *options );
         instance->SetOptions( o );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func CreateImage( file_format_handle hf, const api_image_info* info )
   {
      try
      {
         ImageInfo i;
         APIImageInfoToPCL( i, *info );
         instance->CreateImage( i );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeginKeywordEmbedding( file_format_handle hf )
   {
      try
      {
         instance->BeginKeywordEmbedding();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func AddKeyword( file_format_handle hf, const char* name, const char* value, const char* comment )
   {
      try
      {
         FITSHeaderKeyword k( name, value, comment );
         instance->AddKeyword( k );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func EndKeywordEmbedding( file_format_handle hf )
   {
      try
      {
         instance->EndKeywordEmbedding();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeginICCProfileEmbedding( file_format_handle hf )
   {
      try
      {
         instance->BeginICCProfileEmbedding();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func SetICCProfile( file_format_handle hf, const void* iccData )
   {
      try
      {
         instance->SetICCProfile( ICCProfile( iccData ) );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func EndICCProfileEmbedding( file_format_handle hf )
   {
      try
      {
         instance->EndICCProfileEmbedding();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeginMetadataEmbedding( file_format_handle hf )
   {
      try
      {
         instance->BeginMetadataEmbedding();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func SetMetadata( file_format_handle hf, const void* data, uint32 size )
   {
      try
      {
         instance->SetMetadata( data, size );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func EndMetadataEmbedding( file_format_handle hf )
   {
      try
      {
         instance->EndMetadataEmbedding();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeginThumbnailEmbedding( file_format_handle hf )
   {
      try
      {
         instance->BeginThumbnailEmbedding();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func SetThumbnail( file_format_handle hf, const_image_handle hImage )
   {
      try
      {
         uint32 bitsPerSample;
         api_bool isFloat;
         if ( !(*API->SharedImage->GetImageFormat)( hImage, &bitsPerSample, &isFloat ) )
            throw 0;

         UInt8Image thumbnail;

         if ( isFloat )
            switch ( bitsPerSample )
            {
            case 32 :
               thumbnail.Assign( pcl::Image( const_cast<image_handle>( hImage ) ) );
               break;
            case 64 :
               thumbnail.Assign( pcl::DImage( const_cast<image_handle>( hImage ) ) );
               break;
            default :
               return api_false;
            }
         else
            switch ( bitsPerSample )
            {
            case  8 :
               thumbnail.Assign( UInt8Image( const_cast<image_handle>( hImage ) ) );
               break;
            case 16 :
               thumbnail.Assign( UInt16Image( const_cast<image_handle>( hImage ) ) );
               break;
            case 32 :
               thumbnail.Assign( UInt32Image( const_cast<image_handle>( hImage ) ) );
               break;
            default :
               return api_false;
            }

         instance->SetThumbnail( thumbnail );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func EndThumbnailEmbedding( file_format_handle hf )
   {
      try
      {
         instance->EndThumbnailEmbedding();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func WriteImage( file_format_handle hf, const_image_handle hImage )
   {
      try
      {
         uint32 bitsPerSample;
         api_bool isFloat;
         if ( !(*API->SharedImage->GetImageFormat)( hImage, &bitsPerSample, &isFloat ) )
            throw 0;

         if ( isFloat )
            switch ( bitsPerSample )
            {
            case 32 :
               instance->WriteImage( pcl::Image( const_cast<image_handle>( hImage ) ) );
               break;
            case 64 :
               instance->WriteImage( pcl::DImage( const_cast<image_handle>( hImage ) ) );
               break;
            default :
               return api_false;
            }
         else
            switch ( bitsPerSample )
            {
            case  8 :
               instance->WriteImage( UInt8Image( const_cast<image_handle>( hImage ) ) );
               break;
            case 16 :
               instance->WriteImage( UInt16Image( const_cast<image_handle>( hImage ) ) );
               break;
            case 32 :
               instance->WriteImage( UInt32Image( const_cast<image_handle>( hImage ) ) );
               break;
            default :
               return api_false;
            }

         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func WritePixels( file_format_handle hf, const void* source,
      uint32 startRow, uint32 numRows, uint32 channel, uint32 bitsPerSample, api_bool isFloat, api_bool isComplex )
   {
      try
      {
         if ( isComplex )
            return api_false;

         if ( isFloat )
            switch ( bitsPerSample )
            {
            case 32 :
               instance->Write( (const float*)source, startRow, numRows, channel );
               break;
            case 64 :
               instance->Write( (const double*)source, startRow, numRows, channel );
               break;
            default :
               return api_false;
            }
         else
            switch ( bitsPerSample )
            {
            case  8 :
               instance->Write( (const uint8*)source, startRow, numRows, channel );
               break;
            case 16 :
               instance->Write( (const uint16*)source, startRow, numRows, channel );
               break;
            case 32 :
               instance->Write( (const uint32*)source, startRow, numRows, channel );
               break;
            default :
               return api_false;
            }

         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func QueryInexactRead( const_file_format_handle hf )
   {
      try
      {
         return (api_bool)constInstance->WasInexactRead();
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func QueryLossyWrite( const_file_format_handle hf )
   {
      try
      {
         return (api_bool)constInstance->WasLossyWrite();
      }
      ERROR_HANDLER
      return api_false;
   }

}; // FileFormatDispatcher

#undef format
#undef instance
#undef constInstance

// ----------------------------------------------------------------------------

void MetaFileFormat::PerformAPIDefinitions() const
{
   (*API->FileFormatDefinition->EnterFileFormatDefinitionContext)();

   {
      Array<const char16_type*> cext;
      StringList extList = FileExtensions();
      for ( StringList::const_iterator i = extList.Begin(); i != extList.End(); ++i )
         cext.Add( i->c_str() );
      cext.Add( 0 );

      Array<const char*> cmime;
      IsoStringList mimeList = MimeTypes();
      for ( IsoStringList::const_iterator i = mimeList.Begin(); i != mimeList.End(); ++i )
         cmime.Add( i->c_str() );
      cmime.Add( 0 );

      IsoString name = Name();

      (*API->FileFormatDefinition->BeginFileFormatDefinition)( this, name.c_str(), cext.Begin(), cmime.Begin() );
   }

   (*API->FileFormatDefinition->SetFileFormatVersion)( Version() );

   {
      String desc = Description();
      if ( !desc.IsEmpty() )
         (*API->FileFormatDefinition->SetFileFormatDescription)( desc.c_str() );
   }

   {
      String impl = Implementation();
      if ( !impl.IsEmpty() )
         (*API->FileFormatDefinition->SetFileFormatImplementation)( impl.c_str() );
   }

   if ( IconImageXPM() != 0 )
      (*API->FileFormatDefinition->SetFileFormatIconImage)( IconImageXPM() );
   else
   {
      String path = IconImageFile();
      if ( !path.IsEmpty() )
         (*API->FileFormatDefinition->SetFileFormatIconImageFile)( path.c_str() );
   }

   if ( SmallIconImageXPM() != 0 )
      (*API->FileFormatDefinition->SetFileFormatIconSmallImage)( SmallIconImageXPM() );
   else
   {
      String path = SmallIconImageFile();
      if ( !path.IsEmpty() )
         (*API->FileFormatDefinition->SetFileFormatIconSmallImageFile)( path.c_str() );
   }

   api_format_caps caps;

   caps.canRead = CanRead();
   caps.canWrite = CanWrite();
   caps.canReadIncrementally = CanReadIncrementally();
   caps.canWriteIncrementally = CanWriteIncrementally();
   caps.canStore8bit = CanStore8Bit();
   caps.canStore16bit = CanStore16Bit();
   caps.canStore32bit = CanStore32Bit();
   caps.canStore64bit = CanStore64Bit();
   caps.canStoreFloat = CanStoreFloat();
   caps.canStoreDouble = CanStoreDouble();
   caps.canStoreComplex = CanStoreComplex();
   caps.canStoreDComplex = CanStoreDComplex();
   caps.canStoreGrayscale = CanStoreGrayscale();
   caps.canStoreRGBColor = CanStoreRGBColor();
   caps.canStoreAlphaChannels = CanStoreAlphaChannels();
   caps.canStoreResolution = CanStoreResolution();
   caps.canStoreKeywords = CanStoreKeywords();
   caps.canStoreICCProfiles = CanStoreICCProfiles();
   caps.canStoreMetaData = CanStoreMetadata();
   caps.canStoreThumbnails = CanStoreThumbnails();
   caps.canStoreProperties = CanStoreProperties();
   caps.supportsCompression = SupportsCompression();
   caps.supportsMultipleImages = SupportsMultipleImages();
   caps.usesFormatSpecificData = UsesFormatSpecificData();
   caps.canEditPreferences = CanEditPreferences();

   (*API->FileFormatDefinition->SetFileFormatCaps)( &caps );

   (*API->FileFormatDefinition->SetFileFormatCreationRoutine)( FileFormatDispatcher::CreateFileFormat );
   (*API->FileFormatDefinition->SetFileFormatDestructionRoutine)( FileFormatDispatcher::DestroyFileFormat );
   (*API->FileFormatDefinition->SetFileFormatValidateFormatSpecificDataRoutine)( FileFormatDispatcher::ValidateFormatSpecificData );
   (*API->FileFormatDefinition->SetFileFormatDisposeFormatSpecificDataRoutine)( FileFormatDispatcher::DisposeFormatSpecificData );
   (*API->FileFormatDefinition->SetFileFormatEditPreferencesRoutine)( FileFormatDispatcher::EditFileFormatPreferences );
   (*API->FileFormatDefinition->SetFileFormatOpenRoutine)( FileFormatDispatcher::Open );
   (*API->FileFormatDefinition->SetFileFormatGetImageCountRoutine)( FileFormatDispatcher::GetImageCount );
   (*API->FileFormatDefinition->SetFileFormatGetImageIdRoutine)( FileFormatDispatcher::GetImageId );
   (*API->FileFormatDefinition->SetFileFormatGetImageDescriptionRoutine)( FileFormatDispatcher::GetImageDescription );
   (*API->FileFormatDefinition->SetFileFormatCloseRoutine)( FileFormatDispatcher::Close );
   (*API->FileFormatDefinition->SetFileFormatIsOpenRoutine)( FileFormatDispatcher::IsOpen );
   (*API->FileFormatDefinition->SetFileFormatGetFilePathRoutine)( FileFormatDispatcher::FilePath );
   (*API->FileFormatDefinition->SetFileFormatSetSelectedImageIndexRoutine)( FileFormatDispatcher::SelectImage );
   (*API->FileFormatDefinition->SetFileFormatGetSelectedImageIndexRoutine)( FileFormatDispatcher::SelectedImageIndex );
   (*API->FileFormatDefinition->SetFileFormatSetFormatSpecificDataRoutine)( FileFormatDispatcher::SetFormatSpecificData );
   (*API->FileFormatDefinition->SetFileFormatGetFormatSpecificDataRoutine)( FileFormatDispatcher::GetFormatSpecificData );
   (*API->FileFormatDefinition->SetFileFormatGetImagePropertiesRoutine)( FileFormatDispatcher::GetImageProperties );
   (*API->FileFormatDefinition->SetFileFormatBeginKeywordExtractionRoutine)( FileFormatDispatcher::BeginKeywordExtraction );
   (*API->FileFormatDefinition->SetFileFormatGetKeywordCountRoutine)( FileFormatDispatcher::GetKeywordCount );
   (*API->FileFormatDefinition->SetFileFormatGetNextKeywordRoutine)( FileFormatDispatcher::GetNextKeyword );
   (*API->FileFormatDefinition->SetFileFormatEndKeywordExtractionRoutine)( FileFormatDispatcher::EndKeywordExtraction );
   (*API->FileFormatDefinition->SetFileFormatBeginICCProfileExtractionRoutine)( FileFormatDispatcher::BeginICCProfileExtraction );
   (*API->FileFormatDefinition->SetFileFormatGetICCProfileRoutine)( FileFormatDispatcher::GetICCProfile );
   (*API->FileFormatDefinition->SetFileFormatEndICCProfileExtractionRoutine)( FileFormatDispatcher::EndICCProfileExtraction );
   (*API->FileFormatDefinition->SetFileFormatBeginMetadataExtractionRoutine)( FileFormatDispatcher::BeginMetadataExtraction );
   (*API->FileFormatDefinition->SetFileFormatGetMetadataRoutine)( FileFormatDispatcher::GetMetadata );
   (*API->FileFormatDefinition->SetFileFormatEndMetadataExtractionRoutine)( FileFormatDispatcher::EndMetadataExtraction );
   (*API->FileFormatDefinition->SetFileFormatBeginThumbnailExtractionRoutine)( FileFormatDispatcher::BeginThumbnailExtraction );
   (*API->FileFormatDefinition->SetFileFormatGetThumbnailRoutine)( FileFormatDispatcher::GetThumbnail );
   (*API->FileFormatDefinition->SetFileFormatEndThumbnailExtractionRoutine)( FileFormatDispatcher::EndThumbnailExtraction );
   (*API->FileFormatDefinition->SetFileFormatEnumerateImagePropertiesRoutine)( FileFormatDispatcher::EnumerateImageProperties );
   (*API->FileFormatDefinition->SetFileFormatBeginPropertyExtractionRoutine)( FileFormatDispatcher::BeginPropertyExtraction );
   (*API->FileFormatDefinition->SetFileFormatGetImagePropertyRoutine)( FileFormatDispatcher::GetImageProperty );
   (*API->FileFormatDefinition->SetFileFormatEndPropertyExtractionRoutine)( FileFormatDispatcher::EndPropertyExtraction );
   (*API->FileFormatDefinition->SetFileFormatBeginPropertyEmbeddingRoutine)( FileFormatDispatcher::BeginPropertyEmbedding );
   (*API->FileFormatDefinition->SetFileFormatSetImagePropertyRoutine)( FileFormatDispatcher::SetImageProperty );
   (*API->FileFormatDefinition->SetFileFormatEndPropertyEmbeddingRoutine)( FileFormatDispatcher::EndPropertyEmbedding );
   (*API->FileFormatDefinition->SetFileFormatReadImageRoutine)( FileFormatDispatcher::ReadImage );
   (*API->FileFormatDefinition->SetFileFormatReadPixelsRoutine)( FileFormatDispatcher::ReadPixels );
   (*API->FileFormatDefinition->SetFileFormatQueryOptionsRoutine)( FileFormatDispatcher::QueryImageFileOptions );
   (*API->FileFormatDefinition->SetFileFormatCreateRoutine)( FileFormatDispatcher::Create );
   (*API->FileFormatDefinition->SetFileFormatSetImageIdRoutine)( FileFormatDispatcher::SetImageId );
   (*API->FileFormatDefinition->SetFileFormatSetImageOptionsRoutine)( FileFormatDispatcher::SetImageOptions );
   (*API->FileFormatDefinition->SetFileFormatCreateImageRoutine)( FileFormatDispatcher::CreateImage );
   (*API->FileFormatDefinition->SetFileFormatBeginKeywordEmbeddingRoutine)( FileFormatDispatcher::BeginKeywordEmbedding );
   (*API->FileFormatDefinition->SetFileFormatAddKeywordRoutine)( FileFormatDispatcher::AddKeyword );
   (*API->FileFormatDefinition->SetFileFormatEndKeywordEmbeddingRoutine)( FileFormatDispatcher::EndKeywordEmbedding );
   (*API->FileFormatDefinition->SetFileFormatBeginICCProfileEmbeddingRoutine)( FileFormatDispatcher::BeginICCProfileEmbedding );
   (*API->FileFormatDefinition->SetFileFormatSetICCProfileRoutine)( FileFormatDispatcher::SetICCProfile );
   (*API->FileFormatDefinition->SetFileFormatEndICCProfileEmbeddingRoutine)( FileFormatDispatcher::EndICCProfileEmbedding );
   (*API->FileFormatDefinition->SetFileFormatBeginMetadataEmbeddingRoutine)( FileFormatDispatcher::BeginMetadataEmbedding );
   (*API->FileFormatDefinition->SetFileFormatSetMetadataRoutine)( FileFormatDispatcher::SetMetadata );
   (*API->FileFormatDefinition->SetFileFormatEndMetadataEmbeddingRoutine)( FileFormatDispatcher::EndMetadataEmbedding );
   (*API->FileFormatDefinition->SetFileFormatBeginThumbnailEmbeddingRoutine)( FileFormatDispatcher::BeginThumbnailEmbedding );
   (*API->FileFormatDefinition->SetFileFormatSetThumbnailRoutine)( FileFormatDispatcher::SetThumbnail );
   (*API->FileFormatDefinition->SetFileFormatEndThumbnailEmbeddingRoutine)( FileFormatDispatcher::EndThumbnailEmbedding );
   (*API->FileFormatDefinition->SetFileFormatWriteImageRoutine)( FileFormatDispatcher::WriteImage );
   (*API->FileFormatDefinition->SetFileFormatWritePixelsRoutine)( FileFormatDispatcher::WritePixels );
   (*API->FileFormatDefinition->SetFileFormatQueryInexactReadRoutine)( FileFormatDispatcher::QueryInexactRead );
   (*API->FileFormatDefinition->SetFileFormatQueryLossyWriteRoutine)( FileFormatDispatcher::QueryLossyWrite );

   (*API->FileFormatDefinition->EndFileFormatDefinition)();

   (*API->FileFormatDefinition->ExitFileFormatDefinitionContext)();
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/MetaFileFormat.cpp - Released 2014/11/14 17:17:01 UTC
