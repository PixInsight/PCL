// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/FileFormatImplementation.cpp - Released 2014/11/14 17:17:00 UTC
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

#include <pcl/Exception.h>
#include <pcl/FileFormatImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct FileFormatProperty
{
   IsoString id;
   Variant   value;

   FileFormatProperty() : id(), value()
   {
   }

   FileFormatProperty( const IsoString& i, const Variant& v = Variant() ) : id( i ), value( v )
   {
   }

   FileFormatProperty( const FileFormatProperty& x ) : id( x.id ), value( x.value )
   {
   }

   FileFormatProperty& operator =( const FileFormatProperty& x )
   {
      id = x.id;
      value = x.value;
      return *this;
   }

   bool operator ==( const FileFormatProperty& x ) const
   {
      return id == x.id;
   }

   bool operator <( const FileFormatProperty& x ) const
   {
      return id < x.id;
   }
};

typedef Array<FileFormatProperty> FileFormatPropertyArray;

// ----------------------------------------------------------------------------

struct FileFormatImplementationPrivate
{
           FITSKeywordArray           keywords;
   mutable FITSKeywordArray::iterator keywordIterator;
           ICCProfile                 iccProfile;
           ByteArray                  metadata;
           UInt8Image                 thumbnail;
           FileFormatPropertyArray    properties;

   FileFormatImplementationPrivate() :
   keywords(), keywordIterator( 0 ), iccProfile(), metadata( 0 ), thumbnail(), properties()
   {
   }
};

// ----------------------------------------------------------------------------

FileFormatImplementation::FileFormatImplementation( const MetaFileFormat* m ) :
meta( m ), data( 0 ), description()
{
}

FileFormatImplementation::~FileFormatImplementation()
{
   if ( data != 0 )
      delete data, data = 0;

   /*
    * The following would only happen upon a catastrophic situation. Under
    * normal operation, the PI core application will always close files prior
    * to instance destruction.
    *
    * if ( IsOpen() )
    *   Close();
    *
    * Since MSVC seems to have problems with virtual functions called from
    * destructors (even virtual destructors), I decided to comment out the two
    * lines above.
    */
}

// ----------------------------------------------------------------------------

#define MANDATORY( funcName )             \
   __Mandatory( meta->Name(), funcName )

static void __Mandatory( const IsoString& formatName, const char* funcName )
{
   throw Error( String( formatName ) + "FileFormatImplementation: " +
                funcName + "() must be reimplemented in descendant class" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Close()
{
   MANDATORY( "Close" );
}

// ----------------------------------------------------------------------------

bool FileFormatImplementation::IsOpen() const
{
   MANDATORY( "IsOpen" );
   return false;
}

// ----------------------------------------------------------------------------

String FileFormatImplementation::FilePath() const
{
   MANDATORY( "FilePath" );
   return String();
}

// ----------------------------------------------------------------------------

ImageDescriptionArray FileFormatImplementation::Open( const String&, const IsoString& )
{
   MANDATORY( "Open" );
   return ImageDescriptionArray();
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::SelectImage( int )
{
   MANDATORY( "SelectImage" );
}

// ----------------------------------------------------------------------------

int FileFormatImplementation::SelectedImageIndex() const
{
   MANDATORY( "SelectedImageIndex" );
   return -1;
}

// ----------------------------------------------------------------------------

void* FileFormatImplementation::FormatSpecificData() const
{
   MANDATORY( "FormatSpecificData" );
   return 0;
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Extract( FITSKeywordArray& )
{
   MANDATORY( "ExtractKeywords" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Extract( ICCProfile& )
{
   MANDATORY( "ExtractICCProfile" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Extract( void*&, size_type& )
{
   MANDATORY( "ExtractMetadata" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Extract( pcl::UInt8Image& )
{
   MANDATORY( "ExtractThumbnail" );
}

// ----------------------------------------------------------------------------

ImagePropertyDescriptionArray FileFormatImplementation::Properties()
{
   MANDATORY( "Properties" );
   return ImagePropertyDescriptionArray();
}

// ----------------------------------------------------------------------------

Variant FileFormatImplementation::ReadProperty( const IsoString& property )
{
   MANDATORY( "ReadProperty" );
   return Variant();
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::WriteProperty( const IsoString& property, const Variant& value )
{
   MANDATORY( "WriteProperty" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::ReadImage( pcl::Image& )
{
   MANDATORY( "ReadImage" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::ReadImage( pcl::DImage& )
{
   MANDATORY( "ReadDImage" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::ReadImage( UInt8Image& )
{
   MANDATORY( "ReadUInt8Image" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::ReadImage( UInt16Image& )
{
   MANDATORY( "ReadUInt16Image" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::ReadImage( UInt32Image& )
{
   MANDATORY( "ReadUInt32Image" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Read( pcl::Image::sample*, int, int, int )
{
   MANDATORY( "ReadIncrementalImage" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Read( pcl::DImage::sample*, int, int, int )
{
   MANDATORY( "ReadIncrementalDImage" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Read( UInt8Image::sample*, int, int, int )
{
   MANDATORY( "ReadIncrementalUInt8Image" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Read( UInt16Image::sample*, int, int, int )
{
   MANDATORY( "ReadIncrementalUInt16Image" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Read( UInt32Image::sample*, int, int, int )
{
   MANDATORY( "ReadIncrementalUInt32Image" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Create( const String&, int, const IsoString& )
{
   MANDATORY( "Create" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::SetFormatSpecificData( const void* )
{
   MANDATORY( "SetFormatSpecificData" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Embed( const FITSKeywordArray& )
{
   MANDATORY( "EmbedKeywords" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Embed( const ICCProfile& )
{
   MANDATORY( "EmbedICCProfile" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Embed( const void*, size_type )
{
   MANDATORY( "EmbedMetadata" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Embed( const pcl::UInt8Image& )
{
   MANDATORY( "EmbedThumbnail" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::WriteImage( const pcl::Image& )
{
   MANDATORY( "WriteImage" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::WriteImage( const pcl::DImage& )
{
   MANDATORY( "WriteDImage" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::WriteImage( const UInt8Image& )
{
   MANDATORY( "WriteUInt8Image" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::WriteImage( const UInt16Image& )
{
   MANDATORY( "WriteUInt16Image" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::WriteImage( const UInt32Image& )
{
   MANDATORY( "WriteUInt32Image" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::CreateImage( const ImageInfo& )
{
   MANDATORY( "CreateImage" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Write( const pcl::Image::sample*, int, int, int )
{
   MANDATORY( "WriteIncrementalImage" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Write( const pcl::DImage::sample*, int, int, int )
{
   MANDATORY( "WriteIncrementalDImage" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Write( const UInt8Image::sample*, int, int, int )
{
   MANDATORY( "WriteIncrementalUInt8Image" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Write( const UInt16Image::sample*, int, int, int )
{
   MANDATORY( "WriteIncrementalUInt16Image" );
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::Write( const UInt32Image::sample*, int, int, int )
{
   MANDATORY( "WriteIncrementalUInt32Image" );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FileFormatImplementation::BeginPrivate()
{
   if ( data == 0 )
      data = new FileFormatImplementationPrivate;
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::BeginKeywordExtraction()
{
   BeginPrivate();
   data->keywords.Clear();
   Extract( data->keywords );
   data->keywordIterator = data->keywords.Begin();
}

size_type FileFormatImplementation::NumberOfKeywords() const
{
   return data->keywords.Length();
}

bool FileFormatImplementation::GetNextKeyword( FITSHeaderKeyword& k ) const
{
   if ( data->keywordIterator == data->keywords.End() )
      return false;
   k = *data->keywordIterator++; // N.B.: mutable keywordIterator
   return true;
}

void FileFormatImplementation::EndKeywordExtraction()
{
   data->keywords.Clear();
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::BeginICCProfileExtraction()
{
   BeginPrivate();
   data->iccProfile.Clear();
   Extract( data->iccProfile );
}

const ICCProfile& FileFormatImplementation::GetICCProfile() const
{
   return data->iccProfile;
}

void FileFormatImplementation::EndICCProfileExtraction()
{
   data->iccProfile.Clear();
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::BeginMetadataExtraction()
{
   BeginPrivate();
   data->metadata.Clear();
   void* metadata = 0;
   size_type size = 0;
   Extract( metadata, size );
   if ( metadata != 0 )
      data->metadata.Import( reinterpret_cast<uint8*>( metadata ), reinterpret_cast<uint8*>( metadata )+size );
}

const void* FileFormatImplementation::GetMetadata( size_type& size )
{
   size = data->metadata.Length();
   return data->metadata.Begin();
}

void FileFormatImplementation::EndMetadataExtraction()
{
   data->metadata.Clear();
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::BeginThumbnailExtraction()
{
   BeginPrivate();
   data->thumbnail.FreeData();
   Extract( data->thumbnail );
}

const UInt8Image& FileFormatImplementation::GetThumbnail() const
{
   return data->thumbnail;
}

void FileFormatImplementation::EndThumbnailExtraction()
{
   data->thumbnail.FreeData();
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::BeginPropertyExtraction()
{
   BeginPrivate();
   data->properties.Clear();
}

const Variant& FileFormatImplementation::GetImageProperty( const IsoString& id )
{
   IsoString tid = id.Trimmed();
   FileFormatPropertyArray::const_iterator i = data->properties.Search( tid );
   if ( i != data->properties.End() )
      return i->value;
   data->properties.Append( FileFormatProperty( tid, ReadProperty( tid ) ) );
   return data->properties.Search( tid )->value;
}

void FileFormatImplementation::EndPropertyExtraction()
{
   data->properties.Clear();
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::BeginKeywordEmbedding()
{
   BeginPrivate();
   data->keywords.Clear();
}

void FileFormatImplementation::AddKeyword( const FITSHeaderKeyword& k )
{
   data->keywords.Add( k );
}

void FileFormatImplementation::EndKeywordEmbedding()
{
   Embed( data->keywords );
   data->keywords.Clear();
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::BeginICCProfileEmbedding()
{
   BeginPrivate();
   data->iccProfile.Clear();
}

void FileFormatImplementation::SetICCProfile( const ICCProfile& icc )
{
   if ( icc.IsProfile() )
   {
      data->iccProfile = icc;
      data->iccProfile.SetEmbeddedFlag();
   }
}

void FileFormatImplementation::EndICCProfileEmbedding()
{
   if ( data->iccProfile.IsProfile() ) // ### should allow embedding empty profiles here?
      Embed( data->iccProfile );
   data->iccProfile.Clear();
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::BeginMetadataEmbedding()
{
   BeginPrivate();
   data->metadata.Clear();
}

void FileFormatImplementation::SetMetadata( const void* metadata, size_type size )
{
   if ( metadata != 0 && size > 0 )
   {
      data->metadata = ByteArray( size );
      ::memcpy( data->metadata.Begin(), metadata, size );
   }
}

void FileFormatImplementation::EndMetadataEmbedding()
{
   if ( !data->metadata.IsEmpty() )
   {
      Embed( data->metadata.Begin(), data->metadata.Length() );
      data->metadata.Clear();
   }
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::BeginThumbnailEmbedding()
{
   BeginPrivate();
   data->thumbnail.FreeData();
}

void FileFormatImplementation::SetThumbnail( const UInt8Image& img )
{
   if ( !img.IsEmpty() )
      data->thumbnail.Assign( img );
}

void FileFormatImplementation::EndThumbnailEmbedding()
{
   if ( !data->thumbnail.IsEmpty() )
   {
      Embed( data->thumbnail );
      data->thumbnail.FreeData();
   }
}

// ----------------------------------------------------------------------------

void FileFormatImplementation::BeginPropertyEmbedding()
{
   BeginPrivate();
   data->properties.Clear();
}

void FileFormatImplementation::SetImageProperty( const IsoString& id, const Variant& value )
{
   IsoString tid = id.Trimmed();
   if ( !tid.IsEmpty() && value.IsValid() )
   {
      FileFormatPropertyArray::iterator i = data->properties.Search( tid );
      if ( i != data->properties.End() )
         i->value = value;
      else
         data->properties.Append( FileFormatProperty( tid, value ) );
   }
}

void FileFormatImplementation::EndPropertyEmbedding()
{
   for ( FileFormatPropertyArray::const_iterator i = data->properties.Begin(); i != data->properties.End(); ++i )
      WriteProperty( i->id, i->value );
   data->properties.Clear();
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/FileFormatImplementation.cpp - Released 2014/11/14 17:17:00 UTC
