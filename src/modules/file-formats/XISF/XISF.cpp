// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard XISF File Format Module Version 01.00.00.0023
// ****************************************************************************
// XISF.cpp - Released 2014/11/30 10:38:10 UTC
// ****************************************************************************
// This file is part of the standard XISF PixInsight module.
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

#include "XISF.h"

#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QUrl>

#include <pcl/Console.h>
#include <pcl/FITSHeaderKeyword.h>
#include <pcl/Image.h>
#include <pcl/MetaModule.h>
#include <pcl/Random.h>
#include <pcl/Version.h>

#include <zlib/zlib.h>

// ----------------------------------------------------------------------------

/*
 * Prefix for identifiers of reserved XISF properties.
 */
#define XISF_INTERNAL_PREFIX "XISF:"

/*
 * Maximum depth of an XML error recovery loop.
 */
#define XISF_UNWIND_MAX 8

/*
 * XML error handling macros
 */

#define XML_READ_ERROR( xml )                                                                \
   catch ( ... )                                                                             \
   {                                                                                         \
      try                                                                                    \
      {                                                                                      \
         throw;                                                                              \
      }                                                                                      \
      catch ( pcl::CatchedException& )                                                       \
      {                                                                                      \
      }                                                                                      \
      catch ( pcl::SourceCodeError& x )                                                      \
      {                                                                                      \
         x.Show();                                                                           \
      }                                                                                      \
      catch ( pcl::Exception& x )                                                            \
      {                                                                                      \
         try                                                                                 \
         {                                                                                   \
            throw pcl::SourceCodeError( x.Message(), xml.lineNumber(), xml.columnNumber() ); \
         }                                                                                   \
         catch ( pcl::Exception& x )                                                         \
         {                                                                                   \
            x.Show();                                                                        \
         }                                                                                   \
      }                                                                                      \
      catch ( ... )                                                                          \
      {                                                                                      \
         try                                                                                 \
         {                                                                                   \
            throw pcl::Error( "Unknown exception" );                                         \
         }                                                                                   \
         catch ( pcl::Exception& x )                                                         \
         {                                                                                   \
            x.Show();                                                                        \
         }                                                                                   \
      }                                                                                      \
   }

#define XML_READ_ERROR_UNWIND( xml, elementName )                                            \
   catch ( ... )                                                                             \
   {                                                                                         \
      for ( int i = 0; i < XISF_UNWIND_MAX && xml.name() != elementName; ++i )               \
         xml.skipCurrentElement();                                                           \
      try                                                                                    \
      {                                                                                      \
         throw;                                                                              \
      }                                                                                      \
      XML_READ_ERROR( xml )                                                                  \
   }

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * XISF File Signature
 *
 * All XISF version 1.0 files begin with the following sequence:
 *
 * XISF0100<header-length><reserved>
 *
 * where 'XISF0100' is the 'magic marker' identifying the format, and
 * <header-length> is the size in bytes of the XML file header as a 32-bit
 * unsigned integer. <reserved> is a 32-bit unsigned integer reserved for
 * future use; it must be zero. After the file signature sequence comes the XML
 * header and all attached data blocks.
 *
 * In this file signature as well as in the rest of this implementation, we
 * assume a little-endian hardware architecture. At present, only Intel-based
 * machines, which are little-endian, are targetted by the PixInsight platform.
 */
struct XISFFileSignature
{
   uint8  magic[ 8 ];   // XISF0100
   uint32 headerLength; // length in bytes of the XML file header
   uint32 reserved;     // reserved - must be zero

   XISFFileSignature( uint64 length = 0 )
   {
      magic[0] = 'X'; magic[1] = 'I'; magic[2] = 'S'; magic[3] = 'F';
      magic[4] = '0'; magic[5] = '1'; magic[6] = '0'; magic[7] = '0';
      headerLength = length;
      reserved = 0;
   }

   XISFFileSignature( const XISFFileSignature& ) = default;

   void Validate() const
   {
      if ( magic[0] != 'X' || magic[1] != 'I' || magic[2] != 'S' || magic[3] != 'F' )
         throw Error( "Not an XISF file." );
      if ( magic[4] != '0' || magic[5] != '1' || magic[6] != '0' || magic[7] != '0' )
         throw Error( "Not an XISF version 1.0 file." );
      if ( headerLength < 245 ) // length of an empty XISF header, from "<?xml..." to </xisf>
         throw Error( "Invalid or corrupted XISF file." );
   }
};

// ----------------------------------------------------------------------------

const char* XISFEngineBase::SampleFormatId( int bitsPerSample, bool floatSample, bool complexSample )
{
   if ( complexSample )
   {
      switch ( bitsPerSample )
      {
      case 32: return "Complex32";
      case 64: return "Complex64";
      }
   }
   else if ( floatSample )
   {
      switch ( bitsPerSample )
      {
      case 32: return "Float32";
      case 64: return "Float64";
      }
   }
   else
   {
      switch ( bitsPerSample )
      {
      case  8: return "UInt8";
      case 16: return "UInt16";
      case 32: return "UInt32";
      case 64: return "UInt64";
      }
   }

   return 0;
}

bool XISFEngineBase::GetSampleFormatFromId( int& bitsPerSample, bool& floatSample, bool& complexSample, const IsoString& formatId )
{
   IsoString id = formatId.LowerCase();
   if ( id == "float32" )
   {
      bitsPerSample = 32;
      floatSample = true;
      complexSample = false;
   }
   else if ( id == "float64" )
   {
      bitsPerSample = 64;
      floatSample = true;
      complexSample = false;
   }
   else if ( id == "uint16" )
   {
      bitsPerSample = 16;
      floatSample = false;
      complexSample = false;
   }
   else if ( id == "uint8" )
   {
      bitsPerSample = 8;
      floatSample = false;
      complexSample = false;
   }
   else if ( id == "uint32" )
   {
      bitsPerSample = 32;
      floatSample = false;
      complexSample = false;
   }
   else if ( id == "complex32" )
   {
      bitsPerSample = 32;
      floatSample = true;
      complexSample = true;
   }
   else if ( id == "complex64" )
   {
      bitsPerSample = 64;
      floatSample = true;
      complexSample = true;
   }
   else if ( id == "uint64" )
   {
      bitsPerSample = 64;
      floatSample = false;
      complexSample = false;
   }
   else
      return false;

   return true;
}

const char* XISFEngineBase::ColorSpaceId( int colorSpace )
{
   switch ( colorSpace )
   {
   case ColorSpace::Gray:
      return "Gray";
   case ColorSpace::RGB:
      return "RGB";
   case ColorSpace::CIEXYZ:
      return "CIEXYZ";
   case ColorSpace::CIELab:
      return "CIELab";
   case ColorSpace::CIELch:
      return "CIELch";
   case ColorSpace::HSV:
      return "HSV";
   case ColorSpace::HSI:
      return "HSI";
   default:
      return 0;
   }
}

int XISFEngineBase::ColorSpaceFromId( const IsoString& spaceId )
{
   IsoString id = spaceId.LowerCase();
   if ( id == "gray" )
      return ColorSpace::Gray;
   if ( id == "rgb" )
      return ColorSpace::RGB;
   if ( id == "ciexyz" )
      return ColorSpace::CIEXYZ;
   if ( id == "cielab" )
      return ColorSpace::CIELab;
   if ( id == "cielch" )
      return ColorSpace::CIELch;
   if ( id == "hsv" )
      return ColorSpace::HSV;
   if ( id == "hsi" )
      return ColorSpace::HSI;
   return ColorSpace::Unknown;
}

const char* XISFEngineBase::CFATypeId( int cfa )
{
   switch ( cfa )
   {
   case CFAType::BGGR:
      return "BGGR";
   case CFAType::GRBG:
      return "GRBG";
   case CFAType::GBRG:
      return "GBRG";
   case CFAType::RGGB:
      return "RGGB";
   case CFAType::CYGM:
      return "CYGM";
   default:
      return 0;
   }
}

int XISFEngineBase::CFATypeFromId( const IsoString& cfaId )
{
   if ( cfaId.IsEmpty() )
      return CFAType::None;
   IsoString id = cfaId.LowerCase();
   if ( id == "bggr" )
      return CFAType::BGGR;
   if ( id == "grbg" )
      return CFAType::GRBG;
   if ( id == "gbrg" )
      return CFAType::GBRG;
   if ( id == "rggb" )
      return CFAType::RGGB;
   if ( id == "cygm" )
      return CFAType::CYGM;
   return CFAType::None;
}

const char* XISFEngineBase::PropertyTypeId( int type )
{
   switch ( type )
   {
   case VariantType::Bool:       return "Boolean";
   case VariantType::Int8:       return "Int8";
   case VariantType::Int16:      return "Int16";
   case VariantType::Int32:      return "Int32";
   case VariantType::Int64:      return "Int64";
   case VariantType::UInt8:      return "UInt8";
   case VariantType::UInt16:     return "UInt16";
   case VariantType::UInt32:     return "UInt32";
   case VariantType::UInt64:     return "UInt64";
   case VariantType::Float:      return "Float32";
   case VariantType::Double:     return "Float64";
   case VariantType::ByteArray:  return "ByteArray";
   case VariantType::IVector:    return "IVector";
   case VariantType::UIVector:   return "UIVector";
   case VariantType::I64Vector:  return "I64Vector";
   case VariantType::UI64Vector: return "UI64Vector";
   case VariantType::FVector:    return "F32Vector";
   case VariantType::DVector:    return "F64Vector";
   case VariantType::ByteMatrix: return "ByteMatrix";
   case VariantType::IMatrix:    return "IMatrix";
   case VariantType::UIMatrix:   return "UIMatrix";
   case VariantType::I64Matrix:  return "I64Matrix";
   case VariantType::UI64Matrix: return "UI64Matrix";
   case VariantType::FMatrix:    return "F32Matrix";
   case VariantType::DMatrix:    return "F64Matrix";
   case VariantType::String:     return "String16";
   case VariantType::IsoString:  return "String8";
   default:                      return 0;
   }
}

int XISFEngineBase::PropertyTypeFromId( const IsoString& typeId )
{
   IsoString id = typeId.LowerCase();
   if ( id == "boolean" || id == "bool" )
      return VariantType::Bool;
   if ( id == "int8" )
      return VariantType::Int8;
   if ( id == "int16" )
      return VariantType::Int16;
   if ( id == "int32" )
      return VariantType::Int32;
   if ( id == "int64" )
      return VariantType::Int64;
   if ( id == "uint8" )
      return VariantType::UInt8;
   if ( id == "uint16" )
      return VariantType::UInt16;
   if ( id == "uint32" )
      return VariantType::UInt32;
   if ( id == "uint64" )
      return VariantType::UInt64;
   if ( id == "float32" || id == "float" )
      return VariantType::Float;
   if ( id == "float64" || id == "double" )
      return VariantType::Double;
   if ( id == "bytearray" )
      return VariantType::ByteArray;
   if ( id == "ivector" )
      return VariantType::IVector;
   if ( id == "uivector" )
      return VariantType::UIVector;
   if ( id == "i64vector" )
      return VariantType::I64Vector;
   if ( id == "ui64vector" )
      return VariantType::UI64Vector;
   if ( id == "f32vector" || id == "fvector" )
      return VariantType::FVector;
   if ( id == "f64vector" || id == "dvector" )
      return VariantType::DVector;
   if ( id == "bytematrix" )
      return VariantType::ByteMatrix;
   if ( id == "imatrix" )
      return VariantType::IMatrix;
   if ( id == "uimatrix" )
      return VariantType::UIMatrix;
   if ( id == "i64matrix" )
      return VariantType::I64Matrix;
   if ( id == "ui64matrix" )
      return VariantType::UI64Matrix;
   if ( id == "f32matrix" || id == "fmatrix" )
      return VariantType::FMatrix;
   if ( id == "f64matrix" || id == "dmatrix" )
      return VariantType::DMatrix;
   if ( id == "string16" || id == "string" || id == "utf16string" )
      return VariantType::String;
   if ( id == "string8" || id == "isostring" || id == "utf8string" )
      return VariantType::IsoString;
   return VariantType::Invalid;
}

bool XISFEngineBase::IsInternalPropertyId( const IsoString& id )
{
   return id.BeginsWith( XISF_INTERNAL_PREFIX );
}

IsoString XISFEngineBase::InternalPropertyId( const IsoString& id )
{
   return XISF_INTERNAL_PREFIX + id;
}

// ----------------------------------------------------------------------------

/*
 * Generic data block in an XISF input stream.
 */
struct XISFInputDataBlock
{
   /*
    * For blocks stored as attachments, the position and size members provide
    * direct file coordinates.
    *
    * For inline and embedded blocks, we have position=0 and the data already
    * acquired in the data array. The size member is redundant in this case.
    */
   fpos_type  position;         // absolute file position in bytes
   fsize_type size;             // block size in bytes
   bool       compressed : 1;   // zlib compression ?
   fsize_type uncompressedSize; // if compressed, size of the uncompressed data
   ByteArray  data;             // block data

   XISFInputDataBlock() : position( 0 ), size( 0 ), compressed( false ), uncompressedSize( 0 ), data()
   {
   }

   XISFInputDataBlock( const XISFInputDataBlock& ) = default;
   XISFInputDataBlock& operator =( const XISFInputDataBlock& ) = default;

   bool HasData() const
   {
      return !data.IsEmpty();
   }

   bool IsCompressed() const
   {
      return compressed;
   }

   bool IsValid() const
   {
      return position > 0 || HasData();
   }

   bool IsEmpty() const
   {
      return !HasData() && size <= 0;
   }

   void UncompressData()
   {
      if ( HasData() )
         if ( IsCompressed() )
         {
            if ( uncompressedSize == 0 || uncompressedSize >= uint32( -1 ) )
               throw Error( "Internal error: Invalid uncompressed data size." );

            ByteArray uncompressedData = ByteArray( size_type( uncompressedSize ) );
            unsigned long resultSize = uint32( uncompressedSize );
            int result = ::uncompress( uncompressedData.Begin(), &resultSize, data.Begin(), (unsigned long)data.Length() );
            if ( result != Z_OK )
               throw Error( "Failed to uncompress block data." );
            if ( fsize_type( resultSize ) != uncompressedSize )
               throw Error( "Failed to uncompress block data: Uncompressed data size mismatch." );

            data = uncompressedData;
            compressed = false;
         }
   }
};

/*
 * Image data block in an XISF input stream.
 */
struct XISFInputImageBlock : public XISFInputDataBlock
{
   ImageInfo info; // geometry and color space parameters

   XISFInputImageBlock() = default;
   XISFInputImageBlock( const XISFInputImageBlock& ) = default;
   XISFInputImageBlock& operator =( const XISFInputImageBlock& ) = default;
};

/*
 * Property data block in an XISF input stream.
 */
struct XISFInputPropertyBlock : public XISFInputDataBlock
{
           IsoString  id;         // unique property identifier
           int        type;       // property data type
           Array<int> dimensions; // dimensions of a vector or matrix property
   mutable Variant    value;      // property value

   XISFInputPropertyBlock() = default;
   XISFInputPropertyBlock( const XISFInputPropertyBlock& ) = default;
   XISFInputPropertyBlock& operator =( const XISFInputPropertyBlock& ) = default;

   XISFInputPropertyBlock( const IsoString& a_id ) : id( a_id )
   {
   }

   bool HasValue() const
   {
      return value.IsValid();
   }

   bool operator ==( const XISFInputPropertyBlock& x ) const
   {
      return id == x.id;
   }

   bool operator <( const XISFInputPropertyBlock& x ) const
   {
      return id < x.id;
   }
};

typedef SortedArray<XISFInputPropertyBlock> XISFInputPropertyBlockArray;

// ----------------------------------------------------------------------------

/*
 * XISF input stream engine.
 */
class XISFReaderEngine : public XISFEngineBase
{
public:

   XISFReaderEngine() = default;

   virtual ~XISFReaderEngine()
   {
   }

   /*
    * Set format-specific options.
    */
   void SetOptions( const XISFOptions& options )
   {
      m_xisfOptions = options;
   }

   /*
    * Set hints string for InputHints metadata property generation.
    */
   void SetHints( const IsoString& hints )
   {
      m_hints = hints;
   }

   bool IsOpen() const
   {
      return !m_path.IsEmpty();
   }

   /*
    * Open an XISF file for read only access. This member function opens a
    * local file and parses the whole XML header.
    */
   void Open( const String& path )
   {
      Reset();

#ifdef __PCL_WINDOWS
      m_path = File::WindowsPathToUnix( path );
#else
      m_path = path;
#endif
      m_path.Trim();

      try
      {
         m_file.OpenForReading( m_path );
         IsoString text;
         {
            XISFFileSignature signature;
            m_file.Read( signature );
            signature.Validate();

            m_headerLength = signature.headerLength;
            m_fileSize = m_file.Size();
            m_minBlockPos = m_headerLength + sizeof( XISFFileSignature );

            text.Reserve( m_headerLength );
            m_file.Read( reinterpret_cast<void*>( text.Begin() ), m_headerLength );
            text[m_headerLength] = '\0';
         }

         if ( text.IsEmpty() )
            throw Error( "Empty XISF file" );

         QXmlStreamReader xml( text.c_str() );

         if ( !xml.readNextStartElement() )
         {
            if ( xml.hasError() )
               throw SourceCodeError( String( xml.errorString() ), xml.lineNumber(), xml.columnNumber() );
            throw Error( "Malformed XML source." );
         }

         if ( xml.name() != "xisf" || xml.attributes().value( "version" ) != "1.0" )
            throw Error( "Not an XISF version 1.0 file" );

         try
         {
            while ( xml.readNextStartElement() )
            {
               if ( xml.name() == "Image" )
               {
                  try
                  {
                     ImageData data;

                     /*
                      * Image elements can only have attachment or embedded
                      * storage.
                      */
                     GetBlockData( data.image, xml );

                     /*
                      * Image element attributes.
                      */

                     data.id = IsoString( xml.attributes().value( "id" ).toString() ).Trimmed();
                     if ( !data.id.IsEmpty() )
                        if ( data.id.IsValidIdentifier() )
                        {
                           XISFInputPropertyBlock property;
                           property.id = InternalPropertyId( "ImageIdentifier" );
                           property.type = VariantType::IsoString;
                           property.value = data.id;
                           AddPropertyBlock( data.properties, property, xml );
                        }
                        else
                        {
                           Warning( xml, "Ignoring invalid image identifier '" + data.id + "'" );
                           data.id.Clear();
                        }

                     GetImageGeometry( data.image, xml );

                     // sampleFormat="<f>"
                     IsoString s = IsoString( xml.attributes().value( "sampleFormat" ).toString() ).Trimmed();
                     if ( s.IsEmpty() )
                        throw Error( "Missing image sampleFormat attribute." );
                     {
                        int bitsPerSample;
                        bool floatSample, complexSample;
                        if ( !GetSampleFormatFromId( bitsPerSample, floatSample, complexSample, s ) )
                           throw Error( "Invalid/unknown image sample format '" + s + "'" );
                        if ( bitsPerSample == 64 && !floatSample )
                           throw Error( "Unsupported 64-bit integer image format." );
                        data.options.bitsPerSample = bitsPerSample;
                        data.options.ieeefpSampleFormat = floatSample;
                        data.options.complexSample = complexSample;
                     }

                     data.options.lowerRange = 0;
                     if ( data.options.ieeefpSampleFormat )
                        data.options.upperRange = 1;
                     else
                        data.options.upperRange = (uint32( 1 ) << data.options.bitsPerSample) - 1;

                     s = IsoString( xml.attributes().value( "bounds" ).toString() ).Trimmed();
                     if ( !s.IsEmpty() )
                     {
                        // bounds="<l>:<u>"
                        IsoStringList tokens;
                        s.Break( tokens, ':', true/*trim*/ );
                        if ( tokens.Length() < 2 )
                           throw Error( "Malformed image bounds attribute: '" + s + "'" );
                        data.options.lowerRange = tokens[0].ToDouble();
                        data.options.upperRange = tokens[1].ToDouble();

                        if ( tokens.Length() > 2 )
                           Warning( xml, "Ignoring excess image bounds data: '" + s + "'" );

                        if ( xml.attributes().hasAttribute( "lowerBound" ) ||
                             xml.attributes().hasAttribute( "upperBound" ) )
                           Warning( xml, "Ignoring redundant image bounds attributes." );
                     }
                     else
                     {
                        // lowerBound="<l>"
                        s = IsoString( xml.attributes().value( "lowerBound" ).toString() ).Trimmed();
                        if ( s.IsEmpty() )
                        {
                           if ( data.options.ieeefpSampleFormat )
                              throw Error( "Missing image bounds or lowerBound attribute, which is mandatory for a floating point real or complex image." );
                        }
                        else
                           data.options.lowerRange = s.ToDouble();

                        // upperBound="<u>"
                        s = IsoString( xml.attributes().value( "upperBound" ).toString() ).Trimmed();
                        if ( s.IsEmpty() )
                        {
                           if ( data.options.ieeefpSampleFormat )
                              throw Error( "Missing image upperBound attribute, which is mandatory for a floating point real or complex image." );
                        }
                        else
                           data.options.upperRange = s.ToDouble();
                     }

                     if ( !data.options.ieeefpSampleFormat )
                     {
                        if ( data.options.lowerRange < 0 || data.options.lowerRange >= (uint32( 1 ) << data.options.bitsPerSample) )
                           throw Error( "Invalid " + String( data.options.bitsPerSample ) + "-bit integer lower bound: " + s );
                        if ( data.options.upperRange < 0 || data.options.upperRange >= (uint32( 1 ) << data.options.bitsPerSample) )
                           throw Error( "Invalid " + String( data.options.bitsPerSample ) + "-bit integer upper bound: " + s );
                     }

                     if ( data.options.upperRange < data.options.lowerRange )
                     {
                        Swap( data.options.lowerRange, data.options.upperRange );
                        Warning( xml, "Swapping unordered lower and upper bound attribute values (good try! :)" );
                     }

                     if ( 1 == 1 + data.options.upperRange - data.options.lowerRange )
                        Warning( xml, "Empty or infinitesimal pixel sample range." );

                     // colorSpace="<s>"
                     s = IsoString( xml.attributes().value( "colorSpace" ).toString() ).Trimmed();
                     if ( s.IsEmpty() )
                     {
                        data.image.info.colorSpace = ColorSpace::Gray;
                        Warning( xml, "Missing colorSpace image attribute; assuming the grayscale color space." );
                     }
                     else
                     {
                        data.image.info.colorSpace = ColorSpaceFromId( s );
                        if ( data.image.info.colorSpace == ColorSpace::Unknown )
                           throw Error( "Invalid/unknown image color space '" + s + "'" );
                     }

                     data.image.info.supported = data.image.info.IsValid();
                     if ( !data.image.info.supported )
                        throw Error( "Invalid/unsupported image parameters." );

                     // cfaType="<a>"
                     s = IsoString( xml.attributes().value( "cfaType" ).toString() ).Trimmed();
                     if ( !s.IsEmpty() )
                     {
                        data.options.cfaType = CFATypeFromId( s );
                        if ( data.options.cfaType == CFAType::None )
                           throw Error( "Invalid/unsupported CFA type '" + s + "'" );
                     }

                     if ( m_xisfOptions.verbosity > 0 )
                     {
                        m_console.WriteLn( "<end><cbr>Loading image" + (data.id.IsEmpty() ? String() : String( " '" + data.id + '\'' )) +
                           String().Format( ": w=%d h=%d n=%d ",
                                            data.image.info.width, data.image.info.height, data.image.info.numberOfChannels ) +
                           ColorSpaceId( data.image.info.colorSpace ) + ' ' +
                           SampleFormatId( data.options.bitsPerSample, data.options.ieeefpSampleFormat, data.options.complexSample ) );
                        Module->ProcessEvents();
                     }

                     /*
                      * Child Image elements.
                      */
                     while ( xml.readNextStartElement() )
                     {
                        if ( xml.name() == "Property" )
                        {
                           /*
                            * Embedded image properties.
                            */
                           if ( !m_xisfOptions.ignoreProperties )
                           {
                              try
                              {
                                 GetProperty( data.properties, xml );
                              }
                              XML_READ_ERROR_UNWIND( xml, "Property" )
                           }
                        }
                        else if ( xml.name() == "FITSKeyword" )
                        {
                           /*
                            * FITS header keywords.
                            */
                           if ( !m_xisfOptions.ignoreFITSKeywords )
                           {
                              try
                              {
                                 GetFITSKeyword( data.properties, data.keywords, xml );
                              }
                              XML_READ_ERROR_UNWIND( xml, "FITSKeyword" )
                           }
                        }
                        else if ( xml.name() == "RGBWS" )
                        {
                           /*
                            * RGB working space parameters.
                            */
                           if ( !m_xisfOptions.ignoreEmbeddedData )
                           {
                              try
                              {
                                 GetRGBWS( data.rgbws, xml );
                              }
                              XML_READ_ERROR_UNWIND( xml, "RGBWS" )
                           }
                        }
                        else if ( xml.name() == "Resolution" )
                        {
                           /*
                            * Image resolution data.
                            */
                           if ( !m_xisfOptions.ignoreEmbeddedData )
                           {
                              try
                              {
                                 GetImageResolution( data.options, xml );
                              }
                              XML_READ_ERROR_UNWIND( xml, "Resolution" )
                           }
                        }
                        else if ( xml.name() == "ICCProfile" )
                        {
                           /*
                            * Embedded ICC profile.
                            */
                           if ( !m_xisfOptions.ignoreEmbeddedData )
                           {
                              try
                              {
                                 if ( data.iccProfile.IsValid() )
                                    Warning( xml, "Redefining ICCProfile Image element." );

                                 GetBlockData( data.iccProfile, xml );
                                 if ( data.iccProfile.IsEmpty() )
                                    throw Error( "Missing ICC profile data." );
                              }
                              XML_READ_ERROR_UNWIND( xml, "ICCProfile" )
                           }
                        }
                        else if ( xml.name() == "Thumbnail" )
                        {
                           /*
                            * Embedded image thumbnail.
                            */
                           if ( !m_xisfOptions.ignoreEmbeddedData )
                           {
                              try
                              {
                                 if ( data.thumbnail.IsValid() )
                                    Warning( xml, "Redefining thumbnail Image element." );

                                 GetThumbnail( data.thumbnail, xml );
                              }
                              XML_READ_ERROR_UNWIND( xml, "Thumbnail" )
                           }
                        }
                        else if ( xml.name() == "Data" )
                        {
                           /*
                            * Image data (location="embedded").
                            */
                           if ( !data.image.IsEmpty() )
                              Warning( xml, String( "Redefining image data." ) );

                           GetImageEmbeddedData( data.image, xml );
                        }
                        else
                        {
                           Warning( xml, String( "Skipping unknown \'" + xml.name().toString() + "\' Image element." ) );
                        }

                        if ( xml.isCharacters() )
                           Warning( xml, String( "Ignoring unexpected Image element contents." ) );

                        if ( !xml.isEndElement() )
                           xml.skipCurrentElement();
                     }

                     if ( m_xisfOptions.verbosity > 0 )
                        if ( !data.properties.IsEmpty() )
                           m_console.WriteLn( "<end><cbr>" + String( data.properties.Length() ) + " image " +
                                              ((data.properties.Length() > 1) ? "properties" : "property") );

                     m_images.Append( data );
                  }
                  XML_READ_ERROR_UNWIND( xml, "Image" )
               }
               else if ( xml.name() == "Property" )
               {
                  /*
                   * Global property.
                   */
                  if ( !m_xisfOptions.ignoreProperties )
                  {
                     try
                     {
                        GetProperty( m_properties, xml, false/*isInternal*/ );
                     }
                     XML_READ_ERROR_UNWIND( xml, "Property" )
                  }
               }
               else if ( xml.name() == "Metadata" )
               {
                  try
                  {
                     /*
                      * The Metadata element contains a sequence of property
                      * child elements.
                      */
                     while ( xml.readNextStartElement() )
                     {
                        if ( xml.name() == "Property" )
                        {
                           try
                           {
                              GetProperty( m_properties, xml, true/*isInternal*/ );
                           }
                           XML_READ_ERROR_UNWIND( xml, "Property" )
                        }
                        else
                        {
                           Warning( xml, String( "Skipping unknown \'" + xml.name().toString() + "\' Metadata element." ) );
                        }

                        if ( xml.isCharacters() )
                           Warning( xml, String( "Ignoring unexpected Metadata element contents." ) );

                        if ( !xml.isEndElement() )
                           xml.skipCurrentElement();
                     }
                  }
                  XML_READ_ERROR_UNWIND( xml, "Metadata" )
               }
               else
               {
                  Warning( xml, String( "Skipping unknown \'" + xml.name().toString() + "\' element." ) );
               }

               if ( xml.isCharacters() )
                  Warning( xml, String( "Ignoring unexpected \'" + xml.name().toString() + "\' element contents." ) );

               if ( !xml.isEndElement() )
                  xml.skipCurrentElement();
            }

            if ( xml.hasError() )
               throw SourceCodeError( String( xml.errorString() ), xml.lineNumber(), xml.columnNumber() );

            if ( m_images.IsEmpty() )
               throw Error( "The XSIF file contains no readable image." );

            /*
             * Generated internal properties.
             */
            if ( m_xisfOptions.autoMetadata )
            {
               XISFInputPropertyBlock property;
               property.id = InternalPropertyId( "ResourceURL" );
               property.type = VariantType::String;
               property.value = String( QUrl::fromLocalFile( m_path ).toString( QUrl::RemoveUserInfo|QUrl::RemovePassword ) );
               AddPropertyBlock( m_properties, property, xml );

               property.id = InternalPropertyId( "LoadTime" );
               property.type = VariantType::IsoString;
               property.value = IsoString( QDateTime::currentDateTimeUtc().toString( Qt::ISODate ) );
               AddPropertyBlock( m_properties, property, xml );

               if ( !m_hints.IsEmpty() )
               {
                  property.id = InternalPropertyId( "InputHints" );
                  property.type = VariantType::IsoString;
                  property.value = m_hints;
                  AddPropertyBlock( m_properties, property, xml );
               }
            }

            /*
             * Global properties are exposed to all images.
             */
            if ( !m_properties.IsEmpty() )
               for ( ImageDataArray::iterator i = m_images.Begin(); i != m_images.End(); ++i )
                  i->properties.Add( m_properties );
         }
         catch ( ... )
         {
            try
            {
               throw;
            }
            catch ( SourceCodeError& )
            {
               throw;
            }
            catch ( Error& x )
            {
               throw SourceCodeError( x.Message(), xml.lineNumber(), xml.columnNumber() );
            }
            catch ( ... )
            {
               throw;
            }
         }
      }
      catch ( ... )
      {
         Reset();
         throw;
      }
   }

   /*
    * Close the input stream and reset all internal data structures.
    */
   void Close()
   {
      Reset();
   }

   /*
    * The file path of the current input stream.
    */
   String FilePath() const
   {
      return m_path;
   }

   /*
    * The number of images stored in this XISF file.
    */
   int NumberOfImages() const
   {
      return int( m_images.Length() );
   }

   /*
    * Select the current image for input operations. index must be in the range
    * [0,NumberOfImages()-1].
    */
   void SelectImage( int index )
   {
      CheckImageAccess();
      if ( index < 0 || index >= NumberOfImages() )
         throw Error( "Image index out of range: " + String( index ) );
      m_currentImage = index;
   }

   /*
    * Zero-based index of the current image.
    */
   int SelectedImageIndex() const
   {
      return m_currentImage;
   }

   /*
    * Geometry and color space parameters of the current image.
    */
   const pcl::ImageInfo& ImageInfo() const
   {
      CheckImageAccess();
      return m_images[m_currentImage].image.info;
   }

   /*
    * Format-independent options and parameters of the current image.
    */
   const pcl::ImageOptions& ImageOptions() const
   {
      CheckImageAccess();
      return m_images[m_currentImage].options;
   }

   /*
    * Sets format-independent options for the current image. Only options that
    * can change the reading behavior are taken into account.
    */
   void SetImageOptions( const pcl::ImageOptions& options )
   {
      CheckImageAccess();
      m_images[m_currentImage].options.readNormalized = options.readNormalized;
   }

   /*
    * Identifier (optional) of the current image.
    */
   IsoString ImageId() const
   {
      CheckImageAccess();
      return m_images[m_currentImage].id;
   }

   /*
    * The set of FITS header keywords associated with the current image.
    */
   FITSKeywordArray Keywords() const
   {
      CheckImageAccess();
      FITSKeywordArray keywords = m_images[m_currentImage].keywords;
      if ( m_xisfOptions.verbosity > 0 )
         if ( !keywords.IsEmpty() )
            m_console.WriteLn( "<end><cbr>" + String( keywords.Length() ) + " FITS keyword(s) extracted." );
      return keywords;
   }

   /*
    * The ICC profile structure associated with the current image.
    */
   pcl::ICCProfile ICCProfile()
   {
      CheckImageAccess();
      XISFInputDataBlock& block = m_images[m_currentImage].iccProfile;
      if ( !block.IsValid() )
         return pcl::ICCProfile();
      if ( !block.HasData() )
      {
         block.data = ByteArray( size_type( block.size ) );
         m_file.SetPosition( block.position );
         m_file.Read( block.data.Begin(), block.size );

         if ( block.IsCompressed() )
            UncompressBlock( block );
      }
      pcl::ICCProfile icc( reinterpret_cast<const void*>( block.data.Begin() ) );
      if ( m_xisfOptions.verbosity > 0 )
         if ( icc.IsProfile() )
            m_console.WriteLn( "<end><cbr>ICC profile extracted: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes." );
      return icc;
   }

   /*
    * Metadata block associated with the current image.
    */
   ByteArray Metadata()
   {
      CheckImageAccess();
      return ByteArray(); // ### TODO
      //m_images[m_currentImage].metadata;
   }

   /*
    * The thumbnail image associated with the current image.
    */
   UInt8Image Thumbnail()
   {
      CheckImageAccess();
      XISFInputImageBlock& block = m_images[m_currentImage].thumbnail;
      if ( !block.IsValid() )
         return UInt8Image();

      if ( block.IsCompressed() )
      {
         if ( !block.HasData() )
         {
            block.data = ByteArray( size_type( block.size ) );
            m_file.SetPosition( block.position );
            m_file.Read( block.data.Begin(), block.size );
         }

         UncompressBlock( block );
      }

      UInt8Image thumbnail;
      thumbnail.AllocateData( block.info.width, block.info.height, block.info.numberOfChannels, ColorSpace::value_type( block.info.colorSpace ) );
      if ( (block.HasData() ? block.data.Length() : size_type( block.size )) != thumbnail.ImageSize() )
         throw Error( "Internal error: invalid thumbnail block size." );

      for ( int i = 0; i < thumbnail.NumberOfChannels(); ++i )
         if ( block.HasData() )
         {
            // location="inline"
            ::memcpy( thumbnail[i], block.data.At( i*thumbnail.ChannelSize() ), thumbnail.ChannelSize() );
         }
         else
         {
            // location="attachment:<position>:<size>"
            m_file.SetPosition( block.position + i*thumbnail.ChannelSize() );
            m_file.Read( thumbnail[i], thumbnail.ChannelSize() );
         }

      if ( m_xisfOptions.verbosity > 1 )
         if ( !thumbnail.IsEmpty() )
            m_console.WriteLn( "<end><cbr>Image thumbnail extracted " +
               String().Format( "(w=%d h=%d n=%d)", thumbnail.Width(), thumbnail.Height(), thumbnail.NumberOfChannels() ) );
      return thumbnail;
   }

   /*
    * Returns a descriptive list of the properties associated with the current
    * image.
    */
   ImagePropertyDescriptionArray Properties() const
   {
      CheckImageAccess();
      ImagePropertyDescriptionArray descriptions;
      const XISFInputPropertyBlockArray& properties = m_images[m_currentImage].properties;
      for ( XISFInputPropertyBlockArray::const_iterator i = properties.Begin(); i != properties.End(); ++i )
         descriptions.Append( ImagePropertyDescription( i->id, Variant::data_type( i->type ) ) );
      return descriptions;
   }

   /*
    * Read the value of a property of the current image.
    */
   Variant Property( const IsoString& id )
   {
      CheckImageAccess();
      XISFInputPropertyBlockArray::const_iterator i = m_images[m_currentImage].properties.Search( id );
      if ( i == m_images[m_currentImage].properties.End() )
         return Variant();

      if ( !i->HasValue() )
      {
         if ( Variant::IsScalarType( i->type ) || i->type == VariantType::String || i->type == VariantType::IsoString )
            throw Error( "Internal error: invalid or corrupted image property '" + i->id + "'" );

         if ( Variant::IsVectorType( i->type ) )
         {
            if ( i->dimensions.Length() != 1 || i->dimensions[0] < 0 )
               throw Error( "Internal error: invalid vector length for image property '" + i->id + "'" );

            int length = i->dimensions[0];
            switch ( i->type )
            {
            case VariantType::ByteArray:
               i->value = Variant( ByteArray( size_type( length ) ) );
               break;
            case VariantType::IVector:
               i->value = Variant( IVector( length ) );
               break;
            case VariantType::UIVector:
               i->value = Variant( UIVector( length ) );
               break;
            case VariantType::I64Vector:
               i->value = Variant( I64Vector( length ) );
               break;
            case VariantType::UI64Vector:
               i->value = Variant( UI64Vector( length ) );
               break;
            case VariantType::FVector:
               i->value = Variant( FVector( length ) );
               break;
            case VariantType::DVector:
               i->value = Variant( DVector( length ) );
               break;
            default:
               throw Error( "Internal error: invalid vector data type for property '" + i->id + "'" );
            }
         }
         else if ( Variant::IsMatrixType( i->type ) )
         {
            if ( i->dimensions.Length() != 2 || i->dimensions[0] < 0 || i->dimensions[1] < 0 )
               throw Error( "Internal error: invalid matrix dimensions for image property '" + i->id + "'" );

            int rows = i->dimensions[0];
            int cols = i->dimensions[1];
            switch ( i->type )
            {
            case VariantType::ByteMatrix:
               i->value = Variant( ByteMatrix( rows, cols ) );
               break;
            case VariantType::IMatrix:
               i->value = Variant( IMatrix( rows, cols ) );
               break;
            case VariantType::UIMatrix:
               i->value = Variant( UIMatrix( rows, cols ) );
               break;
            case VariantType::I64Matrix:
               i->value = Variant( I64Matrix( rows, cols ) );
               break;
            case VariantType::UI64Matrix:
               i->value = Variant( UI64Matrix( rows, cols ) );
               break;
            case VariantType::FMatrix:
               i->value = Variant( FMatrix( rows, cols ) );
               break;
            case VariantType::DMatrix:
               i->value = Variant( DMatrix( rows, cols ) );
               break;
            default:
               throw Error( "Internal error: invalid matrix data type for property '" + i->id + "'" );
            }
         }
         else
            throw Error( "Internal error: invalid data type for property '" + i->id + "'" );

         if ( i->value.BlockSize() > 0 )
         {
            XISFInputPropertyBlock& property = const_cast<XISFInputPropertyBlock&>( *i );

            if ( !property.HasData() )
               if ( property.IsCompressed() )
               {
                  property.data = ByteArray( size_type( property.size ) );
                  m_file.SetPosition( property.position );
                  m_file.Read( property.data.Begin(), property.size );

                  UncompressBlock( property );
               }

            if ( property.HasData() )
            {
               // location="inline" or compressed data
               if ( property.data.Length() != property.value.BlockSize() )
                  throw Error( "Invalid block size for property '" + i->id + "'" );
               ::memcpy( const_cast<void*>( property.value.InternalBlockAddress() ), property.data.Begin(), property.data.Length() );
               property.data.Clear();
            }
            else
            {
               // location="attachment:<position>:<size>"
               if ( size_type( property.size ) != property.value.BlockSize() )
                  throw Error( "Invalid block size for property '" + i->id + "'" );
               m_file.SetPosition( property.position );
               m_file.Read( const_cast<void*>( property.value.InternalBlockAddress() ), property.size );
            }
         }
         else
         {
            if ( i->HasData() || i->size != 0 )
               throw Error( "Internal error: Invalid block size for property '" + i->id + "'" );
         }
      }

      if ( m_xisfOptions.verbosity > 1 )
         m_console.WriteLn( "<end><cbr>Property '" + i->id + "' (" + PropertyTypeId( i->type ) + ") extracted." );

      return i->value;
   }

   /*
    * Read the current image and store it (possibly involving a pixel sample
    * data type conversion) in the specified ImageVariant.
    */
   void ReadImage( ImageVariant& image )
   {
      if ( image )
         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: ReadImage( static_cast<FImage&>( *image ) ); break;
            case 64: ReadImage( static_cast<DImage&>( *image ) ); break;
            }
         else if ( image.IsComplexSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: ReadImage( static_cast<ComplexImage&>( *image ) ); break;
            case 64: ReadImage( static_cast<DComplexImage&>( *image ) ); break;
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8: ReadImage( static_cast<UInt8Image&>( *image ) ); break;
            case 16: ReadImage( static_cast<UInt16Image&>( *image ) ); break;
            case 32: ReadImage( static_cast<UInt32Image&>( *image ) ); break;
            }
   }

   /*
    * Read the current image and store it (possibly involving a pixel sample
    * data type conversion) in the specified GenericImage<>.
    */
   template <class P>
   void ReadImage( GenericImage<P>& image )
   {
      CheckImageAccess();

      const pcl::ImageOptions& options = m_images[m_currentImage].options;
      if ( options.bitsPerSample != P::BitsPerSample() ||
           options.ieeefpSampleFormat && !options.complexSample != P::IsFloatSample() ||
           options.complexSample != P::IsComplexSample() )
      {
         ImageVariant tmp;
         tmp.CreateImage( options.ieeefpSampleFormat, options.complexSample, options.bitsPerSample );
         ReadImage( tmp );
         ImageVariant( &image ).CopyImage( tmp );
         return;
      }

      XISFInputImageBlock& block = m_images[m_currentImage].image;
      if ( !block.IsValid() )
         throw Error( "Internal error: invalid image block." );

      bool loadedCompressedData = false;
      if ( block.IsCompressed() )
      {
         if ( !block.HasData() )
         {
            block.data = ByteArray( size_type( block.size ) );
            m_file.SetPosition( block.position );
            m_file.Read( block.data.Begin(), block.size );
            loadedCompressedData = true;
         }

         UncompressBlock( block );
      }

      image.AllocateData( block.info.width, block.info.height, block.info.numberOfChannels, ColorSpace::value_type( block.info.colorSpace ) );
      if ( (block.HasData() ? block.data.Length() : size_type( block.size )) != image.ImageSize() )
         throw Error( "Internal error: invalid image block size." );

      for ( int i = 0; i < image.NumberOfChannels(); ++i )
         if ( block.HasData() )
         {
            // location="inline"
            ::memcpy( image[i], block.data.At( i*image.ChannelSize() ), image.ChannelSize() );
         }
         else
         {
            // location="attachment:<position>:<size>"
            m_file.SetPosition( block.position + i*image.ChannelSize() );
            m_file.Read( image[i], image.ChannelSize() );
         }

      if ( block.HasData() )
         if ( loadedCompressedData )
         {
            block.compressed = true;
            block.data.Clear();
         }

      if ( options.readNormalized )
         NormalizeImage( image, options );
   }

   /*
    * Read a block of contiguous pixel sample rows from the current image in
    * this input stream.
    */
   template <typename T, class P>
   void ReadSamples( T* buffer, int startRow, int rowCount, int channel, P* )
   {
      XISFInputImageBlock& block = m_images[m_currentImage].image;
      if ( !block.IsValid() )
         throw Error( "Internal error: invalid image block." );

      if ( block.IsCompressed() )
         if ( block.HasData() )
            UncompressBlock( block );
         else
            throw Error( "Cannot perform sequential/random disk read operations on a compressed data block." );

      const pcl::ImageOptions& options = m_images[m_currentImage].options;
      if ( options.complexSample )
         switch ( options.bitsPerSample )
         {
         case 32: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (ComplexPixelTraits*)0 ); break;
         case 64: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (DComplexPixelTraits*)0 ); break;
         }
      else if ( options.ieeefpSampleFormat )
         switch ( options.bitsPerSample )
         {
         case 32: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (FloatPixelTraits*)0 ); break;
         case 64: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (DoublePixelTraits*)0 ); break;
         }
      else
         switch ( options.bitsPerSample )
         {
         case  8: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (UInt8PixelTraits*)0 ); break;
         case 16: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (UInt16PixelTraits*)0 ); break;
         case 32: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (UInt32PixelTraits*)0 ); break;
         }
   }

   /*
    * Returns the list of warning messages generated during XML header
    * deserialization.
    */
   const StringList& Warnings() const
   {
      return m_warnings;
   }

private:

   /*
    * Stream data
    */
   XISFOptions       m_xisfOptions;    // format-specific options
   IsoString         m_hints;          // format hints (only used to generate metadata)
   mutable Console   m_console;        // for verbosity
   IsoString         m_path;           // path of the input file
   File              m_file;           // the input file
   fsize_type        m_fileSize;       // size in bytes of the input file
   fsize_type        m_headerLength;   // length in bytes of the XML file header
   fsize_type        m_minBlockPos;    // minimum valid absolute block position in bytes

   /*
    * Image data
    */
   struct ImageData
   {
      pcl::ImageOptions           options;    // image geometry and color space
      IsoString                   id;         // optional image identifier
      RGBColorSystem              rgbws;      // RGB working space
      FITSKeywordArray            keywords;   // legacy FITS header keywords
      XISFInputImageBlock         image;      // image data block
      XISFInputDataBlock          iccProfile; // ICC profile data block
      XISFInputImageBlock         thumbnail;  // image thumbnail data block
      XISFInputPropertyBlockArray properties; // image properties
      // ### TODO: metadata
   };

   typedef Array<ImageData> ImageDataArray;

   ImageDataArray    m_images;         // data of all readable images
   int               m_currentImage;   // index of the selected image

   XISFInputPropertyBlockArray
                     m_properties;     // global properties

   int               m_historyKeywordCount; // conversion of multiple HISTORY
   int               m_commentKeywordCount; //   and COMMENT FITS keywords

   StringList        m_warnings;       // XML warnings

   /*
    * Get the position and size of a block from the current XML element.
    */
   void GetBlockData( XISFInputDataBlock& block, QXmlStreamReader& xml )
   {
      block.position = 0;
      block.size = 0;
      block.data.Clear();

      GetBlockCompression( block, xml );

      IsoString s = IsoString( xml.attributes().value( "location" ).toString() ).Trimmed();
      if ( s.IsEmpty() )
         throw Error( "Missing block location attribute." );

      IsoStringList tokens;
      s.Break( tokens, ':', true/*trim*/ );

      if ( tokens[0] == "attachment" )
      {
         if ( tokens.Length() != 3 )
            throw Error( "Malformed attachment location attribute: '" + s + "'" );
         block.position = tokens[1].ToUInt64();
         if ( block.position < m_minBlockPos || block.position >= m_fileSize )
            throw Error( "Invalid block position: " + s );
         block.size = tokens[2].ToUInt64();
         if ( block.size == 0 || block.size + block.position > m_fileSize )
            throw Error( "Invalid block size: " + s );
      }
      else if ( tokens[0] == "inline" )
      {
         if ( tokens.Length() != 2 )
            throw Error( "Malformed inline location attribute: '" + s + "'" );
         IsoString encodedData = IsoString( xml.readElementText() ).Trimmed();
         tokens[1].ToLowerCase(); // encoding is case-insensitive
         if ( tokens[1] == "base64" )
            block.data = encodedData.FromBase64();
         else if ( tokens[1] == "hex" )
            block.data = encodedData.FromHex();
         else
            throw Error( "Invalid/unsupported block data encoding '" + tokens[1] + "'" );
         block.size = block.data.Length();

         if ( block.IsCompressed() )
            UncompressBlock( block );
      }
      else if ( tokens[0] == "embedded" )
      {
         if ( tokens.Length() != 1 )
            throw Error( "Malformed embedded location attribute: '" + s + "'" );
         // NB: Embedded data will be available in a child <Data> element.
      }
      else if ( tokens[0] == "url" )
      {
         if ( tokens.Length() != 2 )
            throw Error( "Malformed url location attribute: '" + s + "'" );
         throw Error( "URL block locations are not supported by this XISF implementation: '" + s + "'" );
         // ### TODO: Support location="url:..."
      }
      else
      {
         throw Error( "Invalid/unsupported block location: '" + s + "'" );
      }
   }

   void GetImageEmbeddedData( XISFInputDataBlock& block, QXmlStreamReader& xml )
   {
      GetBlockCompression( block, xml );

      IsoString encoding = IsoString( xml.attributes().value( "encoding" ).toString() ).Trimmed().LowerCase();
      if ( encoding.IsEmpty() )
      {
         encoding = "base64";
         Warning( xml, String( "Missing data encoding attribute; assuming base64 encoding by default." ) );
      }

      IsoString encodedData = IsoString( xml.readElementText() ).Trimmed();
      if ( encoding == "base64" )
         block.data = encodedData.FromBase64();
      else if ( encoding == "hex" )
         block.data = encodedData.FromHex();
      else
         throw Error( "Invalid/unsupported data encoding '" + encoding + "'" );
      block.size = block.data.Length();

      if ( block.IsCompressed() )
         UncompressBlock( block );
   }

   /*
    * Get block compression parameters: compression algorithm and uncompressed
    * data size in bytes.
    */
   void GetBlockCompression( XISFInputDataBlock& block, QXmlStreamReader& xml )
   {
      block.compressed = false;
      block.uncompressedSize = 0;

      IsoString s = IsoString( xml.attributes().value( "compression" ).toString() ).Trimmed();
      if ( !s.IsEmpty() )
      {
         IsoStringList tokens;
         s.Break( tokens, ':', true/*trim*/ );

         if ( tokens[0] == "zlib" )
         {
            if ( tokens.Length() != 2 )
               throw Error( "Malformed block compression attribute: '" + s + "'" );
            block.compressed = true;
            block.uncompressedSize = tokens[1].ToUInt64();
            if ( block.uncompressedSize == 0 || block.uncompressedSize >= uint32( -1 ) )
               throw Error( "Invalid uncompressed block size: " + s );

         }
         else
            throw Error( "Unknown data compression algorithm '" + tokens[0] + "'" );
      }
   }

   /*
    * Uncompress an input data block.
    */
   void UncompressBlock( XISFInputDataBlock& block )
   {
      if ( m_xisfOptions.verbosity > 0 )
      {
         m_console.Write( "<end><cbr>Uncompressing block: " + File::SizeAsString( block.size ) + " -> " );
         Module->ProcessEvents();
      }

      block.UncompressData();

      if ( m_xisfOptions.verbosity > 0 )
      {
         m_console.WriteLn( File::SizeAsString( block.data.Length() ) +
               String().Format( " (%.2f%%)", 100*double( block.data.Length() - block.size )/block.data.Length() ) );
         Module->ProcessEvents();
      }
   }

   /*
    * Add a new property block to the specified property list.
    */
   void AddPropertyBlock( XISFInputPropertyBlockArray& properties, const XISFInputPropertyBlock& property, QXmlStreamReader& xml )
   {
      XISFInputPropertyBlockArray::const_iterator i = properties.Search( property );
      if ( i == properties.End() )
         properties.Add( property );
      else
      {
         const_cast<XISFInputPropertyBlock&>( *i ) = property;
         Warning( xml, "Redefining property '" + property.id + "'" );
      }
   }

   /*
    * Deserialize an XISF property.
    */
   void GetProperty( XISFInputPropertyBlockArray& properties, QXmlStreamReader& xml, bool isInternal = false )
   {
      XISFInputPropertyBlock property;

      property.id = IsoString( xml.attributes().value( "id" ).toString() ).Trimmed();
      if ( property.id.IsEmpty() )
         throw Error( "Missing property id attribute." );
      if ( isInternal )
         property.id = InternalPropertyId( property.id );

      IsoString s = IsoString( xml.attributes().value( "type" ).toString() ).Trimmed();
      if ( s.IsEmpty() )
         throw Error( "Missing property type attribute." );
      property.type = PropertyTypeFromId( s );
      if ( property.type == VariantType::Invalid )
         throw Error( "Invalid/unsupported property type '" + s + "'" );

      if ( Variant::IsScalarType( property.type ) )
      {
         IsoString v = IsoString( xml.attributes().value( "value" ).toString() ).Trimmed();
         switch ( property.type )
         {
         case VariantType::Bool:
            property.value = Variant( v.ToBool() );
            break;
         case VariantType::Int8:
            property.value = Variant( int8( v.ToInt() ) );
            break;
         case VariantType::Int16:
            property.value = Variant( int16( v.ToInt() ) );
            break;
         case VariantType::Int32:
            property.value = Variant( int32( v.ToInt() ) );
            break;
         case VariantType::Int64:
            property.value = Variant( int64( v.ToInt64() ) );
            break;
         case VariantType::UInt8:
            property.value = Variant( uint8( v.ToUInt() ) );
            break;
         case VariantType::UInt16:
            property.value = Variant( uint16( v.ToUInt() ) );
            break;
         case VariantType::UInt32:
            property.value = Variant( uint32( v.ToUInt() ) );
            break;
         case VariantType::UInt64:
            property.value = Variant( uint64( v.ToUInt64() ) );
            break;
         case VariantType::Float:
            property.value = Variant( v.ToFloat() );
            break;
         case VariantType::Double:
            property.value = Variant( v.ToDouble() );
            break;
         default:
            throw Error( "Internal error: Invalid scalar property type '" + s + "'" );
         }
      }
      else
      {
         if ( property.type == VariantType::IsoString )
         {
            property.value = Variant( IsoString( String( xml.readElementText() ).ToUTF8() ) );
         }
         else if ( property.type == VariantType::String )
         {
            property.value = Variant( String( xml.readElementText() ) );
         }
         else if ( Variant::IsVectorType( property.type ) )
         {
            s = IsoString( xml.attributes().value( "length" ).toString() ).Trimmed();
            if ( s.IsEmpty() )
               throw Error( "Missing vector length attribute." );
            int length = s.ToInt();
            if ( length < 0 )
               throw Error( "Invalid vector length: " + s );
            property.dimensions.Append( length );

            GetBlockData( property, xml );
         }
         else if ( Variant::IsMatrixType( property.type ) )
         {
            s = IsoString( xml.attributes().value( "rows" ).toString() ).Trimmed();
            if ( s.IsEmpty() )
               throw Error( "Missing matrix rows attribute." );
            int n = s.ToInt();
            if ( n <= 0 )
               throw Error( "Invalid matrix dimension: " + s );
            property.dimensions.Append( n );

            s = IsoString( xml.attributes().value( "columns" ).toString() ).Trimmed();
            if ( s.IsEmpty() )
               throw Error( "Missing matrix columns attribute." );
            n = s.ToInt();
            if ( n <= 0 )
               throw Error( "Invalid matrix dimension: " + s );
            property.dimensions.Append( n );

            GetBlockData( property, xml );
         }
         else
            throw Error( "Internal error: Invalid property type '" + s + "'" );
      }

      AddPropertyBlock( properties, property, xml );
   }

   void GetFITSKeyword( XISFInputPropertyBlockArray& properties, FITSKeywordArray& keywords, QXmlStreamReader& xml )
   {
      IsoString name = IsoString( xml.attributes().value( "name" ).toString() ).Trimmed();
      IsoString value = IsoString( xml.attributes().value( "value" ).toString() ).Trimmed();
      IsoString comment = IsoString( xml.attributes().value( "comment" ).toString() ).Trimmed();
      FITSHeaderKeyword keyword( name, value, comment );
      keywords.Append( keyword );
      if ( m_xisfOptions.importFITSKeywords )
         GetPropertyFromFITSKeyword( properties, keyword, xml );
   }

   void GetPropertyFromFITSKeyword( XISFInputPropertyBlockArray& properties, const FITSHeaderKeyword& keyword, QXmlStreamReader& xml )
   {
      bool isString = keyword.IsString(); // do this *before* StripValueDelimiters()

      FITSHeaderKeyword k = keyword;
      k.StripValueDelimiters();
      IsoString kname = k.name.UpperCase(); // just in case!

      /*
       * Do not import irrelevant/mechanic FITS keywords.
       * This includes some of our reserved keywords.
       */
      if ( kname          == "SIMPLE"    ||
           kname          == "BITPIX"    ||
           kname.BeginsWith( "NAXIS" )   ||
           kname          == "EXTEND"    ||
           kname          == "NEXTEND"   ||
           kname          == "BSCALE"    ||
           kname          == "BZERO"     ||
           kname          == "PROGRAM"   ||
           kname          == "CREATOR"   ||
           kname          == "CONFIGUR"  ||
           kname          == "XTENSION"  ||
           kname          == "PCOUNT"    ||
           kname          == "GCOUNT"    ||
           kname          == "FILENAME"  ||
           kname          == "FILETYPE"  ||
           kname          == "ROOTNAME"  ||
           kname          == "HDUNAME"   ||
           kname          == "EXTNAME"   ||
           kname.BeginsWith( "TLMIN" )   ||
           kname.BeginsWith( "TLMAX" )   ||
           kname.BeginsWith( "TDMIN" )   ||
           kname.BeginsWith( "TDMAX" )   ||
           kname.BeginsWith( "TDBIN" )   ||
           kname          == "PINSIGHT"  ||
           kname          == "COLORSPC"  ||
           kname          == "ALPHACHN"  ||
           kname          == "RESOLUTN"  ||
           kname          == "XRESOLTN"  ||
           kname          == "YRESOLTN"  ||
           kname          == "RESOUNIT"  ||
           kname          == "ICCPROFL"  ||
           kname          == "THUMBIMG"  ||
           kname          == "XMPDATA" ) return;

      XISFInputPropertyBlock property;

      property.id = "FITS:" + kname;

      /*
       * HISTORY and COMMENT keywords are special in that they don't have a
       * value. The whole contents is in their comments.
       */
      if ( kname == "HISTORY" )
      {
         property.id.AppendFormat( "%03d", ++m_historyKeywordCount );
         property.type = VariantType::IsoString;
         property.value = k.comment;
      }
      else if ( kname == "COMMENT" )
      {
         property.id.AppendFormat( "%03d", ++m_commentKeywordCount );
         property.type = VariantType::IsoString;
         property.value = k.comment;
      }
      else
      {
         if ( k.IsNull() ) // empty value?
            return;

         if ( isString )
         {
            property.type = VariantType::IsoString;
            property.value = k.value;
         }
         else if ( k.IsBoolean() )
         {
            property.type = VariantType::Boolean;
            property.value = k.value == 'T';
         }
         else if ( k.IsNumeric() )
         {
            double value;
            if ( !k.GetNumericValue( value ) )
               return;
            property.type = VariantType::Double;
            property.value = value;
         }
         else  // !? else what?
            return;
      }

      AddPropertyBlock( properties, property, xml );
   }

   void GetImageGeometry( XISFInputImageBlock& image, QXmlStreamReader& xml )
   {
      IsoString s = IsoString( xml.attributes().value( "geometry" ).toString() ).Trimmed();
      if ( !s.IsEmpty() )
      {
         // geometry="<w>:<h>:<n>"
         IsoStringList tokens;
         s.Break( tokens, ':', true/*trim*/ );
         if ( tokens.Length() < 2 )
            throw Error( "Insufficient image geometry parameters: '" + s + "'" );
         if ( tokens.Length() < 3 )
            throw Error( "One-dimensional images are not supported by this implementation: '" + s + "'" );
         if ( tokens.Length() > 3 )
            throw Error( "This implementation only supports two-dimensional images: '" + s + "'" );
         image.info.width = tokens[0].ToInt();
         image.info.height = tokens[1].ToInt();
         image.info.numberOfChannels = tokens[2].ToInt();

         if ( xml.attributes().hasAttribute( "width" ) ||
              xml.attributes().hasAttribute( "height" ) ||
              xml.attributes().hasAttribute( "numberOfChannels" ) )
            Warning( xml, "Ignoring redundant (incorrect, actually) image geometry attributes." );
      }
      else
      {
         // width="<w>" height="<h>" numberOfChannels="<n>"
         s = IsoString( xml.attributes().value( "width" ).toString() ).Trimmed();
         if ( s.IsEmpty() )
            throw Error( "Missing image geometry or width attribute." );
         image.info.width = s.ToInt();
         s = IsoString( xml.attributes().value( "height" ).toString() ).Trimmed();
         if ( s.IsEmpty() )
            throw Error( "Missing image height attribute." );
         image.info.height = s.ToInt();
         s = IsoString( xml.attributes().value( "numberOfChannels" ).toString() ).Trimmed();
         if ( s.IsEmpty() )
            throw Error( "Missing image numberOfChannels attribute." );
         image.info.numberOfChannels = s.ToInt();
      }

      if ( image.info.width < 1 )
         throw Error( "Invalid image width: " + String( image.info.width ) );
      if ( image.info.height < 1 )
         throw Error( "Invalid image height: " + String( image.info.height ) );
      if ( image.info.numberOfChannels < 1 )
         throw Error( "Invalid number of image channels: " + String( image.info.numberOfChannels ) );
   }

   void GetRGBWS( RGBColorSystem& rgbws, QXmlStreamReader& xml )
   {
      float gamma, x[ 3 ], y[ 3 ], Y[ 3 ];
      bool sRGB;
      IsoString sgamma = IsoString( xml.attributes().value( "gamma" ).toString() ).Trimmed();
      IsoString sxr = IsoString( xml.attributes().value( "xr" ).toString() ).Trimmed();
      IsoString sxg = IsoString( xml.attributes().value( "xg" ).toString() ).Trimmed();
      IsoString sxb = IsoString( xml.attributes().value( "xb" ).toString() ).Trimmed();
      IsoString syr = IsoString( xml.attributes().value( "yr" ).toString() ).Trimmed();
      IsoString syg = IsoString( xml.attributes().value( "yg" ).toString() ).Trimmed();
      IsoString syb = IsoString( xml.attributes().value( "yb" ).toString() ).Trimmed();
      IsoString sYr = IsoString( xml.attributes().value( "Yr" ).toString() ).Trimmed();
      IsoString sYg = IsoString( xml.attributes().value( "Yg" ).toString() ).Trimmed();
      IsoString sYb = IsoString( xml.attributes().value( "Yb" ).toString() ).Trimmed();

      if ( sgamma.IsEmpty() ||
              sxr.IsEmpty() || sxg.IsEmpty() || sxb.IsEmpty() ||
              syr.IsEmpty() || syg.IsEmpty() || syb.IsEmpty() ||
              sYr.IsEmpty() || sYg.IsEmpty() || sYb.IsEmpty() )
         throw Error( "Missing required RGBWS parameter attribute(s)." );

      if ( sgamma.LowerCase() == "srgb" )
      {
         gamma = 2.2;
         sRGB = true;
      }
      else
      {
         gamma = sgamma.ToFloat();
         sRGB = false;
      }
      x[0] = sxr.ToFloat();
      x[1] = sxg.ToFloat();
      x[2] = sxb.ToFloat();
      y[0] = syr.ToFloat();
      y[1] = syg.ToFloat();
      y[2] = syb.ToFloat();
      Y[0] = sYr.ToFloat();
      Y[1] = sYg.ToFloat();
      Y[2] = sYb.ToFloat();

      rgbws = RGBColorSystem( gamma, sRGB, x, y, Y );
   }

   void GetImageResolution( pcl::ImageOptions& options, QXmlStreamReader& xml )
   {
      IsoString s = IsoString( xml.attributes().value( "horizontal" ).toString() ).Trimmed();
      if ( s.IsEmpty() )
         throw Error( "Missing horizontal resolution attribute." );
      options.xResolution = s.ToDouble();
      if ( options.xResolution <= 0 || !IsFinite( options.xResolution ) )
         throw Error( "Invalid horizontal resolution: " + s );

      s = IsoString( xml.attributes().value( "vertical" ).toString() ).Trimmed();
      if ( s.IsEmpty() )
         throw Error( "Missing vertical resolution attribute." );
      options.yResolution = s.ToDouble();
      if ( options.yResolution <= 0 || !IsFinite( options.yResolution ) )
         throw Error( "Invalid vertical resolution: " + s );

      s = IsoString( xml.attributes().value( "unit" ).toString() ).Trimmed();
      if ( !s.IsEmpty() )
      {
         IsoString u = s.LowerCase();
         if ( u == "cm" || u == "metric" )
            options.metricResolution = true;
         else if ( u == "inch" || u == "english" )
            options.metricResolution = false;
         else
            throw Error( "Unknown/unsupported resolution unit '" + s + "'" );
      }
   }

   void GetThumbnail( XISFInputImageBlock& thumbnail, QXmlStreamReader& xml )
   {
      GetImageGeometry( thumbnail, xml );

      IsoString s = IsoString( xml.attributes().value( "sampleFormat" ).toString() ).Trimmed();
      if ( !s.IsEmpty() )
         if ( s.LowerCase() != "uint8" )
            throw Error( "Invalid/unsupported thumbnail sampleFormat attribute. Should be UInt8 format." );

      s = IsoString( xml.attributes().value( "colorSpace" ).toString() ).Trimmed();
      if ( s.IsEmpty() )
         thumbnail.info.colorSpace = ColorSpace::Gray;
      else
      {
         thumbnail.info.colorSpace = ColorSpaceFromId( s );
         if ( thumbnail.info.colorSpace == ColorSpace::Unknown )
            throw Error( "Invalid/unknown thumbnail color space '" + s + "'" );
         if ( thumbnail.info.colorSpace != ColorSpace::RGB && thumbnail.info.colorSpace != ColorSpace::Gray )
            throw Error( "Unsupported thumbnail color space '" + s + "'. Should be either RGB or grayscale." );
      }

      GetBlockData( thumbnail, xml );

      while ( xml.readNextStartElement() )
      {
         if ( xml.name() == "Data" )
         {
            /*
             * Embedded image data (location="embedded").
             */
            if ( !thumbnail.IsEmpty() )
               Warning( xml, String( "Redefining embedded thumbnail data." ) );

            GetImageEmbeddedData( thumbnail, xml );
         }
         else
         {
            Warning( xml, String( "Skipping unknown \'" + xml.name().toString() + "\' Thumbnail element." ) );
         }

         if ( xml.isCharacters() )
            Warning( xml, String( "Ignoring unexpected thumbnail element contents." ) );

         if ( !xml.isEndElement() )
            xml.skipCurrentElement();
      }

      if ( thumbnail.info.width > XISF_THUMBNAIL_MAX || thumbnail.info.height > XISF_THUMBNAIL_MAX )
      {
         Warning( xml, String().Format( "Ignoring too big image thumbnail (%dx%d)", thumbnail.info.width, thumbnail.info.height ) );
         thumbnail.data.Clear();
         thumbnail.position = thumbnail.size = 0;
         thumbnail.info = ImageInfo();
      }
   }

   /*
    * Sequential/random read of a block of contiguous rows of pixel samples in
    * the native sample data type of the current image.
    */
   template <class P>
   void ReadSamples( typename P::sample* buffer, int startRow, int rowCount, int channel, P*, P* )
   {
      const XISFInputImageBlock& block = m_images[m_currentImage].image;

      size_type count = BlockSampleCount( rowCount );
      if ( block.HasData() )
      {
         // location="inline"
         ::memcpy( buffer, block.data.At( BlockSampleOffset( startRow, channel ) ), count*sizeof( *buffer ) );
      }
      else
      {
         // location="attachment:<position>:<size>"
         m_file.SetPosition( block.position + BlockSampleOffset( startRow, channel ) );
         m_file.Read( buffer, count*sizeof( *buffer ) );
      }

      if ( m_images[m_currentImage].options.readNormalized )
         NormalizeSamples( buffer, count, m_images[m_currentImage].options );
   }

   /*
    * Sequential/random read of a block of contiguous rows of pixel samples in
    * a foreign sample data type.
    */
   template <class O, class P>
   void ReadSamples( typename O::sample* buffer, int startRow, int rowCount, int channel, O*, P* )
   {
      const XISFInputImageBlock& block = m_images[m_currentImage].image;

      Array<typename P::sample> data( BlockSampleCount( rowCount ) );
      if ( block.HasData() )
      {
         // location="inline"
         ::memcpy( data.Begin(), block.data.At( BlockSampleOffset( startRow, channel ) ), data.Length()*sizeof( *data ) );
      }
      else
      {
         // location="attachment:<position>:<size>"
         m_file.SetPosition( block.position + BlockSampleOffset( startRow, channel ) );
         m_file.Read( data.Begin(), data.Length()*sizeof( *data ) );
      }

      if ( m_images[m_currentImage].options.readNormalized )
         NormalizeSamples( data.Begin(), data.Length(), m_images[m_currentImage].options );

      for ( typename Array<typename P::sample>::const_iterator i = data.Begin(); i != data.End(); ++i, ++buffer )
         P::FromSample( *buffer, *i );
   }

   /*
    * Normalization of floating point and integer real pixel samples.
    */

#define NORMALIZE_FLOAT_IMAGE( I )                                            \
      /*                                                                      \
       * Replace NaNs and infinities with minimum values.                     \
       * Truncate out-of-range sample values                                  \
       */                                                                     \
      for ( int c = 0; c < image.NumberOfChannels(); ++c )                    \
         for ( I::sample_iterator i( image, c ); i; ++i )                     \
            if ( !IsFinite( *i ) || *i < options.lowerRange )                 \
               *i = options.lowerRange;                                       \
            else if ( *i > options.upperRange )                               \
               *i = options.upperRange;                                       \
                                                                              \
      /*                                                                      \
       * Normalize to [0,1]                                                   \
       */                                                                     \
      if ( options.lowerRange != 0 || options.upperRange != 1 )               \
      {                                                                       \
         double range = options.upperRange - options.lowerRange;              \
         if ( 1 + range != 1 )                                                \
         {                                                                    \
            for ( int c = 0; c < image.NumberOfChannels(); ++c )              \
               for ( I::sample_iterator i( image, c ); i; ++i )               \
                  *i = (*i - options.lowerRange)/range;                       \
         }                                                                    \
         else if ( options.lowerRange < 0 || options.lowerRange > 1 )         \
         {                                                                    \
            double f = Range( options.lowerRange, 0.0, 1.0 );                 \
            for ( int c = 0; c < image.NumberOfChannels(); ++c )              \
               for ( I::sample_iterator i( image, c ); i; ++i )               \
                  *i = f;                                                     \
         }                                                                    \
      }

#define NORMALIZE_INTEGER_IMAGE( I )                                          \
      /*                                                                      \
       * Truncate out-of-range sample values                                  \
       */                                                                     \
      I::sample r0 = I::pixel_traits::FloatToSample( Ceil( options.lowerRange ) );  \
      I::sample r1 = I::pixel_traits::FloatToSample( Floor( options.upperRange ) ); \
      for ( int c = 0; c < image.NumberOfChannels(); ++c )                    \
         for ( I::sample_iterator i( image, c ); i; ++i )                     \
            if ( *i < r0 )                                                    \
               *i = r0;                                                       \
            else if ( *i > r1 )                                               \
               *i = r1;                                                       \
                                                                              \
      /*                                                                      \
       * Normalize to [0,I::pixel_traits::MaxSampleValue()]                   \
       */                                                                     \
      if ( options.lowerRange > 0 ||                                          \
           options.upperRange < I::pixel_traits::MaxSampleValue() )           \
      {                                                                       \
         double range = options.upperRange - options.lowerRange;              \
         if ( 1 + range != 1 )                                                \
         {                                                                    \
            double scale = I::pixel_traits::MaxSampleValue()/range;           \
            for ( int c = 0; c < image.NumberOfChannels(); ++c )              \
               for ( I::sample_iterator i( image, c ); i; ++i )               \
                  *i = (*i - options.lowerRange)*scale;                       \
         }                                                                    \
      }

   void NormalizeImage( Image& image, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_FLOAT_IMAGE( Image )
   }

   void NormalizeImage( DImage& image, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_FLOAT_IMAGE( DImage )
   }

   void NormalizeImage( ComplexImage& image, const pcl::ImageOptions& options ) const
   {
   }

   void NormalizeImage( DComplexImage& image, const pcl::ImageOptions& options ) const
   {
   }

   void NormalizeImage( UInt8Image& image, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_INTEGER_IMAGE( UInt8Image )
   }

   void NormalizeImage( UInt16Image& image, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_INTEGER_IMAGE( UInt16Image )
   }

   void NormalizeImage( UInt32Image& image, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_INTEGER_IMAGE( UInt32Image )
   }

#define NORMALIZE_FLOAT_SAMPLES( P )                                          \
      /*                                                                      \
       * Replace NaNs and infinities with minimum values.                     \
       * Truncate out-of-range sample values                                  \
       */                                                                     \
      for ( P::sample* i = buffer, * j = i + count; i < j; ++i )              \
         if ( !IsFinite( *i ) || *i < options.lowerRange )                    \
            *i = options.lowerRange;                                          \
         else if ( *i > options.upperRange )                                  \
            *i = options.upperRange;                                          \
                                                                              \
      /*                                                                      \
       * Normalize to [0,1]                                                   \
       */                                                                     \
      if ( options.lowerRange != 0 || options.upperRange != 1 )               \
      {                                                                       \
         double range = options.upperRange - options.lowerRange;              \
         if ( 1 + range != 1 )                                                \
         {                                                                    \
            for ( P::sample* i = buffer, * j = i + count; i < j; ++i )        \
               *i = (*i - options.lowerRange)/range;                          \
         }                                                                    \
         else if ( options.lowerRange < 0 || options.lowerRange > 1 )         \
         {                                                                    \
            double f = Range( options.lowerRange, 0.0, 1.0 );                 \
            for ( P::sample* i = buffer, * j = i + count; i < j; ++i )        \
               *i = f;                                                        \
         }                                                                    \
      }

#define NORMALIZE_INTEGER_SAMPLES( P )                                        \
      /*                                                                      \
       * Truncate out-of-range sample values                                  \
       */                                                                     \
      P::sample r0 = P::FloatToSample( Ceil( options.lowerRange ) );          \
      P::sample r1 = P::FloatToSample( Floor( options.upperRange ) );         \
      for ( P::sample* i = buffer, * j = i + count; i < j; ++i )              \
         if ( *i < r0 )                                                       \
            *i = r0;                                                          \
         else if ( *i > r1 )                                                  \
            *i = r1;                                                          \
                                                                              \
      /*                                                                      \
       * Normalize to [0,P::MaxSampleValue()]                                 \
       */                                                                     \
      if ( options.lowerRange > 0 || options.upperRange < P::MaxSampleValue() ) \
      {                                                                       \
         double range = options.upperRange - options.lowerRange;              \
         if ( 1 + range != 1 )                                                \
         {                                                                    \
            double scale = P::MaxSampleValue()/range;                         \
            for ( P::sample* i = buffer, * j = i + count; i < j; ++i )        \
               *i = (*i - options.lowerRange)*scale;                          \
         }                                                                    \
      }

   void NormalizeSamples( FloatPixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_FLOAT_SAMPLES( FloatPixelTraits )
   }

   void NormalizeSamples( DoublePixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_FLOAT_SAMPLES( DoublePixelTraits )
   }

   void NormalizeSamples( ComplexPixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
   }

   void NormalizeSamples( DComplexPixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
   }

   void NormalizeSamples( UInt8PixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_INTEGER_SAMPLES( UInt8PixelTraits )
   }

   void NormalizeSamples( UInt16PixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_INTEGER_SAMPLES( UInt16PixelTraits )
   }

   void NormalizeSamples( UInt32PixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_INTEGER_SAMPLES( UInt32PixelTraits )
   }

   /*
    * The absolute file position in bytes of the first pixel sample in the
    * specified row of a channel of the current image.
    */
   size_type BlockSampleOffset( size_type startRow, size_type channel ) const
   {
      const pcl::ImageOptions& options = m_images[m_currentImage].options;
      const XISFInputImageBlock& block = m_images[m_currentImage].image;
      return size_type( options.bitsPerSample >> 3 ) * (size_type( channel ) * BlockSampleCount( block.info.height ) + BlockSampleCount( startRow ));
   }

   /*
    * Total number of samples in a block of contiguous pixel rows in the
    * current image.
    */
   size_type BlockSampleCount( size_type rowCount ) const
   {
      const XISFInputImageBlock& block = m_images[m_currentImage].image;
      return rowCount * size_type( block.info.width );
   }

   /*
    * Size in bytes of a block of contiguous rows of pixel samples in the
    * current image.
    */
   size_type BlockSampleSize( size_type rowCount ) const
   {
      const pcl::ImageOptions& options = m_images[m_currentImage].options;
      return size_type( options.bitsPerSample >> 3 ) * BlockSampleCount( rowCount );
   }

   /*
    * Reset the state of the engine and destroy all internal data structures.
    * This includes closing the input file if it has been opened.
    */
   void Reset()
   {
      m_path.Clear();
      m_file.Close();
      m_fileSize = 0;
      m_headerLength = 0;
      m_minBlockPos = 0;
      m_images.Clear();
      m_currentImage = 0;
      m_properties.Clear();
      m_historyKeywordCount = 0;
      m_commentKeywordCount = 0;
      m_warnings.Clear();
   }

   /*
    * Add a new warning message during the XML header deserialization phase.
    */
   void Warning( const QXmlStreamReader& xml, const String& text )
   {
      if ( m_xisfOptions.noWarnings )
         return;
      int line = xml.lineNumber();
      int column = xml.columnNumber();
      String warning = text;
      if ( line > 0 && column > 0 )
         warning.AppendFormat( " (line=%d, column=%d)", line, column );
      else if ( line > 0 )
         warning.AppendFormat( " (line=%d)", line );
      if ( m_xisfOptions.warningsAreErrors )
         throw Error( warning );
      m_warnings.Append( warning );
   }

   /*
    * Ensure that the input file has been opened before reading an image.
    */
   void CheckImageAccess() const
   {
      if ( !IsOpen() )
         throw Error( "Internal error: Invalid image access." );
   }

   XISFReaderEngine( const XISFReaderEngine& ) = delete;
};

// ----------------------------------------------------------------------------

XISFReader::XISFReader() : m_engine( 0 ), m_options()
{
}

XISFReader::~XISFReader()
{
   Close();
}

void XISFReader::SetOptions( const XISFOptions& options )
{
   m_options = options;
   if ( IsOpen() )
      m_engine->SetOptions( m_options );
}

void XISFReader::SetHints( const IsoString& hints )
{
   m_hints = hints.Trimmed();
   if ( IsOpen() )
      m_engine->SetHints( m_hints );
}

bool XISFReader::IsOpen() const
{
   return m_engine != 0;
}

void XISFReader::Open( const String& filePath )
{
   CheckClosedStream( "Open" );
   m_engine = new XISFReaderEngine;
   m_engine->SetOptions( m_options );
   m_engine->SetHints( m_hints );
   m_engine->Open( filePath );
}

StringList XISFReader::Warnings() const
{
   return IsOpen() ? m_engine->Warnings() : StringList();
}

void XISFReader::Close()
{
   if ( IsOpen() )
   {
      m_engine->Close();
      delete m_engine, m_engine = 0;
   }
}

String XISFReader::FilePath() const
{
   return IsOpen() ? m_engine->FilePath() : String();
}

int XISFReader::NumberOfImages() const
{
   CheckOpenStream( "NumberOfImages" );
   return m_engine->NumberOfImages();
}

void XISFReader::SelectImage( int index )
{
   CheckOpenStream( "SelectImage" );
   m_engine->SelectImage( index );
}

int XISFReader::SelectedImageIndex() const
{
   return IsOpen() ? m_engine->SelectedImageIndex() : -1;
}

const pcl::ImageInfo& XISFReader::ImageInfo() const
{
   CheckOpenStream( "ImageInfo" );
   return m_engine->ImageInfo();
}

const pcl::ImageOptions& XISFReader::ImageOptions() const
{
   CheckOpenStream( "ImageOptions" );
   return m_engine->ImageOptions();
}

void XISFReader::SetImageOptions( const pcl::ImageOptions& options )
{
   CheckOpenStream( "SetImageOptions" );
   m_engine->SetImageOptions( options );
}

IsoString XISFReader::ImageId() const
{
   CheckOpenStream( "ImageId" );
   return m_engine->ImageId();
}

bool XISFReader::Extract( FITSKeywordArray& keywords )
{
   CheckOpenStream( "Extract" );
   keywords = m_engine->Keywords();
   return true;
}

bool XISFReader::Extract( ICCProfile& iccProfile )
{
   CheckOpenStream( "Extract" );
   iccProfile = m_engine->ICCProfile();
   return true;
}

bool XISFReader::Extract( ByteArray& data )
{
   CheckOpenStream( "Extract" );
   data = m_engine->Metadata();
   return true;
}

bool XISFReader::Extract( UInt8Image& thumbnail )
{
   CheckOpenStream( "Extract" );
   thumbnail = m_engine->Thumbnail();
   return true;
}

bool XISFReader::Extract( Variant& property, const IsoString& id )
{
   CheckOpenStream( "Extract" );
   property = m_engine->Property( id );
   return true;
}

ImagePropertyDescriptionArray XISFReader::Properties() const
{
   CheckOpenStream( "Properties" );
   return m_engine->Properties();
}

void XISFReader::ReadImage( FImage& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadImage( DImage& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadImage( ComplexImage& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadImage( DComplexImage& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadImage( UInt8Image& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadImage( UInt16Image& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadImage( UInt32Image& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadSamples( FImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (FloatPixelTraits*)0 );
}

void XISFReader::ReadSamples( DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (DoublePixelTraits*)0 );
}

void XISFReader::ReadSamples( ComplexImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (ComplexPixelTraits*)0 );
}

void XISFReader::ReadSamples( DComplexImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (DComplexPixelTraits*)0 );
}

void XISFReader::ReadSamples( UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (UInt8PixelTraits*)0 );
}

void XISFReader::ReadSamples( UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (UInt16PixelTraits*)0 );
}

void XISFReader::ReadSamples( UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (UInt32PixelTraits*)0 );
}

void XISFReader::CheckOpenStream( const char* memberFunction ) const
{
   if ( !IsOpen() )
      throw Error( "Invalid call to XISFReader::" + String( memberFunction ) + "() on a closed stream." );
}

void XISFReader::CheckClosedStream( const char* memberFunction ) const
{
   if ( IsOpen() )
      throw Error( "Invalid call to XISFReader::" + String( memberFunction ) + "() on an open stream." );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct XISFOutputBlock
{
   IsoString attachmentPos;
   bool      compressed : 1;
   ByteArray data;

   XISFOutputBlock() : attachmentPos(), compressed( false ), data()
   {
   }

   XISFOutputBlock( const ByteArray& a_data ) :
   attachmentPos( RandomAttachmentPos() ), compressed( false ), data( a_data )
   {
   }

   XISFOutputBlock( const void* blockData, size_type blockSize ) :
   attachmentPos( RandomAttachmentPos() ), compressed( false ),
   data( reinterpret_cast<const uint8*>( blockData ), reinterpret_cast<const uint8*>( blockData )+blockSize )
   {
   }

   XISFOutputBlock( const XISFOutputBlock& ) = default;
   XISFOutputBlock& operator =( const XISFOutputBlock& ) = default;

   bool HasData() const
   {
      return !data.IsEmpty();
   }

   bool IsCompressed() const
   {
      return compressed;
   }

   void Clear()
   {
      attachmentPos.Clear();
      data.Clear();
   }

   void CompressData( int level )
   {
      if ( HasData() )
      {
         unsigned long compressedSize = ::compressBound( uint32( data.Length() ) );
         ByteArray compressedData = ByteArray( size_type( compressedSize ) );
         int result = ::compress2( compressedData.Begin(), &compressedSize, data.Begin(), uint32( data.Length() ), level );
         if ( result != Z_OK )
            throw Error( "Failed to compress block data." );
         data = ByteArray( compressedData.Begin(), compressedData.At( compressedSize ) );
         compressed = true;
      }
   }

private:

   static IsoString RandomAttachmentPos()
   {
      static const char* C = "Aa9Bb0CcD8dEe1Ff7Gg2Hh6Ii3Jj4Kk5Ll4MmNn5OoP3pQq6Rr2Ss7TtU1uVv8Ww0Xx9YyZz";
      static const int N1 = ::strlen( C ) - 1;
      static XorShift1024* R = 0;
      if ( R == 0 )
         R = new XorShift1024;
      IsoString token;
      token.Reserve( 16 );
      for ( int i = 0; i < 16; ++i )
         token[i] = C[R->UIN( N1 )];
      token[16] = uint8( 0 );
      return "attachment:" + token;
   }
};

typedef Array<XISFOutputBlock> XISFOutputBlockArray;

// ----------------------------------------------------------------------------

struct XISFOutputProperty
{
   IsoString id;
   Variant   value;

   XISFOutputProperty() = default;
   XISFOutputProperty( const XISFOutputProperty& ) = default;

   XISFOutputProperty( const IsoString& a_id, const Variant& a_value = Variant() ) :
   id( a_id ), value( a_value )
   {
   }

   bool operator ==( const XISFOutputProperty& x ) const
   {
      return id == x.id;
   }

   bool operator <( const XISFOutputProperty& x ) const
   {
      return id < x.id;
   }
};

typedef SortedArray<XISFOutputProperty> XISFOutputPropertyArray;

// ----------------------------------------------------------------------------

class XISFWriterEngine : public XISFEngineBase
{
public:

   XISFWriterEngine() : XISFEngineBase(), m_xml( 0 )
   {
   }

   ~XISFWriterEngine()
   {
      DestroyXMLWriter();
   }

   /*
    * Set format-specific options.
    */
   void SetOptions( const XISFOptions& options )
   {
      m_xisfOptions = options;
   }

   /*
    * Set hints string for OutputHints metadata property generation.
    */
   void SetHints( const IsoString& hints )
   {
      m_hints = hints;
   }

   /*
    * Start a new XISF file.
    */
   void Open( const String& path )
   {
      Reset();

#ifdef __PCL_WINDOWS
      m_path = File::WindowsPathToUnix( path );
#else
      m_path = path;
#endif
      m_path.Trim();

      StartDocument();
   }

   /*
    * Flush and close the current output stream.
    */
   void Close()
   {
      if ( !IsOpen() )
         return;

      /*
       * Complete the XML document.
       */
      EndDocument();

      /*
       * Replace block position attributes.
       */
      for ( int n = 0, i = 0; ; ++i )
      {
         fpos_type pos = AlignedPosition( sizeof( XISFFileSignature ) + m_text.count() );
         for ( XISFOutputBlockArray::iterator i = m_blocks.Begin(); i < m_blocks.End(); ++i )
         {
            IsoString attachmentPos = IsoString().Format( "attachment:%llu", pos );
            m_text.replace( i->attachmentPos.c_str(), attachmentPos.c_str() );
            pos = AlignedPosition( pos + i->data.Length() );
            i->attachmentPos = attachmentPos;
         }
         if ( m_text.count() == n )
            break;
         n = m_text.count();
      }

      /*
       * Write the XISF file.
       */
      XISFFileSignature signature( m_text.count() );
      ByteArray zero( size_type( m_xisfOptions.blockAlignmentSize ), uint8( 0 ) );
      File f;
      f.CreateForWriting( m_path );
      f.Write( signature );
      f.Write( reinterpret_cast<const void*>( m_text.constData() ), m_text.count() );
      for ( XISFOutputBlockArray::const_iterator i = m_blocks.Begin(); i < m_blocks.End(); ++i )
      {
         fpos_type pos = f.Position();
         fpos_type pad = AlignedPosition( pos ) - pos;
         if ( pad )
            f.Write( zero.Begin(), pad );
         f.Write( i->data.Begin(), i->data.Length() );
      }
      f.Close();

      Reset();
   }

   bool IsOpen() const
   {
      return m_xml != 0;
   }

   String FilePath() const
   {
      return m_path;
   }

   /*
    * format-independent options set for the current image.
    */
   const pcl::ImageOptions& ImageOptions() const
   {
      return m_options;
   }

   /*
    * Set format-independent image parameters and options.
    */
   void SetImageOptions( const pcl::ImageOptions& options )
   {
      m_options = options;
   }

   /*
    * Set the identifier of the current image in this output stream.
    */
   void SetImageId( const IsoString& id )
   {
      m_id = id;
   }

   /*
    * Set the RGB working space of the current image in this output stream.
    */
   void SetRGBWorkingSpace( const RGBColorSystem& rgbws )
   {
      m_rgbws = rgbws;
   }

   /*
    * Embed a list of FITS header keywords in the current image of this output
    * stream.
    */
   void SetKeywords( const FITSKeywordArray& keywords )
   {
      m_keywords = keywords;
   }

   FITSKeywordArray EmbeddedKeywords() const
   {
      return m_keywords;
   }

   /*
    * Embed an ICC profile structure for the current image in this output
    * stream.
    */
   void SetICCProfile( const ICCProfile& iccProfile )
   {
      m_iccProfile = iccProfile;
   }

   ICCProfile EmbeddedICCProfile() const
   {
      return m_iccProfile;
   }

   /*
    * Embed a metadata block in the current image in this output stream.
    */
   void SetMetadata( const ByteArray& metadata )
   {
      // ### TODO
   }

   ByteArray EmbeddedMetadata() const
   {
      // ### TODO
      return ByteArray();
   }

   /*
    * Embed a thumbnail image for the current image in this output stream.
    */
   void SetThumbnail( const UInt8Image& thumbnail )
   {
      if ( thumbnail.Width() <= XISF_THUMBNAIL_MAX && thumbnail.Height() <= XISF_THUMBNAIL_MAX )
         m_thumbnail = thumbnail;
   }

   UInt8Image EmbeddedThumbnail() const
   {
      return m_thumbnail;
   }

   /*
    * Create a new image property, or change its value if the property already
    * exists in this output stream.
    */
   void SetProperty( const Variant& value, const IsoString& id )
   {
      if ( !IsInternalPropertyId( id ) )
      {
         XISFOutputPropertyArray::const_iterator i = m_properties.Search( id );
         if ( i == m_properties.End() )
            m_properties.Add( XISFOutputProperty( id, value ) );
         else
            const_cast<XISFOutputProperty&>( *i ).value = value;
         if ( m_xisfOptions.verbosity > 1 )
            m_console.WriteLn( "<end><cbr>Property '" + id + "' (" + PropertyTypeId( value.Type() ) + ") embedded." );
      }
   }

   /*
    * Generate a new Image element to store the specified image.
    */
   void WriteImage( const ImageVariant& image )
   {
      if ( image )
         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: WriteImage( static_cast<const FImage&>( *image ) ); break;
            case 64: WriteImage( static_cast<const DImage&>( *image ) ); break;
            }
         else if ( image.IsComplexSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: WriteImage( static_cast<const ComplexImage&>( *image ) ); break;
            case 64: WriteImage( static_cast<const DComplexImage&>( *image ) ); break;
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8: WriteImage( static_cast<const UInt8Image&>( *image ) ); break;
            case 16: WriteImage( static_cast<const UInt16Image&>( *image ) ); break;
            case 32: WriteImage( static_cast<const UInt32Image&>( *image ) ); break;
            }
   }

   /*
    * Generate a new Image element to store the specified image.
    */
   template <class P>
   void WriteImage( const GenericImage<P>& image )
   {
      if ( !IsOpen() )
         return;

      if ( m_options.bitsPerSample != P::BitsPerSample() ||
           m_options.ieeefpSampleFormat != P::IsFloatSample() ||
           m_options.complexSample != P::IsComplexSample() )
      {
         ImageVariant tmp;
         tmp.CreateImage( m_options.ieeefpSampleFormat,
                          m_options.complexSample,
                          m_options.bitsPerSample ).CopyImage( image );
         WriteImage( tmp );
         return;
      }

      if ( m_xisfOptions.verbosity > 0 )
      {
         m_console.WriteLn( "<end><cbr>Writing image" + (m_id.IsEmpty() ? String() : String( " '" + m_id + '\'' )) +
            String().Format( ": w=%d h=%d n=%d ",
                              image.Width(), image.Height(), image.NumberOfChannels() ) +
            ColorSpaceId( image.ColorSpace() ) + ' ' +
            SampleFormatId( P::BitsPerSample(), P::IsFloatSample(), P::IsComplexSample() ) );
         Module->ProcessEvents();
      }

      /*
       * ### NB: To support embedded storage (for very small images) and child
       * elements, serialization of an image consists of the following steps:
       *
       * 1. Open the Image element and prepare some internal structures
       *    (geometry, color space, etc.)
       *
       * 2. Call WriteImageAttributes() to generate all required attributes.
       *
       * 3. Call NewBlock() to generate the location attribute, and possibly
       *    emit embedded pixel data.
       *
       * 4. Call WriteImageElements() to generate all child Image elements.
       *
       * 5. Close the Image element.
       */

      StartElement( "Image" );

      m_rgbws = image.RGBWorkingSpace();

      m_info = ImageInfo( image );
      if ( image.FirstSelectedChannel() != 0 || image.LastSelectedChannel() < 2 )
         m_info.colorSpace = ColorSpace::Gray;

      WriteImageAttributes();
      NewBlock( image );
      WriteImageElements();

      EndElement(); // Image
   }

   /*
    * Create a new Image element with the specified image parameters, and
    * prepare for sequential/random write access.
    */
   void CreateImage( const pcl::ImageInfo& info )
   {
      if ( !IsOpen() )
         return;

      /*
       * ### See comment in the WriteImage() function above.
       */
      StartElement( "Image" );
      m_info = info;
      WriteImageAttributes();
      NewRandomAccessBlock();
      WriteImageElements();
      EndElement(); // Image
   }

   /*
    * Sequential/random write of pixel samples.
    */
   template <typename T, class P>
   void WriteSamples( const T* buffer, int startRow, int rowCount, int channel, P* )
   {
      if ( m_randomBlock.HasData() ) // CreateImage() should have been called before
         if ( m_options.complexSample )
            switch ( m_options.bitsPerSample )
            {
            case 32: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (ComplexPixelTraits*)0 ); break;
            case 64: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (DComplexPixelTraits*)0 ); break;
            }
         else if ( m_options.ieeefpSampleFormat )
            switch ( m_options.bitsPerSample )
            {
            case 32: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (FloatPixelTraits*)0 ); break;
            case 64: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (DoublePixelTraits*)0 ); break;
            }
         else
            switch ( m_options.bitsPerSample )
            {
            case  8: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (UInt8PixelTraits*)0 ); break;
            case 16: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (UInt16PixelTraits*)0 ); break;
            case 32: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (UInt32PixelTraits*)0 ); break;
            }
   }

private:

   /*
    * Stream data
    */
   XISFOptions             m_xisfOptions; // format-specific options
   IsoString               m_hints;       // format hints (only used to generate metadata)
   mutable Console         m_console;     // for verbosity
   QXmlStreamWriter*       m_xml;         // XML output stream
   IsoString               m_path;        // path of the current output file
   QByteArray              m_text;        // XML header

   /*
    * Image data
    */
   pcl::ImageOptions       m_options;     // format-independent options
   pcl::ImageInfo          m_info;        // geometry and color space data
   IsoString               m_id;          // optional image identifier
   RGBColorSystem          m_rgbws;       // RGB working space
   FITSKeywordArray        m_keywords;    // compatibility FITS header keywords
   ICCProfile              m_iccProfile;  // ICC profile
   UInt8Image              m_thumbnail;   // thumbnail image
   XISFOutputPropertyArray m_properties;  // image properties
   XISFOutputBlockArray    m_blocks;      // generic blobs
   XISFOutputBlock         m_randomBlock; // sequential/random access block
   // ### TODO: metadata

   /*
    * Reset the state of the engine and destroy all internal data structures
    */
   void Reset()
   {
      DestroyXMLWriter();
      m_path.Clear();
      m_text = QByteArray();
      m_info.Reset();
      m_id.Clear();
      m_keywords.Clear();
      m_iccProfile.Clear();
      m_thumbnail.FreeData();
      m_properties.Clear();
      m_blocks.Clear();
      m_randomBlock.Clear();
   }

   /*
    * Destroy the XML stream writer.
    */
   void DestroyXMLWriter()
   {
      if ( m_xml != 0 )
         delete m_xml, m_xml = 0;
   }

   /*
    * Start a new XML document.
    */
   void StartDocument()
   {
      DestroyXMLWriter();

      m_xml = new QXmlStreamWriter( &m_text );
      m_xml->setCodec( "UTF-8" );
      m_xml->setAutoFormatting( true );
      m_xml->setAutoFormattingIndent( 3 );

      m_xml->writeStartDocument();
      m_xml->writeComment( "\nExtensible Image Serialization Format - XISF version 1.0"
                           "\nCreated with PixInsight - http://pixinsight.com/"
                           "\n" );

      StartElement( "xisf" );
      SetElementAttribute( "version", "1.0" );
      SetElementAttribute( "xmlns",              "http://www.pixinsight.com" );
      SetElementAttribute( "xmlns:xsi",          "http://www.w3.org/2001/XMLSchema-instance" );
      SetElementAttribute( "xsi:schemaLocation", "http://www.pixinsight.com http://pixinsight.com/xsd/xisf-1.0.xsd" );
   }

   /*
    * Terminate an XML document.
    */
   void EndDocument()
   {
      /*
       * XISF metadata properties.
       */
      StartElement( "Metadata" );
      WriteProperty( "CreationTime", IsoString( QDateTime::currentDateTimeUtc().toString( Qt::ISODate ) ) );
      WriteProperty( "CreatorApplication", String( PixInsightVersion::AsString( false/*withCodename*/ ) ) );
      WriteProperty( "CreatorModule", String( Module->ReadableVersion() ) );
      WriteProperty( "CreatorOS", String(
#ifdef __PCL_FREEBSD
                           "FreeBSD"
#endif
#ifdef __PCL_LINUX
                           "Linux"
#endif
#ifdef __PCL_MACOSX
                           "OS X"
#endif
#ifdef __PCL_WINDOWS
                           "Windows"
#endif
                         ) );
      if ( m_xisfOptions.compressData )
      {
         WriteProperty( "CompressionMethod", IsoString( "zlib" ) );
         WriteProperty( "CompressionLevel", int32( m_xisfOptions.compressionLevel ) );
      }
      else
      {
         WriteProperty( "BlockAlignmentSize", m_xisfOptions.blockAlignmentSize );
         WriteProperty( "MaxInlineBlockSize", m_xisfOptions.maxInlineBlockSize );
      }

      if ( !m_hints.IsEmpty() )
         WriteProperty( "OutputHints", m_hints );

      EndElement();

      m_xml->writeEndDocument();
      DestroyXMLWriter();
   }

   /*
    * Start a new XML element.
    */
   void StartElement( const IsoString& m_id )
   {
      m_xml->writeStartElement( m_id );
   }

   /*
    * Write an XML element attribute.
    */
   void SetElementAttribute( const IsoString& m_id, const IsoString& value )
   {
      m_xml->writeAttribute( m_id, value );
   }

   /*
    * Write contents in the current XML element from UTF-8 source.
    */
   void WriteElementText( const IsoString& text )
   {
      m_xml->writeCharacters( String( text.UTF8ToUTF16() ) );
   }

   /*
    * Write contents in the current XML element from UTF-16 source.
    */
   void WriteElementText( const String& text )
   {
      m_xml->writeCharacters( text );
   }
   void WriteElementText( const QString& text )
   {
      m_xml->writeCharacters( text );
   }

   /*
    * Terminate an XML element.
    */
   void EndElement()
   {
      m_xml->writeEndElement();
   }

   /*
    * Generate a new output data block.
    *
    * For compressed data, write the required compression="<alg>:<usize>",
    * where <alg> is the compression algorithm and <usize> is the uncompressed
    * block size.
    *
    * For inline and embedded data, write the required
    * location="inline:<encoding>" or location="embedded" attributes and emit
    * base64 encoded data.
    *
    * For attached data, write the location="attachment:<pos>:<size>" attribute
    * and generate an offline block structure to be written upon stream
    * completion.
    */
   void NewBlock( const void* blockData, size_type blockSize, bool canInline = true )
   {
      XISFOutputBlock block( blockData, blockSize );

      if ( m_xisfOptions.compressData )
         if ( blockSize >= uint32( -1 ) ) // zlib works with 32-bit block lengths.
            m_console.WarningLn( "<end><cbr>** Warning: Cannot compress blocks larger than 4 GiB." );
         else
            CompressBlock( block );

      if ( block.data.Length() <= m_xisfOptions.maxInlineBlockSize )
      {
         /*
          * Generate inline or embedded data.
          */
         if ( canInline )
         {
            if ( block.IsCompressed() )
               SetElementAttribute( "compression", "zlib:" + IsoString( blockSize ) );
            SetElementAttribute( "location", "inline:base64" );
            WriteElementText( IsoString::ToBase64( block.data ) );
         }
         else
         {
            SetElementAttribute( "location", "embedded" );
            StartElement( "Data" );
            if ( block.IsCompressed() )
               SetElementAttribute( "compression", "zlib:" + IsoString( blockSize ) );
            SetElementAttribute( "encoding", "base64" );
            WriteElementText( IsoString::ToBase64( block.data ) );
            EndElement();
         }
      }
      else
      {
         /*
          * Prepare a new block for attachment.
          */
         if ( block.IsCompressed() )
            SetElementAttribute( "compression", "zlib:" + IsoString( blockSize ) );
         SetElementAttribute( "location", block.attachmentPos + ':' + IsoString( block.data.Length() ) );
         m_blocks.Append( block );
      }
   }

   /*
    * Generate a new image block.
    *
    * Image blocks can only be stored as attachments or as embedded data.
    * Inline image blocks are not feasible because the Image element has child
    * elements.
    */
   template <class P>
   void NewBlock( const GenericImage<P>& image )
   {
      ByteArray blockData;
      if ( image.IsFullSelection() )
      {
         for ( int i = image.FirstSelectedChannel(); i <= image.LastSelectedChannel(); ++i )
            blockData.Append( reinterpret_cast<const uint8*>( image[i] ),
                              reinterpret_cast<const uint8*>( image[i] ) + image.ChannelSize() );
      }
      else
      {
         Rect r = image.SelectedRectangle();
         size_type bytesPerRow = r.Width() * image.BytesPerSample();
         for ( int i = image.FirstSelectedChannel(); i <= image.LastSelectedChannel(); ++i )
         {
            const typename P::sample* p = image.PixelAddress( r.LeftTop(), i );
            for ( int j = r.y0; j < r.y1; ++j, p += image.Width() )
               blockData.Append( reinterpret_cast<const uint8*>( p ),
                                 reinterpret_cast<const uint8*>( p ) + bytesPerRow );
         }
      }

      NewBlock( blockData.Begin(), blockData.Length(), false/*canInline*/ );
   }

   /*
    * Generate a new data block for incremental/random image output.
    *
    * Random access blocks can only be stored as attachments in the current
    * implementation. This is a consequence of the sequential behavior of
    * QXmlStreamWriter.
    */
   void NewRandomAccessBlock()
   {
      m_randomBlock = XISFOutputBlock( ByteArray(
         size_type( m_info.width ) * m_info.height * m_info.numberOfChannels * (m_options.bitsPerSample >> 3) ) );
      SetElementAttribute( "location", m_randomBlock.attachmentPos + ':' + IsoString( m_randomBlock.data.Length() ) );
      if ( m_xisfOptions.compressData )
         m_console.WarningLn( "<end><cbr>** Warning: Random-access output blocks cannot be compressed." );
   }

   /*
    * Compress an output data block.
    */
   void CompressBlock( XISFOutputBlock& block )
   {
      size_type uncompressedSize = block.data.Length();
      if ( m_xisfOptions.verbosity > 0 )
      {
         m_console.Write( "<end><cbr>Compressing block: " + File::SizeAsString( uncompressedSize ) + " -> " );
         Module->ProcessEvents();
      }

      block.CompressData( m_xisfOptions.compressionLevel );

      if ( m_xisfOptions.verbosity > 0 )
      {
         m_console.WriteLn( File::SizeAsString( block.data.Length() ) +
               String().Format( " (%.2f%%)", 100*double( uncompressedSize - block.data.Length() )/uncompressedSize ) );
         Module->ProcessEvents();
      }
   }

   /*
    * Generate XML output for Image element attributes.
    */
   void WriteImageAttributes()
   {
      if ( !m_id.IsEmpty() )
         SetElementAttribute( "id", m_id );

      SetElementAttribute( "geometry", String().Format( "%d:%d:%d", m_info.width, m_info.height, m_info.numberOfChannels ) );
      SetElementAttribute( "sampleFormat", SampleFormatId( m_options.bitsPerSample, m_options.ieeefpSampleFormat, m_options.complexSample ) );

      if ( m_options.ieeefpSampleFormat )
      {
         if ( m_xisfOptions.outputUpperBound < m_xisfOptions.outputLowerBound )
            Swap( m_xisfOptions.outputLowerBound, m_xisfOptions.outputUpperBound );
         SetElementAttribute( "bounds", IsoString().Format( "%.16lg:%.16lg", m_xisfOptions.outputLowerBound, m_xisfOptions.outputUpperBound ) );
      }

      SetElementAttribute( "colorSpace", ColorSpaceId( m_info.colorSpace ) );

      if ( m_options.cfaType != CFAType::None )
         SetElementAttribute( "cfaType", CFATypeId( m_options.cfaType ) );
   }

   /*
    * Generate XML output for Image child elements.
    */
   void WriteImageElements()
   {
      if ( m_xisfOptions.storeFITSKeywords )
      {
         for ( FITSKeywordArray::const_iterator i = m_keywords.Begin(); i != m_keywords.End(); ++i )
         {
            StartElement( "FITSKeyword" );
            SetElementAttribute( "name", i->name );
            SetElementAttribute( "value", i->value );
            SetElementAttribute( "comment", i->comment );
            EndElement();
         }
         if ( m_xisfOptions.verbosity > 0 )
            if ( !m_keywords.IsEmpty() )
            {
               m_console.WriteLn( "<end><cbr>" + String( m_keywords.Length() ) + " FITS keyword(s) embedded." );
               Module->ProcessEvents();
            }
      }

      // if ( m_options.embedRGBWS ) ### TODO
         if ( m_rgbws != RGBColorSystem::sRGB )
         {
            StartElement( "RGBWS" );
            SetElementAttribute( "gamma", m_rgbws.IsSRGB() ? String( "sRGB" ) : String( m_rgbws.Gamma() ) );
            SetElementAttribute( "xr", String( m_rgbws.ChromaticityXCoordinates()[0] ) );
            SetElementAttribute( "xg", String( m_rgbws.ChromaticityXCoordinates()[1] ) );
            SetElementAttribute( "xb", String( m_rgbws.ChromaticityXCoordinates()[2] ) );
            SetElementAttribute( "yr", String( m_rgbws.ChromaticityYCoordinates()[0] ) );
            SetElementAttribute( "yg", String( m_rgbws.ChromaticityYCoordinates()[1] ) );
            SetElementAttribute( "yb", String( m_rgbws.ChromaticityYCoordinates()[2] ) );
            SetElementAttribute( "Yr", String( m_rgbws.LuminanceCoefficients()[0] ) );
            SetElementAttribute( "Yg", String( m_rgbws.LuminanceCoefficients()[1] ) );
            SetElementAttribute( "Yb", String( m_rgbws.LuminanceCoefficients()[2] ) );
            EndElement();
            if ( m_xisfOptions.verbosity > 0 )
            {
               m_console.WriteLn( "<end><cbr>RGBWS parameters embedded." );
               Module->ProcessEvents();
            }
         }

      // if ( m_options.embedResolution ) ### TODO
         if ( m_options.xResolution > 0 && m_options.yResolution > 0 )
         {
            StartElement( "Resolution" );
            SetElementAttribute( "horizontal", String( m_options.xResolution ) );
            SetElementAttribute( "vertical", String( m_options.yResolution ) );
            SetElementAttribute( "unit", m_options.metricResolution ? "cm" : "inch" );
            EndElement();
            if ( m_xisfOptions.verbosity > 1 )
            {
               m_console.WriteLn( "<end><cbr>Image resolution parameters embedded." );
               Module->ProcessEvents();
            }
         }

      /*
      if ( m_options.embedMetadata )
         ### TODO
      */

      if ( m_options.embedICCProfile )
         if ( m_iccProfile.IsProfile() )
         {
            StartElement( "ICCProfile" );
            NewBlock( reinterpret_cast<const void*>( m_iccProfile.ProfileData().Begin() ), m_iccProfile.ProfileSize() );
            EndElement();
            if ( m_xisfOptions.verbosity > 0 )
            {
               m_console.WriteLn( "<end><cbr>ICC profile embedded: \'" + m_iccProfile.Description() + "\', " + String( m_iccProfile.ProfileSize() ) + " bytes." );
               Module->ProcessEvents();
            }
         }

      if ( m_options.embedProperties )
      {
         for ( XISFOutputPropertyArray::const_iterator i = m_properties.Begin(); i != m_properties.End(); ++i )
            WriteProperty( *i );
         if ( m_xisfOptions.verbosity > 0 )
            if ( !m_properties.IsEmpty() )
            {
               m_console.WriteLn( "<end><cbr>" + String( m_properties.Length() ) + " image " +
                                  ((m_properties.Length() > 1) ? "properties" : "property") + " embedded." );
               Module->ProcessEvents();
            }
      }

      if ( m_options.embedThumbnail )
         if ( !m_thumbnail.IsEmpty() )
         {
            m_thumbnail.ResetSelections();
            StartElement( "Thumbnail" );
            SetElementAttribute( "geometry", String().Format( "%d:%d:%d", m_thumbnail.Width(), m_thumbnail.Height(), m_thumbnail.NumberOfChannels() ) );
            SetElementAttribute( "sampleFormat", "UInt8" );
            SetElementAttribute( "colorSpace", m_thumbnail.IsColor() ? "RGB" : "Gray" );
            NewBlock( m_thumbnail );
            EndElement();
            if ( m_xisfOptions.verbosity > 1 )
            {
               m_console.WriteLn( "<end><cbr>Image thumbnail embedded " +
                  String().Format( "(w=%d h=%d n=%d)", m_thumbnail.Width(), m_thumbnail.Height(), m_thumbnail.NumberOfChannels() ) );
               Module->ProcessEvents();
            }
         }
   }

   /*
    * Generate XML output for a Property element.
    */
   void WriteProperty( const IsoString& id, const Variant& value )
   {
      StartElement( "Property" );
      SetElementAttribute( "id", id );
      SetElementAttribute( "type", PropertyTypeId( value.Type() ) );

      if ( value.IsScalar() )
      {
         SetElementAttribute( "value", value.ToString() );
      }
      else if ( value.Type() == VariantType::IsoString )
      {
         WriteElementText( QString::fromUtf8( reinterpret_cast<const char*>( value.InternalBlockAddress() ) ) );
      }
      else if ( value.Type() == VariantType::String )
      {
         WriteElementText( QString::fromUtf16( reinterpret_cast<const uint16*>( value.InternalBlockAddress() ) ) );
      }
      else if ( value.IsVector() )
      {
         SetElementAttribute( "length", String( value.VectorLength() ) );
         NewBlock( value.InternalBlockAddress(), value.BlockSize() );
      }
      else if ( value.IsMatrix() )
      {
         Rect d = value.MatrixDimensions();
         SetElementAttribute( "rows", String( d.Height() ) );
         SetElementAttribute( "columns", String( d.Width() ) );
         NewBlock( value.InternalBlockAddress(), value.BlockSize() );
      }
      else
      {
         throw Error( "Internal error: Invalid property data type: " + String( value.Type() ) );
      }

      EndElement();
   }

   void WriteProperty( const XISFOutputProperty& property )
   {
      WriteProperty( property.id, property.value );
   }

   /*
    * Write samples from source data in a foreign sample data type.
    */
   template <class I, class P>
   void WriteSamples( const typename I::sample* buffer, int startRow, int rowCount, int channel, I*, P* )
   {
      typename P::sample* p = reinterpret_cast<typename P::sample*>( m_randomBlock.data.At( BlockSampleOffset( startRow, channel ) ) );
      size_type n = BlockSampleCount( rowCount );
      for ( size_type i = 0; i < n; ++i, ++p, ++buffer )
         *p = P::ToSample( *buffer );
   }

   /*
    * Write samples from source in the native pixel sample data type.
    */
   template <class P>
   void WriteSamples( const typename P::sample* buffer, int startRow, int rowCount, int channel, P*, P* )
   {
      ::memcpy( m_randomBlock.data.At( BlockSampleOffset( startRow, channel ) ), buffer, BlockSampleSize( rowCount ) );
   }

   /*
    * Byte offset of the first pixel sample in a given row of a given channel
    * of the current image.
    */
   size_type BlockSampleOffset( size_type row, size_type channel ) const
   {
      return size_type( m_options.bitsPerSample >> 3 ) * (size_type( channel ) * BlockSampleCount( m_info.height ) + BlockSampleCount( row ));
   }

   /*
    * Number of samples in a block of rowCount contiguous pixel sample rows of
    * the current image.
    */
   size_type BlockSampleCount( size_type rowCount ) const
   {
      return rowCount * size_type( m_info.width );
   }

   /*
    * Size in bytes of a block of rowCount contiguous pixel sample rows of the
    * current image.
    */
   size_type BlockSampleSize( size_type rowCount ) const
   {
      return size_type( m_options.bitsPerSample >> 3 ) * BlockSampleCount( rowCount );
   }

   /*
    * Given an absolute file position upos in bytes, compute the next aligned
    * position >= upos.
    */
   fpos_type AlignedPosition( fpos_type upos ) const
   {
      if ( m_xisfOptions.compressData || m_xisfOptions.blockAlignmentSize < 2 )
         return upos;
      fpos_type apos = m_xisfOptions.blockAlignmentSize * (upos/m_xisfOptions.blockAlignmentSize);
      if ( apos < upos )
         apos += m_xisfOptions.blockAlignmentSize;
      return apos;
   }
};

// ----------------------------------------------------------------------------

XISFWriter::XISFWriter() : m_engine( 0 ), m_options()
{
}

XISFWriter::~XISFWriter()
{
   Close();
}

void XISFWriter::SetOptions( const XISFOptions& options )
{
   m_options = options;
   if ( IsOpen() )
      m_engine->SetOptions( m_options );
}

void XISFWriter::SetHints( const IsoString& hints )
{
   m_hints = hints.Trimmed();
   if ( IsOpen() )
      m_engine->SetHints( m_hints );
}

bool XISFWriter::IsOpen() const
{
   return m_engine != 0;
}

void XISFWriter::Create( const String& filePath, int/*count*/ )
{
   CheckClosedStream( "Create" );
   m_engine = new XISFWriterEngine;
   m_engine->SetOptions( m_options );
   m_engine->SetHints( m_hints );
   m_engine->Open( filePath );
}

void XISFWriter::Close()
{
   if ( IsOpen() )
   {
      m_engine->Close();
      delete m_engine, m_engine = 0;
   }
}

String XISFWriter::FilePath() const
{
   return IsOpen() ? m_engine->FilePath() : String();
}

const pcl::ImageOptions& XISFWriter::ImageOptions() const
{
   CheckOpenStream( "ImageOptions" );
   return m_engine->ImageOptions();
}

void XISFWriter::SetImageOptions( const pcl::ImageOptions& options )
{
   CheckOpenStream( "SetImageOptions" );
   m_engine->SetImageOptions( options );
}

void XISFWriter::SetImageId( const IsoString& id )
{
   CheckOpenStream( "SetImageId" );
   m_engine->SetImageId( id );
}

void XISFWriter::Embed( const FITSKeywordArray& keywords )
{
   CheckOpenStream( "Embed" );
   m_engine->SetKeywords( keywords );
}

FITSKeywordArray XISFWriter::EmbeddedKeywords() const
{
   return IsOpen() ? m_engine->EmbeddedKeywords() : FITSKeywordArray();
}

void XISFWriter::Embed( const ICCProfile& iccProfile )
{
   CheckOpenStream( "Embed" );
   m_engine->SetICCProfile( iccProfile );
}

ICCProfile XISFWriter::EmbeddedICCProfile() const
{
   return IsOpen() ? m_engine->EmbeddedICCProfile() : ICCProfile();
}

void XISFWriter::Embed( const ByteArray& metadata )
{
   CheckOpenStream( "Embed" );
   m_engine->SetMetadata( metadata );
}

ByteArray XISFWriter::EmbeddedMetadata() const
{
   return IsOpen() ? m_engine->EmbeddedMetadata() : ByteArray();
}

void XISFWriter::Embed( const UInt8Image& thumbnail )
{
   CheckOpenStream( "Embed" );
   m_engine->SetThumbnail( thumbnail );
}

UInt8Image XISFWriter::EmbeddedThumbnail() const
{
   return IsOpen() ? m_engine->EmbeddedThumbnail() : UInt8Image();
}

void XISFWriter::Embed( const Variant& value, const IsoString& id )
{
   CheckOpenStream( "Embed" );
   m_engine->SetProperty( value, id );
}

void XISFWriter::WriteImage( const Image& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::WriteImage( const DImage& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::WriteImage( const ComplexImage& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::WriteImage( const DComplexImage& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::WriteImage( const UInt8Image& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::WriteImage( const UInt16Image& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::WriteImage( const UInt32Image& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::CreateImage( const pcl::ImageInfo& info )
{
   CheckOpenStream( "CreateImage" );
   m_engine->CreateImage( info );
}

void XISFWriter::WriteSamples( const FImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (FloatPixelTraits*)0 );
}

void XISFWriter::WriteSamples( const DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (DoublePixelTraits*)0 );
}

void XISFWriter::WriteSamples( const ComplexImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (ComplexPixelTraits*)0 );
}

void XISFWriter::WriteSamples( const DComplexImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (DComplexPixelTraits*)0 );
}

void XISFWriter::WriteSamples( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (UInt8PixelTraits*)0 );
}

void XISFWriter::WriteSamples( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (UInt16PixelTraits*)0 );
}

void XISFWriter::WriteSamples( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (UInt32PixelTraits*)0 );
}

void XISFWriter::CheckOpenStream( const char* memberFunction ) const
{
   if ( !IsOpen() )
      throw Error( "Invalid call to XISFWriter::" + String( memberFunction ) + "() on a closed stream." );
}

void XISFWriter::CheckClosedStream( const char* memberFunction ) const
{
   if ( IsOpen() )
      throw Error( "Invalid call to XISFWriter::" + String( memberFunction ) + "() on an open stream." );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF XISF.cpp - Released 2014/11/30 10:38:10 UTC
