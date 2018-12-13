//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/XISF.cpp - Released 2018-12-12T09:24:30Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/Atomic.h>
#include <pcl/AutoLock.h>
#include <pcl/Compression.h>
#include <pcl/Cryptography.h>
#include <pcl/PixelTraits.h> // PTLUT
#include <pcl/XISF.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#ifdef __clang__

const fsize_type XISF::DefaultBlockAlignSize = 4096;
const fsize_type XISF::DefaultMaxBlockInlineSize = 3072;
const int XISF::MaxThumbnailSize = 1024;
const XISF::block_checksum XISF::DefaultChecksum = XISFChecksum::None;
const XISF::block_compression XISF::DefaultCompression = XISFCompression::None;
const int XISF::DefaultCompressionLevel = 0;
const int XISF::MaxCompressionLevel = 100;
const int XISF::DefaultVerbosity = 1;
const double XISF::DefaultOutputLowerBound = 0.0;
const double XISF::DefaultOutputUpperBound = 1.0;
const bool XISF::DefaultStoreFITSKeywords = true;
const bool XISF::DefaultIgnoreFITSKeywords = false;
const bool XISF::DefaultImportFITSKeywords = false;
const bool XISF::DefaultIgnoreEmbeddedData = false;
const bool XISF::DefaultIgnoreProperties = false;
const bool XISF::DefaultAutoMetadata = true;
const bool XISF::DefaultNoWarnings = false;
const bool XISF::DefaultWarningsAreErrors = false;
const char* XISF::InternalNamespacePrefix = "XISF:";

#endif

// ----------------------------------------------------------------------------

void XISFFileSignature::Validate() const
{
   if ( magic[0] != 'X' || magic[1] != 'I' || magic[2] != 'S' || magic[3] != 'F' )
      throw Error( "Not a monolithic XISF file." );
   if ( magic[4] != '0' || magic[5] != '1' || magic[6] != '0' || magic[7] != '0' )
      throw Error( "Not an XISF version 1.0 file." );
   if ( headerLength < 65 ) // minimum length of an empty XISF header, from "<?xml..." to </xisf>
      throw Error( "Invalid or corrupted XISF file." );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

const char* XISF::SampleFormatId( int bitsPerSample, bool floatSample, bool complexSample )
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

   return nullptr;
}

// ----------------------------------------------------------------------------

bool XISF::GetSampleFormatFromId( int& bitsPerSample, bool& floatSample, bool& complexSample, const String& id )
{
   String format = id.CaseFolded();
   if ( format == "float32" )
   {
      bitsPerSample = 32;
      floatSample = true;
      complexSample = false;
   }
   else if ( format == "uint16" )
   {
      bitsPerSample = 16;
      floatSample = false;
      complexSample = false;
   }
   else if ( format == "uint8" )
   {
      bitsPerSample = 8;
      floatSample = false;
      complexSample = false;
   }
   else if ( format == "float64" )
   {
      bitsPerSample = 64;
      floatSample = true;
      complexSample = false;
   }
   else if ( format == "uint32" )
   {
      bitsPerSample = 32;
      floatSample = false;
      complexSample = false;
   }
   else if ( format == "complex32" )
   {
      bitsPerSample = 32;
      floatSample = true;
      complexSample = true;
   }
   else if ( format == "complex64" )
   {
      bitsPerSample = 64;
      floatSample = true;
      complexSample = true;
   }
   else if ( format == "uint64" )
   {
      bitsPerSample = 64;
      floatSample = false;
      complexSample = false;
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

const char* XISF::ColorSpaceId( XISF::color_space colorSpace )
{
   switch ( colorSpace )
   {
   case ColorSpace::Gray:
      return "Gray";
   case ColorSpace::RGB:
      return "RGB";
   case ColorSpace::CIELab:
      return "CIELab";
   default:
      return nullptr;
   }
}

// ----------------------------------------------------------------------------

XISF::color_space XISF::ColorSpaceFromId( const String& id )
{
   String colorSpace = id.CaseFolded();
   if ( colorSpace == "gray" )
      return ColorSpace::Gray;
   if ( colorSpace == "rgb" )
      return ColorSpace::RGB;
   if ( colorSpace == "cielab" )
      return ColorSpace::CIELab;
   return ColorSpace::Unknown;
}

// ----------------------------------------------------------------------------

const char* XISF::PropertyTypeId( XISF::property_type type )
{
   switch ( type )
   {
   case VariantType::Boolean:    return "Boolean";
   case VariantType::Int8:       return "Int8";
   case VariantType::Int16:      return "Int16";
   case VariantType::Int32:      return "Int32";
   case VariantType::Int64:      return "Int64";
   case VariantType::UInt8:      return "UInt8";
   case VariantType::UInt16:     return "UInt16";
   case VariantType::UInt32:     return "UInt32";
   case VariantType::UInt64:     return "UInt64";
   case VariantType::Float32:    return "Float32";
   case VariantType::Float64:    return "Float64";

   case VariantType::Complex32:  return "Complex32";
   case VariantType::Complex64:  return "Complex64";

   case VariantType::I8Vector:   return "I8Vector";
   case VariantType::UI8Vector:  return "UI8Vector";
   case VariantType::I16Vector:  return "I16Vector";
   case VariantType::UI16Vector: return "UI16Vector";
   case VariantType::I32Vector:  return "I32Vector";
   case VariantType::UI32Vector: return "UI32Vector";
   case VariantType::I64Vector:  return "I64Vector";
   case VariantType::UI64Vector: return "UI64Vector";
   case VariantType::F32Vector:  return "F32Vector";
   case VariantType::F64Vector:  return "F64Vector";
   case VariantType::C32Vector:  return "C32Vector";
   case VariantType::C64Vector:  return "C64Vector";

   case VariantType::I8Matrix:   return "I8Matrix";
   case VariantType::UI8Matrix:  return "UI8Matrix";
   case VariantType::I16Matrix:  return "I16Matrix";
   case VariantType::UI16Matrix: return "UI16Matrix";
   case VariantType::I32Matrix:  return "I32Matrix";
   case VariantType::UI32Matrix: return "UI32Matrix";
   case VariantType::I64Matrix:  return "I64Matrix";
   case VariantType::UI64Matrix: return "UI64Matrix";
   case VariantType::F32Matrix:  return "F32Matrix";
   case VariantType::F64Matrix:  return "F64Matrix";
   case VariantType::C32Matrix:  return "C32Matrix";
   case VariantType::C64Matrix:  return "C64Matrix";

   case VariantType::String:     return "String";
   case VariantType::IsoString:  return "String";

   case VariantType::TimePoint:  return "TimePoint";

   default:                      return nullptr;
   }
}

// ----------------------------------------------------------------------------

XISF::property_type XISF::PropertyTypeFromId( const String& id )
{
   String type = id.CaseFolded();

   if ( type == "boolean" )
      return VariantType::Boolean;
   if ( type == "int8" )
      return VariantType::Int8;
   if ( type == "int16" || type == "short" )
      return VariantType::Int16;
   if ( type == "int32" || type == "int" )
      return VariantType::Int32;
   if ( type == "int64" )
      return VariantType::Int64;
   if ( type == "uint8" || type == "byte" )
      return VariantType::UInt8;
   if ( type == "uint16" || type == "ushort" )
      return VariantType::UInt16;
   if ( type == "uint32" || type == "uint" )
      return VariantType::UInt32;
   if ( type == "uint64" )
      return VariantType::UInt64;
   if ( type == "float32" || type == "float" )
      return VariantType::Float;
   if ( type == "float64" || type == "double" )
      return VariantType::Double;

   if ( type == "complex32" )
      return VariantType::Complex32;
   if ( type == "complex64" || type == "complex" )
      return VariantType::Complex64;

   if ( type == "i8vector" )
      return VariantType::I8Vector;
   if ( type == "ui8vector" || type == "bytearray" || type == "bytevector" )
      return VariantType::UI8Vector;
   if ( type == "i16vector" )
      return VariantType::I16Vector;
   if ( type == "ui16vector" )
      return VariantType::UI16Vector;
   if ( type == "i32vector" || type == "ivector" )
      return VariantType::I32Vector;
   if ( type == "ui32vector" || type == "uivector" )
      return VariantType::UI32Vector;
   if ( type == "i64vector" )
      return VariantType::I64Vector;
   if ( type == "ui64vector" )
      return VariantType::UI64Vector;
   if ( type == "f32vector" )
      return VariantType::F32Vector;
   if ( type == "f64vector" || type == "vector" )
      return VariantType::F64Vector;
   if ( type == "c32vector" )
      return VariantType::C32Vector;
   if ( type == "c64vector" )
      return VariantType::C64Vector;

   if ( type == "i8matrix" )
      return VariantType::I8Matrix;
   if ( type == "ui8matrix" || type == "bytematrix" )
      return VariantType::UI8Matrix;
   if ( type == "i16matrix" )
      return VariantType::I16Matrix;
   if ( type == "ui16matrix" )
      return VariantType::UI16Matrix;
   if ( type == "i32matrix" || type == "imatrix" )
      return VariantType::I32Matrix;
   if ( type == "ui32matrix" || type == "uimatrix" )
      return VariantType::UI32Matrix;
   if ( type == "i64matrix" )
      return VariantType::I64Matrix;
   if ( type == "ui64matrix" )
      return VariantType::UI64Matrix;
   if ( type == "f32matrix" )
      return VariantType::F32Matrix;
   if ( type == "f64matrix" || type == "matrix" )
      return VariantType::F64Matrix;
   if ( type == "c32matrix" )
      return VariantType::C32Matrix;
   if ( type == "c64matrix" )
      return VariantType::C64Matrix;

   if ( type == "string" || type == "string8" )
      return VariantType::IsoString;
   if ( type == "string16" )
      return VariantType::String;

   if ( type == "timepoint" )
      return VariantType::TimePoint;

   return VariantType::Invalid;
}

// ----------------------------------------------------------------------------

const char* XISF::CompressionCodecId( XISF::block_compression codec )
{
   switch ( codec )
   {
   case XISFCompression::Zlib:
      return "zlib";
   case XISFCompression::LZ4:
      return "lz4";
   case XISFCompression::LZ4HC:
      return "lz4hc";
   case XISFCompression::Zlib_Sh:
      return "zlib+sh";
   case XISFCompression::LZ4_Sh:
      return "lz4+sh";
   case XISFCompression::LZ4HC_Sh:
      return "lz4hc+sh";
   default:
   case XISFCompression::None:
      return "";
   }
}

// ----------------------------------------------------------------------------

XISF::block_compression XISF::CompressionCodecFromId( const String& id )
{
   if ( id.IsEmpty() )
      return XISFCompression::None;
   String codec = id.CaseFolded();
   if ( codec == "zlib" )
      return XISFCompression::Zlib;
   if ( codec == "lz4" )
      return XISFCompression::LZ4;
   if ( codec == "lz4hc" )
      return XISFCompression::LZ4HC;
   if ( codec == "zlib+sh" )
      return XISFCompression::Zlib_Sh;
   if ( codec == "lz4+sh" )
      return XISFCompression::LZ4_Sh;
   if ( codec == "lz4hc+sh" )
      return XISFCompression::LZ4HC_Sh;
   return XISFCompression::Unknown;
}

// ----------------------------------------------------------------------------

Compression* XISF::NewCompression( XISF::block_compression codec, size_type itemSize )
{
   Compression* compressor = nullptr;
   switch ( codec )
   {
   case XISFCompression::Zlib:
   case XISFCompression::Zlib_Sh:
      compressor = new ZLibCompression;
      break;
   case XISFCompression::LZ4:
   case XISFCompression::LZ4_Sh:
      compressor = new LZ4Compression;
      break;
   case XISFCompression::LZ4HC:
   case XISFCompression::LZ4HC_Sh:
      compressor = new LZ4HCCompression;
      break;
   default: // ?!
   case XISFCompression::None:
      throw Error( "XISF::NewCompression(): "
         + String().Format( "Internal error: Invalid compression codec 0x%02x", codec ) );
   }
   switch ( codec )
   {
   case XISFCompression::Zlib_Sh:
   case XISFCompression::LZ4_Sh:
   case XISFCompression::LZ4HC_Sh:
      if ( itemSize > 1 )
      {
         compressor->EnableByteShuffling();
         compressor->SetItemSize( itemSize );
         break;
      }
      // else fall through
   default:
      compressor->DisableByteShuffling();
      break;
   }
   compressor->DisableChecksums();
   return compressor;
}

// ----------------------------------------------------------------------------

int XISF::CompressionLevelForMethod( XISF::block_compression codec, int level )
{
   level = Range( level, 0, MaxCompressionLevel );
   int maxLevel;
   switch ( codec )
   {
   default: // ?!
   case XISFCompression::None:
      return 0;
   case XISFCompression::Zlib:
   case XISFCompression::Zlib_Sh:
      if ( level == 0 )
         return 6;
      maxLevel = 9;
      break;
   case XISFCompression::LZ4:
   case XISFCompression::LZ4_Sh:
      if ( level == 0 )
         return 64;
      maxLevel = 64;
      break;
   case XISFCompression::LZ4HC:
   case XISFCompression::LZ4HC_Sh:
      if ( level == 0 )
         return 9;
      maxLevel = 16;
      break;
   }
   return Max( 1, RoundInt( double( level )/MaxCompressionLevel * maxLevel ) );
}

// ----------------------------------------------------------------------------

bool XISF::CompressionUsesByteShuffle( XISF::block_compression codec )
{
   switch ( codec )
   {
   case XISFCompression::Zlib_Sh:
   case XISFCompression::LZ4_Sh:
   case XISFCompression::LZ4HC_Sh:
      return true;
   default:
      return false;
   }
}

// ----------------------------------------------------------------------------

XISF::block_compression XISF::CompressionCodecNoShuffle( XISF::block_compression codec )
{
   switch ( codec )
   {
   case XISFCompression::Zlib_Sh:
      return XISFCompression::Zlib;
   case XISFCompression::LZ4_Sh:
      return XISFCompression::LZ4;
   case XISFCompression::LZ4HC_Sh:
      return XISFCompression::LZ4HC;
   default:
      return codec;
   }
}

// ----------------------------------------------------------------------------

bool XISF::CompressionNeedsItemSize( XISF::block_compression codec )
{
   return CompressionUsesByteShuffle( codec );
}

// ----------------------------------------------------------------------------

const char* XISF::ChecksumAlgorithmId( XISF::block_checksum algorithm )
{
   switch ( algorithm )
   {
   case XISFChecksum::SHA1:
      return "sha1";
   case XISFChecksum::SHA256:
      return "sha256";
   case XISFChecksum::SHA512:
      return "sha512";
   default:
   case XISFChecksum::None:
      return "";
   }
}

// ----------------------------------------------------------------------------

XISF::block_checksum XISF::ChecksumAlgorithmFromId( const String& id )
{
   if ( id.IsEmpty() )
      return XISFChecksum::None;
   String algorithm = id.CaseFolded();
   if ( algorithm == "sha1" || algorithm == "sha-1" )
      return XISFChecksum::SHA1;
   if ( algorithm == "sha256" || algorithm == "sha-256" )
      return XISFChecksum::SHA256;
   if ( algorithm == "sha512" || algorithm == "sha-512" )
      return XISFChecksum::SHA512;
   return XISFChecksum::Unknown;
}

// ----------------------------------------------------------------------------

size_type XISF::ChecksumLength( XISF::block_checksum algorithm )
{
   switch ( algorithm )
   {
   case XISFChecksum::SHA1:
      return 20;
   case XISFChecksum::SHA256:
      return 32;
   case XISFChecksum::SHA512:
      return 64;
   default: // ?!
   case XISFChecksum::None:
      throw Error( "XISF::ChecksumLength(): "
         + String().Format( "Internal error: Invalid cryptographic hashing algorithm 0x%02x", algorithm ) );
   }
}

// ----------------------------------------------------------------------------

CryptographicHash* XISF::NewCryptographicHash( XISF::block_checksum algorithm )
{
   switch ( algorithm )
   {
   case XISFChecksum::SHA1:
      return new SHA1;
   case XISFChecksum::SHA256:
      return new SHA256;
   case XISFChecksum::SHA512:
      return new SHA512;
   default: // ?!
   case XISFChecksum::None:
      throw Error( "XISF::NewCryptographicHash(): "
         + String().Format( "Internal error: Invalid cryptographic hashing algorithm 0x%02x", algorithm ) );
   }
}

// ----------------------------------------------------------------------------

void XISF::EnsurePTLUTInitialized()
{
   static AtomicInt initialized;
   if ( initialized.Load() == 0 )
   {
      static Mutex mutex;
      volatile AutoLock lock( mutex );
      if ( initialized.Load() == 0 )
      {
         if ( PTLUT == nullptr )
         {
            float*  pFLUT8   = new float [ uint8_max +1 ]; // uint8 -> float
            float*  pFLUTA   = new float [ uint8_max +1 ]; // i/255^2 (direct 8-bit normalization)
            float*  p1FLUT8  = new float [ uint8_max +1 ]; // 1 - pFLUT8
            double* pDLUT8   = new double[ uint8_max +1 ]; // uint8 -> double
            double* pDLUTA   = new double[ uint8_max +1 ]; // i/255^2 (direct 8-bit normalization)
            double* p1DLUT8  = new double[ uint8_max +1 ]; // 1- pDLUT8
            uint16* p16LUT8  = new uint16[ uint8_max +1 ]; // uint8 -> uint16
            uint32* p20LUT8  = new uint32[ uint8_max +1 ]; // uint8 -> uint20
            uint32* p24LUT8  = new uint32[ uint8_max +1 ]; // uint8 -> uint24
            uint32* p32LUT8  = new uint32[ uint8_max +1 ]; // uint8 -> uint32
            float*  pFLUT16  = new float [ uint16_max+1 ]; // uint16 -> float
            double* pDLUT16  = new double[ uint16_max+1 ]; // uint16 -> double
            uint8*  p8LUT16  = new uint8 [ uint16_max+1 ]; // uint16 -> uint8
            uint32* p20LUT16 = new uint32[ uint16_max+1 ]; // uint16 -> uint20
            uint32* p24LUT16 = new uint32[ uint16_max+1 ]; // uint16 -> uint24
            uint32* p32LUT16 = new uint32[ uint16_max+1 ]; // uint16 -> uint32

            for ( uint32 i = 0; i <= uint8_max; ++i )
            {
               pFLUT8[i]   = float( i )/float( uint8_max );
               pFLUTA[i]   = float( i )/65025.0F;
               p1FLUT8[i]  = 1.0F - float( i )/float( uint8_max );
               pDLUT8[i]   = double( i )/double( uint8_max );
               pDLUTA[i]   = double( i )/65025.0;
               p1DLUT8[i]  = 1.0 - double( i )/double( uint8_max );
               p16LUT8[i]  = uint16( uint32( i )*uint32( uint8_to_uint16 ) );
               p20LUT8[i]  = uint32( RoundInt( double( i )*uint8_to_uint20 ) );
               p24LUT8[i]  = uint32( i * uint8_to_uint24 );
               p32LUT8[i]  = uint32( uint64( i )*uint64( uint8_to_uint32 ) );
            }

            for ( uint32 i = 0; i <= uint16_max; ++i )
            {
               pFLUT16[i]  = float( i )/float( uint16_max );
               pDLUT16[i]  = double( i )/double( uint16_max );
               p8LUT16[i]  = uint8( RoundInt( double( i )*uint16_to_uint8 ) );
               p20LUT16[i] = uint32( RoundInt( double( i )*uint16_to_uint20 ) );
               p24LUT16[i] = uint32( RoundInt( double( i )*uint16_to_uint24 ) );
               p32LUT16[i] = uint32( uint64( i )*uint64( uint16_to_uint32 ) );
            }

            PixelTraitsLUT* lut = new PixelTraitsLUT;
            lut->pFLUT8   = pFLUT8;
            lut->pFLUTA   = pFLUTA;
            lut->p1FLUT8  = p1FLUT8;
            lut->pDLUT8   = pDLUT8;
            lut->pDLUTA   = pDLUTA;
            lut->p1DLUT8  = p1DLUT8;
            lut->p16LUT8  = p16LUT8;
            lut->p20LUT8  = p20LUT8;
            lut->p24LUT8  = p24LUT8;
            lut->p32LUT8  = p32LUT8;
            lut->pFLUT16  = pFLUT16;
            lut->pDLUT16  = pDLUT16;
            lut->p8LUT16  = p8LUT16;
            lut->p20LUT16 = p20LUT16;
            lut->p24LUT16 = p24LUT16;
            lut->p32LUT16 = p32LUT16;

            PTLUT = lut;
         }

         initialized.Store( 1 );
      }
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/XISF.cpp - Released 2018-12-12T09:24:30Z
