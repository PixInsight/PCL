//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/FileFormatInstance.cpp - Released 2017-06-21T11:36:44Z
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

#include <pcl/AutoLock.h>
#include <pcl/ColorFilterArray.h>
#include <pcl/DisplayFunction.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/ICCProfile.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static void APIHackingAttempt( const String& routineId )
{
   throw Error( "* Warning * Hacking attempt detected in low-level API call: FileFormat->" + routineId );
}

// ----------------------------------------------------------------------------

class FileFormatInstancePrivate
{
public:

   template <class I>
   static bool ReadImage( FileFormatInstance* instance, I& image )
   {
      if ( !image.IsShared() )
      {
         I tmp( (void*)0, 0, 0 );
         if ( !ReadImage( instance, tmp ) )
            return false;
         image.Assign( tmp );
         return true;
      }

      if ( (*API->FileFormat->ReadImage)( instance->handle, image.Allocator().Handle() ) == api_false )
         return false;
      image.Synchronize();
      return true;
   }

   template <class I>
   static bool WriteImage( FileFormatInstance* instance, const I& image )
   {
      if ( !image.IsShared() || !image.IsCompletelySelected() )
      {
         I tmp( (void*)0, 0, 0 );
         tmp.Assign( image );
         return WriteImage( instance, tmp );
      }

      image.PushSelections();
      image.ResetSelections();

      ImageInfo info1( image );

      image.PopSelections();

      bool ok = (*API->FileFormat->WriteImage)( instance->handle, image.Allocator().Handle() ) != api_false;

      image.PushSelections();
      image.ResetSelections();

      ImageInfo info2( image );

      image.PopSelections();

      if ( info1 != info2 )
         APIHackingAttempt( "WriteImage" );

      return ok;
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FileFormatInstance::FileFormatInstance( const FileFormat& fmt ) :
   UIObject( (*API->FileFormat->CreateFileFormatInstance)( ModuleHandle(), fmt.Handle() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateFileFormatInstance" );
}

// ----------------------------------------------------------------------------

FileFormatInstance& FileFormatInstance::Null()
{
   static FileFormatInstance* nullInstance = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullInstance == nullptr )
      nullInstance = new FileFormatInstance( nullptr );
   return *nullInstance;
}

// ----------------------------------------------------------------------------

FileFormat FileFormatInstance::Format() const
{
   return FileFormat( (const void*)(*API->FileFormat->GetFileFormatInstanceFormat)( handle ) );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Close()
{
   return (*API->FileFormat->CloseImageFile)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::IsOpen() const
{
   return (*API->FileFormat->IsImageFileOpen)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

String FileFormatInstance::FilePath() const
{
   size_type len = 0;
   (*API->FileFormat->GetImageFilePath)( handle, 0, &len );

   String path;
   if ( len > 0 )
   {
      path.SetLength( len );
      if ( (*API->FileFormat->GetImageFilePath)( handle, path.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetImageFilePath" );
      path.ResizeToNullTerminated();
   }
   return path;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Open( ImageDescriptionArray& images,
                               const String& filePath, const IsoString& hints )
{
   images.Clear();

   if ( (*API->FileFormat->OpenImageFileEx)( handle, filePath.c_str(), hints.c_str(), 0/*flags*/ ) == api_false )
      return false;

   for ( uint32 i = 0, n = (*API->FileFormat->GetImageCount)( handle ); i < n; ++i )
   {
      IsoString id;
      size_type len = 0;
      (*API->FileFormat->GetImageId)( handle, 0, &len, i );
      if ( len > 0 )
      {
         id.SetLength( len );
         if ( (*API->FileFormat->GetImageId)( handle, id.Begin(), &len, i ) == api_false )
            throw APIFunctionError( "GetImageId" );
         id.ResizeToNullTerminated();
      }

      api_image_info info;
      api_image_options options;
      if ( (*API->FileFormat->GetImageDescription)( handle, &info, &options, i ) == api_false )
         throw APIFunctionError( "GetImageDescription" );

      ImageDescription d;
      d.id = id;
      APIImageInfoToPCL( d.info, info );
      APIImageOptionsToPCL( d.options, options );
      images.Add( d );
   }

   return true;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::SelectImage( int index )
{
   return (*API->FileFormat->SelectImage)( handle, uint32( index ) ) != api_false;
}

// ----------------------------------------------------------------------------

int FileFormatInstance::SelectedImageIndex() const
{
   return int( (*API->FileFormat->GetSelectedImageIndex)( handle ) );
}

// ----------------------------------------------------------------------------

const void* FileFormatInstance::FormatSpecificData() const
{
   return (*API->FileFormat->GetFormatSpecificData)( handle );
}

// ----------------------------------------------------------------------------

String FileFormatInstance::ImageFormatInfo() const
{
   size_type len = 0;
   (*API->FileFormat->GetImageFormatInfo)( handle, 0, &len );

   String properties;
   if ( len > 0 )
   {
      properties.SetLength( len );
      if ( (*API->FileFormat->GetImageFormatInfo)( handle, properties.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetImageFormatInfo" );
      properties.ResizeToNullTerminated();
   }
   return properties;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadICCProfile( ICCProfile& icc )
{
   try
   {
      icc.Clear();

      if ( (*API->FileFormat->BeginICCProfileExtraction)( handle ) == api_false )
         return false;

      const void* iccData = (*API->FileFormat->GetICCProfile)( handle );
      if ( iccData != 0 )
         icc.Set( iccData );

      (*API->FileFormat->EndICCProfileExtraction)( handle );
      return true;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndICCProfileExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadRGBWorkingSpace( RGBColorSystem& rgbws )
{
   try
   {
      rgbws = RGBColorSystem();

      if ( (*API->FileFormat->BeginRGBWSExtraction)( handle ) == api_false )
         return false;

      float gamma = rgbws.Gamma();
      api_bool issRGB = rgbws.IsSRGB();
      FVector x = rgbws.ChromaticityXCoordinates();
      FVector y = rgbws.ChromaticityYCoordinates();
      FVector Y = rgbws.LuminanceCoefficients();

      bool ok = (*API->FileFormat->GetImageRGBWS)( handle, &gamma, &issRGB, x.Begin(), y.Begin(), Y.Begin() ) != api_false;
      if ( ok )
         rgbws = RGBColorSystem( gamma, issRGB, x, y, Y );

      (*API->FileFormat->EndRGBWSExtraction)( handle );
      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndRGBWSExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool  FileFormatInstance::ReadDisplayFunction( DisplayFunction& df )
{
   try
   {
      df = DisplayFunction();

      if ( (*API->FileFormat->BeginDisplayFunctionExtraction)( handle ) == api_false )
         return false;

      DVector m, s, h, l, r;
      df.GetDisplayFunctionParameters( m, s, h, l, r );

      bool ok = (*API->FileFormat->GetImageDisplayFunction)( handle, m.Begin(), s.Begin(), h.Begin(), l.Begin(), r.Begin() ) != api_false;
      if ( ok )
         df = DisplayFunction( m, s, h, l, r );

      (*API->FileFormat->EndDisplayFunctionExtraction)( handle );
      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndDisplayFunctionExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadColorFilterArray( ColorFilterArray& cfa )
{
   try
   {
      cfa = ColorFilterArray();

      if ( (*API->FileFormat->BeginColorFilterArrayExtraction)( handle ) == api_false )
         return false;

      size_type patternLen = 0, nameLen = 0;
      (*API->FileFormat->GetImageColorFilterArray)( handle, 0, &patternLen, 0, 0, 0, &nameLen );

      bool ok = true;
      if ( patternLen > 0 )
      {
         IsoString pattern;
         pattern.SetLength( patternLen );
         String name;
         if ( nameLen > 0 )
            name.SetLength( nameLen );
         int32 width, height;
         ok = (*API->FileFormat->GetImageColorFilterArray)( handle,
                              pattern.Begin(), &patternLen, &width, &height, name.Begin(), &nameLen ) != api_false;
         if ( ok )
         {
            pattern.ResizeToNullTerminated();
            name.ResizeToNullTerminated();
            cfa = ColorFilterArray( pattern, width, height, name );
         }
      }

      (*API->FileFormat->EndColorFilterArrayExtraction)( handle );
      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndColorFilterArrayExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadThumbnail( pcl::UInt8Image& thumbnail )
{
   try
   {
      thumbnail.FreeData();

      if ( (*API->FileFormat->BeginThumbnailExtraction)( handle ) == api_false )
         return false;

      if ( thumbnail.IsShared() )
      {
         (*API->FileFormat->GetThumbnail)( handle, thumbnail.Allocator().Handle() );
         thumbnail.Synchronize();
      }
      else
      {
         UInt8Image img( (void*)0, 0, 0 );
         (*API->FileFormat->GetThumbnail)( handle, img.Allocator().Handle() );
         img.Synchronize();
         thumbnail.Assign( img );
      }

      (*API->FileFormat->EndThumbnailExtraction)( handle );
      return true;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndThumbnailExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadFITSKeywords( FITSKeywordArray& keywords )
{
   try
   {
      keywords.Clear();

      if ( (*API->FileFormat->BeginKeywordExtraction)( handle ) == api_false )
         return false;

      for ( size_type i = 0, count = (*API->FileFormat->GetKeywordCount)( handle ); i < count; ++i )
      {
         IsoString name, value, comment;
         name.Reserve( 96 );
         value.Reserve( 96 );
         comment.Reserve( 96 );

         if ( (*API->FileFormat->GetNextKeyword)( handle,
                  name.Begin(), value.Begin(), comment.Begin(), 81 ) == api_false )
            throw APIFunctionError( "GetNextKeyword" );

         name.ResizeToNullTerminated();
         value.ResizeToNullTerminated();
         comment.ResizeToNullTerminated();

         keywords.Add( FITSHeaderKeyword( name, value, comment ) );
      }

      (*API->FileFormat->EndKeywordExtraction)( handle );
      return true;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndKeywordExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

static api_bool APIPropertyEnumerationCallback( const char* id, uint64 type, void* data )
{
   reinterpret_cast<PropertyDescriptionArray*>( data )->Append(
         PropertyDescription( id, VariantTypeFromAPIPropertyType( type ) ) );
   return api_true;
}

// ----------------------------------------------------------------------------

PropertyDescriptionArray FileFormatInstance::Properties()
{
   PropertyDescriptionArray properties;
   IsoString id;
   size_type len = 0;
   (*API->FileFormat->EnumerateProperties)( handle, 0, 0, &len, 0 ); // 1st call to get max identifier length
   if ( len > 0 )
   {
      id.Reserve( len );
      if ( (*API->FileFormat->EnumerateProperties)( handle, APIPropertyEnumerationCallback,
                                                    id.Begin(), &len, &properties ) == api_false )
         throw APIFunctionError( "EnumerateProperties" );
   }
   return properties;
}

// ----------------------------------------------------------------------------

Variant FileFormatInstance::ReadProperty( const IsoString& property )
{
   try
   {
      if ( (*API->FileFormat->BeginPropertyExtraction)( handle ) == api_false )
         return Variant();

      api_property_value apiValue;
      if ( (*API->FileFormat->GetProperty)( handle, property.c_str(), &apiValue ) == api_false )
      {
         apiValue.data.blockValue = nullptr;
         apiValue.type = VTYPE_INVALID;
      }

      (*API->FileFormat->EndPropertyExtraction)( handle );

      return VariantFromAPIPropertyValue( apiValue );
   }
   catch ( ... )
   {
      (*API->FileFormat->EndPropertyExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

PropertyArray FileFormatInstance::ReadProperties()
{
   PropertyDescriptionArray descriptions = Properties();
   try
   {
      PropertyArray properties;
      if ( (*API->FileFormat->BeginPropertyExtraction)( handle ) != api_false )
      {
         for ( const pcl::PropertyDescription& description : descriptions )
         {
            api_property_value apiValue;
            if ( (*API->FileFormat->GetProperty)( handle, description.id.c_str(), &apiValue ) != api_false )
               properties << Property( description.id, VariantFromAPIPropertyValue( apiValue ) );
         }

         (*API->FileFormat->EndPropertyExtraction)( handle );
      }
      return properties;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndPropertyExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

PropertyDescriptionArray FileFormatInstance::ImageProperties()
{
   PropertyDescriptionArray properties;
   IsoString id;
   size_type len = 0;
   (*API->FileFormat->EnumerateImageProperties)( handle, 0, 0, &len, 0 ); // 1st call to get max identifier length
   if ( len > 0 )
   {
      id.Reserve( len );
      if ( (*API->FileFormat->EnumerateImageProperties)( handle, APIPropertyEnumerationCallback,
                                                         id.Begin(), &len, &properties ) == api_false )
         throw APIFunctionError( "EnumerateImageProperties" );
   }
   return properties;
}

// ----------------------------------------------------------------------------

Variant FileFormatInstance::ReadImageProperty( const IsoString& property )
{
   try
   {
      if ( (*API->FileFormat->BeginImagePropertyExtraction)( handle ) == api_false )
         return Variant();

      api_property_value apiValue;
      if ( (*API->FileFormat->GetImageProperty)( handle, property.c_str(), &apiValue ) == api_false )
      {
         apiValue.data.blockValue = nullptr;
         apiValue.type = VTYPE_INVALID;
      }

      (*API->FileFormat->EndImagePropertyExtraction)( handle );

      return VariantFromAPIPropertyValue( apiValue );
   }
   catch ( ... )
   {
      (*API->FileFormat->EndImagePropertyExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

PropertyArray FileFormatInstance::ReadImageProperties()
{
   PropertyDescriptionArray descriptions = ImageProperties();
   try
   {
      PropertyArray properties;
      if ( (*API->FileFormat->BeginImagePropertyExtraction)( handle ) != api_false )
      {
         for ( const pcl::PropertyDescription& description : descriptions )
         {
            api_property_value apiValue;
            if ( (*API->FileFormat->GetImageProperty)( handle, description.id.c_str(), &apiValue ) != api_false )
               properties << Property( description.id, VariantFromAPIPropertyValue( apiValue ) );
         }

         (*API->FileFormat->EndImagePropertyExtraction)( handle );
      }
      return properties;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndImagePropertyExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadImage( pcl::Image& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

bool FileFormatInstance::ReadImage( pcl::DImage& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

bool FileFormatInstance::ReadImage( UInt8Image& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

bool FileFormatInstance::ReadImage( UInt16Image& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

bool FileFormatInstance::ReadImage( UInt32Image& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

// ----------------------------------------------------------------------------

static bool ReadSamples( file_format_handle handle,
      void* buffer, int startRow, int rowCount, int channel, int bitsPerSample, bool floatSample )
{
   return (*API->FileFormat->ReadSamples)( handle, buffer, startRow, rowCount, channel,
                                          bitsPerSample, floatSample, false ) != api_false;
}

bool FileFormatInstance::ReadSamples( pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::ReadSamples( handle, buffer, startRow, rowCount, channel, 32, true );
}

bool FileFormatInstance::ReadSamples( pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::ReadSamples( handle, buffer, startRow, rowCount, channel, 64, true );
}

bool FileFormatInstance::ReadSamples( UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::ReadSamples( handle, buffer, startRow, rowCount, channel, 8, false );
}

bool FileFormatInstance::ReadSamples( UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::ReadSamples( handle, buffer, startRow, rowCount, channel, 16, false );
}

bool FileFormatInstance::ReadSamples( UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::ReadSamples( handle, buffer, startRow, rowCount, channel, 32, false );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WasInexactRead() const
{
   return (*API->FileFormat->WasInexactRead)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::QueryOptions( Array<ImageOptions>& options, Array<const void*>& formatOptions )
{
   uint32 n = uint32( options.Length() );

   Array<api_image_options> o( (size_type)n );
   for ( uint32 i = 0; i < n; ++i )
      PCLImageOptionsToAPI( o[i], options[i] );

   if ( (*API->FileFormat->QueryImageFileOptions)( handle, o.Begin(), formatOptions.Begin(), n ) == api_false )
      return false;

   for ( uint32 i = 0; i < n; ++i )
      APIImageOptionsToPCL( options[i], o[i] );
   return true;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Create( const String& filePath, const IsoString& hints, int count )
{
   return (*API->FileFormat->CreateImageFileEx)( handle, filePath.c_str(), uint32( count ),
                                                 hints.c_str(), 0/*flags*/ ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::SetId( const IsoString& id )
{
   return (*API->FileFormat->SetImageId)( handle, id.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::SetOptions( const ImageOptions& options )
{
   api_image_options o;
   PCLImageOptionsToAPI( o, options );
   return (*API->FileFormat->SetImageOptions)( handle, &o ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::SetFormatSpecificData( const void* data )
{
   return (*API->FileFormat->SetFormatSpecificData)( handle, data ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteICCProfile( const ICCProfile& icc )
{
   try
   {
      if ( (*API->FileFormat->BeginICCProfileEmbedding)( handle ) == api_false )
         return false;

      bool ok = true;

      if ( icc.IsProfile() ) // ### should allow embedding empty profiles ?
      {
         ICCProfile safeCopy = icc;
         ok = (*API->FileFormat->SetICCProfile)( handle, safeCopy.ProfileData().Begin() ) != api_false;

         if ( !safeCopy.IsSameProfile( icc ) || safeCopy.FilePath() != icc.FilePath() )
            APIHackingAttempt( "SetICCProfile" );
      }

      (*API->FileFormat->EndICCProfileEmbedding)( handle );

      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndICCProfileEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteRGBWorkingSpace( const RGBColorSystem& rgbws )
{
   try
   {
      if ( (*API->FileFormat->BeginRGBWSEmbedding)( handle ) == api_false )
         return false;

      float gamma = rgbws.Gamma();
      api_bool issRGB = rgbws.IsSRGB();
      FVector x = rgbws.ChromaticityXCoordinates();
      FVector y = rgbws.ChromaticityYCoordinates();
      FVector Y = rgbws.LuminanceCoefficients();

      bool ok = (*API->FileFormat->SetImageRGBWS)( handle, gamma, issRGB, x.Begin(), y.Begin(), Y.Begin() ) != api_false;

      if ( rgbws.Gamma() != gamma ||
           rgbws.IsSRGB() != (issRGB != api_false) ||
           rgbws.ChromaticityXCoordinates() != x ||
           rgbws.ChromaticityYCoordinates() != y ||
           rgbws.LuminanceCoefficients() != Y )
      {
         APIHackingAttempt( "WriteRGBWorkingSpace" );
      }

      (*API->FileFormat->EndRGBWSEmbedding)( handle );

      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndRGBWSEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteDisplayFunction( const DisplayFunction& df )
{
   try
   {
      if ( (*API->FileFormat->BeginDisplayFunctionEmbedding)( handle ) == api_false )
         return false;

      DVector m, s, h, l, r;
      df.GetDisplayFunctionParameters( m, s, h, l, r );

      bool ok = (*API->FileFormat->SetImageDisplayFunction)( handle, m.Begin(), s.Begin(), h.Begin(), l.Begin(), r.Begin() ) != api_false;

      DVector m1, s1, h1, l1, r1;
      df.GetDisplayFunctionParameters( m1, s1, h1, l1, r1 );
      if ( m1 != m || s1 != s || h1 != h || l1 != l || r1 != r )
         APIHackingAttempt( "WriteDisplayFunction" );

      (*API->FileFormat->EndDisplayFunctionEmbedding)( handle );
      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndDisplayFunctionEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteColorFilterArray( const ColorFilterArray& cfa )
{
   try
   {
      if ( (*API->FileFormat->BeginColorFilterArrayEmbedding)( handle ) == api_false )
         return false;

      IsoString pattern = cfa.Pattern();
      pattern.EnsureUnique();
      String name = cfa.Name();
      name.EnsureUnique();

      bool ok = (*API->FileFormat->SetImageColorFilterArray)( handle,
                           pattern.c_str(), cfa.Width(), cfa.Height(), name.c_str() ) != api_false;

      if ( cfa.Pattern() != pattern || cfa.Name() != name )
         APIHackingAttempt( "WriteColorFilterArray" );

      (*API->FileFormat->EndColorFilterArrayEmbedding)( handle );
      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndColorFilterArrayEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteThumbnail( const pcl::UInt8Image& thumbnail )
{
   try
   {
      if ( (*API->FileFormat->BeginThumbnailEmbedding)( handle ) == api_false )
         return false;

      thumbnail.PushSelections();
      thumbnail.ResetSelections();

      ImageInfo info( thumbnail );

      thumbnail.PopSelections();

      bool ok;

      if ( thumbnail.IsShared() )
         ok = (*API->FileFormat->SetThumbnail)( handle, thumbnail.Allocator().Handle() ) != api_false;
      else
      {
         UInt8Image tmp( (void*)0, 0, 0 );
         tmp.Assign( thumbnail );
         ok = (*API->FileFormat->SetThumbnail)( handle, tmp.Allocator().Handle() ) != api_false;
      }

      thumbnail.PushSelections();
      thumbnail.ResetSelections();

      if ( info != ImageInfo( thumbnail ) )
         APIHackingAttempt( "SetThumbnail" );

      thumbnail.PopSelections();

      (*API->FileFormat->EndThumbnailEmbedding)( handle );

      return ok;
   }
   catch ( ... )
   {
      if ( thumbnail.CanPopSelections() )
         thumbnail.PopSelections();
      (*API->FileFormat->EndThumbnailEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteFITSKeywords( const FITSKeywordArray& keywords )
{
   try
   {
      if ( (*API->FileFormat->BeginKeywordEmbedding)( handle ) == api_false )
         return false;

      bool ok = true;

      for ( const FITSHeaderKeyword& k : keywords )
         if ( (*API->FileFormat->AddKeyword)( handle, k.name.c_str(), k.value.c_str(), k.comment.c_str() ) == api_false )
         {
            ok = false;
            break;
         }

      (*API->FileFormat->EndKeywordEmbedding)( handle );

      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndKeywordEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteProperty( const IsoString& property, const Variant& value )
{
   try
   {
      if ( (*API->FileFormat->BeginPropertyEmbedding)( handle ) == api_false )
         return false;

      bool ok = true;

      if ( value.IsValid() )
      {
         api_property_value apiValue;
         APIPropertyValueFromVariant( apiValue, value );
         api_property_value safeCopy = apiValue;
         ok = (*API->FileFormat->SetProperty)( handle, property.c_str(), &safeCopy ) != api_false;

         if ( safeCopy.data.blockValue != apiValue.data.blockValue ||
              safeCopy.dimX != apiValue.dimX || safeCopy.dimY != apiValue.dimY ||
              safeCopy.dimZ != apiValue.dimZ || safeCopy.dimT != apiValue.dimT || safeCopy.type != apiValue.type )
         {
            APIHackingAttempt( "SetProperty" );
         }
      }

      (*API->FileFormat->EndPropertyEmbedding)( handle );

      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndPropertyEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteProperties( const PropertyArray& properties )
{
   try
   {
      if ( (*API->FileFormat->BeginPropertyEmbedding)( handle ) == api_false )
         return false;

      for ( const Property& property : properties )
         if ( property.IsValid() )
         {
            api_property_value apiValue;
            APIPropertyValueFromVariant( apiValue, property.Value() );
            api_property_value safeCopy = apiValue;
            bool ok = (*API->FileFormat->SetProperty)( handle, property.Id().c_str(), &safeCopy ) != api_false;

            if ( safeCopy.data.blockValue != apiValue.data.blockValue ||
                 safeCopy.dimX != apiValue.dimX || safeCopy.dimY != apiValue.dimY ||
                 safeCopy.dimZ != apiValue.dimZ || safeCopy.dimT != apiValue.dimT || safeCopy.type != apiValue.type )
            {
               APIHackingAttempt( "SetProperty" );
            }

            if ( !ok )
            {
               (*API->FileFormat->EndPropertyEmbedding)( handle );
               return false;
            }
         }

      (*API->FileFormat->EndPropertyEmbedding)( handle );
      return true;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndPropertyEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteImageProperty( const IsoString& property, const Variant& value )
{
   try
   {
      if ( (*API->FileFormat->BeginImagePropertyEmbedding)( handle ) == api_false )
         return false;

      bool ok = true;

      if ( value.IsValid() )
      {
         api_property_value apiValue;
         APIPropertyValueFromVariant( apiValue, value );
         api_property_value safeCopy = apiValue;
         ok = (*API->FileFormat->SetImageProperty)( handle, property.c_str(), &safeCopy ) != api_false;

         if ( safeCopy.data.blockValue != apiValue.data.blockValue ||
              safeCopy.dimX != apiValue.dimX || safeCopy.dimY != apiValue.dimY ||
              safeCopy.dimZ != apiValue.dimZ || safeCopy.dimT != apiValue.dimT || safeCopy.type != apiValue.type )
         {
            APIHackingAttempt( "SetImageProperty" );
         }
      }

      (*API->FileFormat->EndImagePropertyEmbedding)( handle );

      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndImagePropertyEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteImageProperties( const PropertyArray& properties )
{
   try
   {
      if ( (*API->FileFormat->BeginImagePropertyEmbedding)( handle ) == api_false )
         return false;

      for ( const Property& property : properties )
         if ( property.IsValid() )
         {
            api_property_value apiValue;
            APIPropertyValueFromVariant( apiValue, property.Value() );
            api_property_value safeCopy = apiValue;
            bool ok = (*API->FileFormat->SetImageProperty)( handle, property.Id().c_str(), &safeCopy ) != api_false;

            if ( safeCopy.data.blockValue != apiValue.data.blockValue ||
                 safeCopy.dimX != apiValue.dimX || safeCopy.dimY != apiValue.dimY ||
                 safeCopy.dimZ != apiValue.dimZ || safeCopy.dimT != apiValue.dimT || safeCopy.type != apiValue.type )
            {
               APIHackingAttempt( "SetImageProperty" );
            }

            if ( !ok )
            {
               (*API->FileFormat->EndImagePropertyEmbedding)( handle );
               return false;
            }
         }

      (*API->FileFormat->EndImagePropertyEmbedding)( handle );
      return true;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndImagePropertyEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteImage( const pcl::Image& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

bool FileFormatInstance::WriteImage( const pcl::DImage& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

bool FileFormatInstance::WriteImage( const UInt8Image& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

bool FileFormatInstance::WriteImage( const UInt16Image& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

bool FileFormatInstance::WriteImage( const UInt32Image& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::CreateImage( const ImageInfo& info )
{
   api_image_info i;
   PCLImageInfoToAPI( i, info );
   return (*API->FileFormat->CreateImage)( handle, &i ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::CloseImage()
{
   return (*API->FileFormat->CloseImage)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

static bool WriteSamples( file_format_handle handle,
   const void* buffer, int startRow, int rowCount, int channel, int bitsPerSample, bool floatSample )
{
   return (*API->FileFormat->WriteSamples)( handle, buffer, startRow, rowCount, channel,
                                             bitsPerSample, floatSample, false ) != api_false;
}

bool FileFormatInstance::WriteSamples( const pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::WriteSamples( handle, buffer, startRow, rowCount, channel, 32, true );
}

bool FileFormatInstance::WriteSamples( const pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::WriteSamples( handle, buffer, startRow, rowCount, channel, 64, true );
}

bool FileFormatInstance::WriteSamples( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::WriteSamples( handle, buffer, startRow, rowCount, channel, 8, false );
}

bool FileFormatInstance::WriteSamples( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::WriteSamples( handle, buffer, startRow, rowCount, channel, 16, false );
}

bool FileFormatInstance::WriteSamples( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::WriteSamples( handle, buffer, startRow, rowCount, channel, 32, false );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WasLossyWrite() const
{
   return (*API->FileFormat->WasLossyWrite)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void* FileFormatInstance::CloneHandle() const
{
   throw Error( "FileFormatInstance::CloneHandle(): Cannot clone a file format instance handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FileFormatInstance.cpp - Released 2017-06-21T11:36:44Z
