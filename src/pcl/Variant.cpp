//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/Variant.cpp - Released 2018-12-12T09:24:30Z
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

#include <pcl/Variant.h>
#include <pcl/Exception.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

const char* Variant::TypeAsString( int type )
{
   switch ( type )
   {
   case VariantType::Invalid:               return "Invalid";
   case VariantType::Bool:                  return "Bool";
   case VariantType::Int8:                  return "Int8";
   case VariantType::Int16:                 return "Int16";
   case VariantType::Int32:                 return "Int32";
   case VariantType::Int64:                 return "Int64";
   case VariantType::UInt8:                 return "UInt8";
   case VariantType::UInt16:                return "UInt16";
   case VariantType::UInt32:                return "UInt32";
   case VariantType::UInt64:                return "UInt64";
   case VariantType::Float32:               return "Float";
   case VariantType::Float64:               return "Double";
   case VariantType::Complex32:             return "FComplex";
   case VariantType::Complex64:             return "DComplex";
   case VariantType::TimePoint:             return "TimePoint";
   case VariantType::I32Point:              return "Point";
   case VariantType::F32Point:              return "FPoint";
   case VariantType::F64Point:              return "DPoint";
   case VariantType::I32Rect:               return "Rect";
   case VariantType::F32Rect:               return "FRect";
   case VariantType::F64Rect:               return "DRect";
   case VariantType::I8Vector:              return "CharVector";
   case VariantType::UI8Vector:             return "ByteVector";
   case VariantType::I16Vector:             return "I16Vector";
   case VariantType::UI16Vector:            return "UI16Vector";
   case VariantType::I32Vector:             return "IVector";
   case VariantType::UI32Vector:            return "UIVector";
   case VariantType::I64Vector:             return "I64Vector";
   case VariantType::UI64Vector:            return "UI64Vector";
   case VariantType::F32Vector:             return "FVector";
   case VariantType::F64Vector:             return "DVector";
   case VariantType::C32Vector:             return "C32Vector";
   case VariantType::C64Vector:             return "C64Vector";
   case VariantType::I8Matrix:              return "CharMatrix";
   case VariantType::UI8Matrix:             return "ByteMatrix";
   case VariantType::I16Matrix:             return "I16Matrix";
   case VariantType::UI16Matrix:            return "UI16Matrix";
   case VariantType::I32Matrix:             return "IMatrix";
   case VariantType::UI32Matrix:            return "UIMatrix";
   case VariantType::I64Matrix:             return "I64Matrix";
   case VariantType::UI64Matrix:            return "UI64Matrix";
   case VariantType::F32Matrix:             return "FMatrix";
   case VariantType::F64Matrix:             return "DMatrix";
   case VariantType::C32Matrix:             return "C32Matrix";
   case VariantType::C64Matrix:             return "C64Matrix";
   case VariantType::ByteArray:             return "ByteArray";
   case VariantType::String:                return "String";
   case VariantType::IsoString:             return "IsoString";
   case VariantType::StringList:            return "StringList";
   case VariantType::IsoStringList:         return "IsoStringList";
   case VariantType::StringKeyValue:        return "StringKeyValue";
   case VariantType::IsoStringKeyValue:     return "IsoStringKeyValue";
   case VariantType::StringKeyValueList:    return "StringKeyValueList";
   case VariantType::IsoStringKeyValueList: return "IsoStringKeyValueList";
   default:                                 return "unknown";
   }
}

bool Variant::ToBool() const
{
   switch ( m_type )
   {
   case VariantType::Invalid:               return false;
   case VariantType::Bool:                  return m_data.boolValue;
   case VariantType::Int8:                  return m_data.int8Value != 0;
   case VariantType::Int16:                 return m_data.int16Value != 0;
   case VariantType::Int32:                 return m_data.int32Value != 0;
   case VariantType::Int64:                 return m_data.int64Value != 0;
   case VariantType::UInt8:                 return m_data.uint8Value != 0;
   case VariantType::UInt16:                return m_data.uint16Value != 0;
   case VariantType::UInt32:                return m_data.uint32Value != 0;
   case VariantType::UInt64:                return m_data.uint64Value != 0;
   case VariantType::Float32:               return m_data.float32Value != 0;
   case VariantType::Float64:               return m_data.float64Value != 0;
   case VariantType::Complex32:             return *m_data.complex32Value != 0;
   case VariantType::Complex64:             return *m_data.complex64Value != 0;
   case VariantType::I32Point:              return *m_data.i32PointValue != 0;
   case VariantType::F32Point:              return *m_data.f32PointValue != 0.0f;
   case VariantType::F64Point:              return *m_data.f64PointValue != 0.0;
   case VariantType::I32Rect:               return *m_data.i32RectValue != 0;
   case VariantType::F32Rect:               return *m_data.f32RectValue != 0.0f;
   case VariantType::F64Rect:               return *m_data.f64RectValue != 0.0;
   case VariantType::I8Vector:              return !m_data.i8VectorValue->IsEmpty();
   case VariantType::UI8Vector:             return !m_data.ui8VectorValue->IsEmpty();
   case VariantType::I16Vector:             return !m_data.i16VectorValue->IsEmpty();
   case VariantType::UI16Vector:            return !m_data.ui16VectorValue->IsEmpty();
   case VariantType::I32Vector:             return !m_data.i32VectorValue->IsEmpty();
   case VariantType::UI32Vector:            return !m_data.ui32VectorValue->IsEmpty();
   case VariantType::I64Vector:             return !m_data.i64VectorValue->IsEmpty();
   case VariantType::UI64Vector:            return !m_data.ui64VectorValue->IsEmpty();
   case VariantType::F32Vector:             return !m_data.f32VectorValue->IsEmpty();
   case VariantType::F64Vector:             return !m_data.f64VectorValue->IsEmpty();
   case VariantType::C32Vector:             return !m_data.c32VectorValue->IsEmpty();
   case VariantType::C64Vector:             return !m_data.c64VectorValue->IsEmpty();
   case VariantType::I8Matrix:              return !m_data.i8MatrixValue->IsEmpty();
   case VariantType::UI8Matrix:             return !m_data.ui8MatrixValue->IsEmpty();
   case VariantType::I16Matrix:             return !m_data.i16MatrixValue->IsEmpty();
   case VariantType::UI16Matrix:            return !m_data.ui16MatrixValue->IsEmpty();
   case VariantType::I32Matrix:             return !m_data.i32MatrixValue->IsEmpty();
   case VariantType::UI32Matrix:            return !m_data.ui32VectorValue->IsEmpty();
   case VariantType::I64Matrix:             return !m_data.i64VectorValue->IsEmpty();
   case VariantType::UI64Matrix:            return !m_data.ui64VectorValue->IsEmpty();
   case VariantType::F32Matrix:             return !m_data.f32MatrixValue->IsEmpty();
   case VariantType::F64Matrix:             return !m_data.f64MatrixValue->IsEmpty();
   case VariantType::C32Matrix:             return !m_data.c32MatrixValue->IsEmpty();
   case VariantType::C64Matrix:             return !m_data.c64MatrixValue->IsEmpty();
   case VariantType::ByteArray:             return !m_data.byteArrayValue->IsEmpty();
   case VariantType::String:                return !m_data.stringValue->IsEmpty();
   case VariantType::IsoString:             return !m_data.isoStringValue->IsEmpty();
   case VariantType::StringList:            return !m_data.stringListValue->IsEmpty();
   case VariantType::IsoStringList:         return !m_data.isoStringListValue->IsEmpty();
   case VariantType::StringKeyValue:        return !m_data.stringKeyValueValue->value.IsEmpty();
   case VariantType::IsoStringKeyValue:     return !m_data.isoStringKeyValueValue->value.IsEmpty();
   case VariantType::StringKeyValueList:    return !m_data.stringKeyValueListValue->IsEmpty();
   case VariantType::IsoStringKeyValueList: return !m_data.isoStringKeyValueListValue->IsEmpty();
   default:
      throw Error( "Variant::ToBool(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

int Variant::ToInt() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return int32( m_data.boolValue );
   case VariantType::Int8:      return int32( m_data.int8Value );
   case VariantType::Int16:     return int32( m_data.int16Value );
   case VariantType::Int32:     return int32( m_data.int32Value );
   case VariantType::Int64:     return int32( m_data.int64Value );
   case VariantType::UInt8:     return int32( m_data.uint8Value );
   case VariantType::UInt16:    return int32( m_data.uint16Value );
   case VariantType::UInt32:    return int32( m_data.uint32Value );
   case VariantType::UInt64:    return int32( m_data.uint64Value );
   case VariantType::Float32:   return int32( m_data.float32Value );
   case VariantType::Float64:   return int32( m_data.float64Value );
   case VariantType::Complex32: return int32( m_data.complex32Value->Mag() );
   case VariantType::Complex64: return int32( m_data.complex64Value->Mag() );
   case VariantType::TimePoint: return int32( m_data.timePointValue->JDI() );
   case VariantType::String:    return m_data.stringValue->ToInt();
   case VariantType::IsoString: return m_data.isoStringValue->ToInt();
   default:
      throw Error( "Variant::ToInt(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

int64 Variant::ToInt64() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return int64( m_data.boolValue );
   case VariantType::Int8:      return int64( m_data.int8Value );
   case VariantType::Int16:     return int64( m_data.int16Value );
   case VariantType::Int32:     return int64( m_data.int32Value );
   case VariantType::Int64:     return m_data.int64Value;
   case VariantType::UInt8:     return int64( m_data.uint8Value );
   case VariantType::UInt16:    return int64( m_data.uint16Value );
   case VariantType::UInt32:    return int64( m_data.uint32Value );
   case VariantType::UInt64:    return int64( m_data.uint64Value );
   case VariantType::Float32:   return int64( m_data.float32Value );
   case VariantType::Float64:   return int64( m_data.float64Value );
   case VariantType::Complex32: return int64( m_data.complex32Value->Mag() );
   case VariantType::Complex64: return int64( m_data.complex64Value->Mag() );
   case VariantType::TimePoint: return int64( m_data.timePointValue->JDI() );
   case VariantType::String:    return m_data.stringValue->ToInt64();
   case VariantType::IsoString: return m_data.isoStringValue->ToInt64();
   default:
      throw Error( "Variant::ToInt64(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

unsigned int Variant::ToUInt() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return uint32( m_data.boolValue );
   case VariantType::Int8:      return uint32( m_data.int8Value );
   case VariantType::Int16:     return uint32( m_data.int16Value );
   case VariantType::Int32:     return uint32( m_data.int32Value );
   case VariantType::Int64:     return uint32( m_data.int64Value );
   case VariantType::UInt8:     return uint32( m_data.uint8Value );
   case VariantType::UInt16:    return uint32( m_data.uint16Value );
   case VariantType::UInt32:    return uint32( m_data.uint32Value );
   case VariantType::UInt64:    return uint32( m_data.uint64Value );
   case VariantType::Float32:   return uint32( m_data.float32Value );
   case VariantType::Float64:   return uint32( m_data.float64Value );
   case VariantType::Complex32: return uint32( m_data.complex32Value->Mag() );
   case VariantType::Complex64: return uint32( m_data.complex64Value->Mag() );
   case VariantType::TimePoint: return uint32( m_data.timePointValue->JDI() );
   case VariantType::String:    return m_data.stringValue->ToUInt();
   case VariantType::IsoString: return m_data.isoStringValue->ToUInt();
   default:
      throw Error( "Variant::ToUInt(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

uint64 Variant::ToUInt64() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return uint64( m_data.boolValue );
   case VariantType::Int8:      return uint64( m_data.int8Value );
   case VariantType::Int16:     return uint64( m_data.int16Value );
   case VariantType::Int32:     return uint64( m_data.int32Value );
   case VariantType::Int64:     return uint64( m_data.int64Value );
   case VariantType::UInt8:     return uint64( m_data.uint8Value );
   case VariantType::UInt16:    return uint64( m_data.uint16Value );
   case VariantType::UInt32:    return uint64( m_data.uint32Value );
   case VariantType::UInt64:    return m_data.uint64Value;
   case VariantType::Float32:   return uint64( m_data.float32Value );
   case VariantType::Float64:   return uint64( m_data.float64Value );
   case VariantType::Complex32: return uint64( m_data.complex32Value->Mag() );
   case VariantType::Complex64: return uint64( m_data.complex64Value->Mag() );
   case VariantType::TimePoint: return uint64( m_data.timePointValue->JDI() );
   case VariantType::String:    return m_data.stringValue->ToUInt64();
   case VariantType::IsoString: return m_data.isoStringValue->ToUInt64();
   default:
      throw Error( "Variant::ToUInt64(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

float Variant::ToFloat() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return float( m_data.boolValue );
   case VariantType::Int8:      return float( m_data.int8Value );
   case VariantType::Int16:     return float( m_data.int16Value );
   case VariantType::Int32:     return float( m_data.int32Value );
   case VariantType::Int64:     return float( m_data.int64Value );
   case VariantType::UInt8:     return float( m_data.uint8Value );
   case VariantType::UInt16:    return float( m_data.uint16Value );
   case VariantType::UInt32:    return float( m_data.uint32Value );
   case VariantType::UInt64:    return float( m_data.uint64Value );
   case VariantType::Float32:   return m_data.float32Value;
   case VariantType::Float64:   return float( m_data.float64Value );
   case VariantType::Complex32: return float( m_data.complex32Value->Mag() );
   case VariantType::Complex64: return float( m_data.complex64Value->Mag() );
   case VariantType::TimePoint: return float( m_data.timePointValue->JD() );
   case VariantType::String:    return m_data.stringValue->ToFloat();
   case VariantType::IsoString: return m_data.isoStringValue->ToFloat();
   default:
      throw Error( "Variant::ToFloat(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

double Variant::ToDouble() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return double( m_data.boolValue );
   case VariantType::Int8:      return double( m_data.int8Value );
   case VariantType::Int16:     return double( m_data.int16Value );
   case VariantType::Int32:     return double( m_data.int32Value );
   case VariantType::Int64:     return double( m_data.int64Value );
   case VariantType::UInt8:     return double( m_data.uint8Value );
   case VariantType::UInt16:    return double( m_data.uint16Value );
   case VariantType::UInt32:    return double( m_data.uint32Value );
   case VariantType::UInt64:    return double( m_data.uint64Value );
   case VariantType::Float32:   return double( m_data.float32Value );
   case VariantType::Float64:   return m_data.float64Value;
   case VariantType::Complex32: return double( m_data.complex32Value->Mag() );
   case VariantType::Complex64: return double( m_data.complex64Value->Mag() );
   case VariantType::TimePoint: return double( m_data.timePointValue->JD() );
   case VariantType::String:    return m_data.stringValue->ToDouble();
   case VariantType::IsoString: return m_data.isoStringValue->ToDouble();
   default:
      throw Error( "Variant::ToDouble(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

fcomplex Variant::ToFComplex() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return fcomplex( float( m_data.boolValue ) );
   case VariantType::Int8:      return fcomplex( float( m_data.int8Value ) );
   case VariantType::Int16:     return fcomplex( float( m_data.int16Value ) );
   case VariantType::Int32:     return fcomplex( float( m_data.int32Value ) );
   case VariantType::Int64:     return fcomplex( float( m_data.int64Value ) );
   case VariantType::UInt8:     return fcomplex( float( m_data.uint8Value ) );
   case VariantType::UInt16:    return fcomplex( float( m_data.uint16Value ) );
   case VariantType::UInt32:    return fcomplex( float( m_data.uint32Value ) );
   case VariantType::UInt64:    return fcomplex( float( m_data.uint64Value ) );
   case VariantType::Float32:   return fcomplex( m_data.float32Value );
   case VariantType::Float64:   return fcomplex( float( m_data.float64Value ) );
   case VariantType::Complex32: return *m_data.complex32Value;
   case VariantType::Complex64: return fcomplex( *m_data.complex64Value );
   case VariantType::TimePoint: return fcomplex( float( m_data.timePointValue->JD() ) );
   case VariantType::I32Point:  return fcomplex( float( m_data.i32PointValue->x ), float( m_data.i32PointValue->y ) );
   case VariantType::F32Point:  return fcomplex( m_data.f32PointValue->x, m_data.f32PointValue->y );
   case VariantType::F64Point:  return fcomplex( float( m_data.f64PointValue->x ), float( m_data.f64PointValue->y ) );
   case VariantType::I32Rect:   return fcomplex( float( m_data.i32RectValue->x0 ), float( m_data.i32RectValue->y0 ) );
   case VariantType::F32Rect:   return fcomplex( m_data.f32RectValue->x0, m_data.f32RectValue->y0 );
   case VariantType::F64Rect:   return fcomplex( float( m_data.f64RectValue->x0 ), float( m_data.f64RectValue->y0 ) );
   default:
      throw Error( "Variant::ToFComplex(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

dcomplex Variant::ToDComplex() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return dcomplex( double( m_data.boolValue ) );
   case VariantType::Int8:      return dcomplex( double( m_data.int8Value ) );
   case VariantType::Int16:     return dcomplex( double( m_data.int16Value ) );
   case VariantType::Int32:     return dcomplex( double( m_data.int32Value ) );
   case VariantType::Int64:     return dcomplex( double( m_data.int64Value ) );
   case VariantType::UInt8:     return dcomplex( double( m_data.uint8Value ) );
   case VariantType::UInt16:    return dcomplex( double( m_data.uint16Value ) );
   case VariantType::UInt32:    return dcomplex( double( m_data.uint32Value ) );
   case VariantType::UInt64:    return dcomplex( double( m_data.uint64Value ) );
   case VariantType::Float32:   return dcomplex( double( m_data.float32Value ) );
   case VariantType::Float64:   return dcomplex( m_data.float64Value );
   case VariantType::Complex32: return dcomplex( *m_data.complex32Value );
   case VariantType::Complex64: return *m_data.complex64Value;
   case VariantType::TimePoint: return dcomplex( double( m_data.timePointValue->JD() ) );
   case VariantType::I32Point:  return dcomplex( double( m_data.i32PointValue->x ), double( m_data.i32PointValue->y ) );
   case VariantType::F32Point:  return dcomplex( double( m_data.f32PointValue->x ), double( m_data.f32PointValue->y ) );
   case VariantType::F64Point:  return dcomplex( m_data.f64PointValue->x, m_data.f64PointValue->y );
   case VariantType::I32Rect:   return dcomplex( double( m_data.i32RectValue->x0 ), double( m_data.i32RectValue->y0 ) );
   case VariantType::F32Rect:   return dcomplex( double( m_data.f32RectValue->x0 ), double( m_data.f32RectValue->y0 ) );
   case VariantType::F64Rect:   return dcomplex( m_data.f64RectValue->x0, m_data.f64RectValue->y0 );
   default:
      throw Error( "Variant::ToDComplex(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

TimePoint Variant::ToTimePoint() const
{
   switch ( m_type )
   {
   case VariantType::Int8:      return TimePoint( double( m_data.int8Value ) );
   case VariantType::Int16:     return TimePoint( double( m_data.int16Value ) );
   case VariantType::Int32:     return TimePoint( double( m_data.int32Value ) );
   case VariantType::Int64:     return TimePoint( double( m_data.int64Value ) );
   case VariantType::UInt8:     return TimePoint( double( m_data.uint8Value ) );
   case VariantType::UInt16:    return TimePoint( double( m_data.uint16Value ) );
   case VariantType::UInt32:    return TimePoint( double( m_data.uint32Value ) );
   case VariantType::UInt64:    return TimePoint( double( m_data.uint64Value ) );
   case VariantType::Float32:   return TimePoint( double( m_data.float32Value ) );
   case VariantType::Float64:   return TimePoint( m_data.float64Value );
   case VariantType::TimePoint: return *m_data.timePointValue;
   case VariantType::String:    return TimePoint( *m_data.stringValue );
   case VariantType::IsoString: return TimePoint( *m_data.isoStringValue );
   default:
      throw Error( "Variant::ToTimePoint(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

Point Variant::ToPoint() const
{
   switch ( m_type )
   {
   case VariantType::Int8:      return Point( int32( m_data.int8Value ) );
   case VariantType::Int16:     return Point( int32( m_data.int16Value ) );
   case VariantType::Int32:     return Point( int32( m_data.int32Value ) );
   case VariantType::Int64:     return Point( int32( m_data.int64Value ) );
   case VariantType::UInt8:     return Point( int32( m_data.uint8Value ) );
   case VariantType::UInt16:    return Point( int32( m_data.uint16Value ) );
   case VariantType::UInt32:    return Point( int32( m_data.uint32Value ) );
   case VariantType::UInt64:    return Point( int32( m_data.uint64Value ) );
   case VariantType::Float32:   return Point( int32( m_data.float32Value ) );
   case VariantType::Float64:   return Point( int32( m_data.float64Value ) );
   case VariantType::Complex32: return Point( int32( m_data.complex32Value->Real() ), int32( m_data.complex32Value->Imag() ) );
   case VariantType::Complex64: return Point( int32( m_data.complex64Value->Real() ), int32( m_data.complex64Value->Imag() ) );
   case VariantType::I32Point:  return *m_data.i32PointValue;
   case VariantType::F32Point:  return Point( *m_data.f32PointValue );
   case VariantType::F64Point:  return Point( *m_data.f64PointValue );
   case VariantType::I32Rect:   return Point( m_data.i32RectValue->x0, m_data.i32RectValue->y0 );
   case VariantType::F32Rect:   return Point( int32( m_data.f32RectValue->x0 ), int32( m_data.f32RectValue->y0 ) );
   case VariantType::F64Rect:   return Point( int32( m_data.f64RectValue->x0 ), int32( m_data.f64RectValue->y0 ) );
   default:
      throw Error( "Variant::ToPoint(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

FPoint Variant::ToFPoint() const
{
   switch ( m_type )
   {
   case VariantType::Int8:      return FPoint( float( m_data.int8Value ) );
   case VariantType::Int16:     return FPoint( float( m_data.int16Value ) );
   case VariantType::Int32:     return FPoint( float( m_data.int32Value ) );
   case VariantType::Int64:     return FPoint( float( m_data.int64Value ) );
   case VariantType::UInt8:     return FPoint( float( m_data.uint8Value ) );
   case VariantType::UInt16:    return FPoint( float( m_data.uint16Value ) );
   case VariantType::UInt32:    return FPoint( float( m_data.uint32Value ) );
   case VariantType::UInt64:    return FPoint( float( m_data.uint64Value ) );
   case VariantType::Float32:   return FPoint( m_data.float32Value );
   case VariantType::Float64:   return FPoint( float( m_data.float64Value ) );
   case VariantType::Complex32: return FPoint( m_data.complex32Value->Real(), m_data.complex32Value->Imag() );
   case VariantType::Complex64: return FPoint( float( m_data.complex64Value->Real() ), float( m_data.complex64Value->Imag() ) );
   case VariantType::I32Point:  return FPoint( *m_data.i32PointValue );
   case VariantType::F32Point:  return *m_data.f32PointValue;
   case VariantType::F64Point:  return FPoint( *m_data.f64PointValue );
   case VariantType::I32Rect:   return FPoint( float( m_data.i32RectValue->x0 ), float( m_data.i32RectValue->y0 ) );
   case VariantType::F32Rect:   return FPoint( m_data.f32RectValue->x0, m_data.f32RectValue->y0 );
   case VariantType::F64Rect:   return FPoint( float( m_data.f64RectValue->x0 ), float( m_data.f64RectValue->y0 ) );
   default:
      throw Error( "Variant::ToFPoint(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

DPoint Variant::ToDPoint() const
{
   switch ( m_type )
   {
   case VariantType::Int8:      return DPoint( double( m_data.int8Value ) );
   case VariantType::Int16:     return DPoint( double( m_data.int16Value ) );
   case VariantType::Int32:     return DPoint( double( m_data.int32Value ) );
   case VariantType::Int64:     return DPoint( double( m_data.int64Value ) );
   case VariantType::UInt8:     return DPoint( double( m_data.uint8Value ) );
   case VariantType::UInt16:    return DPoint( double( m_data.uint16Value ) );
   case VariantType::UInt32:    return DPoint( double( m_data.uint32Value ) );
   case VariantType::UInt64:    return DPoint( double( m_data.uint64Value ) );
   case VariantType::Float32:   return DPoint( double( m_data.float32Value ) );
   case VariantType::Float64:   return DPoint( m_data.float64Value );
   case VariantType::Complex32: return DPoint( double( m_data.complex32Value->Real() ), double( m_data.complex32Value->Imag() ) );
   case VariantType::Complex64: return DPoint( m_data.complex64Value->Real(), m_data.complex64Value->Imag() );
   case VariantType::I32Point:  return DPoint( *m_data.i32PointValue );
   case VariantType::F32Point:  return DPoint( *m_data.f32PointValue );
   case VariantType::F64Point:  return *m_data.f64PointValue;
   case VariantType::I32Rect:   return DPoint( double( m_data.i32RectValue->x0 ), double( m_data.i32RectValue->y0 ) );
   case VariantType::F32Rect:   return DPoint( double( m_data.f32RectValue->x0 ), double( m_data.f32RectValue->y0 ) );
   case VariantType::F64Rect:   return DPoint( m_data.f64RectValue->x0, m_data.f64RectValue->y0 );
   default:
      throw Error( "Variant::ToDPoint(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

Rect Variant::ToRect() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return Rect( int32( m_data.boolValue ) );
   case VariantType::Int8:      return Rect( int32( m_data.int8Value ) );
   case VariantType::Int16:     return Rect( int32( m_data.int16Value ) );
   case VariantType::Int32:     return Rect( int32( m_data.int32Value ) );
   case VariantType::Int64:     return Rect( int32( m_data.int64Value ) );
   case VariantType::UInt8:     return Rect( int32( m_data.uint8Value ) );
   case VariantType::UInt16:    return Rect( int32( m_data.uint16Value ) );
   case VariantType::UInt32:    return Rect( int32( m_data.uint32Value ) );
   case VariantType::UInt64:    return Rect( int32( m_data.uint64Value ) );
   case VariantType::Float32:   return Rect( int32( m_data.float32Value ) );
   case VariantType::Float64:   return Rect( int32( m_data.float64Value ) );
   case VariantType::Complex32: return Rect( int32( m_data.complex32Value->Mag() ) );
   case VariantType::Complex64: return Rect( int32( m_data.complex64Value->Mag() ) );
   case VariantType::I32Point:  return Rect( m_data.i32PointValue->x, m_data.i32PointValue->y );
   case VariantType::F32Point:  return Rect( int32( m_data.f32PointValue->x ), int32( m_data.f32PointValue->y ) );
   case VariantType::F64Point:  return Rect( int32( m_data.f64PointValue->x ), int32( m_data.f64PointValue->y ) );
   case VariantType::I32Rect:   return *m_data.i32RectValue;
   case VariantType::F32Rect:   return Rect( *m_data.f32RectValue );
   case VariantType::F64Rect:   return Rect( *m_data.f64RectValue );
   default:
      throw Error( "Variant::ToRect(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

FRect Variant::ToFRect() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return FRect( float( m_data.boolValue ) );
   case VariantType::Int8:      return FRect( float( m_data.int8Value ) );
   case VariantType::Int16:     return FRect( float( m_data.int16Value ) );
   case VariantType::Int32:     return FRect( float( m_data.int32Value ) );
   case VariantType::Int64:     return FRect( float( m_data.int64Value ) );
   case VariantType::UInt8:     return FRect( float( m_data.uint8Value ) );
   case VariantType::UInt16:    return FRect( float( m_data.uint16Value ) );
   case VariantType::UInt32:    return FRect( float( m_data.uint32Value ) );
   case VariantType::UInt64:    return FRect( float( m_data.uint64Value ) );
   case VariantType::Float32:   return FRect( float( m_data.float32Value ) );
   case VariantType::Float64:   return FRect( float( m_data.float64Value ) );
   case VariantType::Complex32: return FRect( m_data.complex32Value->Mag() );
   case VariantType::Complex64: return FRect( float( m_data.complex64Value->Mag() ) );
   case VariantType::I32Point:  return FRect( float( m_data.i32PointValue->x ), float( m_data.i32PointValue->y ) );
   case VariantType::F32Point:  return FRect( m_data.f32PointValue->x, m_data.f32PointValue->y );
   case VariantType::F64Point:  return FRect( float( m_data.f64PointValue->x ), float( m_data.f64PointValue->y ) );
   case VariantType::I32Rect:   return FRect( *m_data.i32RectValue );
   case VariantType::F32Rect:   return *m_data.f32RectValue;
   case VariantType::F64Rect:   return FRect( *m_data.f64RectValue );
   default:
      throw Error( "Variant::ToFRect(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

DRect Variant::ToDRect() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return DRect( double( m_data.boolValue ) );
   case VariantType::Int8:      return DRect( double( m_data.int8Value ) );
   case VariantType::Int16:     return DRect( double( m_data.int16Value ) );
   case VariantType::Int32:     return DRect( double( m_data.int32Value ) );
   case VariantType::Int64:     return DRect( double( m_data.int64Value ) );
   case VariantType::UInt8:     return DRect( double( m_data.uint8Value ) );
   case VariantType::UInt16:    return DRect( double( m_data.uint16Value ) );
   case VariantType::UInt32:    return DRect( double( m_data.uint32Value ) );
   case VariantType::UInt64:    return DRect( double( m_data.uint64Value ) );
   case VariantType::Float32:   return DRect( double( m_data.float32Value ) );
   case VariantType::Float64:   return DRect( double( m_data.float64Value ) );
   case VariantType::Complex32: return DRect( double( m_data.complex32Value->Mag() ) );
   case VariantType::Complex64: return DRect( m_data.complex64Value->Mag() );
   case VariantType::I32Point:  return DRect( double( m_data.i32PointValue->x ), double( m_data.i32PointValue->y ) );
   case VariantType::F32Point:  return DRect( double( m_data.f32PointValue->x ), double( m_data.f32PointValue->y ) );
   case VariantType::F64Point:  return DRect( m_data.f64PointValue->x, m_data.f64PointValue->y );
   case VariantType::I32Rect:   return DRect( *m_data.i32RectValue );
   case VariantType::F32Rect:   return DRect( *m_data.f32RectValue );
   case VariantType::F64Rect:   return *m_data.f64RectValue;
   default:
      throw Error( "Variant::ToDRect(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

CharVector Variant::ToCharVector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return CharVector( int8( m_data.boolValue ), 1 );
   case VariantType::Int8:
      return CharVector( m_data.int8Value, 1 );
   case VariantType::Int16:
      return CharVector( int8( m_data.int16Value ), 1 );
   case VariantType::Int32:
      return CharVector( int8( m_data.int32Value ), 1 );
   case VariantType::Int64:
      return CharVector( int8( m_data.int64Value ), 1 );
   case VariantType::UInt8:
      return CharVector( int8( m_data.uint8Value ), 1 );
   case VariantType::UInt16:
      return CharVector( int8( m_data.uint16Value ), 1 );
   case VariantType::UInt32:
      return CharVector( int8( m_data.uint32Value ), 1 );
   case VariantType::UInt64:
      return CharVector( int8( m_data.uint64Value ), 1 );
   case VariantType::Float32:
      return CharVector( int8( m_data.float32Value ), 1 );
   case VariantType::Float64:
      return CharVector( int8( m_data.float64Value ), 1 );

   case VariantType::Complex32:
      {
         CharVector v( 2 );
         v[0] = int8( m_data.complex32Value->Real() );
         v[1] = int8( m_data.complex32Value->Imag() );
         return v;
      }
   case VariantType::Complex64:
      {
         CharVector v( 2 );
         v[0] = int8( m_data.complex64Value->Real() );
         v[1] = int8( m_data.complex64Value->Imag() );
         return v;
      }

   case VariantType::I32Point:
      {
         CharVector v( 2 );
         v[0] = int8( m_data.i32PointValue->x );
         v[1] = int8( m_data.i32PointValue->y );
         return v;
      }
   case VariantType::F32Point:
      {
         CharVector v( 2 );
         v[0] = int8( m_data.f32PointValue->x );
         v[1] = int8( m_data.f32PointValue->y );
         return v;
      }
   case VariantType::F64Point:
      {
         CharVector v( 2 );
         v[0] = int8( m_data.f64PointValue->x );
         v[1] = int8( m_data.f64PointValue->y );
         return v;
      }

   case VariantType::I32Rect:
      {
         CharVector v( 4 );
         v[0] = int8( m_data.i32RectValue->x0 );
         v[1] = int8( m_data.i32RectValue->y0 );
         v[2] = int8( m_data.i32RectValue->x1 );
         v[3] = int8( m_data.i32RectValue->y1 );
         return v;
      }
   case VariantType::F32Rect:
      {
         CharVector v( 4 );
         v[0] = int8( m_data.f32RectValue->x0 );
         v[1] = int8( m_data.f32RectValue->y0 );
         v[2] = int8( m_data.f32RectValue->x1 );
         v[3] = int8( m_data.f32RectValue->y1 );
         return v;
      }
   case VariantType::F64Rect:
      {
         CharVector v( 4 );
         v[0] = int8( m_data.f64RectValue->x0 );
         v[1] = int8( m_data.f64RectValue->y0 );
         v[2] = int8( m_data.f64RectValue->x1 );
         v[3] = int8( m_data.f64RectValue->y1 );
         return v;
      }

   case VariantType::I8Vector:
      return *m_data.i8VectorValue;
   case VariantType::UI8Vector:
      return CharVector( m_data.ui8VectorValue->Begin(), m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return CharVector( m_data.i16VectorValue->Begin(), m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return CharVector( m_data.ui16VectorValue->Begin(), m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return CharVector( m_data.i32VectorValue->Begin(), m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return CharVector( m_data.ui32VectorValue->Begin(), m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return CharVector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return CharVector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return CharVector( m_data.f32VectorValue->Begin(), m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return CharVector( m_data.f64VectorValue->Begin(), m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         CharVector v( m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int8( c->Mag() );
         return v;
      }
   case VariantType::C64Vector:
      {
         CharVector v( m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int8( c->Mag() );
         return v;
      }

   case VariantType::I8Matrix:
      return CharVector( m_data.i8MatrixValue->Begin(), int( m_data.i8MatrixValue->NumberOfElements() ) );
   case VariantType::UI8Matrix:
      return CharVector( m_data.ui8MatrixValue->Begin(), int( m_data.ui8MatrixValue->NumberOfElements() ) );
   case VariantType::I16Matrix:
      return CharVector( m_data.i16MatrixValue->Begin(), int( m_data.i16MatrixValue->NumberOfElements() ) );
   case VariantType::UI16Matrix:
      return CharVector( m_data.ui16MatrixValue->Begin(), int( m_data.ui16MatrixValue->NumberOfElements() ) );
   case VariantType::I32Matrix:
      return CharVector( m_data.i32MatrixValue->Begin(), int( m_data.i32MatrixValue->NumberOfElements() ) );
   case VariantType::UI32Matrix:
      return CharVector( m_data.ui32MatrixValue->Begin(), int( m_data.ui32MatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:
      return CharVector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix:
      return CharVector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::F32Matrix:
      return CharVector( m_data.f32MatrixValue->Begin(), int( m_data.f32MatrixValue->NumberOfElements() ) );
   case VariantType::F64Matrix:
      return CharVector( m_data.f64MatrixValue->Begin(), int( m_data.f64MatrixValue->NumberOfElements() ) );
   case VariantType::C32Matrix:
      {
         CharVector v( int( m_data.c32MatrixValue->NumberOfElements() ) );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int8( c->Mag() );
         return v;
      }
   case VariantType::C64Matrix:
      {
         CharVector v( int( m_data.c64MatrixValue->NumberOfElements() ) );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int8( c->Mag() );
         return v;
      }

   case VariantType::ByteArray:
      return CharVector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return CharVector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return CharVector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToCharVector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

ByteVector Variant::ToByteVector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return ByteVector( uint8( m_data.boolValue ), 1 );
   case VariantType::Int8:
      return ByteVector( uint8( m_data.int8Value ), 1 );
   case VariantType::Int16:
      return ByteVector( uint8( m_data.int16Value ), 1 );
   case VariantType::Int32:
      return ByteVector( uint8( m_data.int32Value ), 1 );
   case VariantType::Int64:
      return ByteVector( uint8( m_data.int64Value ), 1 );
   case VariantType::UInt8:
      return ByteVector( m_data.uint8Value, 1 );
   case VariantType::UInt16:
      return ByteVector( uint8( m_data.uint16Value ), 1 );
   case VariantType::UInt32:
      return ByteVector( uint8( m_data.uint32Value ), 1 );
   case VariantType::UInt64:
      return ByteVector( uint8( m_data.uint64Value ), 1 );
   case VariantType::Float32:
      return ByteVector( uint8( m_data.float32Value ), 1 );
   case VariantType::Float64:
      return ByteVector( uint8( m_data.float64Value ), 1 );

   case VariantType::Complex32:
      {
         ByteVector v( 2 );
         v[0] = uint8( m_data.complex32Value->Real() );
         v[1] = uint8( m_data.complex32Value->Imag() );
         return v;
      }
   case VariantType::Complex64:
      {
         ByteVector v( 2 );
         v[0] = uint8( m_data.complex64Value->Real() );
         v[1] = uint8( m_data.complex64Value->Imag() );
         return v;
      }

   case VariantType::I32Point:
      {
         ByteVector v( 2 );
         v[0] = uint8( m_data.i32PointValue->x );
         v[1] = uint8( m_data.i32PointValue->y );
         return v;
      }
   case VariantType::F32Point:
      {
         ByteVector v( 2 );
         v[0] = uint8( m_data.f32PointValue->x );
         v[1] = uint8( m_data.f32PointValue->y );
         return v;
      }
   case VariantType::F64Point:
      {
         ByteVector v( 2 );
         v[0] = uint8( m_data.f64PointValue->x );
         v[1] = uint8( m_data.f64PointValue->y );
         return v;
      }

   case VariantType::I32Rect:
      {
         ByteVector v( 4 );
         v[0] = uint8( m_data.i32RectValue->x0 );
         v[1] = uint8( m_data.i32RectValue->y0 );
         v[2] = uint8( m_data.i32RectValue->x1 );
         v[3] = uint8( m_data.i32RectValue->y1 );
         return v;
      }
   case VariantType::F32Rect:
      {
         ByteVector v( 4 );
         v[0] = uint8( m_data.f32RectValue->x0 );
         v[1] = uint8( m_data.f32RectValue->y0 );
         v[2] = uint8( m_data.f32RectValue->x1 );
         v[3] = uint8( m_data.f32RectValue->y1 );
         return v;
      }
   case VariantType::F64Rect:
      {
         ByteVector v( 4 );
         v[0] = uint8( m_data.f64RectValue->x0 );
         v[1] = uint8( m_data.f64RectValue->y0 );
         v[2] = uint8( m_data.f64RectValue->x1 );
         v[3] = uint8( m_data.f64RectValue->y1 );
         return v;
      }

   case VariantType::I8Vector:
      return ByteVector( m_data.i8VectorValue->Begin(), m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return *m_data.ui8VectorValue;
   case VariantType::I16Vector:
      return ByteVector( m_data.i16VectorValue->Begin(), m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return ByteVector( m_data.ui16VectorValue->Begin(), m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return ByteVector( m_data.i32VectorValue->Begin(), m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return ByteVector( m_data.ui32VectorValue->Begin(), m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return ByteVector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return ByteVector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return ByteVector( m_data.f32VectorValue->Begin(), m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return ByteVector( m_data.f64VectorValue->Begin(), m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         ByteVector v( m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint8( c->Mag() );
         return v;
      }
   case VariantType::C64Vector:
      {
         ByteVector v( m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint8( c->Mag() );
         return v;
      }

   case VariantType::I8Matrix:
      return ByteVector( m_data.i8MatrixValue->Begin(), int( m_data.i8MatrixValue->NumberOfElements() ) );
   case VariantType::UI8Matrix:
      return ByteVector( m_data.ui8MatrixValue->Begin(), int( m_data.ui8MatrixValue->NumberOfElements() ) );
   case VariantType::I16Matrix:
      return ByteVector( m_data.i16MatrixValue->Begin(), int( m_data.i16MatrixValue->NumberOfElements() ) );
   case VariantType::UI16Matrix:
      return ByteVector( m_data.ui16MatrixValue->Begin(), int( m_data.ui16MatrixValue->NumberOfElements() ) );
   case VariantType::I32Matrix:
      return ByteVector( m_data.i32MatrixValue->Begin(), int( m_data.i32MatrixValue->NumberOfElements() ) );
   case VariantType::UI32Matrix:
      return ByteVector( m_data.ui32MatrixValue->Begin(), int( m_data.ui32MatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:
      return ByteVector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix:
      return ByteVector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::F32Matrix:
      return ByteVector( m_data.f32MatrixValue->Begin(), int( m_data.f32MatrixValue->NumberOfElements() ) );
   case VariantType::F64Matrix:
      return ByteVector( m_data.f64MatrixValue->Begin(), int( m_data.f64MatrixValue->NumberOfElements() ) );
   case VariantType::C32Matrix:
      {
         ByteVector v( int( m_data.c32MatrixValue->NumberOfElements() ) );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint8( c->Mag() );
         return v;
      }
   case VariantType::C64Matrix:
      {
         ByteVector v( int( m_data.c64MatrixValue->NumberOfElements() ) );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint8( c->Mag() );
         return v;
      }

   case VariantType::ByteArray:
      return ByteVector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return ByteVector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return ByteVector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToByteVector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

I16Vector Variant::ToI16Vector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return I16Vector( int16( m_data.boolValue ), 1 );
   case VariantType::Int8:
      return I16Vector( int16( m_data.int8Value ), 1 );
   case VariantType::Int16:
      return I16Vector( m_data.int16Value, 1 );
   case VariantType::Int32:
      return I16Vector( int16( m_data.int32Value ), 1 );
   case VariantType::Int64:
      return I16Vector( int16( m_data.int64Value ), 1 );
   case VariantType::UInt8:
      return I16Vector( int16( m_data.uint8Value ), 1 );
   case VariantType::UInt16:
      return I16Vector( int16( m_data.uint16Value ), 1 );
   case VariantType::UInt32:
      return I16Vector( int16( m_data.uint32Value ), 1 );
   case VariantType::UInt64:
      return I16Vector( int16( m_data.uint64Value ), 1 );
   case VariantType::Float32:
      return I16Vector( int16( m_data.float32Value ), 1 );
   case VariantType::Float64:
      return I16Vector( int16( m_data.float64Value ), 1 );

   case VariantType::Complex32:
      {
         I16Vector v( 2 );
         v[0] = int16( m_data.complex32Value->Real() );
         v[1] = int16( m_data.complex32Value->Imag() );
         return v;
      }
   case VariantType::Complex64:
      {
         I16Vector v( 2 );
         v[0] = int16( m_data.complex64Value->Real() );
         v[1] = int16( m_data.complex64Value->Imag() );
         return v;
      }

   case VariantType::I32Point:
      {
         I16Vector v( 2 );
         v[0] = int16( m_data.i32PointValue->x );
         v[1] = int16( m_data.i32PointValue->y );
         return v;
      }
   case VariantType::F32Point:
      {
         I16Vector v( 2 );
         v[0] = int16( m_data.f32PointValue->x );
         v[1] = int16( m_data.f32PointValue->y );
         return v;
      }
   case VariantType::F64Point:
      {
         I16Vector v( 2 );
         v[0] = int16( m_data.f64PointValue->x );
         v[1] = int16( m_data.f64PointValue->y );
         return v;
      }

   case VariantType::I32Rect:
      {
         I16Vector v( 4 );
         v[0] = int16( m_data.i32RectValue->x0 );
         v[1] = int16( m_data.i32RectValue->y0 );
         v[2] = int16( m_data.i32RectValue->x1 );
         v[3] = int16( m_data.i32RectValue->y1 );
         return v;
      }
   case VariantType::F32Rect:
      {
         I16Vector v( 4 );
         v[0] = int16( m_data.f32RectValue->x0 );
         v[1] = int16( m_data.f32RectValue->y0 );
         v[2] = int16( m_data.f32RectValue->x1 );
         v[3] = int16( m_data.f32RectValue->y1 );
         return v;
      }
   case VariantType::F64Rect:
      {
         I16Vector v( 4 );
         v[0] = int16( m_data.f64RectValue->x0 );
         v[1] = int16( m_data.f64RectValue->y0 );
         v[2] = int16( m_data.f64RectValue->x1 );
         v[3] = int16( m_data.f64RectValue->y1 );
         return v;
      }

   case VariantType::I8Vector:
      return I16Vector( m_data.i8VectorValue->Begin(), m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return I16Vector( m_data.ui8VectorValue->Begin(), m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return *m_data.i16VectorValue;
   case VariantType::UI16Vector:
      return I16Vector( m_data.ui16VectorValue->Begin(), m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return I16Vector( m_data.i32VectorValue->Begin(), m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return I16Vector( m_data.ui32VectorValue->Begin(), m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return I16Vector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return I16Vector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return I16Vector( m_data.f32VectorValue->Begin(), m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return I16Vector( m_data.f64VectorValue->Begin(), m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         I16Vector v( m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int16( c->Mag() );
         return v;
      }
   case VariantType::C64Vector:
      {
         I16Vector v( m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int16( c->Mag() );
         return v;
      }

   case VariantType::I8Matrix:
      return I16Vector( m_data.i8MatrixValue->Begin(), int( m_data.i8MatrixValue->NumberOfElements() ) );
   case VariantType::UI8Matrix:
      return I16Vector( m_data.ui8MatrixValue->Begin(), int( m_data.ui8MatrixValue->NumberOfElements() ) );
   case VariantType::I16Matrix:
      return I16Vector( m_data.i16MatrixValue->Begin(), int( m_data.i16MatrixValue->NumberOfElements() ) );
   case VariantType::UI16Matrix:
      return I16Vector( m_data.ui16MatrixValue->Begin(), int( m_data.ui16MatrixValue->NumberOfElements() ) );
   case VariantType::I32Matrix:
      return I16Vector( m_data.i32MatrixValue->Begin(), int( m_data.i32MatrixValue->NumberOfElements() ) );
   case VariantType::UI32Matrix:
      return I16Vector( m_data.ui32MatrixValue->Begin(), int( m_data.ui32MatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:
      return I16Vector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix:
      return I16Vector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::F32Matrix:
      return I16Vector( m_data.f32MatrixValue->Begin(), int( m_data.f32MatrixValue->NumberOfElements() ) );
   case VariantType::F64Matrix:
      return I16Vector( m_data.f64MatrixValue->Begin(), int( m_data.f64MatrixValue->NumberOfElements() ) );
   case VariantType::C32Matrix:
      {
         I16Vector v( int( m_data.c32MatrixValue->NumberOfElements() ) );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int16( c->Mag() );
         return v;
      }
   case VariantType::C64Matrix:
      {
         I16Vector v( int( m_data.c64MatrixValue->NumberOfElements() ) );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int16( c->Mag() );
         return v;
      }

   case VariantType::ByteArray:
      return I16Vector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return I16Vector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return I16Vector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToI16Vector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

UI16Vector Variant::ToUI16Vector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return UI16Vector( uint16( m_data.boolValue ), 1 );
   case VariantType::Int8:
      return UI16Vector( uint16( m_data.int8Value ), 1 );
   case VariantType::Int16:
      return UI16Vector( uint16( m_data.int16Value ), 1 );
   case VariantType::Int32:
      return UI16Vector( uint16( m_data.int32Value ), 1 );
   case VariantType::Int64:
      return UI16Vector( uint16( m_data.int64Value ), 1 );
   case VariantType::UInt8:
      return UI16Vector( uint16( m_data.uint8Value ), 1 );
   case VariantType::UInt16:
      return UI16Vector( m_data.uint16Value, 1 );
   case VariantType::UInt32:
      return UI16Vector( uint16( m_data.uint32Value ), 1 );
   case VariantType::UInt64:
      return UI16Vector( uint16( m_data.uint64Value ), 1 );
   case VariantType::Float32:
      return UI16Vector( uint16( m_data.float32Value ), 1 );
   case VariantType::Float64:
      return UI16Vector( uint16( m_data.float64Value ), 1 );

   case VariantType::Complex32:
      {
         UI16Vector v( 2 );
         v[0] = uint16( m_data.complex32Value->Real() );
         v[1] = uint16( m_data.complex32Value->Imag() );
         return v;
      }
   case VariantType::Complex64:
      {
         UI16Vector v( 2 );
         v[0] = uint16( m_data.complex64Value->Real() );
         v[1] = uint16( m_data.complex64Value->Imag() );
         return v;
      }

   case VariantType::I32Point:
      {
         UI16Vector v( 2 );
         v[0] = uint16( m_data.i32PointValue->x );
         v[1] = uint16( m_data.i32PointValue->y );
         return v;
      }
   case VariantType::F32Point:
      {
         UI16Vector v( 2 );
         v[0] = uint16( m_data.f32PointValue->x );
         v[1] = uint16( m_data.f32PointValue->y );
         return v;
      }
   case VariantType::F64Point:
      {
         UI16Vector v( 2 );
         v[0] = uint16( m_data.f64PointValue->x );
         v[1] = uint16( m_data.f64PointValue->y );
         return v;
      }

   case VariantType::I32Rect:
      {
         UI16Vector v( 4 );
         v[0] = uint16( m_data.i32RectValue->x0 );
         v[1] = uint16( m_data.i32RectValue->y0 );
         v[2] = uint16( m_data.i32RectValue->x1 );
         v[3] = uint16( m_data.i32RectValue->y1 );
         return v;
      }
   case VariantType::F32Rect:
      {
         UI16Vector v( 4 );
         v[0] = uint16( m_data.f32RectValue->x0 );
         v[1] = uint16( m_data.f32RectValue->y0 );
         v[2] = uint16( m_data.f32RectValue->x1 );
         v[3] = uint16( m_data.f32RectValue->y1 );
         return v;
      }
   case VariantType::F64Rect:
      {
         UI16Vector v( 4 );
         v[0] = uint16( m_data.f64RectValue->x0 );
         v[1] = uint16( m_data.f64RectValue->y0 );
         v[2] = uint16( m_data.f64RectValue->x1 );
         v[3] = uint16( m_data.f64RectValue->y1 );
         return v;
      }

   case VariantType::I8Vector:
      return UI16Vector( m_data.i8VectorValue->Begin(), m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return UI16Vector( m_data.ui8VectorValue->Begin(), m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return UI16Vector( m_data.i16VectorValue->Begin(), m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return *m_data.ui16VectorValue;
   case VariantType::I32Vector:
      return UI16Vector( m_data.i32VectorValue->Begin(), m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return UI16Vector( m_data.ui32VectorValue->Begin(), m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return UI16Vector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return UI16Vector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return UI16Vector( m_data.f32VectorValue->Begin(), m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return UI16Vector( m_data.f64VectorValue->Begin(), m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         UI16Vector v( m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint16( c->Mag() );
         return v;
      }
   case VariantType::C64Vector:
      {
         UI16Vector v( m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint16( c->Mag() );
         return v;
      }

   case VariantType::I8Matrix:
      return UI16Vector( m_data.i8MatrixValue->Begin(), int( m_data.i8MatrixValue->NumberOfElements() ) );
   case VariantType::UI8Matrix:
      return UI16Vector( m_data.ui8MatrixValue->Begin(), int( m_data.ui8MatrixValue->NumberOfElements() ) );
   case VariantType::I16Matrix:
      return UI16Vector( m_data.i16MatrixValue->Begin(), int( m_data.i16MatrixValue->NumberOfElements() ) );
   case VariantType::UI16Matrix:
      return UI16Vector( m_data.ui16MatrixValue->Begin(), int( m_data.ui16MatrixValue->NumberOfElements() ) );
   case VariantType::I32Matrix:
      return UI16Vector( m_data.i32MatrixValue->Begin(), int( m_data.i32MatrixValue->NumberOfElements() ) );
   case VariantType::UI32Matrix:
      return UI16Vector( m_data.ui32MatrixValue->Begin(), int( m_data.ui32MatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:
      return UI16Vector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix:
      return UI16Vector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::F32Matrix:
      return UI16Vector( m_data.f32MatrixValue->Begin(), int( m_data.f32MatrixValue->NumberOfElements() ) );
   case VariantType::F64Matrix:
      return UI16Vector( m_data.f64MatrixValue->Begin(), int( m_data.f64MatrixValue->NumberOfElements() ) );
   case VariantType::C32Matrix:
      {
         UI16Vector v( int( m_data.c32MatrixValue->NumberOfElements() ) );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint16( c->Mag() );
         return v;
      }
   case VariantType::C64Matrix:
      {
         UI16Vector v( int( m_data.c64MatrixValue->NumberOfElements() ) );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint16( c->Mag() );
         return v;
      }

   case VariantType::ByteArray:
      return UI16Vector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return UI16Vector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return UI16Vector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToUI16Vector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

IVector Variant::ToIVector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return IVector( int32( m_data.boolValue ), 1 );
   case VariantType::Int8:
      return IVector( int32( m_data.int8Value ), 1 );
   case VariantType::Int16:
      return IVector( int32( m_data.int16Value ), 1 );
   case VariantType::Int32:
      return IVector( m_data.int32Value, 1 );
   case VariantType::Int64:
      return IVector( int32( m_data.int64Value ), 1 );
   case VariantType::UInt8:
      return IVector( int32( m_data.uint8Value ), 1 );
   case VariantType::UInt16:
      return IVector( int32( m_data.uint16Value ), 1 );
   case VariantType::UInt32:
      return IVector( int32( m_data.uint32Value ), 1 );
   case VariantType::UInt64:
      return IVector( int32( m_data.uint64Value ), 1 );
   case VariantType::Float32:
      return IVector( int32( m_data.float32Value ), 1 );
   case VariantType::Float64:
      return IVector( int32( m_data.float64Value ), 1 );

   case VariantType::Complex32:
      {
         IVector v( 2 );
         v[0] = int32( m_data.complex32Value->Real() );
         v[1] = int32( m_data.complex32Value->Imag() );
         return v;
      }
   case VariantType::Complex64:
      {
         IVector v( 2 );
         v[0] = int32( m_data.complex64Value->Real() );
         v[1] = int32( m_data.complex64Value->Imag() );
         return v;
      }

   case VariantType::I32Point:
      {
         IVector v( 2 );
         v[0] = m_data.i32PointValue->x;
         v[1] = m_data.i32PointValue->y;
         return v;
      }
   case VariantType::F32Point:
      {
         IVector v( 2 );
         v[0] = int32( m_data.f32PointValue->x );
         v[1] = int32( m_data.f32PointValue->y );
         return v;
      }
   case VariantType::F64Point:
      {
         IVector v( 2 );
         v[0] = int32( m_data.f64PointValue->x );
         v[1] = int32( m_data.f64PointValue->y );
         return v;
      }

   case VariantType::I32Rect:
      {
         IVector v( 4 );
         v[0] = m_data.i32RectValue->x0;
         v[1] = m_data.i32RectValue->y0;
         v[2] = m_data.i32RectValue->x1;
         v[3] = m_data.i32RectValue->y1;
         return v;
      }
   case VariantType::F32Rect:
      {
         IVector v( 4 );
         v[0] = int32( m_data.f32RectValue->x0 );
         v[1] = int32( m_data.f32RectValue->y0 );
         v[2] = int32( m_data.f32RectValue->x1 );
         v[3] = int32( m_data.f32RectValue->y1 );
         return v;
      }
   case VariantType::F64Rect:
      {
         IVector v( 4 );
         v[0] = int32( m_data.f64RectValue->x0 );
         v[1] = int32( m_data.f64RectValue->y0 );
         v[2] = int32( m_data.f64RectValue->x1 );
         v[3] = int32( m_data.f64RectValue->y1 );
         return v;
      }

   case VariantType::I8Vector:
      return IVector( m_data.i8VectorValue->Begin(), m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return IVector( m_data.ui8VectorValue->Begin(), m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return IVector( m_data.i16VectorValue->Begin(), m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return IVector( m_data.ui16VectorValue->Begin(), m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return *m_data.i32VectorValue;
   case VariantType::UI32Vector:
      return IVector( m_data.ui32VectorValue->Begin(), m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return IVector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return IVector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return IVector( m_data.f32VectorValue->Begin(), m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return IVector( m_data.f64VectorValue->Begin(), m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         IVector v( m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int32( c->Mag() );
         return v;
      }
   case VariantType::C64Vector:
      {
         IVector v( m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int32( c->Mag() );
         return v;
      }

   case VariantType::I8Matrix:
      return IVector( m_data.i8MatrixValue->Begin(), int( m_data.i8MatrixValue->NumberOfElements() ) );
   case VariantType::UI8Matrix:
      return IVector( m_data.ui8MatrixValue->Begin(), int( m_data.ui8MatrixValue->NumberOfElements() ) );
   case VariantType::I16Matrix:
      return IVector( m_data.i16MatrixValue->Begin(), int( m_data.i16MatrixValue->NumberOfElements() ) );
   case VariantType::UI16Matrix:
      return IVector( m_data.ui16MatrixValue->Begin(), int( m_data.ui16MatrixValue->NumberOfElements() ) );
   case VariantType::I32Matrix:
      return IVector( m_data.i32MatrixValue->Begin(), int( m_data.i32MatrixValue->NumberOfElements() ) );
   case VariantType::UI32Matrix:
      return IVector( m_data.ui32MatrixValue->Begin(), int( m_data.ui32MatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:
      return IVector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix:
      return IVector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::F32Matrix:
      return IVector( m_data.f32MatrixValue->Begin(), int( m_data.f32MatrixValue->NumberOfElements() ) );
   case VariantType::F64Matrix:
      return IVector( m_data.f64MatrixValue->Begin(), int( m_data.f64MatrixValue->NumberOfElements() ) );
   case VariantType::C32Matrix:
      {
         IVector v( int( m_data.c32MatrixValue->NumberOfElements() ) );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int32( c->Mag() );
         return v;
      }
   case VariantType::C64Matrix:
      {
         IVector v( int( m_data.c64MatrixValue->NumberOfElements() ) );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int32( c->Mag() );
         return v;
      }

   case VariantType::ByteArray:
      return IVector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return IVector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return IVector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToIVector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

UIVector Variant::ToUIVector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return UIVector( uint32( m_data.boolValue ), 1 );
   case VariantType::Int8:
      return UIVector( uint32( m_data.int8Value ), 1 );
   case VariantType::Int16:
      return UIVector( uint32( m_data.int16Value ), 1 );
   case VariantType::Int32:
      return UIVector( uint32( m_data.int32Value ), 1 );
   case VariantType::Int64:
      return UIVector( uint32( m_data.int64Value ), 1 );
   case VariantType::UInt8:
      return UIVector( uint32( m_data.uint8Value ), 1 );
   case VariantType::UInt16:
      return UIVector( uint32( m_data.uint16Value ), 1 );
   case VariantType::UInt32:
      return UIVector( m_data.uint32Value, 1 );
   case VariantType::UInt64:
      return UIVector( uint32( m_data.uint64Value ), 1 );
   case VariantType::Float32:
      return UIVector( uint32( m_data.float32Value ), 1 );
   case VariantType::Float64:
      return UIVector( uint32( m_data.float64Value ), 1 );

   case VariantType::Complex32:
      {
         UIVector v( 2 );
         v[0] = uint32( m_data.complex32Value->Real() );
         v[1] = uint32( m_data.complex32Value->Imag() );
         return v;
      }
   case VariantType::Complex64:
      {
         UIVector v( 2 );
         v[0] = uint32( m_data.complex64Value->Real() );
         v[1] = uint32( m_data.complex64Value->Imag() );
         return v;
      }

   case VariantType::I32Point:
      {
         UIVector v( 2 );
         v[0] = uint32( m_data.i32PointValue->x );
         v[1] = uint32( m_data.i32PointValue->y );
         return v;
      }
   case VariantType::F32Point:
      {
         UIVector v( 2 );
         v[0] = uint32( m_data.f32PointValue->x );
         v[1] = uint32( m_data.f32PointValue->y );
         return v;
      }
   case VariantType::F64Point:
      {
         UIVector v( 2 );
         v[0] = uint32( m_data.f64PointValue->x );
         v[1] = uint32( m_data.f64PointValue->y );
         return v;
      }

   case VariantType::I32Rect:
      {
         UIVector v( 4 );
         v[0] = uint32( m_data.i32RectValue->x0 );
         v[1] = uint32( m_data.i32RectValue->y0 );
         v[2] = uint32( m_data.i32RectValue->x1 );
         v[3] = uint32( m_data.i32RectValue->y1 );
         return v;
      }
   case VariantType::F32Rect:
      {
         UIVector v( 4 );
         v[0] = uint32( m_data.f32RectValue->x0 );
         v[1] = uint32( m_data.f32RectValue->y0 );
         v[2] = uint32( m_data.f32RectValue->x1 );
         v[3] = uint32( m_data.f32RectValue->y1 );
         return v;
      }
   case VariantType::F64Rect:
      {
         UIVector v( 4 );
         v[0] = uint32( m_data.f64RectValue->x0 );
         v[1] = uint32( m_data.f64RectValue->y0 );
         v[2] = uint32( m_data.f64RectValue->x1 );
         v[3] = uint32( m_data.f64RectValue->y1 );
         return v;
      }

   case VariantType::I8Vector:
      return UIVector( m_data.i8VectorValue->Begin(), m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return UIVector( m_data.ui8VectorValue->Begin(), m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return UIVector( m_data.i16VectorValue->Begin(), m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return UIVector( m_data.ui16VectorValue->Begin(), m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return UIVector( m_data.i32VectorValue->Begin(), m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return *m_data.ui32VectorValue;
   case VariantType::I64Vector:
      return UIVector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return UIVector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return UIVector( m_data.f32VectorValue->Begin(), m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return UIVector( m_data.f64VectorValue->Begin(), m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         UIVector v( m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint32( c->Mag() );
         return v;
      }
   case VariantType::C64Vector:
      {
         UIVector v( m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint32( c->Mag() );
         return v;
      }

   case VariantType::I8Matrix:
      return UIVector( m_data.i8MatrixValue->Begin(), int( m_data.i8MatrixValue->NumberOfElements() ) );
   case VariantType::UI8Matrix:
      return UIVector( m_data.ui8MatrixValue->Begin(), int( m_data.ui8MatrixValue->NumberOfElements() ) );
   case VariantType::I16Matrix:
      return UIVector( m_data.i16MatrixValue->Begin(), int( m_data.i16MatrixValue->NumberOfElements() ) );
   case VariantType::UI16Matrix:
      return UIVector( m_data.ui16MatrixValue->Begin(), int( m_data.ui16MatrixValue->NumberOfElements() ) );
   case VariantType::I32Matrix:
      return UIVector( m_data.i32MatrixValue->Begin(), int( m_data.i32MatrixValue->NumberOfElements() ) );
   case VariantType::UI32Matrix:
      return UIVector( m_data.ui32MatrixValue->Begin(), int( m_data.ui32MatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:
      return UIVector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix:
      return UIVector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::F32Matrix:
      return UIVector( m_data.f32MatrixValue->Begin(), int( m_data.f32MatrixValue->NumberOfElements() ) );
   case VariantType::F64Matrix:
      return UIVector( m_data.f64MatrixValue->Begin(), int( m_data.f64MatrixValue->NumberOfElements() ) );
   case VariantType::C32Matrix:
      {
         UIVector v( int( m_data.c32MatrixValue->NumberOfElements() ) );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint32( c->Mag() );
         return v;
      }
   case VariantType::C64Matrix:
      {
         UIVector v( int( m_data.c64MatrixValue->NumberOfElements() ) );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint32( c->Mag() );
         return v;
      }

   case VariantType::ByteArray:
      return UIVector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return UIVector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return UIVector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToUIVector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

I64Vector Variant::ToI64Vector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return I64Vector( int64( m_data.boolValue ), 1 );
   case VariantType::Int8:
      return I64Vector( int64( m_data.int8Value ), 1 );
   case VariantType::Int16:
      return I64Vector( int64( m_data.int16Value ), 1 );
   case VariantType::Int32:
      return I64Vector( int64( m_data.int32Value ), 1 );
   case VariantType::Int64:
      return I64Vector( m_data.int64Value, 1 );
   case VariantType::UInt8:
      return I64Vector( int64( m_data.uint8Value ), 1 );
   case VariantType::UInt16:
      return I64Vector( int64( m_data.uint16Value ), 1 );
   case VariantType::UInt32:
      return I64Vector( int64( m_data.uint32Value ), 1 );
   case VariantType::UInt64:
      return I64Vector( int64( m_data.uint64Value ), 1 );
   case VariantType::Float32:
      return I64Vector( int64( m_data.float32Value ), 1 );
   case VariantType::Float64:
      return I64Vector( int64( m_data.float64Value ), 1 );

   case VariantType::Complex32:
      {
         I64Vector v( 2 );
         v[0] = int64( m_data.complex32Value->Real() );
         v[1] = int64( m_data.complex32Value->Imag() );
         return v;
      }
   case VariantType::Complex64:
      {
         I64Vector v( 2 );
         v[0] = int64( m_data.complex64Value->Real() );
         v[1] = int64( m_data.complex64Value->Imag() );
         return v;
      }

   case VariantType::I32Point:
      {
         I64Vector v( 2 );
         v[0] = int64( m_data.i32PointValue->x );
         v[1] = int64( m_data.i32PointValue->y );
         return v;
      }
   case VariantType::F32Point:
      {
         I64Vector v( 2 );
         v[0] = int64( m_data.f32PointValue->x );
         v[1] = int64( m_data.f32PointValue->y );
         return v;
      }
   case VariantType::F64Point:
      {
         I64Vector v( 2 );
         v[0] = int64( m_data.f64PointValue->x );
         v[1] = int64( m_data.f64PointValue->y );
         return v;
      }

   case VariantType::I32Rect:
      {
         I64Vector v( 4 );
         v[0] = int64( m_data.i32RectValue->x0 );
         v[1] = int64( m_data.i32RectValue->y0 );
         v[2] = int64( m_data.i32RectValue->x1 );
         v[3] = int64( m_data.i32RectValue->y1 );
         return v;
      }
   case VariantType::F32Rect:
      {
         I64Vector v( 4 );
         v[0] = int64( m_data.f32RectValue->x0 );
         v[1] = int64( m_data.f32RectValue->y0 );
         v[2] = int64( m_data.f32RectValue->x1 );
         v[3] = int64( m_data.f32RectValue->y1 );
         return v;
      }
   case VariantType::F64Rect:
      {
         I64Vector v( 4 );
         v[0] = int64( m_data.f64RectValue->x0 );
         v[1] = int64( m_data.f64RectValue->y0 );
         v[2] = int64( m_data.f64RectValue->x1 );
         v[3] = int64( m_data.f64RectValue->y1 );
         return v;
      }

   case VariantType::I8Vector:
      return I64Vector( m_data.i8VectorValue->Begin(), m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return I64Vector( m_data.ui8VectorValue->Begin(), m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return I64Vector( m_data.i16VectorValue->Begin(), m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return I64Vector( m_data.ui16VectorValue->Begin(), m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return I64Vector( m_data.i32VectorValue->Begin(), m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return I64Vector( m_data.ui32VectorValue->Begin(), m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return *m_data.i64VectorValue;
   case VariantType::UI64Vector:
      return I64Vector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return I64Vector( m_data.f32VectorValue->Begin(), m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return I64Vector( m_data.f64VectorValue->Begin(), m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         I64Vector v( m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int64( c->Mag() );
         return v;
      }
   case VariantType::C64Vector:
      {
         I64Vector v( m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int64( c->Mag() );
         return v;
      }

   case VariantType::I8Matrix:
      return I64Vector( m_data.i8MatrixValue->Begin(), int( m_data.i8MatrixValue->NumberOfElements() ) );
   case VariantType::UI8Matrix:
      return I64Vector( m_data.ui8MatrixValue->Begin(), int( m_data.ui8MatrixValue->NumberOfElements() ) );
   case VariantType::I16Matrix:
      return I64Vector( m_data.i16MatrixValue->Begin(), int( m_data.i16MatrixValue->NumberOfElements() ) );
   case VariantType::UI16Matrix:
      return I64Vector( m_data.ui16MatrixValue->Begin(), int( m_data.ui16MatrixValue->NumberOfElements() ) );
   case VariantType::I32Matrix:
      return I64Vector( m_data.i32MatrixValue->Begin(), int( m_data.i32MatrixValue->NumberOfElements() ) );
   case VariantType::UI32Matrix:
      return I64Vector( m_data.ui32MatrixValue->Begin(), int( m_data.ui32MatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:
      return I64Vector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix:
      return I64Vector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::F32Matrix:
      return I64Vector( m_data.f32MatrixValue->Begin(), int( m_data.f32MatrixValue->NumberOfElements() ) );
   case VariantType::F64Matrix:
      return I64Vector( m_data.f64MatrixValue->Begin(), int( m_data.f64MatrixValue->NumberOfElements() ) );
   case VariantType::C32Matrix:
      {
         I64Vector v( int( m_data.c32MatrixValue->NumberOfElements() ) );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int64( c->Mag() );
         return v;
      }
   case VariantType::C64Matrix:
      {
         I64Vector v( int( m_data.c64MatrixValue->NumberOfElements() ) );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = int64( c->Mag() );
         return v;
      }

   case VariantType::ByteArray:
      return I64Vector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return I64Vector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return I64Vector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToI64Vector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

UI64Vector Variant::ToUI64Vector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return UI64Vector( uint64( m_data.boolValue ), 1 );
   case VariantType::Int8:
      return UI64Vector( uint64( m_data.int8Value ), 1 );
   case VariantType::Int16:
      return UI64Vector( uint64( m_data.int16Value ), 1 );
   case VariantType::Int32:
      return UI64Vector( uint64( m_data.int32Value ), 1 );
   case VariantType::Int64:
      return UI64Vector( uint64( m_data.int64Value ), 1 );
   case VariantType::UInt8:
      return UI64Vector( uint64( m_data.uint8Value ), 1 );
   case VariantType::UInt16:
      return UI64Vector( uint64( m_data.uint16Value ), 1 );
   case VariantType::UInt32:
      return UI64Vector( uint64( m_data.uint32Value ), 1 );
   case VariantType::UInt64:
      return UI64Vector( m_data.uint64Value, 1 );
   case VariantType::Float32:
      return UI64Vector( uint64( m_data.float32Value ), 1 );
   case VariantType::Float64:
      return UI64Vector( uint64( m_data.float64Value ), 1 );

   case VariantType::Complex32:
      {
         UI64Vector v( 2 );
         v[0] = uint64( m_data.complex32Value->Real() );
         v[1] = uint64( m_data.complex32Value->Imag() );
         return v;
      }
   case VariantType::Complex64:
      {
         UI64Vector v( 2 );
         v[0] = uint64( m_data.complex64Value->Real() );
         v[1] = uint64( m_data.complex64Value->Imag() );
         return v;
      }

   case VariantType::I32Point:
      {
         UI64Vector v( 2 );
         v[0] = uint64( m_data.i32PointValue->x );
         v[1] = uint64( m_data.i32PointValue->y );
         return v;
      }
   case VariantType::F32Point:
      {
         UI64Vector v( 2 );
         v[0] = uint64( m_data.f32PointValue->x );
         v[1] = uint64( m_data.f32PointValue->y );
         return v;
      }
   case VariantType::F64Point:
      {
         UI64Vector v( 2 );
         v[0] = uint64( m_data.f64PointValue->x );
         v[1] = uint64( m_data.f64PointValue->y );
         return v;
      }

   case VariantType::I32Rect:
      {
         UI64Vector v( 4 );
         v[0] = uint64( m_data.i32RectValue->x0 );
         v[1] = uint64( m_data.i32RectValue->y0 );
         v[2] = uint64( m_data.i32RectValue->x1 );
         v[3] = uint64( m_data.i32RectValue->y1 );
         return v;
      }
   case VariantType::F32Rect:
      {
         UI64Vector v( 4 );
         v[0] = uint64( m_data.f32RectValue->x0 );
         v[1] = uint64( m_data.f32RectValue->y0 );
         v[2] = uint64( m_data.f32RectValue->x1 );
         v[3] = uint64( m_data.f32RectValue->y1 );
         return v;
      }
   case VariantType::F64Rect:
      {
         UI64Vector v( 4 );
         v[0] = uint64( m_data.f64RectValue->x0 );
         v[1] = uint64( m_data.f64RectValue->y0 );
         v[2] = uint64( m_data.f64RectValue->x1 );
         v[3] = uint64( m_data.f64RectValue->y1 );
         return v;
      }

   case VariantType::I8Vector:
      return UI64Vector( m_data.i8VectorValue->Begin(), m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return UI64Vector( m_data.ui8VectorValue->Begin(), m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return UI64Vector( m_data.i16VectorValue->Begin(), m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return UI64Vector( m_data.ui16VectorValue->Begin(), m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return UI64Vector( m_data.i32VectorValue->Begin(), m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return UI64Vector( m_data.ui32VectorValue->Begin(), m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return UI64Vector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return *m_data.ui64VectorValue;
   case VariantType::F32Vector:
      return UI64Vector( m_data.f32VectorValue->Begin(), m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return UI64Vector( m_data.f64VectorValue->Begin(), m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         UI64Vector v( m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint64( c->Mag() );
         return v;
      }
   case VariantType::C64Vector:
      {
         UI64Vector v( m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint64( c->Mag() );
         return v;
      }

   case VariantType::I8Matrix:
      return UI64Vector( m_data.i8MatrixValue->Begin(), int( m_data.i8MatrixValue->NumberOfElements() ) );
   case VariantType::UI8Matrix:
      return UI64Vector( m_data.ui8MatrixValue->Begin(), int( m_data.ui8MatrixValue->NumberOfElements() ) );
   case VariantType::I16Matrix:
      return UI64Vector( m_data.i16MatrixValue->Begin(), int( m_data.i16MatrixValue->NumberOfElements() ) );
   case VariantType::UI16Matrix:
      return UI64Vector( m_data.ui16MatrixValue->Begin(), int( m_data.ui16MatrixValue->NumberOfElements() ) );
   case VariantType::I32Matrix:
      return UI64Vector( m_data.i32MatrixValue->Begin(), int( m_data.i32MatrixValue->NumberOfElements() ) );
   case VariantType::UI32Matrix:
      return UI64Vector( m_data.ui32MatrixValue->Begin(), int( m_data.ui32MatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:
      return UI64Vector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix:
      return UI64Vector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::F32Matrix:
      return UI64Vector( m_data.f32MatrixValue->Begin(), int( m_data.f32MatrixValue->NumberOfElements() ) );
   case VariantType::F64Matrix:
      return UI64Vector( m_data.f64MatrixValue->Begin(), int( m_data.f64MatrixValue->NumberOfElements() ) );
   case VariantType::C32Matrix:
      {
         UI64Vector v( int( m_data.c32MatrixValue->NumberOfElements() ) );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint64( c->Mag() );
         return v;
      }
   case VariantType::C64Matrix:
      {
         UI64Vector v( int( m_data.c64MatrixValue->NumberOfElements() ) );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = uint64( c->Mag() );
         return v;
      }

   case VariantType::ByteArray:
      return UI64Vector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return UI64Vector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return UI64Vector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToUI64Vector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

FVector Variant::ToFVector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return FVector( float( m_data.boolValue ), 1 );
   case VariantType::Int8:
      return FVector( float( m_data.int8Value ), 1 );
   case VariantType::Int16:
      return FVector( float( m_data.int16Value ), 1 );
   case VariantType::Int32:
      return FVector( float( m_data.int32Value ), 1 );
   case VariantType::Int64:
      return FVector( float( m_data.int64Value ), 1 );
   case VariantType::UInt8:
      return FVector( float( m_data.uint8Value ), 1 );
   case VariantType::UInt16:
      return FVector( float( m_data.uint16Value ), 1 );
   case VariantType::UInt32:
      return FVector( float( m_data.uint32Value ), 1 );
   case VariantType::UInt64:
      return FVector( float( m_data.uint64Value ), 1 );
   case VariantType::Float32:
      return FVector( m_data.float32Value, 1 );
   case VariantType::Float64:
      return FVector( float( m_data.float64Value ), 1 );

   case VariantType::Complex32:
      {
         FVector v( 2 );
         v[0] = m_data.complex32Value->Real();
         v[1] = m_data.complex32Value->Imag();
         return v;
      }
   case VariantType::Complex64:
      {
         FVector v( 2 );
         v[0] = float( m_data.complex64Value->Real() );
         v[1] = float( m_data.complex64Value->Imag() );
         return v;
      }

   case VariantType::TimePoint:
      {
         FVector v( 2 );
         v[0] = float( m_data.timePointValue->JDI() );
         v[1] = float( m_data.timePointValue->JDF() );
         return v;
      }

   case VariantType::I32Point:
      {
         FVector v( 2 );
         v[0] = float( m_data.i32PointValue->x );
         v[1] = float( m_data.i32PointValue->y );
         return v;
      }
   case VariantType::F32Point:
      {
         FVector v( 2 );
         v[0] = m_data.f32PointValue->x;
         v[1] = m_data.f32PointValue->y;
         return v;
      }
   case VariantType::F64Point:
      {
         FVector v( 2 );
         v[0] = float( m_data.f64PointValue->x );
         v[1] = float( m_data.f64PointValue->y );
         return v;
      }

   case VariantType::I32Rect:
      {
         FVector v( 4 );
         v[0] = float( m_data.i32RectValue->x0 );
         v[1] = float( m_data.i32RectValue->y0 );
         v[2] = float( m_data.i32RectValue->x1 );
         v[3] = float( m_data.i32RectValue->y1 );
         return v;
      }
   case VariantType::F32Rect:
      {
         FVector v( 4 );
         v[0] = m_data.f32RectValue->x0;
         v[1] = m_data.f32RectValue->y0;
         v[2] = m_data.f32RectValue->x1;
         v[3] = m_data.f32RectValue->y1;
         return v;
      }
   case VariantType::F64Rect:
      {
         FVector v( 4 );
         v[0] = float( m_data.f64RectValue->x0 );
         v[1] = float( m_data.f64RectValue->y0 );
         v[2] = float( m_data.f64RectValue->x1 );
         v[3] = float( m_data.f64RectValue->y1 );
         return v;
      }

   case VariantType::I8Vector:
      return FVector( m_data.i8VectorValue->Begin(), m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return FVector( m_data.ui8VectorValue->Begin(), m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return FVector( m_data.i16VectorValue->Begin(), m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return FVector( m_data.ui16VectorValue->Begin(), m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return FVector( m_data.i32VectorValue->Begin(), m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return FVector( m_data.ui32VectorValue->Begin(), m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return FVector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return FVector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return *m_data.f32VectorValue;
   case VariantType::F64Vector:
      return FVector( m_data.f64VectorValue->Begin(), m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         FVector v( m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = c->Mag();
         return v;
      }
   case VariantType::C64Vector:
      {
         FVector v( m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = float( c->Mag() );
         return v;
      }

   case VariantType::I8Matrix:
      return FVector( m_data.i8MatrixValue->Begin(), int( m_data.i8MatrixValue->NumberOfElements() ) );
   case VariantType::UI8Matrix:
      return FVector( m_data.ui8MatrixValue->Begin(), int( m_data.ui8MatrixValue->NumberOfElements() ) );
   case VariantType::I16Matrix:
      return FVector( m_data.i16MatrixValue->Begin(), int( m_data.i16MatrixValue->NumberOfElements() ) );
   case VariantType::UI16Matrix:
      return FVector( m_data.ui16MatrixValue->Begin(), int( m_data.ui16MatrixValue->NumberOfElements() ) );
   case VariantType::I32Matrix:
      return FVector( m_data.i32MatrixValue->Begin(), int( m_data.i32MatrixValue->NumberOfElements() ) );
   case VariantType::UI32Matrix:
      return FVector( m_data.ui32MatrixValue->Begin(), int( m_data.ui32MatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:
      return FVector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix:
      return FVector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::F32Matrix:
      return FVector( m_data.f32MatrixValue->Begin(), int( m_data.f32MatrixValue->NumberOfElements() ) );
   case VariantType::F64Matrix:
      return FVector( m_data.f64MatrixValue->Begin(), int( m_data.f64MatrixValue->NumberOfElements() ) );
   case VariantType::C32Matrix:
      {
         FVector v( int( m_data.c32MatrixValue->NumberOfElements() ) );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = c->Mag();
         return v;
      }
   case VariantType::C64Matrix:
      {
         FVector v( int( m_data.c64MatrixValue->NumberOfElements() ) );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = float( c->Mag() );
         return v;
      }

   case VariantType::ByteArray:
      return FVector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return FVector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return FVector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToFVector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

DVector Variant::ToDVector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return DVector( double( m_data.boolValue ), 1 );
   case VariantType::Int8:
      return DVector( double( m_data.int8Value ), 1 );
   case VariantType::Int16:
      return DVector( double( m_data.int16Value ), 1 );
   case VariantType::Int32:
      return DVector( double( m_data.int32Value ), 1 );
   case VariantType::Int64:
      return DVector( double( m_data.int64Value ), 1 );
   case VariantType::UInt8:
      return DVector( double( m_data.uint8Value ), 1 );
   case VariantType::UInt16:
      return DVector( double( m_data.uint16Value ), 1 );
   case VariantType::UInt32:
      return DVector( double( m_data.uint32Value ), 1 );
   case VariantType::UInt64:
      return DVector( double( m_data.uint64Value ), 1 );
   case VariantType::Float32:
      return DVector( double( m_data.float32Value ), 1 );
   case VariantType::Float64:
      return DVector( m_data.float64Value, 1 );

   case VariantType::Complex32:
      {
         DVector v( 2 );
         v[0] = double( m_data.complex32Value->Real() );
         v[1] = double( m_data.complex32Value->Imag() );
         return v;
      }
   case VariantType::Complex64:
      {
         DVector v( 2 );
         v[0] = m_data.complex64Value->Real();
         v[1] = m_data.complex64Value->Imag();
         return v;
      }

   case VariantType::TimePoint:
      {
         DVector v( 2 );
         v[0] = double( m_data.timePointValue->JDI() );
         v[1] = double( m_data.timePointValue->JDF() );
         return v;
      }

   case VariantType::I32Point:
      {
         DVector v( 2 );
         v[0] = double( m_data.i32PointValue->x );
         v[1] = double( m_data.i32PointValue->y );
         return v;
      }
   case VariantType::F32Point:
      {
         DVector v( 2 );
         v[0] = double( m_data.f32PointValue->x );
         v[1] = double( m_data.f32PointValue->y );
         return v;
      }
   case VariantType::F64Point:
      {
         DVector v( 2 );
         v[0] = m_data.f64PointValue->x;
         v[1] = m_data.f64PointValue->y;
         return v;
      }

   case VariantType::I32Rect:
      {
         DVector v( 4 );
         v[0] = double( m_data.i32RectValue->x0 );
         v[1] = double( m_data.i32RectValue->y0 );
         v[2] = double( m_data.i32RectValue->x1 );
         v[3] = double( m_data.i32RectValue->y1 );
         return v;
      }
   case VariantType::F32Rect:
      {
         DVector v( 4 );
         v[0] = double( m_data.f32RectValue->x0 );
         v[1] = double( m_data.f32RectValue->y0 );
         v[2] = double( m_data.f32RectValue->x1 );
         v[3] = double( m_data.f32RectValue->y1 );
         return v;
      }
   case VariantType::F64Rect:
      {
         DVector v( 4 );
         v[0] = m_data.f64RectValue->x0;
         v[1] = m_data.f64RectValue->y0;
         v[2] = m_data.f64RectValue->x1;
         v[3] = m_data.f64RectValue->y1;
         return v;
      }

   case VariantType::I8Vector:
      return DVector( m_data.i8VectorValue->Begin(), m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return DVector( m_data.ui8VectorValue->Begin(), m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return DVector( m_data.i16VectorValue->Begin(), m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return DVector( m_data.ui16VectorValue->Begin(), m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return DVector( m_data.i32VectorValue->Begin(), m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return DVector( m_data.ui32VectorValue->Begin(), m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return DVector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return DVector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return DVector( m_data.f32VectorValue->Begin(), m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return *m_data.f64VectorValue;
   case VariantType::C32Vector:
      {
         DVector v( m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = double( c->Mag() );
         return v;
      }
   case VariantType::C64Vector:
      {
         DVector v( m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = c->Mag();
         return v;
      }

   case VariantType::I8Matrix:
      return DVector( m_data.i8MatrixValue->Begin(), int( m_data.i8MatrixValue->NumberOfElements() ) );
   case VariantType::UI8Matrix:
      return DVector( m_data.ui8MatrixValue->Begin(), int( m_data.ui8MatrixValue->NumberOfElements() ) );
   case VariantType::I16Matrix:
      return DVector( m_data.i16MatrixValue->Begin(), int( m_data.i16MatrixValue->NumberOfElements() ) );
   case VariantType::UI16Matrix:
      return DVector( m_data.ui16MatrixValue->Begin(), int( m_data.ui16MatrixValue->NumberOfElements() ) );
   case VariantType::I32Matrix:
      return DVector( m_data.i32MatrixValue->Begin(), int( m_data.i32MatrixValue->NumberOfElements() ) );
   case VariantType::UI32Matrix:
      return DVector( m_data.ui32MatrixValue->Begin(), int( m_data.ui32MatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:
      return DVector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix:
      return DVector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::F32Matrix:
      return DVector( m_data.f32MatrixValue->Begin(), int( m_data.f32MatrixValue->NumberOfElements() ) );
   case VariantType::F64Matrix:
      return DVector( m_data.f64MatrixValue->Begin(), int( m_data.f64MatrixValue->NumberOfElements() ) );
   case VariantType::C32Matrix:
      {
         DVector v( int( m_data.c32MatrixValue->NumberOfElements() ) );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = double( c->Mag() );
         return v;
      }
   case VariantType::C64Matrix:
      {
         DVector v( int( m_data.c64MatrixValue->NumberOfElements() ) );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = c->Mag();
         return v;
      }

   case VariantType::ByteArray:
      return DVector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return DVector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return DVector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToDVector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

C32Vector Variant::ToC32Vector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return C32Vector( Complex32( float( m_data.boolValue ) ), 1 );
   case VariantType::Int8:
      return C32Vector( Complex32( float( m_data.int8Value ) ), 1 );
   case VariantType::Int16:
      return C32Vector( Complex32( float( m_data.int16Value ) ), 1 );
   case VariantType::Int32:
      return C32Vector( Complex32( float( m_data.int32Value ) ), 1 );
   case VariantType::Int64:
      return C32Vector( Complex32( float( m_data.int64Value ) ), 1 );
   case VariantType::UInt8:
      return C32Vector( Complex32( float( m_data.uint8Value ) ), 1 );
   case VariantType::UInt16:
      return C32Vector( Complex32( float( m_data.uint16Value ) ), 1 );
   case VariantType::UInt32:
      return C32Vector( Complex32( float( m_data.uint32Value ) ), 1 );
   case VariantType::UInt64:
      return C32Vector( Complex32( float( m_data.uint64Value ) ), 1 );
   case VariantType::Float32:
      return C32Vector( Complex32( m_data.float32Value ), 1 );
   case VariantType::Float64:
      return C32Vector( Complex32( float( m_data.float64Value ) ), 1 );

   case VariantType::Complex32:
      return C32Vector( *m_data.complex32Value, 1 );
   case VariantType::Complex64:
      return C32Vector( Complex32( *m_data.complex64Value ), 1 );

   case VariantType::TimePoint:
      return C32Vector( Complex32( float( m_data.timePointValue->JDI() ), float( m_data.timePointValue->JDF() ) ), 1 );

   case VariantType::I32Point:
      return C32Vector( Complex32( float( m_data.i32PointValue->x ), float( m_data.i32PointValue->y ) ), 1 );
   case VariantType::F32Point:
      return C32Vector( Complex32( m_data.f32PointValue->x, m_data.f32PointValue->y ), 1 );
   case VariantType::F64Point:
      return C32Vector( Complex32( float( m_data.f64PointValue->x ), float( m_data.f64PointValue->y ) ), 1 );

   case VariantType::I32Rect:
      {
         C32Vector v( 2 );
         v[0].Real() = float( m_data.i32RectValue->x0 );
         v[0].Imag() = float( m_data.i32RectValue->y0 );
         v[1].Real() = float( m_data.i32RectValue->x1 );
         v[1].Imag() = float( m_data.i32RectValue->y1 );
         return v;
      }
   case VariantType::F32Rect:
      {
         C32Vector v( 2 );
         v[0].Real() = m_data.f32RectValue->x0;
         v[0].Imag() = m_data.f32RectValue->y0;
         v[1].Real() = m_data.f32RectValue->x1;
         v[1].Imag() = m_data.f32RectValue->y1;
         return v;
      }
   case VariantType::F64Rect:
      {
         C32Vector v( 2 );
         v[0].Real() = float( m_data.f64RectValue->x0 );
         v[0].Imag() = float( m_data.f64RectValue->y0 );
         v[1].Real() = float( m_data.f64RectValue->x1 );
         v[1].Imag() = float( m_data.f64RectValue->y1 );
         return v;
      }

   case VariantType::I8Vector:
      {
         C32Vector v( m_data.i8VectorValue->Length() );
         const int8* r = m_data.i8VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI8Vector:
      {
         C32Vector v( m_data.ui8VectorValue->Length() );
         const uint8* r = m_data.ui8VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::I16Vector:
      {
         C32Vector v( m_data.i16VectorValue->Length() );
         const int16* r = m_data.i16VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI16Vector:
      {
         C32Vector v( m_data.ui16VectorValue->Length() );
         const uint16* r = m_data.ui16VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::I32Vector:
      {
         C32Vector v( m_data.i32VectorValue->Length() );
         const int32* r = m_data.i32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI32Vector:
      {
         C32Vector v( m_data.ui32VectorValue->Length() );
         const uint32* r = m_data.ui32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::I64Vector:
      {
         C32Vector v( m_data.i64VectorValue->Length() );
         const int64* r = m_data.i64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI64Vector:
      {
         C32Vector v( m_data.ui64VectorValue->Length() );
         const uint64* r = m_data.ui64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::F32Vector:
      {
         C32Vector v( m_data.f32VectorValue->Length() );
         const float* r = m_data.f32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = *r;
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::F64Vector:
      {
         C32Vector v( m_data.f64VectorValue->Length() );
         const double* r = m_data.f64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::C32Vector:
      {
         return *m_data.c32VectorValue;
      }
   case VariantType::C64Vector:
      {
         C32Vector v( m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = *c;
         return v;
      }

   case VariantType::I8Matrix:
      {
         C32Vector v( int( m_data.i8MatrixValue->NumberOfElements() ) );
         const int8* r = m_data.i8MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI8Matrix:
      {
         C32Vector v( int( m_data.ui8MatrixValue->NumberOfElements() ) );
         const uint8* r = m_data.ui8MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::I16Matrix:
      {
         C32Vector v( int( m_data.i16MatrixValue->NumberOfElements() ) );
         const int16* r = m_data.i16MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI16Matrix:
      {
         C32Vector v( int( m_data.ui16MatrixValue->NumberOfElements() ) );
         const uint16* r = m_data.ui16MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::I32Matrix:
      {
         C32Vector v( int( m_data.i32MatrixValue->NumberOfElements() ) );
         const int32* r = m_data.i32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI32Matrix:
      {
         C32Vector v( int( m_data.ui32MatrixValue->NumberOfElements() ) );
         const uint32* r = m_data.ui32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::I64Matrix:
      {
         C32Vector v( int( m_data.i64MatrixValue->NumberOfElements() ) );
         const int64* r = m_data.i64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI64Matrix:
      {
         C32Vector v( int( m_data.ui64MatrixValue->NumberOfElements() ) );
         const uint64* r = m_data.ui64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::F32Matrix:
      {
         C32Vector v( int( m_data.f32MatrixValue->NumberOfElements() ) );
         const float* r = m_data.f32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = *r;
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::F64Matrix:
      {
         C32Vector v( int( m_data.f64MatrixValue->NumberOfElements() ) );
         const double* r = m_data.f64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::C32Matrix:
      {
         return C32Vector( m_data.c32MatrixValue->Begin(), int( m_data.c32MatrixValue->NumberOfElements() ) );
      }
   case VariantType::C64Matrix:
      {
         C32Vector v( int( m_data.c64MatrixValue->NumberOfElements() ) );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = *c;
         return v;
      }

   case VariantType::ByteArray:
      {
         C32Vector v( int( m_data.byteArrayValue->Length() ) );
         const uint8* r = m_data.byteArrayValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = float( *r );
            v[i].Imag() = 0;
         }
         return v;
      }

   default:
      throw Error( "Variant::ToC32Vector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

C64Vector Variant::ToC64Vector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return C64Vector( Complex64( double( m_data.boolValue ) ), 1 );
   case VariantType::Int8:
      return C64Vector( Complex64( double( m_data.int8Value ) ), 1 );
   case VariantType::Int16:
      return C64Vector( Complex64( double( m_data.int16Value ) ), 1 );
   case VariantType::Int32:
      return C64Vector( Complex64( double( m_data.int32Value ) ), 1 );
   case VariantType::Int64:
      return C64Vector( Complex64( double( m_data.int64Value ) ), 1 );
   case VariantType::UInt8:
      return C64Vector( Complex64( double( m_data.uint8Value ) ), 1 );
   case VariantType::UInt16:
      return C64Vector( Complex64( double( m_data.uint16Value ) ), 1 );
   case VariantType::UInt32:
      return C64Vector( Complex64( double( m_data.uint32Value ) ), 1 );
   case VariantType::UInt64:
      return C64Vector( Complex64( double( m_data.uint64Value ) ), 1 );
   case VariantType::Float32:
      return C64Vector( Complex64( double( m_data.float32Value ) ), 1 );
   case VariantType::Float64:
      return C64Vector( Complex64( m_data.float64Value ), 1 );

   case VariantType::Complex32:
      return C64Vector( Complex64( *m_data.complex32Value ), 1 );
   case VariantType::Complex64:
      return C64Vector( *m_data.complex64Value, 1 );

   case VariantType::TimePoint:
      return C64Vector( Complex64( double( m_data.timePointValue->JDI() ), double( m_data.timePointValue->JDF() ) ), 1 );

   case VariantType::I32Point:
      return C64Vector( Complex64( double( m_data.i32PointValue->x ), double( m_data.i32PointValue->y ) ), 1 );
   case VariantType::F32Point:
      return C64Vector( Complex64( double( m_data.f32PointValue->x ), double( m_data.f32PointValue->y ) ), 1 );
   case VariantType::F64Point:
      return C64Vector( Complex64( m_data.f64PointValue->x, m_data.f64PointValue->y ), 1 );

   case VariantType::I32Rect:
      {
         C64Vector v( 2 );
         v[0].Real() = double( m_data.i32RectValue->x0 );
         v[0].Imag() = double( m_data.i32RectValue->y0 );
         v[1].Real() = double( m_data.i32RectValue->x1 );
         v[1].Imag() = double( m_data.i32RectValue->y1 );
         return v;
      }
   case VariantType::F32Rect:
      {
         C64Vector v( 2 );
         v[0].Real() = double( m_data.f32RectValue->x0 );
         v[0].Imag() = double( m_data.f32RectValue->y0 );
         v[1].Real() = double( m_data.f32RectValue->x1 );
         v[1].Imag() = double( m_data.f32RectValue->y1 );
         return v;
      }
   case VariantType::F64Rect:
      {
         C64Vector v( 2 );
         v[0].Real() = m_data.f64RectValue->x0;
         v[0].Imag() = m_data.f64RectValue->y0;
         v[1].Real() = m_data.f64RectValue->x1;
         v[1].Imag() = m_data.f64RectValue->y1;
         return v;
      }

   case VariantType::I8Vector:
      {
         C64Vector v( m_data.i8VectorValue->Length() );
         const int8* r = m_data.i8VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI8Vector:
      {
         C64Vector v( m_data.ui8VectorValue->Length() );
         const uint8* r = m_data.ui8VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::I16Vector:
      {
         C64Vector v( m_data.i16VectorValue->Length() );
         const int16* r = m_data.i16VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI16Vector:
      {
         C64Vector v( m_data.ui16VectorValue->Length() );
         const uint16* r = m_data.ui16VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::I32Vector:
      {
         C64Vector v( m_data.i32VectorValue->Length() );
         const int32* r = m_data.i32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI32Vector:
      {
         C64Vector v( m_data.ui32VectorValue->Length() );
         const uint32* r = m_data.ui32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::I64Vector:
      {
         C64Vector v( m_data.i64VectorValue->Length() );
         const int64* r = m_data.i64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI64Vector:
      {
         C64Vector v( m_data.ui64VectorValue->Length() );
         const uint64* r = m_data.ui64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::F32Vector:
      {
         C64Vector v( m_data.f32VectorValue->Length() );
         const float* r = m_data.f32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::F64Vector:
      {
         C64Vector v( m_data.f64VectorValue->Length() );
         const double* r = m_data.f64VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = *r;
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::C32Vector:
      {
         C64Vector v( m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = *c;
         return v;
      }
   case VariantType::C64Vector:
      {
         return *m_data.c64VectorValue;
      }

   case VariantType::I8Matrix:
      {
         C64Vector v( int( m_data.i8MatrixValue->NumberOfElements() ) );
         const int8* r = m_data.i8MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI8Matrix:
      {
         C64Vector v( int( m_data.ui8MatrixValue->NumberOfElements() ) );
         const uint8* r = m_data.ui8MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::I16Matrix:
      {
         C64Vector v( int( m_data.i16MatrixValue->NumberOfElements() ) );
         const int16* r = m_data.i16MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI16Matrix:
      {
         C64Vector v( int( m_data.ui16MatrixValue->NumberOfElements() ) );
         const uint16* r = m_data.ui16MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::I32Matrix:
      {
         C64Vector v( int( m_data.i32MatrixValue->NumberOfElements() ) );
         const int32* r = m_data.i32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI32Matrix:
      {
         C64Vector v( int( m_data.ui32MatrixValue->NumberOfElements() ) );
         const uint32* r = m_data.ui32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::I64Matrix:
      {
         C64Vector v( int( m_data.i64MatrixValue->NumberOfElements() ) );
         const int64* r = m_data.i64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::UI64Matrix:
      {
         C64Vector v( int( m_data.ui64MatrixValue->NumberOfElements() ) );
         const uint64* r = m_data.ui64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::F32Matrix:
      {
         C64Vector v( int( m_data.f32MatrixValue->NumberOfElements() ) );
         const float* r = m_data.f32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::F64Matrix:
      {
         C64Vector v( int( m_data.f64MatrixValue->NumberOfElements() ) );
         const double* r = m_data.f64MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = *r;
            v[i].Imag() = 0;
         }
         return v;
      }
   case VariantType::C32Matrix:
      {
         C64Vector v( int( m_data.c32MatrixValue->NumberOfElements() ) );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++c )
            v[i] = *c;
         return v;
      }
   case VariantType::C64Matrix:
      {
         return C64Vector( m_data.c64MatrixValue->Begin(), int( m_data.c64MatrixValue->NumberOfElements() ) );
      }

   case VariantType::ByteArray:
      {
         C64Vector v( int( m_data.byteArrayValue->Length() ) );
         const uint8* r = m_data.byteArrayValue->Begin();
         for ( int i = 0; i < v.Length(); ++i, ++r )
         {
            v[i].Real() = double( *r );
            v[i].Imag() = 0;
         }
         return v;
      }

   default:
      throw Error( "Variant::ToC64Vector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

CharMatrix Variant::ToCharMatrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return CharMatrix( int8( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:
      return CharMatrix( m_data.int8Value, 1, 1 );
   case VariantType::Int16:
      return CharMatrix( int8( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:
      return CharMatrix( int8( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:
      return CharMatrix( int8( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:
      return CharMatrix( int8( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:
      return CharMatrix( int8( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:
      return CharMatrix( int8( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:
      return CharMatrix( int8( m_data.uint64Value ), 1, 1 );
   case VariantType::Float32:
      return CharMatrix( int8( m_data.float32Value ), 1, 1 );
   case VariantType::Float64:
      return CharMatrix( int8( m_data.float64Value ), 1, 1 );

   case VariantType::Complex32:
      {
         CharMatrix m( 1, 2 );
         m[0][0] = int8( m_data.complex32Value->Real() );
         m[0][1] = int8( m_data.complex32Value->Imag() );
         return m;
      }
   case VariantType::Complex64:
      {
         CharMatrix m( 1, 2 );
         m[0][0] = int8( m_data.complex64Value->Real() );
         m[0][1] = int8( m_data.complex64Value->Imag() );
         return m;
      }

   case VariantType::I32Point:
      {
         CharMatrix m( 1, 2 );
         m[0][0] = int8( m_data.i32PointValue->x );
         m[0][1] = int8( m_data.i32PointValue->y );
         return m;
      }
   case VariantType::F32Point:
      {
         CharMatrix m( 1, 2 );
         m[0][0] = int8( m_data.f32PointValue->x );
         m[0][1] = int8( m_data.f32PointValue->y );
         return m;
      }
   case VariantType::F64Point:
      {
         CharMatrix m( 1, 2 );
         m[0][0] = int8( m_data.f64PointValue->x );
         m[0][1] = int8( m_data.f64PointValue->y );
         return m;
      }

   case VariantType::I32Rect:
      {
         CharMatrix m( 2, 2 );
         m[0][0] = int8( m_data.i32RectValue->x0 );
         m[0][1] = int8( m_data.i32RectValue->y0 );
         m[1][0] = int8( m_data.i32RectValue->x1 );
         m[1][1] = int8( m_data.i32RectValue->y1 );
         return m;
      }
   case VariantType::F32Rect:
      {
         CharMatrix m( 2, 2 );
         m[0][0] = int8( m_data.f32RectValue->x0 );
         m[0][1] = int8( m_data.f32RectValue->y0 );
         m[1][0] = int8( m_data.f32RectValue->x1 );
         m[1][1] = int8( m_data.f32RectValue->y1 );
         return m;
      }
   case VariantType::F64Rect:
      {
         CharMatrix m( 2, 2 );
         m[0][0] = int8( m_data.f64RectValue->x0 );
         m[0][1] = int8( m_data.f64RectValue->y0 );
         m[1][0] = int8( m_data.f64RectValue->x1 );
         m[1][1] = int8( m_data.f64RectValue->y1 );
         return m;
      }

   case VariantType::I8Vector:
      return CharMatrix( m_data.i8VectorValue->Begin(), 1, m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return CharMatrix( m_data.ui8VectorValue->Begin(), 1, m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return CharMatrix( m_data.i16VectorValue->Begin(), 1, m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return CharMatrix( m_data.ui16VectorValue->Begin(), 1, m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return CharMatrix( m_data.i32VectorValue->Begin(), 1, m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return CharMatrix( m_data.ui32VectorValue->Begin(), 1, m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return CharMatrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return CharMatrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return CharMatrix( m_data.f32VectorValue->Begin(), 1, m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return CharMatrix( m_data.f64VectorValue->Begin(), 1, m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         CharMatrix m( 1, m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         int8* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = int8( c->Mag() );
         return m;
      }
   case VariantType::C64Vector:
      {
         CharMatrix m( 1, m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         int8* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = int8( c->Mag() );
         return m;
      }

   case VariantType::I8Matrix:
      return *m_data.i8MatrixValue;
   case VariantType::UI8Matrix:
      return CharMatrix( m_data.ui8MatrixValue->Begin(), m_data.ui8MatrixValue->Rows(), m_data.ui8MatrixValue->Cols() );
   case VariantType::I16Matrix:
      return CharMatrix( m_data.i16MatrixValue->Begin(), m_data.i16MatrixValue->Rows(), m_data.i16MatrixValue->Cols() );
   case VariantType::UI16Matrix:
      return CharMatrix( m_data.ui16MatrixValue->Begin(), m_data.ui16MatrixValue->Rows(), m_data.ui16MatrixValue->Cols() );
   case VariantType::I32Matrix:
      return CharMatrix( m_data.i32MatrixValue->Begin(), m_data.i32MatrixValue->Rows(), m_data.i32MatrixValue->Cols() );
   case VariantType::UI32Matrix:
      return CharMatrix( m_data.ui32MatrixValue->Begin(), m_data.ui32MatrixValue->Rows(), m_data.ui32MatrixValue->Cols() );
   case VariantType::I64Matrix:
      return CharMatrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix:
      return CharMatrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::F32Matrix:
      return CharMatrix( m_data.f32MatrixValue->Begin(), m_data.f32MatrixValue->Rows(), m_data.f32MatrixValue->Cols() );
   case VariantType::F64Matrix:
      return CharMatrix( m_data.f64MatrixValue->Begin(), m_data.f64MatrixValue->Rows(), m_data.f64MatrixValue->Cols() );
   case VariantType::C32Matrix:
      {
         CharMatrix m( m_data.c32MatrixValue->Rows(), m_data.c32MatrixValue->Cols() );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         size_type n = m_data.c32MatrixValue->NumberOfElements();
         int8* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = int8( c->Mag() );
         return m;
      }
   case VariantType::C64Matrix:
      {
         CharMatrix m( m_data.c64MatrixValue->Rows(), m_data.c64MatrixValue->Cols() );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         size_type n = m_data.c64MatrixValue->NumberOfElements();
         int8* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = int8( c->Mag() );
         return m;
      }

   case VariantType::ByteArray:
      return CharMatrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return CharMatrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return CharMatrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToCharMatrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

ByteMatrix Variant::ToByteMatrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return ByteMatrix( uint8( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:
      return ByteMatrix( uint8( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:
      return ByteMatrix( uint8( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:
      return ByteMatrix( uint8( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:
      return ByteMatrix( uint8( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:
      return ByteMatrix( m_data.uint8Value, 1, 1 );
   case VariantType::UInt16:
      return ByteMatrix( uint8( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:
      return ByteMatrix( uint8( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:
      return ByteMatrix( uint8( m_data.uint64Value ), 1, 1 );
   case VariantType::Float32:
      return ByteMatrix( uint8( m_data.float32Value ), 1, 1 );
   case VariantType::Float64:
      return ByteMatrix( uint8( m_data.float64Value ), 1, 1 );

   case VariantType::Complex32:
      {
         ByteMatrix m( 1, 2 );
         m[0][0] = uint8( m_data.complex32Value->Real() );
         m[0][1] = uint8( m_data.complex32Value->Imag() );
         return m;
      }
   case VariantType::Complex64:
      {
         ByteMatrix m( 1, 2 );
         m[0][0] = uint8( m_data.complex64Value->Real() );
         m[0][1] = uint8( m_data.complex64Value->Imag() );
         return m;
      }

   case VariantType::I32Point:
      {
         ByteMatrix m( 1, 2 );
         m[0][0] = uint8( m_data.i32PointValue->x );
         m[0][1] = uint8( m_data.i32PointValue->y );
         return m;
      }
   case VariantType::F32Point:
      {
         ByteMatrix m( 1, 2 );
         m[0][0] = uint8( m_data.f32PointValue->x );
         m[0][1] = uint8( m_data.f32PointValue->y );
         return m;
      }
   case VariantType::F64Point:
      {
         ByteMatrix m( 1, 2 );
         m[0][0] = uint8( m_data.f64PointValue->x );
         m[0][1] = uint8( m_data.f64PointValue->y );
         return m;
      }

   case VariantType::I32Rect:
      {
         ByteMatrix m( 2, 2 );
         m[0][0] = uint8( m_data.i32RectValue->x0 );
         m[0][1] = uint8( m_data.i32RectValue->y0 );
         m[1][0] = uint8( m_data.i32RectValue->x1 );
         m[1][1] = uint8( m_data.i32RectValue->y1 );
         return m;
      }
   case VariantType::F32Rect:
      {
         ByteMatrix m( 2, 2 );
         m[0][0] = uint8( m_data.f32RectValue->x0 );
         m[0][1] = uint8( m_data.f32RectValue->y0 );
         m[1][0] = uint8( m_data.f32RectValue->x1 );
         m[1][1] = uint8( m_data.f32RectValue->y1 );
         return m;
      }
   case VariantType::F64Rect:
      {
         ByteMatrix m( 2, 2 );
         m[0][0] = uint8( m_data.f64RectValue->x0 );
         m[0][1] = uint8( m_data.f64RectValue->y0 );
         m[1][0] = uint8( m_data.f64RectValue->x1 );
         m[1][1] = uint8( m_data.f64RectValue->y1 );
         return m;
      }

   case VariantType::I8Vector:
      return ByteMatrix( m_data.i8VectorValue->Begin(), 1, m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return ByteMatrix( m_data.ui8VectorValue->Begin(), 1, m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return ByteMatrix( m_data.i16VectorValue->Begin(), 1, m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return ByteMatrix( m_data.ui16VectorValue->Begin(), 1, m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return ByteMatrix( m_data.i32VectorValue->Begin(), 1, m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return ByteMatrix( m_data.ui32VectorValue->Begin(), 1, m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return ByteMatrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return ByteMatrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return ByteMatrix( m_data.f32VectorValue->Begin(), 1, m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return ByteMatrix( m_data.f64VectorValue->Begin(), 1, m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         ByteMatrix m( 1, m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         uint8* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = uint8( c->Mag() );
         return m;
      }
   case VariantType::C64Vector:
      {
         ByteMatrix m( 1, m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         uint8* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = uint8( c->Mag() );
         return m;
      }

   case VariantType::I8Matrix:
      return ByteMatrix( m_data.i8MatrixValue->Begin(), m_data.i8MatrixValue->Rows(), m_data.i8MatrixValue->Cols() );
   case VariantType::UI8Matrix:
      return *m_data.ui8MatrixValue;
   case VariantType::I16Matrix:
      return ByteMatrix( m_data.i16MatrixValue->Begin(), m_data.i16MatrixValue->Rows(), m_data.i16MatrixValue->Cols() );
   case VariantType::UI16Matrix:
      return ByteMatrix( m_data.ui16MatrixValue->Begin(), m_data.ui16MatrixValue->Rows(), m_data.ui16MatrixValue->Cols() );
   case VariantType::I32Matrix:
      return ByteMatrix( m_data.i32MatrixValue->Begin(), m_data.i32MatrixValue->Rows(), m_data.i32MatrixValue->Cols() );
   case VariantType::UI32Matrix:
      return ByteMatrix( m_data.ui32MatrixValue->Begin(), m_data.ui32MatrixValue->Rows(), m_data.ui32MatrixValue->Cols() );
   case VariantType::I64Matrix:
      return ByteMatrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix:
      return ByteMatrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::F32Matrix:
      return ByteMatrix( m_data.f32MatrixValue->Begin(), m_data.f32MatrixValue->Rows(), m_data.f32MatrixValue->Cols() );
   case VariantType::F64Matrix:
      return ByteMatrix( m_data.f64MatrixValue->Begin(), m_data.f64MatrixValue->Rows(), m_data.f64MatrixValue->Cols() );
   case VariantType::C32Matrix:
      {
         ByteMatrix m( m_data.c32MatrixValue->Rows(), m_data.c32MatrixValue->Cols() );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         size_type n = m_data.c32MatrixValue->NumberOfElements();
         uint8* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = uint8( c->Mag() );
         return m;
      }
   case VariantType::C64Matrix:
      {
         ByteMatrix m( m_data.c64MatrixValue->Rows(), m_data.c64MatrixValue->Cols() );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         size_type n = m_data.c64MatrixValue->NumberOfElements();
         uint8* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = uint8( c->Mag() );
         return m;
      }

   case VariantType::ByteArray:
      return ByteMatrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return ByteMatrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return ByteMatrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToByteMatrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

I16Matrix Variant::ToI16Matrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return I16Matrix( int16( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:
      return I16Matrix( int16( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:
      return I16Matrix( m_data.int16Value, 1, 1 );
   case VariantType::Int32:
      return I16Matrix( int16( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:
      return I16Matrix( int16( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:
      return I16Matrix( int16( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:
      return I16Matrix( int16( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:
      return I16Matrix( int16( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:
      return I16Matrix( int16( m_data.uint64Value ), 1, 1 );
   case VariantType::Float32:
      return I16Matrix( int16( m_data.float32Value ), 1, 1 );
   case VariantType::Float64:
      return I16Matrix( int16( m_data.float64Value ), 1, 1 );

   case VariantType::Complex32:
      {
         I16Matrix m( 1, 2 );
         m[0][0] = int16( m_data.complex32Value->Real() );
         m[0][1] = int16( m_data.complex32Value->Imag() );
         return m;
      }
   case VariantType::Complex64:
      {
         I16Matrix m( 1, 2 );
         m[0][0] = int16( m_data.complex64Value->Real() );
         m[0][1] = int16( m_data.complex64Value->Imag() );
         return m;
      }

   case VariantType::I32Point:
      {
         I16Matrix m( 1, 2 );
         m[0][0] = m_data.i32PointValue->x;
         m[0][1] = m_data.i32PointValue->y;
         return m;
      }
   case VariantType::F32Point:
      {
         I16Matrix m( 1, 2 );
         m[0][0] = int16( m_data.f32PointValue->x );
         m[0][1] = int16( m_data.f32PointValue->y );
         return m;
      }
   case VariantType::F64Point:
      {
         I16Matrix m( 1, 2 );
         m[0][0] = int16( m_data.f64PointValue->x );
         m[0][1] = int16( m_data.f64PointValue->y );
         return m;
      }

   case VariantType::I32Rect:
      {
         I16Matrix m( 2, 2 );
         m[0][0] = m_data.i32RectValue->x0;
         m[0][1] = m_data.i32RectValue->y0;
         m[1][0] = m_data.i32RectValue->x1;
         m[1][1] = m_data.i32RectValue->y1;
         return m;
      }
   case VariantType::F32Rect:
      {
         I16Matrix m( 2, 2 );
         m[0][0] = int16( m_data.f32RectValue->x0 );
         m[0][1] = int16( m_data.f32RectValue->y0 );
         m[1][0] = int16( m_data.f32RectValue->x1 );
         m[1][1] = int16( m_data.f32RectValue->y1 );
         return m;
      }
   case VariantType::F64Rect:
      {
         I16Matrix m( 2, 2 );
         m[0][0] = int16( m_data.f64RectValue->x0 );
         m[0][1] = int16( m_data.f64RectValue->y0 );
         m[1][0] = int16( m_data.f64RectValue->x1 );
         m[1][1] = int16( m_data.f64RectValue->y1 );
         return m;
      }

   case VariantType::I8Vector:
      return I16Matrix( m_data.i8VectorValue->Begin(), 1, m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return I16Matrix( m_data.ui8VectorValue->Begin(), 1, m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return I16Matrix( m_data.i16VectorValue->Begin(), 1, m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return I16Matrix( m_data.ui16VectorValue->Begin(), 1, m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return I16Matrix( m_data.i32VectorValue->Begin(), 1, m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return I16Matrix( m_data.ui32VectorValue->Begin(), 1, m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return I16Matrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return I16Matrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return I16Matrix( m_data.f32VectorValue->Begin(), 1, m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return I16Matrix( m_data.f64VectorValue->Begin(), 1, m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         I16Matrix m( 1, m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         int16* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = int16( c->Mag() );
         return m;
      }
   case VariantType::C64Vector:
      {
         I16Matrix m( 1, m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         int16* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = int16( c->Mag() );
         return m;
      }

   case VariantType::I8Matrix:
      return I16Matrix( m_data.i8MatrixValue->Begin(), m_data.i8MatrixValue->Rows(), m_data.i8MatrixValue->Cols() );
   case VariantType::UI8Matrix:
      return I16Matrix( m_data.ui8MatrixValue->Begin(), m_data.ui8MatrixValue->Rows(), m_data.ui8MatrixValue->Cols() );
   case VariantType::I16Matrix:
      return *m_data.i16MatrixValue;
   case VariantType::UI16Matrix:
      return I16Matrix( m_data.ui16MatrixValue->Begin(), m_data.ui16MatrixValue->Rows(), m_data.ui16MatrixValue->Cols() );
   case VariantType::I32Matrix:
      return I16Matrix( m_data.i32MatrixValue->Begin(), m_data.i32MatrixValue->Rows(), m_data.i32MatrixValue->Cols() );
   case VariantType::UI32Matrix:
      return I16Matrix( m_data.ui32MatrixValue->Begin(), m_data.ui32MatrixValue->Rows(), m_data.ui32MatrixValue->Cols() );
   case VariantType::I64Matrix:
      return I16Matrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix:
      return I16Matrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::F32Matrix:
      return I16Matrix( m_data.f32MatrixValue->Begin(), m_data.f32MatrixValue->Rows(), m_data.f32MatrixValue->Cols() );
   case VariantType::F64Matrix:
      return I16Matrix( m_data.f64MatrixValue->Begin(), m_data.f64MatrixValue->Rows(), m_data.f64MatrixValue->Cols() );
   case VariantType::C32Matrix:
      {
         I16Matrix m( m_data.c32MatrixValue->Rows(), m_data.c32MatrixValue->Cols() );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         size_type n = m_data.c32MatrixValue->NumberOfElements();
         int16* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = int16( c->Mag() );
         return m;
      }
   case VariantType::C64Matrix:
      {
         I16Matrix m( m_data.c64MatrixValue->Rows(), m_data.c64MatrixValue->Cols() );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         size_type n = m_data.c64MatrixValue->NumberOfElements();
         int16* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = int16( c->Mag() );
         return m;
      }

   case VariantType::ByteArray:
      return I16Matrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return I16Matrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return I16Matrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToI16Matrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

UI16Matrix Variant::ToUI16Matrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return UI16Matrix( uint16( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:
      return UI16Matrix( uint16( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:
      return UI16Matrix( uint16( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:
      return UI16Matrix( uint16( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:
      return UI16Matrix( uint16( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:
      return UI16Matrix( uint16( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:
      return UI16Matrix( m_data.uint16Value, 1, 1 );
   case VariantType::UInt32:
      return UI16Matrix( uint16( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:
      return UI16Matrix( uint16( m_data.uint64Value ), 1, 1 );
   case VariantType::Float32:
      return UI16Matrix( uint16( m_data.float32Value ), 1, 1 );
   case VariantType::Float64:
      return UI16Matrix( uint16( m_data.float64Value ), 1, 1 );

   case VariantType::Complex32:
      {
         UI16Matrix m( 1, 2 );
         m[0][0] = uint16( m_data.complex32Value->Real() );
         m[0][1] = uint16( m_data.complex32Value->Imag() );
         return m;
      }
   case VariantType::Complex64:
      {
         UI16Matrix m( 1, 2 );
         m[0][0] = uint16( m_data.complex64Value->Real() );
         m[0][1] = uint16( m_data.complex64Value->Imag() );
         return m;
      }

   case VariantType::I32Point:
      {
         UI16Matrix m( 1, 2 );
         m[0][0] = uint16( m_data.i32PointValue->x );
         m[0][1] = uint16( m_data.i32PointValue->y );
         return m;
      }
   case VariantType::F32Point:
      {
         UI16Matrix m( 1, 2 );
         m[0][0] = uint16( m_data.f32PointValue->x );
         m[0][1] = uint16( m_data.f32PointValue->y );
         return m;
      }
   case VariantType::F64Point:
      {
         UI16Matrix m( 1, 2 );
         m[0][0] = uint16( m_data.f64PointValue->x );
         m[0][1] = uint16( m_data.f64PointValue->y );
         return m;
      }

   case VariantType::I32Rect:
      {
         UI16Matrix m( 2, 2 );
         m[0][0] = uint16( m_data.i32RectValue->x0 );
         m[0][1] = uint16( m_data.i32RectValue->y0 );
         m[1][0] = uint16( m_data.i32RectValue->x1 );
         m[1][1] = uint16( m_data.i32RectValue->y1 );
         return m;
      }
   case VariantType::F32Rect:
      {
         UI16Matrix m( 2, 2 );
         m[0][0] = uint16( m_data.f32RectValue->x0 );
         m[0][1] = uint16( m_data.f32RectValue->y0 );
         m[1][0] = uint16( m_data.f32RectValue->x1 );
         m[1][1] = uint16( m_data.f32RectValue->y1 );
         return m;
      }
   case VariantType::F64Rect:
      {
         UI16Matrix m( 2, 2 );
         m[0][0] = uint16( m_data.f64RectValue->x0 );
         m[0][1] = uint16( m_data.f64RectValue->y0 );
         m[1][0] = uint16( m_data.f64RectValue->x1 );
         m[1][1] = uint16( m_data.f64RectValue->y1 );
         return m;
      }

   case VariantType::I8Vector:
      return UI16Matrix( m_data.i8VectorValue->Begin(), 1, m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return UI16Matrix( m_data.ui8VectorValue->Begin(), 1, m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return UI16Matrix( m_data.i16VectorValue->Begin(), 1, m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return UI16Matrix( m_data.ui16VectorValue->Begin(), 1, m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return UI16Matrix( m_data.i32VectorValue->Begin(), 1, m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return UI16Matrix( m_data.ui32VectorValue->Begin(), 1, m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return UI16Matrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return UI16Matrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return UI16Matrix( m_data.f32VectorValue->Begin(), 1, m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return UI16Matrix( m_data.f64VectorValue->Begin(), 1, m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         UI16Matrix m( 1, m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         uint16* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = uint16( c->Mag() );
         return m;
      }
   case VariantType::C64Vector:
      {
         UI16Matrix m( 1, m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         uint16* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = uint16( c->Mag() );
         return m;
      }

   case VariantType::I8Matrix:
      return UI16Matrix( m_data.i8MatrixValue->Begin(), m_data.i8MatrixValue->Rows(), m_data.i8MatrixValue->Cols() );
   case VariantType::UI8Matrix:
      return UI16Matrix( m_data.ui8MatrixValue->Begin(), m_data.ui8MatrixValue->Rows(), m_data.ui8MatrixValue->Cols() );
   case VariantType::I16Matrix:
      return UI16Matrix( m_data.i16MatrixValue->Begin(), m_data.i16MatrixValue->Rows(), m_data.i16MatrixValue->Cols() );
   case VariantType::UI16Matrix:
      return *m_data.ui16MatrixValue;
   case VariantType::I32Matrix:
      return UI16Matrix( m_data.i32MatrixValue->Begin(), m_data.i32MatrixValue->Rows(), m_data.i32MatrixValue->Cols() );
   case VariantType::UI32Matrix:
      return UI16Matrix( m_data.ui32MatrixValue->Begin(), m_data.ui32MatrixValue->Rows(), m_data.ui32MatrixValue->Cols() );
   case VariantType::I64Matrix:
      return UI16Matrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix:
      return UI16Matrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::F32Matrix:
      return UI16Matrix( m_data.f32MatrixValue->Begin(), m_data.f32MatrixValue->Rows(), m_data.f32MatrixValue->Cols() );
   case VariantType::F64Matrix:
      return UI16Matrix( m_data.f64MatrixValue->Begin(), m_data.f64MatrixValue->Rows(), m_data.f64MatrixValue->Cols() );
   case VariantType::C32Matrix:
      {
         UI16Matrix m( m_data.c32MatrixValue->Rows(), m_data.c32MatrixValue->Cols() );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         size_type n = m_data.c32MatrixValue->NumberOfElements();
         uint16* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = uint16( c->Mag() );
         return m;
      }
   case VariantType::C64Matrix:
      {
         UI16Matrix m( m_data.c64MatrixValue->Rows(), m_data.c64MatrixValue->Cols() );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         size_type n = m_data.c64MatrixValue->NumberOfElements();
         uint16* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = uint16( c->Mag() );
         return m;
      }

   case VariantType::ByteArray:
      return UI16Matrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return UI16Matrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return UI16Matrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToUI16Matrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

IMatrix Variant::ToIMatrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return IMatrix( int32( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:
      return IMatrix( int32( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:
      return IMatrix( int32( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:
      return IMatrix( m_data.int32Value, 1, 1 );
   case VariantType::Int64:
      return IMatrix( int32( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:
      return IMatrix( int32( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:
      return IMatrix( int32( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:
      return IMatrix( int32( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:
      return IMatrix( int32( m_data.uint64Value ), 1, 1 );
   case VariantType::Float32:
      return IMatrix( int32( m_data.float32Value ), 1, 1 );
   case VariantType::Float64:
      return IMatrix( int32( m_data.float64Value ), 1, 1 );

   case VariantType::Complex32:
      {
         IMatrix m( 1, 2 );
         m[0][0] = int32( m_data.complex32Value->Real() );
         m[0][1] = int32( m_data.complex32Value->Imag() );
         return m;
      }
   case VariantType::Complex64:
      {
         IMatrix m( 1, 2 );
         m[0][0] = int32( m_data.complex64Value->Real() );
         m[0][1] = int32( m_data.complex64Value->Imag() );
         return m;
      }

   case VariantType::I32Point:
      {
         IMatrix m( 1, 2 );
         m[0][0] = m_data.i32PointValue->x;
         m[0][1] = m_data.i32PointValue->y;
         return m;
      }
   case VariantType::F32Point:
      {
         IMatrix m( 1, 2 );
         m[0][0] = int32( m_data.f32PointValue->x );
         m[0][1] = int32( m_data.f32PointValue->y );
         return m;
      }
   case VariantType::F64Point:
      {
         IMatrix m( 1, 2 );
         m[0][0] = int32( m_data.f64PointValue->x );
         m[0][1] = int32( m_data.f64PointValue->y );
         return m;
      }

   case VariantType::I32Rect:
      {
         IMatrix m( 2, 2 );
         m[0][0] = m_data.i32RectValue->x0;
         m[0][1] = m_data.i32RectValue->y0;
         m[1][0] = m_data.i32RectValue->x1;
         m[1][1] = m_data.i32RectValue->y1;
         return m;
      }
   case VariantType::F32Rect:
      {
         IMatrix m( 2, 2 );
         m[0][0] = int32( m_data.f32RectValue->x0 );
         m[0][1] = int32( m_data.f32RectValue->y0 );
         m[1][0] = int32( m_data.f32RectValue->x1 );
         m[1][1] = int32( m_data.f32RectValue->y1 );
         return m;
      }
   case VariantType::F64Rect:
      {
         IMatrix m( 2, 2 );
         m[0][0] = int32( m_data.f64RectValue->x0 );
         m[0][1] = int32( m_data.f64RectValue->y0 );
         m[1][0] = int32( m_data.f64RectValue->x1 );
         m[1][1] = int32( m_data.f64RectValue->y1 );
         return m;
      }

   case VariantType::I8Vector:
      return IMatrix( m_data.i8VectorValue->Begin(), 1, m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return IMatrix( m_data.ui8VectorValue->Begin(), 1, m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return IMatrix( m_data.i16VectorValue->Begin(), 1, m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return IMatrix( m_data.ui16VectorValue->Begin(), 1, m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return IMatrix( m_data.i32VectorValue->Begin(), 1, m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return IMatrix( m_data.ui32VectorValue->Begin(), 1, m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return IMatrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return IMatrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return IMatrix( m_data.f32VectorValue->Begin(), 1, m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return IMatrix( m_data.f64VectorValue->Begin(), 1, m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         IMatrix m( 1, m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         int32* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = int32( c->Mag() );
         return m;
      }
   case VariantType::C64Vector:
      {
         IMatrix m( 1, m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         int32* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = int32( c->Mag() );
         return m;
      }

   case VariantType::I8Matrix:
      return IMatrix( m_data.i8MatrixValue->Begin(), m_data.i8MatrixValue->Rows(), m_data.i8MatrixValue->Cols() );
   case VariantType::UI8Matrix:
      return IMatrix( m_data.ui8MatrixValue->Begin(), m_data.ui8MatrixValue->Rows(), m_data.ui8MatrixValue->Cols() );
   case VariantType::I16Matrix:
      return IMatrix( m_data.i16MatrixValue->Begin(), m_data.i16MatrixValue->Rows(), m_data.i16MatrixValue->Cols() );
   case VariantType::UI16Matrix:
      return IMatrix( m_data.ui16MatrixValue->Begin(), m_data.ui16MatrixValue->Rows(), m_data.ui16MatrixValue->Cols() );
   case VariantType::I32Matrix:
      return *m_data.i32MatrixValue;
   case VariantType::UI32Matrix:
      return IMatrix( m_data.ui32MatrixValue->Begin(), m_data.ui32MatrixValue->Rows(), m_data.ui32MatrixValue->Cols() );
   case VariantType::I64Matrix:
      return IMatrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix:
      return IMatrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::F32Matrix:
      return IMatrix( m_data.f32MatrixValue->Begin(), m_data.f32MatrixValue->Rows(), m_data.f32MatrixValue->Cols() );
   case VariantType::F64Matrix:
      return IMatrix( m_data.f64MatrixValue->Begin(), m_data.f64MatrixValue->Rows(), m_data.f64MatrixValue->Cols() );
   case VariantType::C32Matrix:
      {
         IMatrix m( m_data.c32MatrixValue->Rows(), m_data.c32MatrixValue->Cols() );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         size_type n = m_data.c32MatrixValue->NumberOfElements();
         int32* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = int32( c->Mag() );
         return m;
      }
   case VariantType::C64Matrix:
      {
         IMatrix m( m_data.c64MatrixValue->Rows(), m_data.c64MatrixValue->Cols() );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         size_type n = m_data.c64MatrixValue->NumberOfElements();
         int32* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = int32( c->Mag() );
         return m;
      }

   case VariantType::ByteArray:
      return IMatrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return IMatrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return IMatrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToIMatrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

UIMatrix Variant::ToUIMatrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return UIMatrix( uint32( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:
      return UIMatrix( uint32( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:
      return UIMatrix( uint32( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:
      return UIMatrix( uint32( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:
      return UIMatrix( uint32( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:
      return UIMatrix( uint32( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:
      return UIMatrix( uint32( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:
      return UIMatrix( m_data.uint32Value, 1, 1 );
   case VariantType::UInt64:
      return UIMatrix( uint32( m_data.uint64Value ), 1, 1 );
   case VariantType::Float32:
      return UIMatrix( uint32( m_data.float32Value ), 1, 1 );
   case VariantType::Float64:
      return UIMatrix( uint32( m_data.float64Value ), 1, 1 );

   case VariantType::Complex32:
      {
         UIMatrix m( 1, 2 );
         m[0][0] = uint32( m_data.complex32Value->Real() );
         m[0][1] = uint32( m_data.complex32Value->Imag() );
         return m;
      }
   case VariantType::Complex64:
      {
         UIMatrix m( 1, 2 );
         m[0][0] = uint32( m_data.complex64Value->Real() );
         m[0][1] = uint32( m_data.complex64Value->Imag() );
         return m;
      }

   case VariantType::I32Point:
      {
         UIMatrix m( 1, 2 );
         m[0][0] = uint32( m_data.i32PointValue->x );
         m[0][1] = uint32( m_data.i32PointValue->y );
         return m;
      }
   case VariantType::F32Point:
      {
         UIMatrix m( 1, 2 );
         m[0][0] = uint32( m_data.f32PointValue->x );
         m[0][1] = uint32( m_data.f32PointValue->y );
         return m;
      }
   case VariantType::F64Point:
      {
         UIMatrix m( 1, 2 );
         m[0][0] = uint32( m_data.f64PointValue->x );
         m[0][1] = uint32( m_data.f64PointValue->y );
         return m;
      }

   case VariantType::I32Rect:
      {
         UIMatrix m( 2, 2 );
         m[0][0] = uint32( m_data.i32RectValue->x0 );
         m[0][1] = uint32( m_data.i32RectValue->y0 );
         m[1][0] = uint32( m_data.i32RectValue->x1 );
         m[1][1] = uint32( m_data.i32RectValue->y1 );
         return m;
      }
   case VariantType::F32Rect:
      {
         UIMatrix m( 2, 2 );
         m[0][0] = uint32( m_data.f32RectValue->x0 );
         m[0][1] = uint32( m_data.f32RectValue->y0 );
         m[1][0] = uint32( m_data.f32RectValue->x1 );
         m[1][1] = uint32( m_data.f32RectValue->y1 );
         return m;
      }
   case VariantType::F64Rect:
      {
         UIMatrix m( 2, 2 );
         m[0][0] = uint32( m_data.f64RectValue->x0 );
         m[0][1] = uint32( m_data.f64RectValue->y0 );
         m[1][0] = uint32( m_data.f64RectValue->x1 );
         m[1][1] = uint32( m_data.f64RectValue->y1 );
         return m;
      }

   case VariantType::I8Vector:
      return UIMatrix( m_data.i8VectorValue->Begin(), 1, m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return UIMatrix( m_data.ui8VectorValue->Begin(), 1, m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return UIMatrix( m_data.i16VectorValue->Begin(), 1, m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return UIMatrix( m_data.ui16VectorValue->Begin(), 1, m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return UIMatrix( m_data.i32VectorValue->Begin(), 1, m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return UIMatrix( m_data.ui32VectorValue->Begin(), 1, m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return UIMatrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return UIMatrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return UIMatrix( m_data.f32VectorValue->Begin(), 1, m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return UIMatrix( m_data.f64VectorValue->Begin(), 1, m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         UIMatrix m( 1, m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         uint32* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = uint32( c->Mag() );
         return m;
      }
   case VariantType::C64Vector:
      {
         UIMatrix m( 1, m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         uint32* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = uint32( c->Mag() );
         return m;
      }

   case VariantType::I8Matrix:
      return UIMatrix( m_data.i8MatrixValue->Begin(), m_data.i8MatrixValue->Rows(), m_data.i8MatrixValue->Cols() );
   case VariantType::UI8Matrix:
      return UIMatrix( m_data.ui8MatrixValue->Begin(), m_data.ui8MatrixValue->Rows(), m_data.ui8MatrixValue->Cols() );
   case VariantType::I16Matrix:
      return UIMatrix( m_data.i16MatrixValue->Begin(), m_data.i16MatrixValue->Rows(), m_data.i16MatrixValue->Cols() );
   case VariantType::UI16Matrix:
      return UIMatrix( m_data.ui16MatrixValue->Begin(), m_data.ui16MatrixValue->Rows(), m_data.ui16MatrixValue->Cols() );
   case VariantType::I32Matrix:
      return UIMatrix( m_data.i32MatrixValue->Begin(), m_data.i32MatrixValue->Rows(), m_data.i32MatrixValue->Cols() );
   case VariantType::UI32Matrix:
      return *m_data.ui32MatrixValue;
   case VariantType::I64Matrix:
      return UIMatrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix:
      return UIMatrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::F32Matrix:
      return UIMatrix( m_data.f32MatrixValue->Begin(), m_data.f32MatrixValue->Rows(), m_data.f32MatrixValue->Cols() );
   case VariantType::F64Matrix:
      return UIMatrix( m_data.f64MatrixValue->Begin(), m_data.f64MatrixValue->Rows(), m_data.f64MatrixValue->Cols() );
   case VariantType::C32Matrix:
      {
         UIMatrix m( m_data.c32MatrixValue->Rows(), m_data.c32MatrixValue->Cols() );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         size_type n = m_data.c32MatrixValue->NumberOfElements();
         uint32* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = uint32( c->Mag() );
         return m;
      }
   case VariantType::C64Matrix:
      {
         UIMatrix m( m_data.c64MatrixValue->Rows(), m_data.c64MatrixValue->Cols() );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         size_type n = m_data.c64MatrixValue->NumberOfElements();
         uint32* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = uint32( c->Mag() );
         return m;
      }

   case VariantType::ByteArray:
      return UIMatrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return UIMatrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return UIMatrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToUIMatrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

I64Matrix Variant::ToI64Matrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return I64Matrix( int64( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:
      return I64Matrix( int64( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:
      return I64Matrix( int64( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:
      return I64Matrix( int64( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:
      return I64Matrix( m_data.int64Value, 1, 1 );
   case VariantType::UInt8:
      return I64Matrix( int64( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:
      return I64Matrix( int64( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:
      return I64Matrix( int64( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:
      return I64Matrix( int64( m_data.uint64Value ), 1, 1 );
   case VariantType::Float32:
      return I64Matrix( int64( m_data.float32Value ), 1, 1 );
   case VariantType::Float64:
      return I64Matrix( int64( m_data.float64Value ), 1, 1 );

   case VariantType::Complex32:
      {
         I64Matrix m( 1, 2 );
         m[0][0] = int64( m_data.complex32Value->Real() );
         m[0][1] = int64( m_data.complex32Value->Imag() );
         return m;
      }
   case VariantType::Complex64:
      {
         I64Matrix m( 1, 2 );
         m[0][0] = int64( m_data.complex64Value->Real() );
         m[0][1] = int64( m_data.complex64Value->Imag() );
         return m;
      }

   case VariantType::I32Point:
      {
         I64Matrix m( 1, 2 );
         m[0][0] = int64( m_data.i32PointValue->x );
         m[0][1] = int64( m_data.i32PointValue->y );
         return m;
      }
   case VariantType::F32Point:
      {
         I64Matrix m( 1, 2 );
         m[0][0] = int64( m_data.f32PointValue->x );
         m[0][1] = int64( m_data.f32PointValue->y );
         return m;
      }
   case VariantType::F64Point:
      {
         I64Matrix m( 1, 2 );
         m[0][0] = int64( m_data.f64PointValue->x );
         m[0][1] = int64( m_data.f64PointValue->y );
         return m;
      }

   case VariantType::I32Rect:
      {
         I64Matrix m( 2, 2 );
         m[0][0] = int64( m_data.i32RectValue->x0 );
         m[0][1] = int64( m_data.i32RectValue->y0 );
         m[1][0] = int64( m_data.i32RectValue->x1 );
         m[1][1] = int64( m_data.i32RectValue->y1 );
         return m;
      }
   case VariantType::F32Rect:
      {
         I64Matrix m( 2, 2 );
         m[0][0] = int64( m_data.f32RectValue->x0 );
         m[0][1] = int64( m_data.f32RectValue->y0 );
         m[1][0] = int64( m_data.f32RectValue->x1 );
         m[1][1] = int64( m_data.f32RectValue->y1 );
         return m;
      }
   case VariantType::F64Rect:
      {
         I64Matrix m( 2, 2 );
         m[0][0] = int64( m_data.f64RectValue->x0 );
         m[0][1] = int64( m_data.f64RectValue->y0 );
         m[1][0] = int64( m_data.f64RectValue->x1 );
         m[1][1] = int64( m_data.f64RectValue->y1 );
         return m;
      }

   case VariantType::I8Vector:
      return I64Matrix( m_data.i8VectorValue->Begin(), 1, m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return I64Matrix( m_data.ui8VectorValue->Begin(), 1, m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return I64Matrix( m_data.i16VectorValue->Begin(), 1, m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return I64Matrix( m_data.ui16VectorValue->Begin(), 1, m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return I64Matrix( m_data.i32VectorValue->Begin(), 1, m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return I64Matrix( m_data.ui32VectorValue->Begin(), 1, m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return I64Matrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return I64Matrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return I64Matrix( m_data.f32VectorValue->Begin(), 1, m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return I64Matrix( m_data.f64VectorValue->Begin(), 1, m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         I64Matrix m( 1, m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         int64* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = int64( c->Mag() );
         return m;
      }
   case VariantType::C64Vector:
      {
         I64Matrix m( 1, m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         int64* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = int64( c->Mag() );
         return m;
      }

   case VariantType::I8Matrix:
      return I64Matrix( m_data.i8MatrixValue->Begin(), m_data.i8MatrixValue->Rows(), m_data.i8MatrixValue->Cols() );
   case VariantType::UI8Matrix:
      return I64Matrix( m_data.ui8MatrixValue->Begin(), m_data.ui8MatrixValue->Rows(), m_data.ui8MatrixValue->Cols() );
   case VariantType::I16Matrix:
      return I64Matrix( m_data.i16MatrixValue->Begin(), m_data.i16MatrixValue->Rows(), m_data.i16MatrixValue->Cols() );
   case VariantType::UI16Matrix:
      return I64Matrix( m_data.ui16MatrixValue->Begin(), m_data.ui16MatrixValue->Rows(), m_data.ui16MatrixValue->Cols() );
   case VariantType::I32Matrix:
      return I64Matrix( m_data.i32MatrixValue->Begin(), m_data.i32MatrixValue->Rows(), m_data.i32MatrixValue->Cols() );
   case VariantType::UI32Matrix:
      return I64Matrix( m_data.ui32MatrixValue->Begin(), m_data.ui32MatrixValue->Rows(), m_data.ui32MatrixValue->Cols() );
   case VariantType::I64Matrix:
      return *m_data.i64MatrixValue;
   case VariantType::UI64Matrix:
      return I64Matrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::F32Matrix:
      return I64Matrix( m_data.f32MatrixValue->Begin(), m_data.f32MatrixValue->Rows(), m_data.f32MatrixValue->Cols() );
   case VariantType::F64Matrix:
      return I64Matrix( m_data.f64MatrixValue->Begin(), m_data.f64MatrixValue->Rows(), m_data.f64MatrixValue->Cols() );
   case VariantType::C32Matrix:
      {
         I64Matrix m( m_data.c32MatrixValue->Rows(), m_data.c32MatrixValue->Cols() );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         size_type n = m_data.c32MatrixValue->NumberOfElements();
         int64* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = int64( c->Mag() );
         return m;
      }
   case VariantType::C64Matrix:
      {
         I64Matrix m( m_data.c64MatrixValue->Rows(), m_data.c64MatrixValue->Cols() );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         size_type n = m_data.c64MatrixValue->NumberOfElements();
         int64* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = int64( c->Mag() );
         return m;
      }

   case VariantType::ByteArray:
      return I64Matrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return I64Matrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return I64Matrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToI64Matrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

UI64Matrix Variant::ToUI64Matrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return UI64Matrix( uint64( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:
      return UI64Matrix( uint64( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:
      return UI64Matrix( uint64( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:
      return UI64Matrix( uint64( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:
      return UI64Matrix( uint64( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:
      return UI64Matrix( uint64( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:
      return UI64Matrix( uint64( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:
      return UI64Matrix( uint64( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:
      return UI64Matrix( m_data.uint64Value, 1, 1 );
   case VariantType::Float32:
      return UI64Matrix( uint64( m_data.float32Value ), 1, 1 );
   case VariantType::Float64:
      return UI64Matrix( uint64( m_data.float64Value ), 1, 1 );

   case VariantType::Complex32:
      {
         UI64Matrix m( 1, 2 );
         m[0][0] = uint64( m_data.complex32Value->Real() );
         m[0][1] = uint64( m_data.complex32Value->Imag() );
         return m;
      }
   case VariantType::Complex64:
      {
         UI64Matrix m( 1, 2 );
         m[0][0] = uint64( m_data.complex64Value->Real() );
         m[0][1] = uint64( m_data.complex64Value->Imag() );
         return m;
      }

   case VariantType::I32Point:
      {
         UI64Matrix m( 1, 2 );
         m[0][0] = uint64( m_data.i32PointValue->x );
         m[0][1] = uint64( m_data.i32PointValue->y );
         return m;
      }
   case VariantType::F32Point:
      {
         UI64Matrix m( 1, 2 );
         m[0][0] = uint64( m_data.f32PointValue->x );
         m[0][1] = uint64( m_data.f32PointValue->y );
         return m;
      }
   case VariantType::F64Point:
      {
         UI64Matrix m( 1, 2 );
         m[0][0] = uint64( m_data.f64PointValue->x );
         m[0][1] = uint64( m_data.f64PointValue->y );
         return m;
      }

   case VariantType::I32Rect:
      {
         UI64Matrix m( 2, 2 );
         m[0][0] = uint64( m_data.i32RectValue->x0 );
         m[0][1] = uint64( m_data.i32RectValue->y0 );
         m[1][0] = uint64( m_data.i32RectValue->x1 );
         m[1][1] = uint64( m_data.i32RectValue->y1 );
         return m;
      }
   case VariantType::F32Rect:
      {
         UI64Matrix m( 2, 2 );
         m[0][0] = uint64( m_data.f32RectValue->x0 );
         m[0][1] = uint64( m_data.f32RectValue->y0 );
         m[1][0] = uint64( m_data.f32RectValue->x1 );
         m[1][1] = uint64( m_data.f32RectValue->y1 );
         return m;
      }
   case VariantType::F64Rect:
      {
         UI64Matrix m( 2, 2 );
         m[0][0] = uint64( m_data.f64RectValue->x0 );
         m[0][1] = uint64( m_data.f64RectValue->y0 );
         m[1][0] = uint64( m_data.f64RectValue->x1 );
         m[1][1] = uint64( m_data.f64RectValue->y1 );
         return m;
      }

   case VariantType::I8Vector:
      return UI64Matrix( m_data.i8VectorValue->Begin(), 1, m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return UI64Matrix( m_data.ui8VectorValue->Begin(), 1, m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return UI64Matrix( m_data.i16VectorValue->Begin(), 1, m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return UI64Matrix( m_data.ui16VectorValue->Begin(), 1, m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return UI64Matrix( m_data.i32VectorValue->Begin(), 1, m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return UI64Matrix( m_data.ui32VectorValue->Begin(), 1, m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return UI64Matrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return UI64Matrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return UI64Matrix( m_data.f32VectorValue->Begin(), 1, m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return UI64Matrix( m_data.f64VectorValue->Begin(), 1, m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         UI64Matrix m( 1, m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         uint64* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = uint64( c->Mag() );
         return m;
      }
   case VariantType::C64Vector:
      {
         UI64Matrix m( 1, m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         uint64* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = uint64( c->Mag() );
         return m;
      }

   case VariantType::I8Matrix:
      return UI64Matrix( m_data.i8MatrixValue->Begin(), m_data.i8MatrixValue->Rows(), m_data.i8MatrixValue->Cols() );
   case VariantType::UI8Matrix:
      return UI64Matrix( m_data.ui8MatrixValue->Begin(), m_data.ui8MatrixValue->Rows(), m_data.ui8MatrixValue->Cols() );
   case VariantType::I16Matrix:
      return UI64Matrix( m_data.i16MatrixValue->Begin(), m_data.i16MatrixValue->Rows(), m_data.i16MatrixValue->Cols() );
   case VariantType::UI16Matrix:
      return UI64Matrix( m_data.ui16MatrixValue->Begin(), m_data.ui16MatrixValue->Rows(), m_data.ui16MatrixValue->Cols() );
   case VariantType::I32Matrix:
      return UI64Matrix( m_data.i32MatrixValue->Begin(), m_data.i32MatrixValue->Rows(), m_data.i32MatrixValue->Cols() );
   case VariantType::UI32Matrix:
      return UI64Matrix( m_data.ui32MatrixValue->Begin(), m_data.ui32MatrixValue->Rows(), m_data.ui32MatrixValue->Cols() );
   case VariantType::I64Matrix:
      return UI64Matrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix:
      return *m_data.ui64MatrixValue;
   case VariantType::F32Matrix:
      return UI64Matrix( m_data.f32MatrixValue->Begin(), m_data.f32MatrixValue->Rows(), m_data.f32MatrixValue->Cols() );
   case VariantType::F64Matrix:
      return UI64Matrix( m_data.f64MatrixValue->Begin(), m_data.f64MatrixValue->Rows(), m_data.f64MatrixValue->Cols() );
   case VariantType::C32Matrix:
      {
         UI64Matrix m( m_data.c32MatrixValue->Rows(), m_data.c32MatrixValue->Cols() );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         size_type n = m_data.c32MatrixValue->NumberOfElements();
         uint64* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = uint64( c->Mag() );
         return m;
      }
   case VariantType::C64Matrix:
      {
         UI64Matrix m( m_data.c64MatrixValue->Rows(), m_data.c64MatrixValue->Cols() );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         size_type n = m_data.c64MatrixValue->NumberOfElements();
         uint64* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = uint64( c->Mag() );
         return m;
      }

   case VariantType::ByteArray:
      return UI64Matrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return UI64Matrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return UI64Matrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToUI64Matrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

FMatrix Variant::ToFMatrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return FMatrix( float( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:
      return FMatrix( float( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:
      return FMatrix( float( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:
      return FMatrix( float( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:
      return FMatrix( float( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:
      return FMatrix( float( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:
      return FMatrix( float( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:
      return FMatrix( float( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:
      return FMatrix( float( m_data.uint64Value ), 1, 1 );
   case VariantType::Float32:
      return FMatrix( m_data.float32Value, 1, 1 );
   case VariantType::Float64:
      return FMatrix( float( m_data.float64Value ), 1, 1 );

   case VariantType::Complex32:
      {
         FMatrix m( 1, 2 );
         m[0][0] = m_data.complex32Value->Real();
         m[0][1] = m_data.complex32Value->Imag();
         return m;
      }
   case VariantType::Complex64:
      {
         FMatrix m( 1, 2 );
         m[0][0] = float( m_data.complex64Value->Real() );
         m[0][1] = float( m_data.complex64Value->Imag() );
         return m;
      }

   case VariantType::TimePoint:
      {
         FMatrix m( 1, 2 );
         m[0][0] = float( m_data.timePointValue->JDI() );
         m[0][1] = float( m_data.timePointValue->JDF() );
         return m;
      }

   case VariantType::I32Point:
      {
         FMatrix m( 1, 2 );
         m[0][0] = float( m_data.i32PointValue->x );
         m[0][1] = float( m_data.i32PointValue->y );
         return m;
      }
   case VariantType::F32Point:
      {
         FMatrix m( 1, 2 );
         m[0][0] = m_data.f32PointValue->x;
         m[0][1] = m_data.f32PointValue->y;
         return m;
      }
   case VariantType::F64Point:
      {
         FMatrix m( 1, 2 );
         m[0][0] = float( m_data.f64PointValue->x );
         m[0][1] = float( m_data.f64PointValue->y );
         return m;
      }

   case VariantType::I32Rect:
      {
         FMatrix m( 2, 2 );
         m[0][0] = float( m_data.i32RectValue->x0 );
         m[0][1] = float( m_data.i32RectValue->y0 );
         m[1][0] = float( m_data.i32RectValue->x1 );
         m[1][1] = float( m_data.i32RectValue->y1 );
         return m;
      }
   case VariantType::F32Rect:
      {
         FMatrix m( 2, 2 );
         m[0][0] = m_data.f32RectValue->x0;
         m[0][1] = m_data.f32RectValue->y0;
         m[1][0] = m_data.f32RectValue->x1;
         m[1][1] = m_data.f32RectValue->y1;
         return m;
      }
   case VariantType::F64Rect:
      {
         FMatrix m( 2, 2 );
         m[0][0] = float( m_data.f64RectValue->x0 );
         m[0][1] = float( m_data.f64RectValue->y0 );
         m[1][0] = float( m_data.f64RectValue->x1 );
         m[1][1] = float( m_data.f64RectValue->y1 );
         return m;
      }

   case VariantType::I8Vector:
      return FMatrix( m_data.i8VectorValue->Begin(), 1, m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return FMatrix( m_data.ui8VectorValue->Begin(), 1, m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return FMatrix( m_data.i16VectorValue->Begin(), 1, m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return FMatrix( m_data.ui16VectorValue->Begin(), 1, m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return FMatrix( m_data.i32VectorValue->Begin(), 1, m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return FMatrix( m_data.ui32VectorValue->Begin(), 1, m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return FMatrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return FMatrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return FMatrix( m_data.f32VectorValue->Begin(), 1, m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return FMatrix( m_data.f64VectorValue->Begin(), 1, m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         FMatrix m( 1, m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         float* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = c->Mag();
         return m;
      }
   case VariantType::C64Vector:
      {
         FMatrix m( 1, m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         float* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = float( c->Mag() );
         return m;
      }

   case VariantType::I8Matrix:
      return FMatrix( m_data.i8MatrixValue->Begin(), m_data.i8MatrixValue->Rows(), m_data.i8MatrixValue->Cols() );
   case VariantType::UI8Matrix:
      return FMatrix( m_data.ui8MatrixValue->Begin(), m_data.ui8MatrixValue->Rows(), m_data.ui8MatrixValue->Cols() );
   case VariantType::I16Matrix:
      return FMatrix( m_data.i16MatrixValue->Begin(), m_data.i16MatrixValue->Rows(), m_data.i16MatrixValue->Cols() );
   case VariantType::UI16Matrix:
      return FMatrix( m_data.ui16MatrixValue->Begin(), m_data.ui16MatrixValue->Rows(), m_data.ui16MatrixValue->Cols() );
   case VariantType::I32Matrix:
      return FMatrix( m_data.i32MatrixValue->Begin(), m_data.i32MatrixValue->Rows(), m_data.i32MatrixValue->Cols() );
   case VariantType::UI32Matrix:
      return FMatrix( m_data.ui32MatrixValue->Begin(), m_data.ui32MatrixValue->Rows(), m_data.ui32MatrixValue->Cols() );
   case VariantType::I64Matrix:
      return FMatrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix:
      return FMatrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::F32Matrix:
      return *m_data.f32MatrixValue;
   case VariantType::F64Matrix:
      return FMatrix( m_data.f64MatrixValue->Begin(), m_data.f64MatrixValue->Rows(), m_data.f64MatrixValue->Cols() );
   case VariantType::C32Matrix:
      {
         FMatrix m( m_data.c32MatrixValue->Rows(), m_data.c32MatrixValue->Cols() );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         size_type n = m_data.c32MatrixValue->NumberOfElements();
         float* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = c->Mag();
         return m;
      }
   case VariantType::C64Matrix:
      {
         FMatrix m( m_data.c64MatrixValue->Rows(), m_data.c64MatrixValue->Cols() );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         size_type n = m_data.c64MatrixValue->NumberOfElements();
         float* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = float( c->Mag() );
         return m;
      }

   case VariantType::ByteArray:
      return FMatrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return FMatrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return FMatrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToFMatrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

DMatrix Variant::ToDMatrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return DMatrix( double( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:
      return DMatrix( double( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:
      return DMatrix( double( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:
      return DMatrix( double( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:
      return DMatrix( double( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:
      return DMatrix( double( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:
      return DMatrix( double( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:
      return DMatrix( double( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:
      return DMatrix( double( m_data.uint64Value ), 1, 1 );
   case VariantType::Float32:
      return DMatrix( double( m_data.float32Value ), 1, 1 );
   case VariantType::Float64:
      return DMatrix( m_data.float64Value, 1, 1 );

   case VariantType::Complex32:
      {
         DMatrix m( 1, 2 );
         m[0][0] = double( m_data.complex32Value->Real() );
         m[0][1] = double( m_data.complex32Value->Imag() );
         return m;
      }
   case VariantType::Complex64:
      {
         DMatrix m( 1, 2 );
         m[0][0] = m_data.complex64Value->Real();
         m[0][1] = m_data.complex64Value->Imag();
         return m;
      }

   case VariantType::TimePoint:
      {
         DMatrix m( 1, 2 );
         m[0][0] = double( m_data.timePointValue->JDI() );
         m[0][1] = double( m_data.timePointValue->JDF() );
         return m;
      }

   case VariantType::I32Point:
      {
         DMatrix m( 1, 2 );
         m[0][0] = double( m_data.i32PointValue->x );
         m[0][1] = double( m_data.i32PointValue->y );
         return m;
      }
   case VariantType::F32Point:
      {
         DMatrix m( 1, 2 );
         m[0][0] = double( m_data.f32PointValue->x );
         m[0][1] = double( m_data.f32PointValue->y );
         return m;
      }
   case VariantType::F64Point:
      {
         DMatrix m( 1, 2 );
         m[0][0] = m_data.f64PointValue->x;
         m[0][1] = m_data.f64PointValue->y;
         return m;
      }

   case VariantType::I32Rect:
      {
         DMatrix m( 2, 2 );
         m[0][0] = double( m_data.i32RectValue->x0 );
         m[0][1] = double( m_data.i32RectValue->y0 );
         m[1][0] = double( m_data.i32RectValue->x1 );
         m[1][1] = double( m_data.i32RectValue->y1 );
         return m;
      }
   case VariantType::F32Rect:
      {
         DMatrix m( 2, 2 );
         m[0][0] = double( m_data.f32RectValue->x0 );
         m[0][1] = double( m_data.f32RectValue->y0 );
         m[1][0] = double( m_data.f32RectValue->x1 );
         m[1][1] = double( m_data.f32RectValue->y1 );
         return m;
      }
   case VariantType::F64Rect:
      {
         DMatrix m( 2, 2 );
         m[0][0] = m_data.f64RectValue->x0;
         m[0][1] = m_data.f64RectValue->y0;
         m[1][0] = m_data.f64RectValue->x1;
         m[1][1] = m_data.f64RectValue->y1;
         return m;
      }

   case VariantType::I8Vector:
      return DMatrix( m_data.i8VectorValue->Begin(), 1, m_data.i8VectorValue->Length() );
   case VariantType::UI8Vector:
      return DMatrix( m_data.ui8VectorValue->Begin(), 1, m_data.ui8VectorValue->Length() );
   case VariantType::I16Vector:
      return DMatrix( m_data.i16VectorValue->Begin(), 1, m_data.i16VectorValue->Length() );
   case VariantType::UI16Vector:
      return DMatrix( m_data.ui16VectorValue->Begin(), 1, m_data.ui16VectorValue->Length() );
   case VariantType::I32Vector:
      return DMatrix( m_data.i32VectorValue->Begin(), 1, m_data.i32VectorValue->Length() );
   case VariantType::UI32Vector:
      return DMatrix( m_data.ui32VectorValue->Begin(), 1, m_data.ui32VectorValue->Length() );
   case VariantType::I64Vector:
      return DMatrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector:
      return DMatrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::F32Vector:
      return DMatrix( m_data.f32VectorValue->Begin(), 1, m_data.f32VectorValue->Length() );
   case VariantType::F64Vector:
      return DMatrix( m_data.f64VectorValue->Begin(), 1, m_data.f64VectorValue->Length() );
   case VariantType::C32Vector:
      {
         DMatrix m( 1, m_data.c32VectorValue->Length() );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         double* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = double( c->Mag() );
         return m;
      }
   case VariantType::C64Vector:
      {
         DMatrix m( 1, m_data.c64VectorValue->Length() );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         double* r = m[0];
         for ( int j = 0; j < m.Cols(); ++j, ++c, ++r )
            *r = c->Mag();
         return m;
      }

   case VariantType::I8Matrix:
      return DMatrix( m_data.i8MatrixValue->Begin(), m_data.i8MatrixValue->Rows(), m_data.i8MatrixValue->Cols() );
   case VariantType::UI8Matrix:
      return DMatrix( m_data.ui8MatrixValue->Begin(), m_data.ui8MatrixValue->Rows(), m_data.ui8MatrixValue->Cols() );
   case VariantType::I16Matrix:
      return DMatrix( m_data.i16MatrixValue->Begin(), m_data.i16MatrixValue->Rows(), m_data.i16MatrixValue->Cols() );
   case VariantType::UI16Matrix:
      return DMatrix( m_data.ui16MatrixValue->Begin(), m_data.ui16MatrixValue->Rows(), m_data.ui16MatrixValue->Cols() );
   case VariantType::I32Matrix:
      return DMatrix( m_data.i32MatrixValue->Begin(), m_data.i32MatrixValue->Rows(), m_data.i32MatrixValue->Cols() );
   case VariantType::UI32Matrix:
      return DMatrix( m_data.ui32MatrixValue->Begin(), m_data.ui32MatrixValue->Rows(), m_data.ui32MatrixValue->Cols() );
   case VariantType::I64Matrix:
      return DMatrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix:
      return DMatrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::F32Matrix:
      return DMatrix( m_data.f32MatrixValue->Begin(), m_data.f32MatrixValue->Rows(), m_data.f32MatrixValue->Cols() );
   case VariantType::F64Matrix:
      return *m_data.f64MatrixValue;
   case VariantType::C32Matrix:
      {
         DMatrix m( m_data.c32MatrixValue->Rows(), m_data.c32MatrixValue->Cols() );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         size_type n = m_data.c32MatrixValue->NumberOfElements();
         double* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = double( c->Mag() );
         return m;
      }
   case VariantType::C64Matrix:
      {
         DMatrix m( m_data.c64MatrixValue->Rows(), m_data.c64MatrixValue->Cols() );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         size_type n = m_data.c64MatrixValue->NumberOfElements();
         double* r = m.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, ++r )
            *r = c->Mag();
         return m;
      }

   case VariantType::ByteArray:
      return DMatrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:
      return DMatrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:
      return DMatrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );

   default:
      throw Error( "Variant::ToDMatrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

C32Matrix Variant::ToC32Matrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return C32Matrix( Complex32( float( m_data.boolValue ) ), 1, 1 );
   case VariantType::Int8:
      return C32Matrix( Complex32( float( m_data.int8Value ) ), 1, 1 );
   case VariantType::Int16:
      return C32Matrix( Complex32( float( m_data.int16Value ) ), 1, 1 );
   case VariantType::Int32:
      return C32Matrix( Complex32( float( m_data.int32Value ) ), 1, 1 );
   case VariantType::Int64:
      return C32Matrix( Complex32( float( m_data.int64Value ) ), 1, 1 );
   case VariantType::UInt8:
      return C32Matrix( Complex32( float( m_data.uint8Value ) ), 1, 1 );
   case VariantType::UInt16:
      return C32Matrix( Complex32( float( m_data.uint16Value ) ), 1, 1 );
   case VariantType::UInt32:
      return C32Matrix( Complex32( float( m_data.uint32Value ) ), 1, 1 );
   case VariantType::UInt64:
      return C32Matrix( Complex32( float( m_data.uint64Value ) ), 1, 1 );
   case VariantType::Float32:
      return C32Matrix( Complex32( m_data.float32Value ), 1, 1 );
   case VariantType::Float64:
      return C32Matrix( Complex32( float( m_data.float64Value ) ), 1, 1 );

   case VariantType::Complex32:
      return C32Matrix( *m_data.complex32Value, 1, 1 );
   case VariantType::Complex64:
      return C32Matrix( Complex32( *m_data.complex64Value ), 1, 1 );

   case VariantType::TimePoint:
      return C32Matrix( Complex32( float( m_data.timePointValue->JDI() ), float( m_data.timePointValue->JDF() ) ), 1, 1 );

   case VariantType::I32Point:
      return C32Matrix( Complex32( float( m_data.i32PointValue->x ), float( m_data.i32PointValue->y ) ), 1, 1 );
   case VariantType::F32Point:
      return C32Matrix( Complex32( m_data.f32PointValue->x, m_data.f32PointValue->y ), 1, 1 );
   case VariantType::F64Point:
      return C32Matrix( Complex32( float( m_data.f64PointValue->x ), float( m_data.f64PointValue->y ) ), 1, 1 );

   case VariantType::I32Rect:
      {
         C32Matrix m( 1, 2 );
         m[0][0].Real() = float( m_data.i32RectValue->x0 );
         m[0][0].Imag() = float( m_data.i32RectValue->y0 );
         m[0][1].Real() = float( m_data.i32RectValue->x1 );
         m[0][1].Imag() = float( m_data.i32RectValue->y1 );
         return m;
      }
   case VariantType::F32Rect:
      {
         C32Matrix m( 1, 2 );
         m[0][0].Real() = m_data.f32RectValue->x0;
         m[0][0].Imag() = m_data.f32RectValue->y0;
         m[0][1].Real() = m_data.f32RectValue->x1;
         m[0][1].Imag() = m_data.f32RectValue->y1;
         return m;
      }
   case VariantType::F64Rect:
      {
         C32Matrix m( 1, 2 );
         m[0][0].Real() = float( m_data.f64RectValue->x0 );
         m[0][0].Imag() = float( m_data.f64RectValue->y0 );
         m[0][1].Real() = float( m_data.f64RectValue->x1 );
         m[0][1].Imag() = float( m_data.f64RectValue->y1 );
         return m;
      }

   case VariantType::I8Vector:
      {
         C32Matrix m( 1, m_data.i8VectorValue->Length() );
         const int8* r = m_data.i8VectorValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI8Vector:
      {
         C32Matrix m( 1, m_data.ui8VectorValue->Length() );
         const uint8* r = m_data.ui8VectorValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::I16Vector:
      {
         C32Matrix m( 1, m_data.i16VectorValue->Length() );
         const int16* r = m_data.i16VectorValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI16Vector:
      {
         C32Matrix m( 1, m_data.ui16VectorValue->Length() );
         const uint16* r = m_data.ui16VectorValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::I32Vector:
      {
         C32Matrix m( 1, m_data.i32VectorValue->Length() );
         const int32* r = m_data.i32VectorValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI32Vector:
      {
         C32Matrix m( 1, m_data.ui32VectorValue->Length() );
         const uint32* r = m_data.ui32VectorValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::I64Vector:
      {
         C32Matrix m( 1, m_data.i64VectorValue->Length() );
         const int64* r = m_data.i64VectorValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI64Vector:
      {
         C32Matrix m( 1, m_data.ui64VectorValue->Length() );
         const uint64* r = m_data.ui64VectorValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::F32Vector:
      {
         C32Matrix m( 1, m_data.f32VectorValue->Length() );
         const float* r = m_data.f32VectorValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = *r++;
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::F64Vector:
      {
         C32Matrix m( 1, m_data.f64VectorValue->Length() );
         const double* r = m_data.f64VectorValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::C32Vector:
      {
         return C32Matrix( m_data.c32VectorValue->Begin(), 1, m_data.c32VectorValue->Length() );
      }
   case VariantType::C64Vector:
      {
         return C32Matrix( m_data.c64VectorValue->Begin(), 1, m_data.c64VectorValue->Length() );
      }

   case VariantType::I8Matrix:
      {
         C32Matrix m( m_data.i8MatrixValue->Rows(), m_data.i8MatrixValue->Cols() );
         const int8* r = m_data.i8MatrixValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI8Matrix:
      {
         C32Matrix m( m_data.ui8MatrixValue->Rows(), m_data.ui8MatrixValue->Cols() );
         const uint8* r = m_data.ui8MatrixValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::I16Matrix:
      {
         C32Matrix m( m_data.i16MatrixValue->Rows(), m_data.i16MatrixValue->Cols() );
         const int16* r = m_data.i16MatrixValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI16Matrix:
      {
         C32Matrix m( m_data.ui16MatrixValue->Rows(), m_data.ui16MatrixValue->Cols() );
         const uint16* r = m_data.ui16MatrixValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::I32Matrix:
      {
         C32Matrix m( m_data.i32MatrixValue->Rows(), m_data.i32MatrixValue->Cols() );
         const int32* r = m_data.i32MatrixValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI32Matrix:
      {
         C32Matrix m( m_data.ui32MatrixValue->Rows(), m_data.ui32MatrixValue->Cols() );
         const uint32* r = m_data.ui32MatrixValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::I64Matrix:
      {
         C32Matrix m( m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
         const int64* r = m_data.i64MatrixValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI64Matrix:
      {
         C32Matrix m( m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
         const uint64* r = m_data.ui64MatrixValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::F32Matrix:
      {
         C32Matrix m( m_data.f32MatrixValue->Rows(), m_data.f32MatrixValue->Cols() );
         const float* r = m_data.f32MatrixValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = *r++;
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::F64Matrix:
      {
         C32Matrix m( m_data.f64MatrixValue->Rows(), m_data.f64MatrixValue->Cols() );
         const double* r = m_data.f64MatrixValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::C32Matrix:
      {
         return *m_data.c32MatrixValue;
      }
   case VariantType::C64Matrix:
      {
         return C32Matrix( m_data.c64MatrixValue->Begin(), m_data.c64MatrixValue->Rows(), m_data.c64MatrixValue->Cols() );
      }

   case VariantType::ByteArray:
      {
         C32Matrix m( 1, int( m_data.byteArrayValue->Length() ) );
         const uint8* r = m_data.byteArrayValue->Begin();
         for ( fcomplex& c : m )
         {
            c.Real() = float( *r++ );
            c.Imag() = 0;
         }
         return m;
      }

   default:
      throw Error( "Variant::ToC32Matrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

C64Matrix Variant::ToC64Matrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return C64Matrix( Complex64( double( m_data.boolValue ) ), 1, 1 );
   case VariantType::Int8:
      return C64Matrix( Complex64( double( m_data.int8Value ) ), 1, 1 );
   case VariantType::Int16:
      return C64Matrix( Complex64( double( m_data.int16Value ) ), 1, 1 );
   case VariantType::Int32:
      return C64Matrix( Complex64( double( m_data.int32Value ) ), 1, 1 );
   case VariantType::Int64:
      return C64Matrix( Complex64( double( m_data.int64Value ) ), 1, 1 );
   case VariantType::UInt8:
      return C64Matrix( Complex64( double( m_data.uint8Value ) ), 1, 1 );
   case VariantType::UInt16:
      return C64Matrix( Complex64( double( m_data.uint16Value ) ), 1, 1 );
   case VariantType::UInt32:
      return C64Matrix( Complex64( double( m_data.uint32Value ) ), 1, 1 );
   case VariantType::UInt64:
      return C64Matrix( Complex64( double( m_data.uint64Value ) ), 1, 1 );
   case VariantType::Float32:
      return C64Matrix( Complex64( double( m_data.float32Value ) ), 1, 1 );
   case VariantType::Float64:
      return C64Matrix( Complex64( m_data.float64Value ), 1, 1 );

   case VariantType::Complex32:
      return C64Matrix( Complex64( *m_data.complex32Value ), 1, 1 );
   case VariantType::Complex64:
      return C64Matrix( *m_data.complex64Value, 1, 1 );

   case VariantType::TimePoint:
      return C64Matrix( Complex64( double( m_data.timePointValue->JDI() ), double( m_data.timePointValue->JDF() ) ), 1, 1 );

   case VariantType::I32Point:
      return C64Matrix( Complex64( double( m_data.i32PointValue->x ), double( m_data.i32PointValue->y ) ), 1, 1 );
   case VariantType::F32Point:
      return C64Matrix( Complex64( double( m_data.f32PointValue->x ), double( m_data.f32PointValue->y ) ), 1, 1 );
   case VariantType::F64Point:
      return C64Matrix( Complex64( m_data.f64PointValue->x, m_data.f64PointValue->y ), 1, 1 );

   case VariantType::I32Rect:
      {
         C64Matrix m( 1, 2 );
         m[0][0].Real() = double( m_data.i32RectValue->x0 );
         m[0][0].Imag() = double( m_data.i32RectValue->y0 );
         m[0][1].Real() = double( m_data.i32RectValue->x1 );
         m[0][1].Imag() = double( m_data.i32RectValue->y1 );
         return m;
      }
   case VariantType::F32Rect:
      {
         C64Matrix m( 1, 2 );
         m[0][0].Real() = double( m_data.f32RectValue->x0 );
         m[0][0].Imag() = double( m_data.f32RectValue->y0 );
         m[0][1].Real() = double( m_data.f32RectValue->x1 );
         m[0][1].Imag() = double( m_data.f32RectValue->y1 );
         return m;
      }
   case VariantType::F64Rect:
      {
         C64Matrix m( 1, 2 );
         m[0][0].Real() = m_data.f64RectValue->x0;
         m[0][0].Imag() = m_data.f64RectValue->y0;
         m[0][1].Real() = m_data.f64RectValue->x1;
         m[0][1].Imag() = m_data.f64RectValue->y1;
         return m;
      }

   case VariantType::I8Vector:
      {
         C64Matrix m( 1, m_data.i8VectorValue->Length() );
         const int8* r = m_data.i8VectorValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI8Vector:
      {
         C64Matrix m( 1, m_data.ui8VectorValue->Length() );
         const uint8* r = m_data.ui8VectorValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::I16Vector:
      {
         C64Matrix m( 1, m_data.i16VectorValue->Length() );
         const int16* r = m_data.i16VectorValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI16Vector:
      {
         C64Matrix m( 1, m_data.ui16VectorValue->Length() );
         const uint16* r = m_data.ui16VectorValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::I32Vector:
      {
         C64Matrix m( 1, m_data.i32VectorValue->Length() );
         const int32* r = m_data.i32VectorValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI32Vector:
      {
         C64Matrix m( 1, m_data.ui32VectorValue->Length() );
         const uint32* r = m_data.ui32VectorValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::I64Vector:
      {
         C64Matrix m( 1, m_data.i64VectorValue->Length() );
         const int64* r = m_data.i64VectorValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI64Vector:
      {
         C64Matrix m( 1, m_data.ui64VectorValue->Length() );
         const uint64* r = m_data.ui64VectorValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::F32Vector:
      {
         C64Matrix m( 1, m_data.f32VectorValue->Length() );
         const float* r = m_data.f32VectorValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::F64Vector:
      {
         C64Matrix m( 1, m_data.f64VectorValue->Length() );
         const double* r = m_data.f64VectorValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = *r++;
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::C32Vector:
      {
         return C64Matrix( m_data.c32VectorValue->Begin(), 1, m_data.c32VectorValue->Length() );
      }
   case VariantType::C64Vector:
      {
         return C64Matrix( m_data.c64VectorValue->Begin(), 1, m_data.c64VectorValue->Length() );
      }

   case VariantType::I8Matrix:
      {
         C64Matrix m( m_data.i8MatrixValue->Rows(), m_data.i8MatrixValue->Cols() );
         const int8* r = m_data.i8MatrixValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI8Matrix:
      {
         C64Matrix m( m_data.ui8MatrixValue->Rows(), m_data.ui8MatrixValue->Cols() );
         const uint8* r = m_data.ui8MatrixValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::I16Matrix:
      {
         C64Matrix m( m_data.i16MatrixValue->Rows(), m_data.i16MatrixValue->Cols() );
         const int16* r = m_data.i16MatrixValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI16Matrix:
      {
         C64Matrix m( m_data.ui16MatrixValue->Rows(), m_data.ui16MatrixValue->Cols() );
         const uint16* r = m_data.ui16MatrixValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::I32Matrix:
      {
         C64Matrix m( m_data.i32MatrixValue->Rows(), m_data.i32MatrixValue->Cols() );
         const int32* r = m_data.i32MatrixValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI32Matrix:
      {
         C64Matrix m( m_data.ui32MatrixValue->Rows(), m_data.ui32MatrixValue->Cols() );
         const uint32* r = m_data.ui32MatrixValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::I64Matrix:
      {
         C64Matrix m( m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
         const int64* r = m_data.i64MatrixValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::UI64Matrix:
      {
         C64Matrix m( m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
         const uint64* r = m_data.ui64MatrixValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::F32Matrix:
      {
         C64Matrix m( m_data.f32MatrixValue->Rows(), m_data.f32MatrixValue->Cols() );
         const float* r = m_data.f32MatrixValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::F64Matrix:
      {
         C64Matrix m( m_data.f64MatrixValue->Rows(), m_data.f64MatrixValue->Cols() );
         const double* r = m_data.f64MatrixValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = *r++;
            c.Imag() = 0;
         }
         return m;
      }
   case VariantType::C32Matrix:
      {
         return C64Matrix( m_data.c32MatrixValue->Begin(), m_data.c32MatrixValue->Rows(), m_data.c32MatrixValue->Cols() );
      }
   case VariantType::C64Matrix:
      {
         return *m_data.c64MatrixValue;
      }

   case VariantType::ByteArray:
      {
         C64Matrix m( 1, int( m_data.byteArrayValue->Length() ) );
         const uint8* r = m_data.byteArrayValue->Begin();
         for ( dcomplex& c : m )
         {
            c.Real() = double( *r++ );
            c.Imag() = 0;
         }
         return m;
      }

   default:
      throw Error( "Variant::ToC64Matrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

ByteArray Variant::ToByteArray() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      {
         ByteArray a( 1 );
         *a = uint8( m_data.boolValue );
         return a;
      }
   case VariantType::Int8:
      {
         return ByteArray( 1, uint8( m_data.int8Value ) );
      }
   case VariantType::Int16:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.int16Value );
         return ByteArray( v, v + sizeof( int16 ) );
      }
   case VariantType::Int32:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.int32Value );
         return ByteArray( v, v + sizeof( int32 ) );
      }
   case VariantType::Int64:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.int64Value );
         return ByteArray( v, v + sizeof( int64 ) );
      }
   case VariantType::UInt8:
      {
         return ByteArray( 1, m_data.uint8Value );
      }
   case VariantType::UInt16:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.uint16Value );
         return ByteArray( v, v + sizeof( uint16 ) );
      }
   case VariantType::UInt32:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.uint32Value );
         return ByteArray( v, v + sizeof( uint32 ) );
      }
   case VariantType::UInt64:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.uint64Value );
         return ByteArray( v, v + sizeof( uint64 ) );
      }
   case VariantType::Float32:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.float32Value );
         return ByteArray( v, v + sizeof( float ) );
      }
   case VariantType::Float64:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.float64Value );
         return ByteArray( v, v + sizeof( double ) );
      }

   case VariantType::Complex32:
      {
         float r = m_data.complex32Value->Real();
         float i = m_data.complex32Value->Imag();
         const uint8* v1 = reinterpret_cast<const uint8*>( &r );
         const uint8* v2 = reinterpret_cast<const uint8*>( &i );
         ByteArray a( 2*sizeof( float ) );
         ::memcpy( a.Begin(),                   v1, sizeof( float ) );
         ::memcpy( a.Begin() + sizeof( float ), v2, sizeof( float ) );
         return a;
      }
   case VariantType::Complex64:
      {
         float r = m_data.complex64Value->Real();
         float i = m_data.complex64Value->Imag();
         const uint8* v1 = reinterpret_cast<const uint8*>( &r );
         const uint8* v2 = reinterpret_cast<const uint8*>( &i );
         ByteArray a( 2*sizeof( double ) );
         ::memcpy( a.Begin(),                    v1, sizeof( double ) );
         ::memcpy( a.Begin() + sizeof( double ), v2, sizeof( double ) );
         return a;
      }

   case VariantType::TimePoint:
      {
         int32 jdi = m_data.timePointValue->JDI();
         double jdf = m_data.timePointValue->JDF();
         const uint8* v1 = reinterpret_cast<const uint8*>( &jdi );
         const uint8* v2 = reinterpret_cast<const uint8*>( &jdf );
         ByteArray a( sizeof( jdi ) + sizeof( jdf ) );
         ::memcpy( a.Begin(),                 v1, sizeof( jdi ) );
         ::memcpy( a.Begin() + sizeof( jdi ), v2, sizeof( jdf ) );
         return a;
      }

   case VariantType::I32Point:
      {
         const uint8* v1 = reinterpret_cast<const uint8*>( &m_data.i32PointValue->x );
         const uint8* v2 = reinterpret_cast<const uint8*>( &m_data.i32PointValue->y );
         ByteArray a( 2*sizeof( int32 ) );
         ::memcpy( a.Begin(),                   v1, sizeof( int32 ) );
         ::memcpy( a.Begin() + sizeof( int32 ), v2, sizeof( int32 ) );
         return a;
      }
   case VariantType::F32Point:
      {
         const uint8* v1 = reinterpret_cast<const uint8*>( &m_data.f32PointValue->x );
         const uint8* v2 = reinterpret_cast<const uint8*>( &m_data.f32PointValue->y );
         ByteArray a( 2*sizeof( float ) );
         ::memcpy( a.Begin(),                   v1, sizeof( float ) );
         ::memcpy( a.Begin() + sizeof( float ), v2, sizeof( float ) );
         return a;
      }
   case VariantType::F64Point:
      {
         const uint8* v1 = reinterpret_cast<const uint8*>( &m_data.f64PointValue->x );
         const uint8* v2 = reinterpret_cast<const uint8*>( &m_data.f64PointValue->y );
         ByteArray a( 2*sizeof( double ) );
         ::memcpy( a.Begin(),                    v1, sizeof( double ) );
         ::memcpy( a.Begin() + sizeof( double ), v2, sizeof( double ) );
         return a;
      }

   case VariantType::I32Rect:
      {
         const uint8* v1 = reinterpret_cast<const uint8*>( &m_data.i32RectValue->x0 );
         const uint8* v2 = reinterpret_cast<const uint8*>( &m_data.i32RectValue->y0 );
         const uint8* v3 = reinterpret_cast<const uint8*>( &m_data.i32RectValue->x1 );
         const uint8* v4 = reinterpret_cast<const uint8*>( &m_data.i32RectValue->y1 );
         ByteArray a( 4*sizeof( int32 ) );
         ::memcpy( a.Begin(),                     v1, sizeof( int32 ) );
         ::memcpy( a.Begin() +   sizeof( int32 ), v2, sizeof( int32 ) );
         ::memcpy( a.Begin() + 2*sizeof( int32 ), v3, sizeof( int32 ) );
         ::memcpy( a.Begin() + 3*sizeof( int32 ), v4, sizeof( int32 ) );
         return a;
      }
   case VariantType::F32Rect:
      {
         const uint8* v1 = reinterpret_cast<const uint8*>( &m_data.f32RectValue->x0 );
         const uint8* v2 = reinterpret_cast<const uint8*>( &m_data.f32RectValue->y0 );
         const uint8* v3 = reinterpret_cast<const uint8*>( &m_data.f32RectValue->x1 );
         const uint8* v4 = reinterpret_cast<const uint8*>( &m_data.f32RectValue->y1 );
         ByteArray a( 4*sizeof( float ) );
         ::memcpy( a.Begin(),                     v1, sizeof( float ) );
         ::memcpy( a.Begin() +   sizeof( float ), v2, sizeof( float ) );
         ::memcpy( a.Begin() + 2*sizeof( float ), v3, sizeof( float ) );
         ::memcpy( a.Begin() + 3*sizeof( float ), v4, sizeof( float ) );
         return a;
      }
   case VariantType::F64Rect:
      {
         const uint8* v1 = reinterpret_cast<const uint8*>( &m_data.f64RectValue->x0 );
         const uint8* v2 = reinterpret_cast<const uint8*>( &m_data.f64RectValue->y0 );
         const uint8* v3 = reinterpret_cast<const uint8*>( &m_data.f64RectValue->x1 );
         const uint8* v4 = reinterpret_cast<const uint8*>( &m_data.f64RectValue->y1 );
         ByteArray a( 4*sizeof( double ) );
         ::memcpy( a.Begin(),                      v1, sizeof( double ) );
         ::memcpy( a.Begin() +   sizeof( double ), v2, sizeof( double ) );
         ::memcpy( a.Begin() + 2*sizeof( double ), v3, sizeof( double ) );
         ::memcpy( a.Begin() + 3*sizeof( double ), v4, sizeof( double ) );
         return a;
      }

   case VariantType::I8Vector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.i8VectorValue->Begin() );
         return ByteArray( v, v + m_data.i8VectorValue->Length()*sizeof( int8 ) );
      }
   case VariantType::UI8Vector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.ui8VectorValue->Begin() );
         return ByteArray( v, v + m_data.ui8VectorValue->Length()*sizeof( uint8 ) );
      }
   case VariantType::I16Vector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.i16VectorValue->Begin() );
         return ByteArray( v, v + m_data.i16VectorValue->Length()*sizeof( int16 ) );
      }
   case VariantType::UI16Vector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.ui16VectorValue->Begin() );
         return ByteArray( v, v + m_data.ui16VectorValue->Length()*sizeof( uint16 ) );
      }
   case VariantType::I32Vector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.i32VectorValue->Begin() );
         return ByteArray( v, v + m_data.i32VectorValue->Length()*sizeof( int32 ) );
      }
   case VariantType::UI32Vector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.ui32VectorValue->Begin() );
         return ByteArray( v, v + m_data.ui32VectorValue->Length()*sizeof( uint32 ) );
      }
   case VariantType::I64Vector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.i64VectorValue->Begin() );
         return ByteArray( v, v + m_data.i64VectorValue->Length()*sizeof( int64 ) );
      }
   case VariantType::UI64Vector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.ui64VectorValue->Begin() );
         return ByteArray( v, v + m_data.ui64VectorValue->Length()*sizeof( uint64 ) );
      }
   case VariantType::F32Vector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.f32VectorValue->Begin() );
         return ByteArray( v, v + m_data.f32VectorValue->Length()*sizeof( float ) );
      }
   case VariantType::F64Vector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.f64VectorValue->Begin() );
         return ByteArray( v, v + m_data.f64VectorValue->Length()*sizeof( double ) );
      }
   case VariantType::C32Vector:
      {
         ByteArray a( m_data.c32VectorValue->Length()*2*sizeof( float ) );
         const fcomplex* c = m_data.c32VectorValue->Begin();
         uint8* b = a.Begin();
         for ( int i = 0; i < m_data.c32VectorValue->Length(); ++i, ++c, b += 2*sizeof( float ) )
         {
            float re = c->Real();
            float im = c->Imag();
            const uint8* v1 = reinterpret_cast<const uint8*>( &re );
            const uint8* v2 = reinterpret_cast<const uint8*>( &im );
            ::memcpy( b,                   v1, sizeof( float ) );
            ::memcpy( b + sizeof( float ), v2, sizeof( float ) );
         }
         return a;
      }
   case VariantType::C64Vector:
      {
         ByteArray a( m_data.c64VectorValue->Length()*2*sizeof( double ) );
         const dcomplex* c = m_data.c64VectorValue->Begin();
         uint8* b = a.Begin();
         for ( int i = 0; i < m_data.c64VectorValue->Length(); ++i, ++c, b += 2*sizeof( double ) )
         {
            double re = c->Real();
            double im = c->Imag();
            const uint8* v1 = reinterpret_cast<const uint8*>( &re );
            const uint8* v2 = reinterpret_cast<const uint8*>( &im );
            ::memcpy( b,                    v1, sizeof( double ) );
            ::memcpy( b + sizeof( double ), v2, sizeof( double ) );
         }
         return a;
      }

   case VariantType::I8Matrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.i8MatrixValue->Begin() );
         return ByteArray( v, v + m_data.i8MatrixValue->NumberOfElements()*sizeof( int8 ) );
      }
   case VariantType::UI8Matrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.ui8MatrixValue->Begin() );
         return ByteArray( v, v + m_data.ui8MatrixValue->NumberOfElements()*sizeof( uint8 ) );
      }
   case VariantType::I16Matrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.i16MatrixValue->Begin() );
         return ByteArray( v, v + m_data.i16MatrixValue->NumberOfElements()*sizeof( int16 ) );
      }
   case VariantType::UI16Matrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.ui16MatrixValue->Begin() );
         return ByteArray( v, v + m_data.ui16MatrixValue->NumberOfElements()*sizeof( uint16 ) );
      }
   case VariantType::I32Matrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.i32MatrixValue->Begin() );
         return ByteArray( v, v + m_data.i32MatrixValue->NumberOfElements()*sizeof( int32 ) );
      }
   case VariantType::UI32Matrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.ui32MatrixValue->Begin() );
         return ByteArray( v, v + m_data.ui32MatrixValue->NumberOfElements()*sizeof( uint32 ) );
      }
   case VariantType::I64Matrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.i64MatrixValue->Begin() );
         return ByteArray( v, v + m_data.i64MatrixValue->NumberOfElements()*sizeof( int64 ) );
      }
   case VariantType::UI64Matrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.ui64MatrixValue->Begin() );
         return ByteArray( v, v + m_data.ui64MatrixValue->NumberOfElements()*sizeof( uint64 ) );
      }
   case VariantType::F32Matrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.f32MatrixValue->Begin() );
         return ByteArray( v, v + m_data.f32MatrixValue->NumberOfElements()*sizeof( float ) );
      }
   case VariantType::F64Matrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.f64MatrixValue->Begin() );
         return ByteArray( v, v + m_data.f64MatrixValue->NumberOfElements()*sizeof( double ) );
      }
   case VariantType::C32Matrix:
      {
         size_type n = m_data.c32MatrixValue->NumberOfElements();
         ByteArray a( n*2*sizeof( float ) );
         const fcomplex* c = m_data.c32MatrixValue->Begin();
         uint8* b = a.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, b += 2*sizeof( float ) )
         {
            float re = c->Real();
            float im = c->Imag();
            const uint8* v1 = reinterpret_cast<const uint8*>( &re );
            const uint8* v2 = reinterpret_cast<const uint8*>( &im );
            ::memcpy( b,                   v1, sizeof( float ) );
            ::memcpy( b + sizeof( float ), v2, sizeof( float ) );
         }
         return a;
      }
   case VariantType::C64Matrix:
      {
         size_type n = m_data.c64MatrixValue->NumberOfElements();
         ByteArray a( n*2*sizeof( double ) );
         const dcomplex* c = m_data.c64MatrixValue->Begin();
         uint8* b = a.Begin();
         for ( size_type i = 0; i < n; ++i, ++c, b += 2*sizeof( double ) )
         {
            double re = c->Real();
            double im = c->Imag();
            const uint8* v1 = reinterpret_cast<const uint8*>( &re );
            const uint8* v2 = reinterpret_cast<const uint8*>( &im );
            ::memcpy( b,                    v1, sizeof( double ) );
            ::memcpy( b + sizeof( double ), v2, sizeof( double ) );
         }
         return a;
      }

   case VariantType::ByteArray:
      return *m_data.byteArrayValue;

   case VariantType::String:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.stringValue->c_str() );
         return ByteArray( v, v + m_data.stringValue->Length()*sizeof( String::char_type ) );
      }
   case VariantType::IsoString:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.isoStringValue->c_str() );
         return ByteArray( v, v + m_data.isoStringValue->Length()*sizeof( IsoString::char_type ) );
      }

   default:
      throw Error( "Variant::ToByteArray(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

String Variant::ToString() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return String( m_data.boolValue );
   case VariantType::Int8:
      return String( int( m_data.int8Value ) );
   case VariantType::Int16:
      return String( int( m_data.int16Value ) );
   case VariantType::Int32:
      return String( int( m_data.int32Value ) );
   case VariantType::Int64:
      return String( m_data.int64Value );
   case VariantType::UInt8:
      return String( unsigned( m_data.uint8Value ) );
   case VariantType::UInt16:
      return String( unsigned( m_data.uint16Value ) );
   case VariantType::UInt32:
      return String( unsigned( m_data.uint32Value ) );
   case VariantType::UInt64:
      return String( m_data.uint64Value );
   case VariantType::Float32:
      return String( m_data.float32Value );
   case VariantType::Float64:
      return String( m_data.float64Value );

   case VariantType::Complex32:
      return String().Format( "(%.7g,%.7g)", m_data.complex32Value->Real(), m_data.complex32Value->Imag() );
   case VariantType::Complex64:
      return String().Format( "(%.16g,%.16g)", m_data.complex64Value->Real(), m_data.complex64Value->Imag() );

   case VariantType::TimePoint:
      return m_data.timePointValue->ToString();

   case VariantType::I32Point:
      return String().Format( "(%d,%d)", m_data.i32PointValue->x, m_data.i32PointValue->y );
   case VariantType::F32Point:
      return String().Format( "(%.7g,%.7g)", m_data.f32PointValue->x, m_data.f32PointValue->y );
   case VariantType::F64Point:
      return String().Format( "(%.16g,%.16g)", m_data.f64PointValue->x, m_data.f64PointValue->y );

   case VariantType::I32Rect:
      return String().Format( "(%d,%d,%d,%d)", m_data.i32RectValue->x0, m_data.i32RectValue->y0,
                                               m_data.i32RectValue->x1, m_data.i32RectValue->y1 );
   case VariantType::F32Rect:
      return String().Format( "(%d,%d,%d,%d)", m_data.f32RectValue->x0, m_data.f32RectValue->y0,
                                               m_data.f32RectValue->x1, m_data.f32RectValue->y1 );
   case VariantType::F64Rect:
      return String().Format( "(%d,%d,%d,%d)", m_data.f64RectValue->x0, m_data.f64RectValue->y0,
                                               m_data.f64RectValue->x1, m_data.f64RectValue->y1 );

   case VariantType::I8Vector:
      {
         String s = '{';
         if ( !m_data.i8VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( int( (*m_data.i8VectorValue)[i] ) ) );
               if ( ++i == m_data.i8VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI8Vector:
      {
         String s = '{';
         if ( !m_data.ui8VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( unsigned( (*m_data.ui8VectorValue)[i] ) ) );
               if ( ++i == m_data.ui8VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::I16Vector:
      {
         String s = '{';
         if ( !m_data.i16VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( int( (*m_data.i16VectorValue)[i] ) ) );
               if ( ++i == m_data.i16VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI16Vector:
      {
         String s = '{';
         if ( !m_data.ui16VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( unsigned( (*m_data.ui16VectorValue)[i] ) ) );
               if ( ++i == m_data.ui16VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::I32Vector:
      {
         String s = '{';
         if ( !m_data.i32VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( (*m_data.i32VectorValue)[i] ) );
               if ( ++i == m_data.i32VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI32Vector:
      {
         String s = '{';
         if ( !m_data.ui32VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( (*m_data.ui32VectorValue)[i] ) );
               if ( ++i == m_data.ui32VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::I64Vector:
      {
         String s = '{';
         if ( !m_data.i64VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( (*m_data.i64VectorValue)[i] ) );
               if ( ++i == m_data.i64VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI64Vector:
      {
         String s = '{';
         if ( !m_data.ui64VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( (*m_data.ui64VectorValue)[i] ) );
               if ( ++i == m_data.ui64VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::F32Vector:
      {
         String s = '{';
         if ( !m_data.f32VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( (*m_data.f32VectorValue)[i] ) );
               if ( ++i == m_data.f32VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::F64Vector:
      {
         String s = '{';
         if ( !m_data.f64VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( (*m_data.f64VectorValue)[i] ) );
               if ( ++i == m_data.f64VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::C32Vector:
      {
         String s = '{';
         if ( !m_data.c32VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.AppendFormat( "(%.7g,%.7g)", (*m_data.c32VectorValue)[i].Real(), (*m_data.c32VectorValue)[i].Imag() );
               if ( ++i == m_data.c32VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::C64Vector:
      {
         String s = '{';
         if ( !m_data.c64VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.AppendFormat( "(%.16g,%.16g)", (*m_data.c64VectorValue)[i].Real(), (*m_data.c64VectorValue)[i].Imag() );
               if ( ++i == m_data.c64VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }

   case VariantType::I8Matrix:
      {
         String s = '{';
         if ( !m_data.i8MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( int( (*m_data.i8MatrixValue)[i][j] ) ) );
                  if ( ++j == m_data.i8MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.i8MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI8Matrix:
      {
         String s = '{';
         if ( !m_data.ui8MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( unsigned( (*m_data.ui8MatrixValue)[i][j] ) ) );
                  if ( ++j == m_data.ui8MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.ui8MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::I16Matrix:
      {
         String s = '{';
         if ( !m_data.i16MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( int( (*m_data.i16MatrixValue)[i][j] ) ) );
                  if ( ++j == m_data.i16MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.i16MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI16Matrix:
      {
         String s = '{';
         if ( !m_data.ui16MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( unsigned( (*m_data.ui16MatrixValue)[i][j] ) ) );
                  if ( ++j == m_data.ui16MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.ui16MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::I32Matrix:
      {
         String s = '{';
         if ( !m_data.i32MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( (*m_data.i32MatrixValue)[i][j] ) );
                  if ( ++j == m_data.i32MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.i32MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI32Matrix:
      {
         String s = '{';
         if ( !m_data.ui32MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( (*m_data.ui32MatrixValue)[i][j] ) );
                  if ( ++j == m_data.ui32MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.ui32MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::I64Matrix:
      {
         String s = '{';
         if ( !m_data.i64MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( (*m_data.i64MatrixValue)[i][j] ) );
                  if ( ++j == m_data.i64MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.i64MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI64Matrix:
      {
         String s = '{';
         if ( !m_data.ui64MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( (*m_data.ui64MatrixValue)[i][j] ) );
                  if ( ++j == m_data.ui64MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.ui64MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::F32Matrix:
      {
         String s = '{';
         if ( !m_data.f32MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( (*m_data.f32MatrixValue)[i][j] ) );
                  if ( ++j == m_data.f32MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.f32MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::F64Matrix:
      {
         String s = '{';
         if ( !m_data.f64MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( (*m_data.f64MatrixValue)[i][j] ) );
                  if ( ++j == m_data.f64MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.f64MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::C32Matrix:
      {
         String s = '{';
         if ( !m_data.c32MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.AppendFormat( "(%.7g,%.7g)", (*m_data.c32MatrixValue)[i][j].Real(), (*m_data.c32MatrixValue)[i][j].Imag() );
                  if ( ++j == m_data.c32MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.c32MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::C64Matrix:
      {
         String s = '{';
         if ( !m_data.c64MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.AppendFormat( "(%.16g,%.16g)", (*m_data.c64MatrixValue)[i][j].Real(), (*m_data.c64MatrixValue)[i][j].Imag() );
                  if ( ++j == m_data.c64MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.c64MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }

   case VariantType::ByteArray:
      {
         String s = '{';
         if ( !m_data.byteArrayValue->IsEmpty() )
            for ( size_type i = 0; ; )
            {
               s.Append( String( unsigned( (*m_data.byteArrayValue)[i] ) ) );
               if ( ++i == m_data.byteArrayValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }

   case VariantType::String:
      return *m_data.stringValue;
   case VariantType::IsoString:
      return String( *m_data.isoStringValue );

   case VariantType::StringList:
      {
         String s = '{';
         if ( !m_data.stringListValue->IsEmpty() )
            for ( size_type i = 0; ; )
            {
               s.Append( '\"' );
               s.Append( (*m_data.stringListValue)[i] );
               s.Append( '\"' );
               if ( ++i == m_data.stringListValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::IsoStringList:
      {
         String s = '{';
         if ( !m_data.isoStringListValue->IsEmpty() )
            for ( size_type i = 0; ; )
            {
               s.Append( '\"' );
               s.Append( String( (*m_data.isoStringListValue)[i] ) );
               s.Append( '\"' );
               if ( ++i == m_data.isoStringListValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }

   case VariantType::StringKeyValue:
      return String( "{\"" +
                     m_data.stringKeyValueValue->key +
                     "\",\"" +
                     m_data.stringKeyValueValue->value +
                     "\"}" );
   case VariantType::IsoStringKeyValue:
      return String( "{\"" +
                     m_data.isoStringKeyValueValue->key +
                     "\",\"" +
                     m_data.isoStringKeyValueValue->value +
                     "\"}" );

   case VariantType::StringKeyValueList:
      {
         String s = '{';
         if ( !m_data.stringKeyValueListValue->IsEmpty() )
            for ( size_type i = 0; ; )
            {
               s.Append( "{\"" );
               s.Append( (*m_data.stringKeyValueListValue)[i].key );
               s.Append( "\",\"" );
               s.Append( (*m_data.stringKeyValueListValue)[i].value );
               s.Append( "\"}" );
               if ( ++i == m_data.stringKeyValueListValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::IsoStringKeyValueList:
      {
         String s = '{';
         if ( !m_data.isoStringKeyValueListValue->IsEmpty() )
            for ( size_type i = 0; ; )
            {
               s.Append( "{\"" );
               s.Append( String( (*m_data.isoStringKeyValueListValue)[i].key ) );
               s.Append( "\",\"" );
               s.Append( String( (*m_data.isoStringKeyValueListValue)[i].value ) );
               s.Append( "\"}" );
               if ( ++i == m_data.isoStringKeyValueListValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }

   default:
      throw Error( "Variant::ToString(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

IsoString Variant::ToIsoString() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return IsoString( m_data.boolValue );
   case VariantType::Int8:
      return IsoString( int( m_data.int8Value ) );
   case VariantType::Int16:
      return IsoString( int( m_data.int16Value ) );
   case VariantType::Int32:
      return IsoString( int( m_data.int32Value ) );
   case VariantType::Int64:
      return IsoString( m_data.int64Value );
   case VariantType::UInt8:
      return IsoString( unsigned( m_data.uint8Value ) );
   case VariantType::UInt16:
      return IsoString( unsigned( m_data.uint16Value ) );
   case VariantType::UInt32:
      return IsoString( unsigned( m_data.uint32Value ) );
   case VariantType::UInt64:
      return IsoString( m_data.uint64Value );
   case VariantType::Float32:
      return IsoString( m_data.float32Value );
   case VariantType::Float64:
      return IsoString( m_data.float64Value );

   case VariantType::Complex32:
      return IsoString().Format( "(%.7g,%.7g)", m_data.complex32Value->Real(), m_data.complex32Value->Imag() );
   case VariantType::Complex64:
      return IsoString().Format( "(%.16g,%.16g)", m_data.complex64Value->Real(), m_data.complex64Value->Imag() );

   case VariantType::TimePoint:
      return m_data.timePointValue->ToIsoString();

   case VariantType::I32Point:
      return IsoString().Format( "(%d,%d)", m_data.i32PointValue->x, m_data.i32PointValue->y );
   case VariantType::F32Point:
      return IsoString().Format( "(%.7g,%.7g)", m_data.f32PointValue->x, m_data.f32PointValue->y );
   case VariantType::F64Point:
      return IsoString().Format( "(%.16g,%.16g)", m_data.f64PointValue->x, m_data.f64PointValue->y );

   case VariantType::I32Rect:
      return IsoString().Format( "(%d,%d,%d,%d)", m_data.i32RectValue->x0, m_data.i32RectValue->y0,
                                                  m_data.i32RectValue->x1, m_data.i32RectValue->y1 );
   case VariantType::F32Rect:
      return IsoString().Format( "(%d,%d,%d,%d)", m_data.f32RectValue->x0, m_data.f32RectValue->y0,
                                                  m_data.f32RectValue->x1, m_data.f32RectValue->y1 );
   case VariantType::F64Rect:
      return IsoString().Format( "(%d,%d,%d,%d)", m_data.f64RectValue->x0, m_data.f64RectValue->y0,
                                                  m_data.f64RectValue->x1, m_data.f64RectValue->y1 );

   case VariantType::I8Vector:
      {
         IsoString s = '{';
         if ( !m_data.i8VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( int( (*m_data.i8VectorValue)[i] ) ) );
               if ( ++i == m_data.i8VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI8Vector:
      {
         IsoString s = '{';
         if ( !m_data.ui8VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( unsigned( (*m_data.ui8VectorValue)[i] ) ) );
               if ( ++i == m_data.ui8VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::I16Vector:
      {
         IsoString s = '{';
         if ( !m_data.i16VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( int( (*m_data.i16VectorValue)[i] ) ) );
               if ( ++i == m_data.i16VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI16Vector:
      {
         IsoString s = '{';
         if ( !m_data.ui16VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( unsigned( (*m_data.ui16VectorValue)[i] ) ) );
               if ( ++i == m_data.ui16VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::I32Vector:
      {
         IsoString s = '{';
         if ( !m_data.i32VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( (*m_data.i32VectorValue)[i] ) );
               if ( ++i == m_data.i32VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI32Vector:
      {
         IsoString s = '{';
         if ( !m_data.ui32VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( (*m_data.ui32VectorValue)[i] ) );
               if ( ++i == m_data.ui32VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::I64Vector:
      {
         IsoString s = '{';
         if ( !m_data.i64VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( (*m_data.i64VectorValue)[i] ) );
               if ( ++i == m_data.i64VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI64Vector:
      {
         IsoString s = '{';
         if ( !m_data.ui64VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( (*m_data.ui64VectorValue)[i] ) );
               if ( ++i == m_data.ui64VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::F32Vector:
      {
         IsoString s = '{';
         if ( !m_data.f32VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( (*m_data.f32VectorValue)[i] ) );
               if ( ++i == m_data.f32VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::F64Vector:
      {
         IsoString s = '{';
         if ( !m_data.f64VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( (*m_data.f64VectorValue)[i] ) );
               if ( ++i == m_data.f64VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::C32Vector:
      {
         IsoString s = '{';
         if ( !m_data.c32VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.AppendFormat( "(%.7g,%.7g)", (*m_data.c32VectorValue)[i].Real(), (*m_data.c32VectorValue)[i].Imag() );
               if ( ++i == m_data.c32VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::C64Vector:
      {
         IsoString s = '{';
         if ( !m_data.c64VectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.AppendFormat( "(%.16g,%.16g)", (*m_data.c64VectorValue)[i].Real(), (*m_data.c64VectorValue)[i].Imag() );
               if ( ++i == m_data.c64VectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }

   case VariantType::I8Matrix:
      {
         IsoString s = '{';
         if ( !m_data.i8MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( int( (*m_data.i8MatrixValue)[i][j] ) ) );
                  if ( ++j == m_data.i8MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.i8MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI8Matrix:
      {
         IsoString s = '{';
         if ( !m_data.ui8MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( unsigned( (*m_data.ui8MatrixValue)[i][j] ) ) );
                  if ( ++j == m_data.ui8MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.ui8MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::I16Matrix:
      {
         IsoString s = '{';
         if ( !m_data.i16MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( int( (*m_data.i16MatrixValue)[i][j] ) ) );
                  if ( ++j == m_data.i16MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.i16MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI16Matrix:
      {
         IsoString s = '{';
         if ( !m_data.ui16MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( unsigned( (*m_data.ui16MatrixValue)[i][j] ) ) );
                  if ( ++j == m_data.ui16MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.ui16MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::I32Matrix:
      {
         IsoString s = '{';
         if ( !m_data.i32MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( (*m_data.i32MatrixValue)[i][j] ) );
                  if ( ++j == m_data.i32MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.i32MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI32Matrix:
      {
         IsoString s = '{';
         if ( !m_data.ui32MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( (*m_data.ui32MatrixValue)[i][j] ) );
                  if ( ++j == m_data.ui32MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.ui32MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::I64Matrix:
      {
         IsoString s = '{';
         if ( !m_data.i64MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( (*m_data.i64MatrixValue)[i][j] ) );
                  if ( ++j == m_data.i64MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.i64MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UI64Matrix:
      {
         IsoString s = '{';
         if ( !m_data.ui64MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( (*m_data.ui64MatrixValue)[i][j] ) );
                  if ( ++j == m_data.ui64MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.ui64MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::F32Matrix:
      {
         IsoString s = '{';
         if ( !m_data.f32MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( (*m_data.f32MatrixValue)[i][j] ) );
                  if ( ++j == m_data.f32MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.f32MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::F64Matrix:
      {
         IsoString s = '{';
         if ( !m_data.f64MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( (*m_data.f64MatrixValue)[i][j] ) );
                  if ( ++j == m_data.f64MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.f64MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::C32Matrix:
      {
         IsoString s = '{';
         if ( !m_data.c32MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.AppendFormat( "(%.7g,%.7g)", (*m_data.c32MatrixValue)[i][j].Real(), (*m_data.c32MatrixValue)[i][j].Imag() );
                  if ( ++j == m_data.c32MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.c32MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::C64Matrix:
      {
         IsoString s = '{';
         if ( !m_data.c64MatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.AppendFormat( "(%.16g,%.16g)", (*m_data.c64MatrixValue)[i][j].Real(), (*m_data.c64MatrixValue)[i][j].Imag() );
                  if ( ++j == m_data.c64MatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.c64MatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }

   case VariantType::ByteArray:
      {
         IsoString s = '{';
         if ( !m_data.byteArrayValue->IsEmpty() )
            for ( size_type i = 0; ; )
            {
               s.Append( IsoString( unsigned( (*m_data.byteArrayValue)[i] ) ) );
               if ( ++i == m_data.byteArrayValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }

   case VariantType::String:
      return IsoString( *m_data.stringValue );
   case VariantType::IsoString:
      return *m_data.isoStringValue;

   case VariantType::StringList:
      {
         IsoString s = '{';
         if ( !m_data.stringListValue->IsEmpty() )
            for ( size_type i = 0; ; )
            {
               s.Append( '\"' );
               s.Append( IsoString( (*m_data.stringListValue)[i] ) );
               s.Append( '\"' );
               if ( ++i == m_data.stringListValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::IsoStringList:
      {
         IsoString s = '{';
         if ( !m_data.isoStringListValue->IsEmpty() )
            for ( size_type i = 0; ; )
            {
               s.Append( '\"' );
               s.Append( (*m_data.isoStringListValue)[i] );
               s.Append( '\"' );
               if ( ++i == m_data.isoStringListValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }

   case VariantType::StringKeyValue:
      return IsoString( "{\"" +
                     m_data.stringKeyValueValue->key +
                     "\",\"" +
                     m_data.stringKeyValueValue->value +
                     "\"}" );
   case VariantType::IsoStringKeyValue:
      return IsoString( "{\"" +
                     m_data.isoStringKeyValueValue->key +
                     "\",\"" +
                     m_data.isoStringKeyValueValue->value +
                     "\"}" );

   case VariantType::StringKeyValueList:
      {
         IsoString s = '{';
         if ( !m_data.stringKeyValueListValue->IsEmpty() )
            for ( size_type i = 0; ; )
            {
               s.Append( "{\"" );
               s.Append( IsoString( (*m_data.stringKeyValueListValue)[i].key ) );
               s.Append( "\",\"" );
               s.Append( IsoString( (*m_data.stringKeyValueListValue)[i].value ) );
               s.Append( "\"}" );
               if ( ++i == m_data.stringKeyValueListValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::IsoStringKeyValueList:
      {
         IsoString s = '{';
         if ( !m_data.isoStringKeyValueListValue->IsEmpty() )
            for ( size_type i = 0; ; )
            {
               s.Append( "{\"" );
               s.Append( (*m_data.isoStringKeyValueListValue)[i].key );
               s.Append( "\",\"" );
               s.Append( (*m_data.isoStringKeyValueListValue)[i].value );
               s.Append( "\"}" );
               if ( ++i == m_data.isoStringKeyValueListValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }

   default:
      throw Error( "Variant::ToIsoString(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

StringList Variant::ToStringList() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return StringList( 1, String( m_data.boolValue ) );
   case VariantType::Int8:
      return StringList( 1, String( int( m_data.int8Value ) ) );
   case VariantType::Int16:
      return StringList( 1, String( int( m_data.int16Value ) ) );
   case VariantType::Int32:
      return StringList( 1, String( int( m_data.int32Value ) ) );
   case VariantType::Int64:
      return StringList( 1, String( m_data.int64Value ) );
   case VariantType::UInt8:
      return StringList( 1, String( unsigned( m_data.uint8Value ) ) );
   case VariantType::UInt16:
      return StringList( 1, String( unsigned( m_data.uint16Value ) ) );
   case VariantType::UInt32:
      return StringList( 1, String( unsigned( m_data.uint32Value ) ) );
   case VariantType::UInt64:
      return StringList( 1, String( m_data.uint64Value ) );
   case VariantType::Float32:
      return StringList( 1, String( m_data.float32Value ) );
   case VariantType::Float64:
      return StringList( 1, String( m_data.float64Value ) );

   case VariantType::Complex32:
      {
         StringList s( 2 );
         s[0] = String( m_data.complex32Value->Real() );
         s[1] = String( m_data.complex32Value->Imag() );
         return s;
      }
   case VariantType::Complex64:
      {
         StringList s( 2 );
         s[0] = String( m_data.complex64Value->Real() );
         s[1] = String( m_data.complex64Value->Imag() );
         return s;
      }

   case VariantType::TimePoint:
      return StringList( 1, m_data.timePointValue->ToString() );

   case VariantType::I32Point:
      {
         StringList s( 2 );
         s[0] = String( m_data.i32PointValue->x );
         s[1] = String( m_data.i32PointValue->y );
         return s;
      }
   case VariantType::F32Point:
      {
         StringList s( 2 );
         s[0] = String( m_data.f32PointValue->x );
         s[1] = String( m_data.f32PointValue->y );
         return s;
      }
   case VariantType::F64Point:
      {
         StringList s( 2 );
         s[0] = String( m_data.f64PointValue->x );
         s[1] = String( m_data.f64PointValue->y );
         return s;
      }

   case VariantType::I32Rect:
      {
         StringList s( 4 );
         s[0] = String( m_data.i32RectValue->x0 );
         s[1] = String( m_data.i32RectValue->y0 );
         s[2] = String( m_data.i32RectValue->x1 );
         s[3] = String( m_data.i32RectValue->y1 );
         return s;
      }
   case VariantType::F32Rect:
      {
         StringList s( 4 );
         s[0] = String( m_data.f32RectValue->x0 );
         s[1] = String( m_data.f32RectValue->y0 );
         s[2] = String( m_data.f32RectValue->x1 );
         s[3] = String( m_data.f32RectValue->y1 );
         return s;
      }
   case VariantType::F64Rect:
      {
         StringList s( 4 );
         s[0] = String( m_data.f64RectValue->x0 );
         s[1] = String( m_data.f64RectValue->y0 );
         s[2] = String( m_data.f64RectValue->x1 );
         s[3] = String( m_data.f64RectValue->y1 );
         return s;
      }

   case VariantType::I8Vector:
      {
         StringList s( m_data.i8VectorValue->Length() );
         for ( int i = 0; i < m_data.i8VectorValue->Length(); ++i )
            s[i] = String( int( (*m_data.i8VectorValue)[i] ) );
         return s;
      }
   case VariantType::UI8Vector:
      {
         StringList s( m_data.ui8VectorValue->Length() );
         for ( int i = 0; i < m_data.ui8VectorValue->Length(); ++i )
            s[i] = String( unsigned( (*m_data.ui8VectorValue)[i] ) );
         return s;
      }
   case VariantType::I16Vector:
      {
         StringList s( m_data.i16VectorValue->Length() );
         for ( int i = 0; i < m_data.i16VectorValue->Length(); ++i )
            s[i] = String( int( (*m_data.i16VectorValue)[i] ) );
         return s;
      }
   case VariantType::UI16Vector:
      {
         StringList s( m_data.ui16VectorValue->Length() );
         for ( int i = 0; i < m_data.ui16VectorValue->Length(); ++i )
            s[i] = String( unsigned( (*m_data.ui16VectorValue)[i] ) );
         return s;
      }
   case VariantType::I32Vector:
      {
         StringList s( m_data.i32VectorValue->Length() );
         for ( int i = 0; i < m_data.i32VectorValue->Length(); ++i )
            s[i] = String( (*m_data.i32VectorValue)[i] );
         return s;
      }
   case VariantType::UI32Vector:
      {
         StringList s( m_data.ui32VectorValue->Length() );
         for ( int i = 0; i < m_data.ui32VectorValue->Length(); ++i )
            s[i] = String( (*m_data.ui32VectorValue)[i] );
         return s;
      }
   case VariantType::I64Vector:
      {
         StringList s( m_data.i64VectorValue->Length() );
         for ( int i = 0; i < m_data.i64VectorValue->Length(); ++i )
            s[i] = String( (*m_data.i64VectorValue)[i] );
         return s;
      }
   case VariantType::UI64Vector:
      {
         StringList s( m_data.ui64VectorValue->Length() );
         for ( int i = 0; i < m_data.ui64VectorValue->Length(); ++i )
            s[i] = String( (*m_data.ui64VectorValue)[i] );
         return s;
      }
   case VariantType::F32Vector:
      {
         StringList s( m_data.f32VectorValue->Length() );
         for ( int i = 0; i < m_data.f32VectorValue->Length(); ++i )
            s[i] = String( (*m_data.f32VectorValue)[i] );
         return s;
      }
   case VariantType::F64Vector:
      {
         StringList s( m_data.f64VectorValue->Length() );
         for ( int i = 0; i < m_data.f64VectorValue->Length(); ++i )
            s[i] = String( (*m_data.f64VectorValue)[i] );
         return s;
      }
   case VariantType::C32Vector:
      {
         StringList s( m_data.c32VectorValue->Length() << 1 );
         size_type n = 0;
         for ( int i = 0; i < m_data.c32VectorValue->Length(); ++i )
         {
            s[n++] = String( (*m_data.c32VectorValue)[i].Real() );
            s[n++] = String( (*m_data.c32VectorValue)[i].Imag() );
         }
         return s;
      }
   case VariantType::C64Vector:
      {
         StringList s( m_data.c64VectorValue->Length() << 1 );
         size_type n = 0;
         for ( int i = 0; i < m_data.c64VectorValue->Length(); ++i )
         {
            s[n++] = String( (*m_data.c64VectorValue)[i].Real() );
            s[n++] = String( (*m_data.c64VectorValue)[i].Imag() );
         }
         return s;
      }

   case VariantType::I8Matrix:
      {
         StringList s( m_data.i8MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.i8MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.i8MatrixValue->Cols(); ++j )
               s[n++] = String( int( (*m_data.i8MatrixValue)[i][j] ) );
         return s;
      }
   case VariantType::UI8Matrix:
      {
         StringList s( m_data.ui8MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.ui8MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.ui8MatrixValue->Cols(); ++j )
               s[n++] = String( unsigned( (*m_data.ui8MatrixValue)[i][j] ) );
         return s;
      }
   case VariantType::I16Matrix:
      {
         StringList s( m_data.i16MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.i16MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.i16MatrixValue->Cols(); ++j )
               s[n++] = String( int( (*m_data.i16MatrixValue)[i][j] ) );
         return s;
      }
   case VariantType::UI16Matrix:
      {
         StringList s( m_data.ui16MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.ui16MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.ui16MatrixValue->Cols(); ++j )
               s[n++] = String( unsigned( (*m_data.ui16MatrixValue)[i][j] ) );
         return s;
      }
   case VariantType::I32Matrix:
      {
         StringList s( m_data.i32MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.i32MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.i32MatrixValue->Cols(); ++j )
               s[n++] = String( (*m_data.i32MatrixValue)[i][j] );
         return s;
      }
   case VariantType::UI32Matrix:
      {
         StringList s( m_data.ui32MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.ui32MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.ui32MatrixValue->Cols(); ++j )
               s[n++] = String( (*m_data.ui32MatrixValue)[i][j] );
         return s;
      }
   case VariantType::I64Matrix:
      {
         StringList s( m_data.i64MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.i64MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.i64MatrixValue->Cols(); ++j )
               s[n++] = String( (*m_data.i64MatrixValue)[i][j] );
         return s;
      }
   case VariantType::UI64Matrix:
      {
         StringList s( m_data.ui64MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.ui64MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.ui64MatrixValue->Cols(); ++j )
               s[n++] = String( (*m_data.ui64MatrixValue)[i][j] );
         return s;
      }
   case VariantType::F32Matrix:
      {
         StringList s( m_data.f32MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.f32MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.f32MatrixValue->Cols(); ++j )
               s[n++] = String( (*m_data.f32MatrixValue)[i][j] );
         return s;
      }
   case VariantType::F64Matrix:
      {
         StringList s( m_data.f64MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.f64MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.f64MatrixValue->Cols(); ++j )
               s[n++] = String( (*m_data.f64MatrixValue)[i][j] );
         return s;
      }
   case VariantType::C32Matrix:
      {
         StringList s( m_data.c32MatrixValue->NumberOfElements() << 1 );
         size_type n = 0;
         for ( int i = 0; i < m_data.c32MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.c32MatrixValue->Cols(); ++j )
            {
               s[n++] = String( (*m_data.c32MatrixValue)[i][j].Real() );
               s[n++] = String( (*m_data.c32MatrixValue)[i][j].Imag() );
            }
         return s;
      }
   case VariantType::C64Matrix:
      {
         StringList s( m_data.c64MatrixValue->NumberOfElements() << 1 );
         size_type n = 0;
         for ( int i = 0; i < m_data.c64MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.c64MatrixValue->Cols(); ++j )
            {
               s[n++] = String( (*m_data.c64MatrixValue)[i][j].Real() );
               s[n++] = String( (*m_data.c64MatrixValue)[i][j].Imag() );
            }
         return s;
      }

   case VariantType::ByteArray:
      {
         StringList s( m_data.byteArrayValue->Length() );
         for ( size_type i = 0; i < m_data.byteArrayValue->Length(); ++i )
            s[i] = String( unsigned( (*m_data.byteArrayValue)[i] ) );
         return s;
      }

   case VariantType::String:
      return StringList( 1, *m_data.stringValue );
   case VariantType::IsoString:
      return StringList( 1, String( *m_data.isoStringValue ) );

   case VariantType::StringList:
      return *m_data.stringListValue;
   case VariantType::IsoStringList:
      return StringList( m_data.isoStringListValue->Begin(), m_data.isoStringListValue->End() );

   case VariantType::StringKeyValue:
      {
         StringList s( 2 );
         s[0] = m_data.stringKeyValueValue->key;
         s[1] = m_data.stringKeyValueValue->value;
         return s;
      }
   case VariantType::IsoStringKeyValue:
      {
         StringList s( 2 );
         s[0] = String( m_data.isoStringKeyValueValue->key );
         s[1] = String( m_data.isoStringKeyValueValue->value );
         return s;
      }

   case VariantType::StringKeyValueList:
      {
         StringList s( 2*m_data.stringKeyValueListValue->Length() );
         for ( size_type n = 0, i = 0; i < m_data.stringKeyValueListValue->Length(); ++i )
         {
            s[n++] = (*m_data.stringKeyValueListValue)[i].key;
            s[n++] = (*m_data.stringKeyValueListValue)[i].value;
         }
         return s;
      }
   case VariantType::IsoStringKeyValueList:
      {
         StringList s( 2*m_data.isoStringKeyValueListValue->Length() );
         for ( size_type n = 0, i = 0; i < m_data.isoStringKeyValueListValue->Length(); ++i )
         {
            s[n++] = String( (*m_data.isoStringKeyValueListValue)[i].key );
            s[n++] = String( (*m_data.isoStringKeyValueListValue)[i].value );
         }
         return s;
      }

   default:
      throw Error( "Variant::ToStringList(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

IsoStringList Variant::ToIsoStringList() const
{
   switch ( m_type )
   {
   case VariantType::Bool:
      return IsoStringList( 1, IsoString( m_data.boolValue ) );
   case VariantType::Int8:
      return IsoStringList( 1, IsoString( int( m_data.int8Value ) ) );
   case VariantType::Int16:
      return IsoStringList( 1, IsoString( int( m_data.int16Value ) ) );
   case VariantType::Int32:
      return IsoStringList( 1, IsoString( int( m_data.int32Value ) ) );
   case VariantType::Int64:
      return IsoStringList( 1, IsoString( m_data.int64Value ) );
   case VariantType::UInt8:
      return IsoStringList( 1, IsoString( unsigned( m_data.uint8Value ) ) );
   case VariantType::UInt16:
      return IsoStringList( 1, IsoString( unsigned( m_data.uint16Value ) ) );
   case VariantType::UInt32:
      return IsoStringList( 1, IsoString( unsigned( m_data.uint32Value ) ) );
   case VariantType::UInt64:
      return IsoStringList( 1, IsoString( m_data.uint64Value ) );
   case VariantType::Float32:
      return IsoStringList( 1, IsoString( m_data.float32Value ) );
   case VariantType::Float64:
      return IsoStringList( 1, IsoString( m_data.float64Value ) );

   case VariantType::Complex32:
      {
         IsoStringList s( 2 );
         s[0] = IsoString( m_data.complex32Value->Real() );
         s[1] = IsoString( m_data.complex32Value->Imag() );
         return s;
      }
   case VariantType::Complex64:
      {
         IsoStringList s( 2 );
         s[0] = IsoString( m_data.complex64Value->Real() );
         s[1] = IsoString( m_data.complex64Value->Imag() );
         return s;
      }

   case VariantType::TimePoint:
      return IsoStringList( 1, m_data.timePointValue->ToIsoString() );

   case VariantType::I32Point:
      {
         IsoStringList s( 2 );
         s[0] = IsoString( m_data.i32PointValue->x );
         s[1] = IsoString( m_data.i32PointValue->y );
         return s;
      }
   case VariantType::F32Point:
      {
         IsoStringList s( 2 );
         s[0] = IsoString( m_data.f32PointValue->x );
         s[1] = IsoString( m_data.f32PointValue->y );
         return s;
      }
   case VariantType::F64Point:
      {
         IsoStringList s( 2 );
         s[0] = IsoString( m_data.f64PointValue->x );
         s[1] = IsoString( m_data.f64PointValue->y );
         return s;
      }

   case VariantType::I32Rect:
      {
         IsoStringList s( 4 );
         s[0] = IsoString( m_data.i32RectValue->x0 );
         s[1] = IsoString( m_data.i32RectValue->y0 );
         s[2] = IsoString( m_data.i32RectValue->x1 );
         s[3] = IsoString( m_data.i32RectValue->y1 );
         return s;
      }
   case VariantType::F32Rect:
      {
         IsoStringList s( 4 );
         s[0] = IsoString( m_data.f32RectValue->x0 );
         s[1] = IsoString( m_data.f32RectValue->y0 );
         s[2] = IsoString( m_data.f32RectValue->x1 );
         s[3] = IsoString( m_data.f32RectValue->y1 );
         return s;
      }
   case VariantType::F64Rect:
      {
         IsoStringList s( 4 );
         s[0] = IsoString( m_data.f64RectValue->x0 );
         s[1] = IsoString( m_data.f64RectValue->y0 );
         s[2] = IsoString( m_data.f64RectValue->x1 );
         s[3] = IsoString( m_data.f64RectValue->y1 );
         return s;
      }

   case VariantType::I8Vector:
      {
         IsoStringList s( m_data.i8VectorValue->Length() );
         for ( int i = 0; i < m_data.i8VectorValue->Length(); ++i )
            s[i] = String( int( (*m_data.i8VectorValue)[i] ) );
         return s;
      }
   case VariantType::UI8Vector:
      {
         IsoStringList s( m_data.ui8VectorValue->Length() );
         for ( int i = 0; i < m_data.ui8VectorValue->Length(); ++i )
            s[i] = String( unsigned( (*m_data.ui8VectorValue)[i] ) );
         return s;
      }
   case VariantType::I16Vector:
      {
         IsoStringList s( m_data.i16VectorValue->Length() );
         for ( int i = 0; i < m_data.i16VectorValue->Length(); ++i )
            s[i] = IsoString( int( (*m_data.i16VectorValue)[i] ) );
         return s;
      }
   case VariantType::UI16Vector:
      {
         IsoStringList s( m_data.ui16VectorValue->Length() );
         for ( int i = 0; i < m_data.ui16VectorValue->Length(); ++i )
            s[i] = IsoString( unsigned( (*m_data.ui16VectorValue)[i] ) );
         return s;
      }
   case VariantType::I32Vector:
      {
         IsoStringList s( m_data.i32VectorValue->Length() );
         for ( int i = 0; i < m_data.i32VectorValue->Length(); ++i )
            s[i] = IsoString( (*m_data.i32VectorValue)[i] );
         return s;
      }
   case VariantType::UI32Vector:
      {
         IsoStringList s( m_data.ui32VectorValue->Length() );
         for ( int i = 0; i < m_data.ui32VectorValue->Length(); ++i )
            s[i] = IsoString( (*m_data.ui32VectorValue)[i] );
         return s;
      }
   case VariantType::I64Vector:
      {
         IsoStringList s( m_data.i64VectorValue->Length() );
         for ( int i = 0; i < m_data.i64VectorValue->Length(); ++i )
            s[i] = IsoString( (*m_data.i64VectorValue)[i] );
         return s;
      }
   case VariantType::UI64Vector:
      {
         IsoStringList s( m_data.ui64VectorValue->Length() );
         for ( int i = 0; i < m_data.ui64VectorValue->Length(); ++i )
            s[i] = IsoString( (*m_data.ui64VectorValue)[i] );
         return s;
      }
   case VariantType::F32Vector:
      {
         IsoStringList s( m_data.f32VectorValue->Length() );
         for ( int i = 0; i < m_data.f32VectorValue->Length(); ++i )
            s[i] = IsoString( (*m_data.f32VectorValue)[i] );
         return s;
      }
   case VariantType::F64Vector:
      {
         IsoStringList s( m_data.f64VectorValue->Length() );
         for ( int i = 0; i < m_data.f64VectorValue->Length(); ++i )
            s[i] = IsoString( (*m_data.f64VectorValue)[i] );
         return s;
      }
   case VariantType::C32Vector:
      {
         IsoStringList s( m_data.c32VectorValue->Length() << 1 );
         size_type n = 0;
         for ( int i = 0; i < m_data.c32VectorValue->Length(); ++i )
         {
            s[n++] = IsoString( (*m_data.c32VectorValue)[i].Real() );
            s[n++] = IsoString( (*m_data.c32VectorValue)[i].Imag() );
         }
         return s;
      }
   case VariantType::C64Vector:
      {
         IsoStringList s( m_data.c64VectorValue->Length() << 1 );
         size_type n = 0;
         for ( int i = 0; i < m_data.c64VectorValue->Length(); ++i )
         {
            s[n++] = IsoString( (*m_data.c64VectorValue)[i].Real() );
            s[n++] = IsoString( (*m_data.c64VectorValue)[i].Imag() );
         }
         return s;
      }

   case VariantType::I8Matrix:
      {
         IsoStringList s( m_data.i8MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.i8MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.i8MatrixValue->Cols(); ++j )
               s[n++] = String( int( (*m_data.i8MatrixValue)[i][j] ) );
         return s;
      }
   case VariantType::UI8Matrix:
      {
         IsoStringList s( m_data.ui8MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.ui8MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.ui8MatrixValue->Cols(); ++j )
               s[n++] = String( unsigned( (*m_data.ui8MatrixValue)[i][j] ) );
         return s;
      }
   case VariantType::I16Matrix:
      {
         IsoStringList s( m_data.i16MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.i16MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.i16MatrixValue->Cols(); ++j )
               s[n++] = IsoString( int( (*m_data.i16MatrixValue)[i][j] ) );
         return s;
      }
   case VariantType::UI16Matrix:
      {
         IsoStringList s( m_data.ui16MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.ui16MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.ui16MatrixValue->Cols(); ++j )
               s[n++] = IsoString( unsigned( (*m_data.ui16MatrixValue)[i][j] ) );
         return s;
      }
   case VariantType::I32Matrix:
      {
         IsoStringList s( m_data.i32MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.i32MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.i32MatrixValue->Cols(); ++j )
               s[n++] = IsoString( (*m_data.i32MatrixValue)[i][j] );
         return s;
      }
   case VariantType::UI32Matrix:
      {
         IsoStringList s( m_data.ui32MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.ui32MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.ui32MatrixValue->Cols(); ++j )
               s[n++] = IsoString( (*m_data.ui32MatrixValue)[i][j] );
         return s;
      }
   case VariantType::I64Matrix:
      {
         IsoStringList s( m_data.i64MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.i64MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.i64MatrixValue->Cols(); ++j )
               s[n++] = IsoString( (*m_data.i64MatrixValue)[i][j] );
         return s;
      }
   case VariantType::UI64Matrix:
      {
         IsoStringList s( m_data.ui64MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.ui64MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.ui64MatrixValue->Cols(); ++j )
               s[n++] = IsoString( (*m_data.ui64MatrixValue)[i][j] );
         return s;
      }
   case VariantType::F32Matrix:
      {
         IsoStringList s( m_data.f32MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.f32MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.f32MatrixValue->Cols(); ++j )
               s[n++] = IsoString( (*m_data.f32MatrixValue)[i][j] );
         return s;
      }
   case VariantType::F64Matrix:
      {
         IsoStringList s( m_data.f64MatrixValue->NumberOfElements() );
         size_type n = 0;
         for ( int i = 0; i < m_data.f64MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.f64MatrixValue->Cols(); ++j )
               s[n++] = IsoString( (*m_data.f64MatrixValue)[i][j] );
         return s;
      }
   case VariantType::C32Matrix:
      {
         IsoStringList s( m_data.c32MatrixValue->NumberOfElements() << 1 );
         size_type n = 0;
         for ( int i = 0; i < m_data.c32MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.c32MatrixValue->Cols(); ++j )
            {
               s[n++] = IsoString( (*m_data.c32MatrixValue)[i][j].Real() );
               s[n++] = IsoString( (*m_data.c32MatrixValue)[i][j].Imag() );
            }
         return s;
      }
   case VariantType::C64Matrix:
      {
         IsoStringList s( m_data.c64MatrixValue->NumberOfElements() << 1 );
         size_type n = 0;
         for ( int i = 0; i < m_data.c64MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.c64MatrixValue->Cols(); ++j )
            {
               s[n++] = IsoString( (*m_data.c64MatrixValue)[i][j].Real() );
               s[n++] = IsoString( (*m_data.c64MatrixValue)[i][j].Imag() );
            }
         return s;
      }

   case VariantType::ByteArray:
      {
         IsoStringList s( m_data.byteArrayValue->Length() );
         for ( size_type i = 0; i < m_data.byteArrayValue->Length(); ++i )
            s[i] = IsoString( (*m_data.byteArrayValue)[i] );
         return s;
      }

   case VariantType::String:
      return IsoStringList( 1, IsoString( *m_data.stringValue ) );
   case VariantType::IsoString:
      return IsoStringList( 1, *m_data.isoStringValue );

   case VariantType::StringList:
      return IsoStringList( m_data.stringListValue->Begin(), m_data.stringListValue->End() );
   case VariantType::IsoStringList:
      return *m_data.isoStringListValue;

   case VariantType::StringKeyValue:
      {
         IsoStringList s( 2 );
         s[0] = IsoString( m_data.stringKeyValueValue->key );
         s[1] = IsoString( m_data.stringKeyValueValue->value );
         return s;
      }
   case VariantType::IsoStringKeyValue:
      {
         IsoStringList s( 2 );
         s[0] = m_data.isoStringKeyValueValue->key;
         s[1] = m_data.isoStringKeyValueValue->value;
         return s;
      }

   case VariantType::StringKeyValueList:
      {
         IsoStringList s( 2*m_data.stringKeyValueListValue->Length() );
         for ( size_type n = 0, i = 0; i < m_data.stringKeyValueListValue->Length(); ++i )
         {
            s[n++] = IsoString( (*m_data.stringKeyValueListValue)[i].key );
            s[n++] = IsoString( (*m_data.stringKeyValueListValue)[i].value );
         }
         return s;
      }
   case VariantType::IsoStringKeyValueList:
      {
         IsoStringList s( 2*m_data.isoStringKeyValueListValue->Length() );
         for ( size_type n = 0, i = 0; i < m_data.isoStringKeyValueListValue->Length(); ++i )
         {
            s[n++] = (*m_data.isoStringKeyValueListValue)[i].key;
            s[n++] = (*m_data.isoStringKeyValueListValue)[i].value;
         }
         return s;
      }

   default:
      throw Error( "Variant::ToIsoStringList(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

StringKeyValue Variant::ToStringKeyValue() const
{
   switch ( m_type )
   {
   case VariantType::String:
      return StringKeyValue( *m_data.stringValue,
                             String() );
   case VariantType::IsoString:
      return StringKeyValue( String( *m_data.isoStringValue ),
                             String() );
   case VariantType::StringKeyValue:
      return *m_data.stringKeyValueValue;
   case VariantType::IsoStringKeyValue:
      return StringKeyValue( String( m_data.isoStringKeyValueValue->key ),
                             String( m_data.isoStringKeyValueValue->value ) );
   default:
      throw Error( "Variant::ToStringKeyValue(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

IsoStringKeyValue Variant::ToIsoStringKeyValue() const
{
   switch ( m_type )
   {
   case VariantType::String:
      return IsoStringKeyValue( IsoString( *m_data.stringValue ),
                                IsoString() );
   case VariantType::IsoString:
      return IsoStringKeyValue( *m_data.isoStringValue,
                                IsoString() );
   case VariantType::StringKeyValue:
      return IsoStringKeyValue( IsoString( m_data.stringKeyValueValue->key ),
                                IsoString( m_data.stringKeyValueValue->value ) );
   case VariantType::IsoStringKeyValue:
      return *m_data.isoStringKeyValueValue;
   default:
      throw Error( "Variant::ToIsoStringKeyValue(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

StringKeyValueList Variant::ToStringKeyValueList() const
{
   switch ( m_type )
   {
   case VariantType::StringKeyValue:
      return StringKeyValueList( 1, *m_data.stringKeyValueValue );
   case VariantType::IsoStringKeyValue:
      return StringKeyValueList( 1, StringKeyValue( String( m_data.isoStringKeyValueValue->key ),
                                                    String( m_data.isoStringKeyValueValue->value ) ) );
   case VariantType::StringKeyValueList:
      return *m_data.stringKeyValueListValue;
   case VariantType::IsoStringKeyValueList:
      {
         StringKeyValueList s( m_data.isoStringKeyValueListValue->Length() );
         for ( size_type i = 0; i < m_data.isoStringKeyValueListValue->Length(); ++i )
            s[i] = StringKeyValue( String( (*m_data.isoStringKeyValueListValue)[i].key ),
                                   String( (*m_data.isoStringKeyValueListValue)[i].value ) );
         return s;
      }
   default:
      throw Error( "Variant::ToStringKeyValueList(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

IsoStringKeyValueList Variant::ToIsoStringKeyValueList() const
{
   switch ( m_type )
   {
   case VariantType::StringKeyValue:
      return IsoStringKeyValueList( 1, IsoStringKeyValue( IsoString( m_data.stringKeyValueValue->key ),
                                                          IsoString( m_data.stringKeyValueValue->value ) ) );
   case VariantType::IsoStringKeyValue:
      return IsoStringKeyValueList( 1, *m_data.isoStringKeyValueValue );
   case VariantType::StringKeyValueList:
      {
         IsoStringKeyValueList s( m_data.stringKeyValueListValue->Length() );
         for ( size_type i = 0; i < m_data.stringKeyValueListValue->Length(); ++i )
            s[i] = IsoStringKeyValue( IsoString( (*m_data.stringKeyValueListValue)[i].key ),
                                      IsoString( (*m_data.stringKeyValueListValue)[i].value ) );
         return s;
      }
   case VariantType::IsoStringKeyValueList:
      return *m_data.isoStringKeyValueListValue;
   default:
      throw Error( "Variant::ToIsoStringKeyValueList(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

// ----------------------------------------------------------------------------

const void* Variant::InternalBlockAddress() const
{
   switch ( m_type )
   {
   case VariantType::Bool:       return &m_data.boolValue;
   case VariantType::Int8:       return &m_data.int8Value;
   case VariantType::Int16:      return &m_data.int16Value;
   case VariantType::Int32:      return &m_data.int32Value;
   case VariantType::Int64:      return &m_data.int64Value;
   case VariantType::UInt8:      return &m_data.uint8Value;
   case VariantType::UInt16:     return &m_data.uint16Value;
   case VariantType::UInt32:     return &m_data.uint32Value;
   case VariantType::UInt64:     return &m_data.uint64Value;
   case VariantType::Float32:    return &m_data.float32Value;
   case VariantType::Float64:    return &m_data.float64Value;
   case VariantType::Complex32:  return  m_data.complex32Value;
   case VariantType::Complex64:  return  m_data.complex64Value;
   case VariantType::TimePoint:  return  m_data.timePointValue;
   case VariantType::I32Point:   return  m_data.i32PointValue;
   case VariantType::F32Point:   return  m_data.f32PointValue;
   case VariantType::F64Point:   return  m_data.f64PointValue;
   case VariantType::I32Rect:    return  m_data.i32RectValue;
   case VariantType::F32Rect:    return  m_data.f32RectValue;
   case VariantType::F64Rect:    return  m_data.f64RectValue;
   case VariantType::I8Vector:   return  m_data.i8VectorValue->Begin();
   case VariantType::UI8Vector:  return  m_data.ui8VectorValue->Begin();
   case VariantType::I16Vector:  return  m_data.i16VectorValue->Begin();
   case VariantType::UI16Vector: return  m_data.ui16VectorValue->Begin();
   case VariantType::I32Vector:  return  m_data.i32VectorValue->Begin();
   case VariantType::UI32Vector: return  m_data.ui32VectorValue->Begin();
   case VariantType::I64Vector:  return  m_data.i64VectorValue->Begin();
   case VariantType::UI64Vector: return  m_data.ui64VectorValue->Begin();
   case VariantType::F32Vector:  return  m_data.f32VectorValue->Begin();
   case VariantType::F64Vector:  return  m_data.f64VectorValue->Begin();
   case VariantType::C32Vector:  return  m_data.c32VectorValue->Begin();
   case VariantType::C64Vector:  return  m_data.c64VectorValue->Begin();
   case VariantType::I8Matrix:   return  m_data.i8MatrixValue->Begin();
   case VariantType::UI8Matrix:  return  m_data.ui8MatrixValue->Begin();
   case VariantType::I16Matrix:  return  m_data.i16MatrixValue->Begin();
   case VariantType::UI16Matrix: return  m_data.ui16MatrixValue->Begin();
   case VariantType::I32Matrix:  return  m_data.i32MatrixValue->Begin();
   case VariantType::UI32Matrix: return  m_data.ui32MatrixValue->Begin();
   case VariantType::I64Matrix:  return  m_data.i64MatrixValue->Begin();
   case VariantType::UI64Matrix: return  m_data.ui64MatrixValue->Begin();
   case VariantType::F32Matrix:  return  m_data.f32MatrixValue->Begin();
   case VariantType::F64Matrix:  return  m_data.f64MatrixValue->Begin();
   case VariantType::C32Matrix:  return  m_data.c32MatrixValue->Begin();
   case VariantType::C64Matrix:  return  m_data.c64MatrixValue->Begin();
   case VariantType::ByteArray:  return  m_data.byteArrayValue->Begin();
   case VariantType::String:     return  m_data.stringValue->c_str();
   case VariantType::IsoString:  return  m_data.isoStringValue->c_str();
   default:
      throw Error( String( "Variant::InternalBlockAddress(): Invalid request for data type \'" ) + TypeAsString( m_type ) + '\'' );
   }
}

// ----------------------------------------------------------------------------

size_type Variant::VectorLength() const
{
   switch ( m_type )
   {
   case VariantType::I8Vector:   return m_data.i8VectorValue->Length();
   case VariantType::UI8Vector:  return m_data.ui8VectorValue->Length();
   case VariantType::I16Vector:  return m_data.i16VectorValue->Length();
   case VariantType::UI16Vector: return m_data.ui16VectorValue->Length();
   case VariantType::I32Vector:  return m_data.i32VectorValue->Length();
   case VariantType::UI32Vector: return m_data.ui32VectorValue->Length();
   case VariantType::I64Vector:  return m_data.i64VectorValue->Length();
   case VariantType::UI64Vector: return m_data.ui64VectorValue->Length();
   case VariantType::F32Vector:  return m_data.f32VectorValue->Length();
   case VariantType::F64Vector:  return m_data.f64VectorValue->Length();
   case VariantType::C32Vector:  return m_data.c32VectorValue->Length();
   case VariantType::C64Vector:  return m_data.c64VectorValue->Length();
   case VariantType::ByteArray:  return m_data.byteArrayValue->Length();
   case VariantType::String:     return m_data.stringValue->Length();
   case VariantType::IsoString:  return m_data.isoStringValue->Length();
   default:
      throw Error( String( "Variant::VectorLength(): Invalid request for data type \'" ) + TypeAsString( m_type ) + '\'' );
   }
}

Rect Variant::MatrixDimensions() const
{
   switch ( m_type )
   {
   case VariantType::I8Matrix:   return Rect( m_data.i8MatrixValue->Cols(), m_data.i8MatrixValue->Rows() );
   case VariantType::UI8Matrix:  return Rect( m_data.ui8MatrixValue->Cols(), m_data.ui8MatrixValue->Rows() );
   case VariantType::I16Matrix:  return Rect( m_data.i16MatrixValue->Cols(), m_data.i16MatrixValue->Rows() );
   case VariantType::UI16Matrix: return Rect( m_data.ui16MatrixValue->Cols(), m_data.ui16MatrixValue->Rows() );
   case VariantType::I32Matrix:  return Rect( m_data.i32MatrixValue->Cols(), m_data.i32MatrixValue->Rows() );
   case VariantType::UI32Matrix: return Rect( m_data.ui32MatrixValue->Cols(), m_data.ui32MatrixValue->Rows() );
   case VariantType::I64Matrix:  return Rect( m_data.i64MatrixValue->Cols(), m_data.i64MatrixValue->Rows() );
   case VariantType::UI64Matrix: return Rect( m_data.ui64MatrixValue->Cols(), m_data.ui64MatrixValue->Rows() );
   case VariantType::F32Matrix:  return Rect( m_data.f32MatrixValue->Cols(), m_data.f32MatrixValue->Rows() );
   case VariantType::F64Matrix:  return Rect( m_data.f64MatrixValue->Cols(), m_data.f64MatrixValue->Rows() );
   case VariantType::C32Matrix:  return Rect( m_data.c32MatrixValue->Cols(), m_data.c32MatrixValue->Rows() );
   case VariantType::C64Matrix:  return Rect( m_data.c64MatrixValue->Cols(), m_data.c64MatrixValue->Rows() );
   default:
      throw Error( String( "Variant::MatrixDimensions(): Invalid request for data type \'" ) + TypeAsString( m_type ) + '\'' );
   }
}

size_type Variant::BlockSize() const
{
   switch ( m_type )
   {
   // Scalars
   case VariantType::Bool:       return sizeof( m_data.boolValue );
   case VariantType::Int8:       return sizeof( m_data.int8Value );
   case VariantType::Int16:      return sizeof( m_data.int16Value );
   case VariantType::Int32:      return sizeof( m_data.int32Value );
   case VariantType::Int64:      return sizeof( m_data.int64Value );
   case VariantType::UInt8:      return sizeof( m_data.uint8Value );
   case VariantType::UInt16:     return sizeof( m_data.uint16Value );
   case VariantType::UInt32:     return sizeof( m_data.uint32Value );
   case VariantType::UInt64:     return sizeof( m_data.uint64Value );
   case VariantType::Float32:    return sizeof( m_data.float32Value );
   case VariantType::Float64:    return sizeof( m_data.float64Value );

   // Complex
   case VariantType::Complex32:  return sizeof( *m_data.complex32Value );
   case VariantType::Complex64:  return sizeof( *m_data.complex64Value );

   // Vectors
   case VariantType::I8Vector:   return m_data.i8VectorValue->Size();
   case VariantType::UI8Vector:  return m_data.ui8VectorValue->Size();
   case VariantType::I16Vector:  return m_data.i16VectorValue->Size();
   case VariantType::UI16Vector: return m_data.ui16VectorValue->Size();
   case VariantType::I32Vector:  return m_data.i32VectorValue->Size();
   case VariantType::UI32Vector: return m_data.ui32VectorValue->Size();
   case VariantType::I64Vector:  return m_data.i64VectorValue->Size();
   case VariantType::UI64Vector: return m_data.ui64VectorValue->Size();
   case VariantType::F32Vector:  return m_data.f32VectorValue->Size();
   case VariantType::F64Vector:  return m_data.f64VectorValue->Size();
   case VariantType::C32Vector:  return m_data.c32VectorValue->Size();
   case VariantType::C64Vector:  return m_data.c64VectorValue->Size();
   case VariantType::ByteArray:  return m_data.byteArrayValue->Size();
   case VariantType::String:     return m_data.stringValue->Size();
   case VariantType::IsoString:  return m_data.isoStringValue->Size();

   // Matrices
   case VariantType::I8Matrix:   return m_data.i8MatrixValue->Size();
   case VariantType::UI8Matrix:  return m_data.ui8MatrixValue->Size();
   case VariantType::I16Matrix:  return m_data.i16MatrixValue->Size();
   case VariantType::UI16Matrix: return m_data.ui16MatrixValue->Size();
   case VariantType::I32Matrix:  return m_data.i32MatrixValue->Size();
   case VariantType::UI32Matrix: return m_data.ui32MatrixValue->Size();
   case VariantType::I64Matrix:  return m_data.i64MatrixValue->Size();
   case VariantType::UI64Matrix: return m_data.ui64MatrixValue->Size();
   case VariantType::F32Matrix:  return m_data.f32MatrixValue->Size();
   case VariantType::F64Matrix:  return m_data.f64MatrixValue->Size();
   case VariantType::C32Matrix:  return m_data.c32MatrixValue->Size();
   case VariantType::C64Matrix:  return m_data.c64MatrixValue->Size();

   // Structured or invalid
   default:
      throw Error( String( "Variant::BlockSize(): Invalid request for data type \'" ) + TypeAsString( m_type ) + '\'' );
   }
}

int Variant::BytesPerBlockElementForType( int type )
{
   switch ( type )
   {
   // Scalars
   case VariantType::Bool:       return sizeof( bool );
   case VariantType::Int8:       return sizeof( int8 );
   case VariantType::Int16:      return sizeof( int16 );
   case VariantType::Int32:      return sizeof( int32 );
   case VariantType::Int64:      return sizeof( int64 );
   case VariantType::UInt8:      return sizeof( uint8 );
   case VariantType::UInt16:     return sizeof( uint16 );
   case VariantType::UInt32:     return sizeof( uint32 );
   case VariantType::UInt64:     return sizeof( uint64 );
   case VariantType::Float32:    return sizeof( float );
   case VariantType::Float64:    return sizeof( double );

   // Complex
   case VariantType::Complex32:  return sizeof( Complex32::component );
   case VariantType::Complex64:  return sizeof( Complex64::component );

   // Vectors
   case VariantType::I8Vector:   return sizeof( I8Vector::component );
   case VariantType::UI8Vector:  return sizeof( UI8Vector::component );
   case VariantType::I16Vector:  return sizeof( I16Vector::component );
   case VariantType::UI16Vector: return sizeof( UI16Vector::component );
   case VariantType::I32Vector:  return sizeof( I32Vector::component );
   case VariantType::UI32Vector: return sizeof( UI32Vector::component );
   case VariantType::I64Vector:  return sizeof( I64Vector::component );
   case VariantType::UI64Vector: return sizeof( UI64Vector::component );
   case VariantType::F32Vector:  return sizeof( F32Vector::component );
   case VariantType::F64Vector:  return sizeof( F64Vector::component );
   case VariantType::C32Vector:  return sizeof( C32Vector::component::component );
   case VariantType::C64Vector:  return sizeof( C64Vector::component::component );
   case VariantType::ByteArray:  return sizeof( ByteArray::item_type );
   case VariantType::String:     return int( String::BytesPerChar() );
   case VariantType::IsoString:  return int( IsoString::BytesPerChar() );

   // Matrices
   case VariantType::I8Matrix:   return sizeof( I8Matrix::element );
   case VariantType::UI8Matrix:  return sizeof( UI8Matrix::element );
   case VariantType::I16Matrix:  return sizeof( I16Matrix::element );
   case VariantType::UI16Matrix: return sizeof( UI16Matrix::element );
   case VariantType::I32Matrix:  return sizeof( I32Matrix::element );
   case VariantType::UI32Matrix: return sizeof( UI32Matrix::element );
   case VariantType::I64Matrix:  return sizeof( I64Matrix::element );
   case VariantType::UI64Matrix: return sizeof( UI64Matrix::element );
   case VariantType::F32Matrix:  return sizeof( F32Matrix::element );
   case VariantType::F64Matrix:  return sizeof( F64Matrix::element );
   case VariantType::C32Matrix:  return sizeof( C32Matrix::element::component );
   case VariantType::C64Matrix:  return sizeof( C64Matrix::element::component );

   // Structured or invalid
   default:
      throw Error( String( "Variant::BytesPerBlockElementForType(): Invalid request for data type \'" ) + TypeAsString( type ) + '\'' );
   }
}

// ----------------------------------------------------------------------------

bool Variant::IsScalarType( int type )
{
   switch ( type )
   {
   case VariantType::Bool:
   case VariantType::Int8:
   case VariantType::Int16:
   case VariantType::Int32:
   case VariantType::Int64:
   case VariantType::UInt8:
   case VariantType::UInt16:
   case VariantType::UInt32:
   case VariantType::UInt64:
   case VariantType::Float32:
   case VariantType::Float64:
      return true;

   default:
      return false;
   }
}

bool Variant::IsVectorType( int type )
{
   switch ( type )
   {
   case VariantType::I8Vector:
   case VariantType::UI8Vector:
   case VariantType::I16Vector:
   case VariantType::UI16Vector:
   case VariantType::I32Vector:
   case VariantType::UI32Vector:
   case VariantType::I64Vector:
   case VariantType::UI64Vector:
   case VariantType::F32Vector:
   case VariantType::F64Vector:
   case VariantType::C32Vector:
   case VariantType::C64Vector:
   case VariantType::ByteArray:
   case VariantType::String:
   case VariantType::IsoString:
      return true;

   default:
      return false;
   }
}

bool Variant::IsMatrixType( int type )
{
   switch ( type )
   {
   case VariantType::I8Matrix:
   case VariantType::UI8Matrix:
   case VariantType::I16Matrix:
   case VariantType::UI16Matrix:
   case VariantType::I32Matrix:
   case VariantType::UI32Matrix:
   case VariantType::I64Matrix:
   case VariantType::UI64Matrix:
   case VariantType::F32Matrix:
   case VariantType::F64Matrix:
   case VariantType::C32Matrix:
   case VariantType::C64Matrix:
      return true;

   default:
      return false;
   }
}

bool Variant::IsStructuredType( int type )
{
   switch ( type )
   {
   case VariantType::TimePoint:
   case VariantType::I32Point:
   case VariantType::F32Point:
   case VariantType::F64Point:
   case VariantType::I32Rect:
   case VariantType::F32Rect:
   case VariantType::F64Rect:
   case VariantType::StringList:
   case VariantType::IsoStringList:
   case VariantType::StringKeyValue:
   case VariantType::IsoStringKeyValue:
   case VariantType::StringKeyValueList:
   case VariantType::IsoStringKeyValueList:
      return true;

   default:
      return false;
   }
}

// ----------------------------------------------------------------------------

Variant Variant::ToType( data_type type ) const
{
   switch ( type )
   {
   case VariantType::Bool:       return ToBool();
   case VariantType::Int8:       return int8( ToInt() );
   case VariantType::Int16:      return int16( ToInt() );
   case VariantType::Int32:      return int32( ToInt() );
   case VariantType::Int64:      return ToInt64();
   case VariantType::UInt8:      return uint8( ToUInt() );
   case VariantType::UInt16:     return uint16( ToUInt() );
   case VariantType::UInt32:     return uint32( ToUInt() );
   case VariantType::UInt64:     return ToUInt64();
   case VariantType::Float32:    return ToFloat();
   case VariantType::Float64:    return ToDouble();
   case VariantType::Complex32:  return ToFComplex();
   case VariantType::Complex64:  return ToDComplex();
   case VariantType::TimePoint:  return ToTimePoint();
   case VariantType::I32Point:   return ToPoint();
   case VariantType::F32Point:   return ToFPoint();
   case VariantType::F64Point:   return ToDPoint();
   case VariantType::I32Rect:    return ToRect();
   case VariantType::F32Rect:    return ToFRect();
   case VariantType::F64Rect:    return ToDRect();
   case VariantType::I8Vector:   return ToCharVector();
   case VariantType::UI8Vector:  return ToByteVector();
   case VariantType::I16Vector:  return ToI16Vector();
   case VariantType::UI16Vector: return ToUI16Vector();
   case VariantType::I32Vector:  return ToIVector();
   case VariantType::UI32Vector: return ToUIVector();
   case VariantType::I64Vector:  return ToI64Vector();
   case VariantType::UI64Vector: return ToUI64Vector();
   case VariantType::F32Vector:  return ToFVector();
   case VariantType::F64Vector:  return ToDVector();
   case VariantType::C32Vector:  return ToC32Vector();
   case VariantType::C64Vector:  return ToC64Vector();
   case VariantType::I8Matrix:   return ToCharMatrix();
   case VariantType::UI8Matrix:  return ToByteMatrix();
   case VariantType::I16Matrix:  return ToI16Matrix();
   case VariantType::UI16Matrix: return ToUI16Matrix();
   case VariantType::I32Matrix:  return ToIMatrix();
   case VariantType::UI32Matrix: return ToUIMatrix();
   case VariantType::I64Matrix:  return ToI64Matrix();
   case VariantType::UI64Matrix: return ToUI64Matrix();
   case VariantType::F32Matrix:  return ToFMatrix();
   case VariantType::F64Matrix:  return ToDMatrix();
   case VariantType::C32Matrix:  return ToC32Matrix();
   case VariantType::C64Matrix:  return ToC64Matrix();
   case VariantType::ByteArray:  return ToByteArray();
   case VariantType::String:     return ToString();
   case VariantType::IsoString:  return ToIsoString();
   case VariantType::StringList:    return ToStringList();
   case VariantType::IsoStringList: return ToIsoStringList();
   case VariantType::StringKeyValue:    return ToStringKeyValue();
   case VariantType::IsoStringKeyValue: return ToIsoStringKeyValue();
   case VariantType::StringKeyValueList:    return ToStringKeyValueList();
   case VariantType::IsoStringKeyValueList: return ToIsoStringKeyValueList();
   default:
      // ### TODO: Implement user-defined data types.
      throw Error( String( "Variant::ToType(): Invalid data type \'" ) + String( m_type ) + '\'' );
   }
}

// ----------------------------------------------------------------------------

void Variant::Clear()
{
   switch ( m_type )
   {
   case VariantType::Invalid:
   case VariantType::Bool:
   case VariantType::Int8:
   case VariantType::Int16:
   case VariantType::Int32:
   case VariantType::Int64:
   case VariantType::UInt8:
   case VariantType::UInt16:
   case VariantType::UInt32:
   case VariantType::UInt64:
   case VariantType::Float32:
   case VariantType::Float64:
      break;

   case VariantType::Complex32:
      if ( m_data.complex32Value != nullptr )
         delete m_data.complex32Value, m_data.complex32Value = nullptr;
      break;
   case VariantType::Complex64:
      if ( m_data.complex64Value != nullptr )
         delete m_data.complex64Value, m_data.complex64Value = nullptr;
      break;

   case VariantType::TimePoint:
      if ( m_data.timePointValue != nullptr )
         delete m_data.timePointValue, m_data.timePointValue = nullptr;
      break;

   case VariantType::I32Point:
      if ( m_data.i32PointValue != nullptr )
         delete m_data.i32PointValue, m_data.i32PointValue = nullptr;
      break;
   case VariantType::F32Point:
      if ( m_data.f32PointValue != nullptr )
         delete m_data.f32PointValue, m_data.f32PointValue = nullptr;
      break;
   case VariantType::F64Point:
      if ( m_data.f64PointValue != nullptr )
         delete m_data.f64PointValue, m_data.f64PointValue = nullptr;
      break;

   case VariantType::I32Rect:
      if ( m_data.i32RectValue != nullptr )
         delete m_data.i32RectValue, m_data.i32RectValue = nullptr;
      break;
   case VariantType::F32Rect:
      if ( m_data.f32RectValue != nullptr )
         delete m_data.f32RectValue, m_data.f32RectValue = nullptr;
      break;
   case VariantType::F64Rect:
      if ( m_data.f64RectValue != nullptr )
         delete m_data.f64RectValue, m_data.f64RectValue = nullptr;
      break;

   case VariantType::I8Vector:
      if ( m_data.i8VectorValue != nullptr )
         delete m_data.i8VectorValue, m_data.i8VectorValue = nullptr;
      break;
   case VariantType::UI8Vector:
      if ( m_data.ui8VectorValue != nullptr )
         delete m_data.ui8VectorValue, m_data.ui8VectorValue = nullptr;
      break;
   case VariantType::I16Vector:
      if ( m_data.i16VectorValue != nullptr )
         delete m_data.i16VectorValue, m_data.i16VectorValue = nullptr;
      break;
   case VariantType::UI16Vector:
      if ( m_data.ui16VectorValue != nullptr )
         delete m_data.ui16VectorValue, m_data.ui16VectorValue = nullptr;
      break;
   case VariantType::I32Vector:
      if ( m_data.i32VectorValue != nullptr )
         delete m_data.i32VectorValue, m_data.i32VectorValue = nullptr;
      break;
   case VariantType::UI32Vector:
      if ( m_data.ui32VectorValue != nullptr )
         delete m_data.ui32VectorValue, m_data.ui32VectorValue = nullptr;
      break;
   case VariantType::I64Vector:
      if ( m_data.i64VectorValue != nullptr )
         delete m_data.i64VectorValue, m_data.i64VectorValue = nullptr;
      break;
   case VariantType::UI64Vector:
      if ( m_data.ui64VectorValue != nullptr )
         delete m_data.ui64VectorValue, m_data.ui64VectorValue = nullptr;
      break;
   case VariantType::F32Vector:
      if ( m_data.f32VectorValue != nullptr )
         delete m_data.f32VectorValue, m_data.f32VectorValue = nullptr;
      break;
   case VariantType::F64Vector:
      if ( m_data.f64VectorValue != nullptr )
         delete m_data.f64VectorValue, m_data.f64VectorValue = nullptr;
      break;
   case VariantType::C32Vector:
      if ( m_data.c32VectorValue != nullptr )
         delete m_data.c32VectorValue, m_data.c32VectorValue = nullptr;
      break;
   case VariantType::C64Vector:
      if ( m_data.c64VectorValue != nullptr )
         delete m_data.c64VectorValue, m_data.c64VectorValue = nullptr;
      break;

   case VariantType::I8Matrix:
      if ( m_data.i8MatrixValue != nullptr )
         delete m_data.i8MatrixValue, m_data.i8MatrixValue = nullptr;
      break;
   case VariantType::UI8Matrix:
      if ( m_data.ui8MatrixValue != nullptr )
         delete m_data.ui8MatrixValue, m_data.ui8MatrixValue = nullptr;
      break;
   case VariantType::I16Matrix:
      if ( m_data.i16MatrixValue != nullptr )
         delete m_data.i16MatrixValue, m_data.i16MatrixValue = nullptr;
      break;
   case VariantType::UI16Matrix:
      if ( m_data.ui16MatrixValue != nullptr )
         delete m_data.ui16MatrixValue, m_data.ui16MatrixValue = nullptr;
      break;
   case VariantType::I32Matrix:
      if ( m_data.i32MatrixValue != nullptr )
         delete m_data.i32MatrixValue, m_data.i32MatrixValue = nullptr;
      break;
   case VariantType::UI32Matrix:
      if ( m_data.ui32MatrixValue != nullptr )
         delete m_data.ui32MatrixValue, m_data.ui32MatrixValue = nullptr;
      break;
   case VariantType::I64Matrix:
      if ( m_data.i64MatrixValue != nullptr )
         delete m_data.i64MatrixValue, m_data.i64MatrixValue = nullptr;
      break;
   case VariantType::UI64Matrix:
      if ( m_data.ui64MatrixValue != nullptr )
         delete m_data.ui64MatrixValue, m_data.ui64MatrixValue = nullptr;
      break;
   case VariantType::F32Matrix:
      if ( m_data.f32MatrixValue != nullptr )
         delete m_data.f32MatrixValue, m_data.f32MatrixValue = nullptr;
      break;
   case VariantType::F64Matrix:
      if ( m_data.f64MatrixValue != nullptr )
         delete m_data.f64MatrixValue, m_data.f64MatrixValue = nullptr;
      break;
   case VariantType::C32Matrix:
      if ( m_data.c32MatrixValue != nullptr )
         delete m_data.c32MatrixValue, m_data.c32MatrixValue = nullptr;
      break;
   case VariantType::C64Matrix:
      if ( m_data.c64MatrixValue != nullptr )
         delete m_data.c64MatrixValue, m_data.c64MatrixValue = nullptr;
      break;

   case VariantType::ByteArray:
      if ( m_data.byteArrayValue != nullptr )
         delete m_data.byteArrayValue, m_data.byteArrayValue = nullptr;
      break;

   case VariantType::String:
      if ( m_data.stringValue != nullptr )
         delete m_data.stringValue, m_data.stringValue = nullptr;
      break;
   case VariantType::IsoString:
      if ( m_data.isoStringValue != nullptr )
         delete m_data.isoStringValue, m_data.isoStringValue = nullptr;
      break;

   case VariantType::StringList:
      if ( m_data.stringListValue != nullptr )
         delete m_data.stringListValue, m_data.stringListValue = nullptr;
      break;
   case VariantType::IsoStringList:
      if ( m_data.isoStringListValue != nullptr )
         delete m_data.isoStringListValue, m_data.isoStringListValue = nullptr;
      break;

   case VariantType::StringKeyValue:
      if ( m_data.stringKeyValueValue != nullptr )
         delete m_data.stringKeyValueValue, m_data.stringKeyValueValue = nullptr;
      break;
   case VariantType::IsoStringKeyValue:
      if ( m_data.isoStringKeyValueValue != nullptr )
         delete m_data.isoStringKeyValueValue, m_data.isoStringKeyValueValue = nullptr;
      break;

   case VariantType::StringKeyValueList:
      if ( m_data.stringKeyValueListValue != nullptr )
         delete m_data.stringKeyValueListValue, m_data.stringKeyValueListValue = nullptr;
      break;
   case VariantType::IsoStringKeyValueList:
      if ( m_data.isoStringKeyValueListValue != nullptr )
         delete m_data.isoStringKeyValueListValue, m_data.isoStringKeyValueListValue = nullptr;
      break;

   default:
      // ### TODO: Implement user-defined data types.
      throw Error( "Variant::Clear(): Invalid data type \'" + String( m_type ) + '\'' );
   }
}

// ----------------------------------------------------------------------------

#define COMPARE( item ) \
   (m_data.item != x.m_data.item) ? ((m_data.item < x.m_data.item) ? -1 : +1) : 0
#define PTR_COMPARE( item ) \
   (*m_data.item != *x.m_data.item) ? ((*m_data.item < *x.m_data.item) ? -1 : +1) : 0

int Variant::Compare( const Variant& x ) const
{
   if ( m_type != x.m_type )
      return Compare( x.ToType( data_type( m_type ) ) );

   switch ( m_type )
   {
   case VariantType::Invalid:
      return 0;
   case VariantType::Bool:
      return COMPARE( boolValue );
   case VariantType::Int8:
      return COMPARE( int8Value );
   case VariantType::Int16:
      return COMPARE( int16Value );
   case VariantType::Int32:
      return COMPARE( int32Value );
   case VariantType::Int64:
      return COMPARE( int64Value );
   case VariantType::UInt8:
      return COMPARE( uint8Value );
   case VariantType::UInt16:
      return COMPARE( uint16Value );
   case VariantType::UInt32:
      return COMPARE( uint32Value );
   case VariantType::UInt64:
      return COMPARE( uint64Value );
   case VariantType::Float32:
      return COMPARE( float32Value );
   case VariantType::Float64:
      return COMPARE( float64Value );

   case VariantType::Complex32:
      return PTR_COMPARE( complex32Value );
   case VariantType::Complex64:
      return PTR_COMPARE( complex64Value );

   case VariantType::TimePoint:
      return PTR_COMPARE( timePointValue );

   case VariantType::I32Point:
      return PTR_COMPARE( i32PointValue );
   case VariantType::F32Point:
      return PTR_COMPARE( f32PointValue );
   case VariantType::F64Point:
      return PTR_COMPARE( f64PointValue );

   case VariantType::I32Rect:
      return PTR_COMPARE( i32RectValue );
   case VariantType::F32Rect:
      return PTR_COMPARE( f32RectValue );
   case VariantType::F64Rect:
      return PTR_COMPARE( f64RectValue );

   case VariantType::I8Vector:
      return PTR_COMPARE( i8VectorValue );
   case VariantType::UI8Vector:
      return PTR_COMPARE( ui8VectorValue );
   case VariantType::I16Vector:
      return PTR_COMPARE( i16VectorValue );
   case VariantType::UI16Vector:
      return PTR_COMPARE( ui16VectorValue );
   case VariantType::I32Vector:
      return PTR_COMPARE( i32VectorValue );
   case VariantType::UI32Vector:
      return PTR_COMPARE( ui32VectorValue );
   case VariantType::I64Vector:
      return PTR_COMPARE( i64VectorValue );
   case VariantType::UI64Vector:
      return PTR_COMPARE( ui64VectorValue );
   case VariantType::F32Vector:
      return PTR_COMPARE( f32VectorValue );
   case VariantType::F64Vector:
      return PTR_COMPARE( f64VectorValue );
   case VariantType::C32Vector:
      return PTR_COMPARE( c32VectorValue );
   case VariantType::C64Vector:
      return PTR_COMPARE( c64VectorValue );

   case VariantType::I8Matrix:
      return PTR_COMPARE( i8MatrixValue );
   case VariantType::UI8Matrix:
      return PTR_COMPARE( ui8MatrixValue );
   case VariantType::I16Matrix:
      return PTR_COMPARE( i16MatrixValue );
   case VariantType::UI16Matrix:
      return PTR_COMPARE( ui16MatrixValue );
   case VariantType::I32Matrix:
      return PTR_COMPARE( i32MatrixValue );
   case VariantType::UI32Matrix:
      return PTR_COMPARE( ui32MatrixValue );
   case VariantType::I64Matrix:
      return PTR_COMPARE( i64MatrixValue );
   case VariantType::UI64Matrix:
      return PTR_COMPARE( ui64MatrixValue );
   case VariantType::F32Matrix:
      return PTR_COMPARE( f32MatrixValue );
   case VariantType::F64Matrix:
      return PTR_COMPARE( f64MatrixValue );
   case VariantType::C32Matrix:
      return PTR_COMPARE( c32MatrixValue );
   case VariantType::C64Matrix:
      return PTR_COMPARE( c64MatrixValue );

   case VariantType::ByteArray:
      return PTR_COMPARE( byteArrayValue );

   case VariantType::String:
      return PTR_COMPARE( stringValue );
   case VariantType::IsoString:
      return PTR_COMPARE( isoStringValue );

   case VariantType::StringList:
      return PTR_COMPARE( stringListValue );
   case VariantType::IsoStringList:
      return PTR_COMPARE( isoStringListValue );

   case VariantType::StringKeyValue:
      return PTR_COMPARE( stringKeyValueValue );
   case VariantType::IsoStringKeyValue:
      return PTR_COMPARE( isoStringKeyValueValue );

   case VariantType::StringKeyValueList:
      return PTR_COMPARE( stringKeyValueListValue );
   case VariantType::IsoStringKeyValueList:
      return PTR_COMPARE( isoStringKeyValueListValue );

   default:
      // ### TODO: Implement user-defined data types.
      throw Error( "Variant::Compare(): Invalid data type \'" + String( m_type ) + '\'' );
   }
}

// ----------------------------------------------------------------------------

#define COPY( item ) m_data.item = x.m_data.item
#define PTR_COPY( item, type ) m_data.item = new type( *x.m_data.item )

// ### Internal
void Variant::Copy( const Variant& x )
{
   m_type = x.m_type;

   switch ( m_type )
   {
   case VariantType::Invalid:
      break;
   case VariantType::Bool:
      COPY( boolValue );
      break;
   case VariantType::Int8:
      COPY( int8Value );
      break;
   case VariantType::Int16:
      COPY( int16Value );
      break;
   case VariantType::Int32:
      COPY( int32Value );
      break;
   case VariantType::Int64:
      COPY( int64Value );
      break;
   case VariantType::UInt8:
      COPY( uint8Value );
      break;
   case VariantType::UInt16:
      COPY( uint16Value );
      break;
   case VariantType::UInt32:
      COPY( uint32Value );
      break;
   case VariantType::UInt64:
      COPY( uint64Value );
      break;
   case VariantType::Float32:
      COPY( float32Value );
      break;
   case VariantType::Float64:
      COPY( float64Value );
      break;

   case VariantType::Complex32:
      PTR_COPY( complex32Value, fcomplex );
      break;
   case VariantType::Complex64:
      PTR_COPY( complex64Value, dcomplex );
      break;

   case VariantType::TimePoint:
      PTR_COPY( timePointValue, TimePoint );
      break;

   case VariantType::I32Point:
      PTR_COPY( i32PointValue, Point );
      break;
   case VariantType::F32Point:
      PTR_COPY( f32PointValue, FPoint );
      break;
   case VariantType::F64Point:
      PTR_COPY( f64PointValue, DPoint );
      break;

   case VariantType::I32Rect:
      PTR_COPY( i32RectValue, Rect );
      break;
   case VariantType::F32Rect:
      PTR_COPY( f32RectValue, FRect );
      break;
   case VariantType::F64Rect:
      PTR_COPY( f64RectValue, DRect );
      break;

   case VariantType::I8Vector:
      PTR_COPY( i8VectorValue, CharVector );
      break;
   case VariantType::UI8Vector:
      PTR_COPY( ui8VectorValue, ByteVector );
      break;
   case VariantType::I16Vector:
      PTR_COPY( i16VectorValue, I16Vector );
      break;
   case VariantType::UI16Vector:
      PTR_COPY( ui16VectorValue, UI16Vector );
      break;
   case VariantType::I32Vector:
      PTR_COPY( i32VectorValue, IVector );
      break;
   case VariantType::UI32Vector:
      PTR_COPY( ui32VectorValue, UIVector );
      break;
   case VariantType::I64Vector:
      PTR_COPY( i64VectorValue, I64Vector );
      break;
   case VariantType::UI64Vector:
      PTR_COPY( ui64VectorValue, UI64Vector );
      break;
   case VariantType::F32Vector:
      PTR_COPY( f32VectorValue, FVector );
      break;
   case VariantType::F64Vector:
      PTR_COPY( f64VectorValue, DVector );
      break;
   case VariantType::C32Vector:
      PTR_COPY( c32VectorValue, C32Vector );
      break;
   case VariantType::C64Vector:
      PTR_COPY( c64VectorValue, C64Vector );
      break;

   case VariantType::I8Matrix:
      PTR_COPY( i8MatrixValue, CharMatrix );
      break;
   case VariantType::UI8Matrix:
      PTR_COPY( ui8MatrixValue, ByteMatrix );
      break;
   case VariantType::I16Matrix:
      PTR_COPY( i16MatrixValue, I16Matrix );
      break;
   case VariantType::UI16Matrix:
      PTR_COPY( ui16MatrixValue, UI16Matrix );
      break;
   case VariantType::I32Matrix:
      PTR_COPY( i32MatrixValue, IMatrix );
      break;
   case VariantType::UI32Matrix:
      PTR_COPY( ui32MatrixValue, UIMatrix );
      break;
   case VariantType::I64Matrix:
      PTR_COPY( i64MatrixValue, I64Matrix );
      break;
   case VariantType::UI64Matrix:
      PTR_COPY( ui64MatrixValue, UI64Matrix );
      break;
   case VariantType::F32Matrix:
      PTR_COPY( f32MatrixValue, FMatrix );
      break;
   case VariantType::F64Matrix:
      PTR_COPY( f64MatrixValue, DMatrix );
      break;
   case VariantType::C32Matrix:
      PTR_COPY( c32MatrixValue, C32Matrix );
      break;
   case VariantType::C64Matrix:
      PTR_COPY( c64MatrixValue, C64Matrix );
      break;

   case VariantType::ByteArray:
      PTR_COPY( byteArrayValue, ByteArray );
      break;

   case VariantType::String:
      PTR_COPY( stringValue, String );
      break;
   case VariantType::IsoString:
      PTR_COPY( isoStringValue, IsoString );
      break;

   case VariantType::StringList:
      PTR_COPY( stringListValue, StringList );
      break;
   case VariantType::IsoStringList:
      PTR_COPY( isoStringListValue, IsoStringList );
      break;

   case VariantType::StringKeyValue:
      PTR_COPY( stringKeyValueValue, StringKeyValue );
      break;
   case VariantType::IsoStringKeyValue:
      PTR_COPY( isoStringKeyValueValue, IsoStringKeyValue );
      break;

   case VariantType::StringKeyValueList:
      PTR_COPY( stringKeyValueListValue, StringKeyValueList );
      break;
   case VariantType::IsoStringKeyValueList:
      PTR_COPY( isoStringKeyValueListValue, IsoStringKeyValueList );
      break;

   default:
      // ### TODO: Implement user-defined data types.
      throw Error( "Variant::Copy(): Invalid data type \'" + String( m_type ) + '\'' );
   }
}

// ----------------------------------------------------------------------------

// ### Internal
Variant VariantFromAPIPropertyValue( const api_property_value& apiValue )
{
   switch ( apiValue.type )
   {
   default:
   case VTYPE_INVALID:
      return Variant();
   case VTYPE_BOOLEAN:
      return Variant( bool( apiValue.data.boolValue != api_false ) );
   case VTYPE_INT8:
      return Variant( apiValue.data.int8Value );
   case VTYPE_INT16:
      return Variant( apiValue.data.int16Value );
   case VTYPE_INT32:
      return Variant( apiValue.data.int32Value );
   case VTYPE_INT64:
      return Variant( apiValue.data.int64Value );
   case VTYPE_UINT8:
      return Variant( apiValue.data.uint8Value );
   case VTYPE_UINT16:
      return Variant( apiValue.data.uint16Value );
   case VTYPE_UINT32:
      return Variant( apiValue.data.uint32Value );
   case VTYPE_UINT64:
      return Variant( apiValue.data.uint64Value );
   case VTYPE_FLOAT:
      return Variant( apiValue.data.floatValue );
   case VTYPE_DOUBLE:
      return Variant( apiValue.data.doubleValue );

   case VTYPE_INT8_VECTOR:
      {
         const int8* a = reinterpret_cast<const int8*>( apiValue.data.blockValue );
         return Variant( I8Vector( a, int( apiValue.dimX ) ) );
      }
   case VTYPE_UINT8_VECTOR:
      {
         const uint8* a = reinterpret_cast<const uint8*>( apiValue.data.blockValue );
         return Variant( UI8Vector( a, int( apiValue.dimX ) ) );
      }
   case VTYPE_INT16_VECTOR:
      {
         const int16* a = reinterpret_cast<const int16*>( apiValue.data.blockValue );
         return Variant( I16Vector( a, int( apiValue.dimX ) ) );
      }
   case VTYPE_UINT16_VECTOR:
      {
         const uint16* a = reinterpret_cast<const uint16*>( apiValue.data.blockValue );
         return Variant( UI16Vector( a, int( apiValue.dimX ) ) );
      }
   case VTYPE_IVECTOR:
      {
         const int32* a = reinterpret_cast<const int32*>( apiValue.data.blockValue );
         return Variant( IVector( a, int( apiValue.dimX ) ) );
      }
   case VTYPE_UIVECTOR:
      {
         const uint32* a = reinterpret_cast<const uint32*>( apiValue.data.blockValue );
         return Variant( UIVector( a, int( apiValue.dimX ) ) );
      }
   case VTYPE_INT64_VECTOR:
      {
         const int64* a = reinterpret_cast<const int64*>( apiValue.data.blockValue );
         return Variant( I64Vector( a, int( apiValue.dimX ) ) );
      }
   case VTYPE_UINT64_VECTOR:
      {
         const uint64* a = reinterpret_cast<const uint64*>( apiValue.data.blockValue );
         return Variant( UI64Vector( a, int( apiValue.dimX ) ) );
      }
   case VTYPE_FVECTOR:
      {
         const float* a = reinterpret_cast<const float*>( apiValue.data.blockValue );
         return Variant( FVector( a, int( apiValue.dimX ) ) );
      }
   case VTYPE_DVECTOR:
      {
         const double* a = reinterpret_cast<const double*>( apiValue.data.blockValue );
         return Variant( DVector( a, int( apiValue.dimX ) ) );
      }
   case VTYPE_C32_VECTOR:
      {
         const fcomplex* a = reinterpret_cast<const fcomplex*>( apiValue.data.blockValue );
         return Variant( C32Vector( a, int( apiValue.dimX ) ) );
      }
   case VTYPE_C64_VECTOR:
      {
         const dcomplex* a = reinterpret_cast<const dcomplex*>( apiValue.data.blockValue );
         return Variant( C64Vector( a, int( apiValue.dimX ) ) );
      }
   case VTYPE_INT8_MATRIX:
      {
         const int8* a = reinterpret_cast<const int8*>( apiValue.data.blockValue );
         return Variant( I8Matrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
      }
   case VTYPE_UINT8_MATRIX:
      {
         const uint8* a = reinterpret_cast<const uint8*>( apiValue.data.blockValue );
         return Variant( UI8Matrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
      }
   case VTYPE_INT16_MATRIX:
      {
         const int16* a = reinterpret_cast<const int16*>( apiValue.data.blockValue );
         return Variant( I16Matrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
      }
   case VTYPE_UINT16_MATRIX:
      {
         const uint16* a = reinterpret_cast<const uint16*>( apiValue.data.blockValue );
         return Variant( UI16Matrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
      }
   case VTYPE_IMATRIX:
      {
         const int32* a = reinterpret_cast<const int32*>( apiValue.data.blockValue );
         return Variant( IMatrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
      }
   case VTYPE_UIMATRIX:
      {
         const uint32* a = reinterpret_cast<const uint32*>( apiValue.data.blockValue );
         return Variant( UIMatrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
      }
   case VTYPE_INT64_MATRIX:
      {
         const int64* a = reinterpret_cast<const int64*>( apiValue.data.blockValue );
         return Variant( I64Matrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
      }
   case VTYPE_UINT64_MATRIX:
      {
         const uint64* a = reinterpret_cast<const uint64*>( apiValue.data.blockValue );
         return Variant( I64Matrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
      }
   case VTYPE_FMATRIX:
      {
         const float* a = reinterpret_cast<const float*>( apiValue.data.blockValue );
         return Variant( FMatrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
      }
   case VTYPE_DMATRIX:
      {
         const double* a = reinterpret_cast<const double*>( apiValue.data.blockValue );
         return Variant( DMatrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
      }
   case VTYPE_C32_MATRIX:
      {
         const fcomplex* a = reinterpret_cast<const fcomplex*>( apiValue.data.blockValue );
         return Variant( C32Matrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
      }
   case VTYPE_C64_MATRIX:
      {
         const dcomplex* a = reinterpret_cast<const dcomplex*>( apiValue.data.blockValue );
         return Variant( C64Matrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
      }
   case VTYPE_ISOSTRING:
      return Variant( IsoString( reinterpret_cast<const char*>( apiValue.data.blockValue ),
                                 reinterpret_cast<const char*>( apiValue.data.blockValue ) + apiValue.dimX ) );
   case VTYPE_STRING:
      return Variant( String( reinterpret_cast<const char16_type*>( apiValue.data.blockValue ),
                              reinterpret_cast<const char16_type*>( apiValue.data.blockValue ) + apiValue.dimX ) );
   case VTYPE_COMPLEX32:
      return Variant( *reinterpret_cast<const fcomplex*>( apiValue.data.blockValue ) );
   case VTYPE_COMPLEX64:
      return Variant( *reinterpret_cast<const dcomplex*>( apiValue.data.blockValue ) );

   case VTYPE_TIMEPOINT:
      return Variant( *reinterpret_cast<const TimePoint*>( apiValue.data.blockValue ) );
   }
}

// ----------------------------------------------------------------------------

// ### Internal
void APIPropertyValueFromVariant( api_property_value& apiValue, const Variant& value )
{
   switch ( value.Type() )
   {
   case VariantType::Invalid:
      apiValue.data.blockValue = nullptr;
      apiValue.type = VTYPE_INVALID;
      break;
   case VariantType::Boolean:
      apiValue.data.boolValue = api_bool( value.ToBoolean() );
      apiValue.type = VTYPE_BOOLEAN;
      break;
   case VariantType::Int8:
      apiValue.data.int8Value = int8( value.ToInt() );
      apiValue.type = VTYPE_INT8;
      break;
   case VariantType::Int16:
      apiValue.data.int16Value = int16( value.ToInt() );
      apiValue.type = VTYPE_INT16;
      break;
   case VariantType::Int32:
      apiValue.data.int32Value = value.ToInt();
      apiValue.type = VTYPE_INT32;
      break;
   case VariantType::Int64:
      apiValue.data.int64Value = value.ToInt64();
      apiValue.type = VTYPE_INT64;
      break;
   case VariantType::UInt8:
      apiValue.data.uint8Value = uint8( value.ToUInt() );
      apiValue.type = VTYPE_UINT8;
      break;
   case VariantType::UInt16:
      apiValue.data.uint16Value = uint16( value.ToUInt() );
      apiValue.type = VTYPE_UINT16;
      break;
   case VariantType::UInt32:
      apiValue.data.uint32Value = value.ToUInt();
      apiValue.type = VTYPE_UINT32;
      break;
   case VariantType::UInt64:
      apiValue.data.uint64Value = value.ToUInt64();
      apiValue.type = VTYPE_UINT64;
      break;
   case VariantType::Float32:
      apiValue.data.floatValue = value.ToFloat();
      apiValue.type = VTYPE_FLOAT;
      break;
   case VariantType::Float64:
      apiValue.data.doubleValue = value.ToDouble();
      apiValue.type = VTYPE_DOUBLE;
      break;
   case VariantType::I8Vector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_INT8_VECTOR;
      break;
   case VariantType::ByteArray:
   case VariantType::UI8Vector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_UINT8_VECTOR;
      break;
   case VariantType::I16Vector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_INT16_VECTOR;
      break;
   case VariantType::UI16Vector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_UINT16_VECTOR;
      break;
   case VariantType::I32Vector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_IVECTOR;
      break;
   case VariantType::UI32Vector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_UIVECTOR;
      break;
   case VariantType::I64Vector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_INT64_VECTOR;
      break;
   case VariantType::UI64Vector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_UINT64_VECTOR;
      break;
   case VariantType::F32Vector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_FVECTOR;
      break;
   case VariantType::F64Vector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_DVECTOR;
      break;
   case VariantType::C32Vector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_C32_VECTOR;
      break;
   case VariantType::C64Vector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_C64_VECTOR;
      break;
   case VariantType::I8Matrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_INT8_MATRIX;
      }
      break;
   case VariantType::UI8Matrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_UINT8_MATRIX;
      }
      break;
   case VariantType::I16Matrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_INT16_MATRIX;
      }
      break;
   case VariantType::UI16Matrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_UINT16_MATRIX;
      }
      break;
   case VariantType::I32Matrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_IMATRIX;
      }
      break;
   case VariantType::UI32Matrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_UIMATRIX;
      }
      break;
   case VariantType::I64Matrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_INT64_MATRIX;
      }
      break;
   case VariantType::UI64Matrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_UINT64_MATRIX;
      }
      break;
   case VariantType::F32Matrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_FMATRIX;
      }
      break;
   case VariantType::F64Matrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_DMATRIX;
      }
      break;
   case VariantType::C32Matrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_C32_MATRIX;
      }
      break;
   case VariantType::C64Matrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_C64_MATRIX;
      }
      break;
   case VariantType::String:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_STRING;
      break;
   case VariantType::IsoString:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_ISOSTRING;
      break;
   case VariantType::Complex32:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.type = VTYPE_COMPLEX32;
      break;
   case VariantType::Complex64:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.type = VTYPE_COMPLEX64;
      break;
   case VariantType::TimePoint:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.type = VTYPE_TIMEPOINT;
      break;
   default: // !?
      throw APIError( String( "APIPropertyValueFromVariant(): Unsupported data type \'" ) + value.TypeAsString( value.Type() ) + "\'" );
   }
}

// ----------------------------------------------------------------------------

// ### Internal
Variant::data_type VariantTypeFromAPIPropertyType( uint64 apiType )
{
   switch ( apiType )
   {
   default: // ?!
   case VTYPE_INVALID:       return VariantType::Invalid;
   case VTYPE_BOOLEAN:       return VariantType::Boolean;
   case VTYPE_INT8:          return VariantType::Int8;
   case VTYPE_INT16:         return VariantType::Int16;
   case VTYPE_INT32:         return VariantType::Int32;
   case VTYPE_INT64:         return VariantType::Int64;
   case VTYPE_UINT8:         return VariantType::UInt8;
   case VTYPE_UINT16:        return VariantType::UInt16;
   case VTYPE_UINT32:        return VariantType::UInt32;
   case VTYPE_UINT64:        return VariantType::UInt64;
   case VTYPE_FLOAT:         return VariantType::Float32;
   case VTYPE_DOUBLE:        return VariantType::Float64;
   case VTYPE_INT8_VECTOR:   return VariantType::I8Vector;
   case VTYPE_UINT8_VECTOR:  return VariantType::UI8Vector;
   case VTYPE_INT16_VECTOR:  return VariantType::I16Vector;
   case VTYPE_UINT16_VECTOR: return VariantType::UI16Vector;
   case VTYPE_IVECTOR:       return VariantType::I32Vector;
   case VTYPE_UIVECTOR:      return VariantType::UI32Vector;
   case VTYPE_INT64_VECTOR:  return VariantType::I64Vector;
   case VTYPE_UINT64_VECTOR: return VariantType::UI64Vector;
   case VTYPE_FVECTOR:       return VariantType::F32Vector;
   case VTYPE_DVECTOR:       return VariantType::F64Vector;
   case VTYPE_C32_VECTOR:    return VariantType::C32Vector;
   case VTYPE_C64_VECTOR:    return VariantType::C64Vector;
   case VTYPE_INT8_MATRIX:   return VariantType::I8Matrix;
   case VTYPE_UINT8_MATRIX:  return VariantType::UI8Matrix;
   case VTYPE_INT16_MATRIX:  return VariantType::I16Matrix;
   case VTYPE_UINT16_MATRIX: return VariantType::UI16Matrix;
   case VTYPE_IMATRIX:       return VariantType::I32Matrix;
   case VTYPE_UIMATRIX:      return VariantType::UI32Matrix;
   case VTYPE_INT64_MATRIX:  return VariantType::I64Matrix;
   case VTYPE_UINT64_MATRIX: return VariantType::UI64Matrix;
   case VTYPE_FMATRIX:       return VariantType::F32Matrix;
   case VTYPE_DMATRIX:       return VariantType::F64Matrix;
   case VTYPE_C32_MATRIX:    return VariantType::C32Matrix;
   case VTYPE_C64_MATRIX:    return VariantType::C64Matrix;
   case VTYPE_ISOSTRING:     return VariantType::IsoString;
   case VTYPE_STRING:        return VariantType::String;
   case VTYPE_COMPLEX32:     return VariantType::Complex32;
   case VTYPE_COMPLEX64:     return VariantType::Complex64;
   case VTYPE_TIMEPOINT:     return VariantType::TimePoint;
   }
}

// ### Internal
uint64 APIPropertyTypeFromVariantType( Variant::data_type type )
{
   switch ( type )
   {
   default: // ?!
   case VariantType::Invalid:    return VTYPE_INVALID;
   case VariantType::Boolean:    return VTYPE_BOOLEAN;
   case VariantType::Int8:       return VTYPE_INT8;
   case VariantType::Int16:      return VTYPE_INT16;
   case VariantType::Int32:      return VTYPE_INT32;
   case VariantType::Int64:      return VTYPE_INT64;
   case VariantType::UInt8:      return VTYPE_UINT8;
   case VariantType::UInt16:     return VTYPE_UINT16;
   case VariantType::UInt32:     return VTYPE_UINT32;
   case VariantType::UInt64:     return VTYPE_UINT64;
   case VariantType::Float32:    return VTYPE_FLOAT;
   case VariantType::Float64:    return VTYPE_DOUBLE;
   case VariantType::I8Vector:   return VTYPE_INT8_VECTOR;
   case VariantType::UI8Vector:  return VTYPE_UINT8_VECTOR;
   case VariantType::I16Vector:  return VTYPE_INT16_VECTOR;
   case VariantType::UI16Vector: return VTYPE_UINT16_VECTOR;
   case VariantType::I32Vector:  return VTYPE_IVECTOR;
   case VariantType::UI32Vector: return VTYPE_UIVECTOR;
   case VariantType::I64Vector:  return VTYPE_INT64_VECTOR;
   case VariantType::UI64Vector: return VTYPE_UINT64_VECTOR;
   case VariantType::F32Vector:  return VTYPE_FVECTOR;
   case VariantType::F64Vector:  return VTYPE_DVECTOR;
   case VariantType::C32Vector:  return VTYPE_C32_VECTOR;
   case VariantType::C64Vector:  return VTYPE_C64_VECTOR;
   case VariantType::I8Matrix:   return VTYPE_INT8_MATRIX;
   case VariantType::UI8Matrix:  return VTYPE_UINT8_MATRIX;
   case VariantType::I16Matrix:  return VTYPE_INT16_MATRIX;
   case VariantType::UI16Matrix: return VTYPE_UINT16_MATRIX;
   case VariantType::I32Matrix:  return VTYPE_IMATRIX;
   case VariantType::UI32Matrix: return VTYPE_UIMATRIX;
   case VariantType::I64Matrix:  return VTYPE_INT64_MATRIX;
   case VariantType::UI64Matrix: return VTYPE_UINT64_MATRIX;
   case VariantType::F32Matrix:  return VTYPE_FMATRIX;
   case VariantType::F64Matrix:  return VTYPE_DMATRIX;
   case VariantType::C32Matrix:  return VTYPE_C32_MATRIX;
   case VariantType::C64Matrix:  return VTYPE_C64_MATRIX;
   case VariantType::IsoString:  return VTYPE_ISOSTRING;
   case VariantType::String:     return VTYPE_STRING;
   case VariantType::Complex32:  return VTYPE_COMPLEX32;
   case VariantType::Complex64:  return VTYPE_COMPLEX64;
   case VariantType::TimePoint:  return VTYPE_TIMEPOINT;
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Variant.cpp - Released 2018-12-12T09:24:30Z
