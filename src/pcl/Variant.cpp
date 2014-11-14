// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/Variant.cpp - Released 2014/11/14 17:17:01 UTC
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
   case VariantType::Invalid:               return "invalid";
   case VariantType::Bool:                  return "Bool";
   case VariantType::Int8:                  return "Int8";
   case VariantType::Int16:                 return "Int16";
   case VariantType::Int32:                 return "Int32";
   case VariantType::Int64:                 return "Int64";
   case VariantType::UInt8:                 return "UInt8";
   case VariantType::UInt16:                return "UInt16";
   case VariantType::UInt32:                return "UInt32";
   case VariantType::UInt64:                return "UInt64";
   case VariantType::Float:                 return "Float";
   case VariantType::Double:                return "Double";
   case VariantType::FComplex:              return "FComplex";
   case VariantType::DComplex:              return "DComplex";
   case VariantType::Point:                 return "Point";
   case VariantType::FPoint:                return "FPoint";
   case VariantType::DPoint:                return "DPoint";
   case VariantType::Rect:                  return "Rect";
   case VariantType::FRect:                 return "FRect";
   case VariantType::DRect:                 return "DRect";
   case VariantType::CharVector:            return "CharVector";
   case VariantType::ByteVector:            return "ByteVector";
   case VariantType::IVector:               return "IVector";
   case VariantType::UIVector:              return "UIVector";
   case VariantType::I64Vector:             return "I64Vector";
   case VariantType::UI64Vector:            return "UI64Vector";
   case VariantType::FVector:               return "FVector";
   case VariantType::DVector:               return "DVector";
   case VariantType::CharMatrix:            return "CharMatrix";
   case VariantType::ByteMatrix:            return "ByteMatrix";
   case VariantType::IMatrix:               return "IMatrix";
   case VariantType::UIMatrix:              return "UIMatrix";
   case VariantType::I64Matrix:             return "I64Matrix";
   case VariantType::UI64Matrix:            return "UI64Matrix";
   case VariantType::FMatrix:               return "FMatrix";
   case VariantType::DMatrix:               return "DMatrix";
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
   case VariantType::Float:                 return m_data.floatValue != 0;
   case VariantType::Double:                return m_data.doubleValue != 0;
   case VariantType::FComplex:              return *m_data.fcomplexValue != 0;
   case VariantType::DComplex:              return *m_data.dcomplexValue != 0;
   case VariantType::Point:                 return *m_data.pointValue != 0;
   case VariantType::FPoint:                return *m_data.fpointValue != 0.0f;
   case VariantType::DPoint:                return *m_data.dpointValue != 0.0;
   case VariantType::Rect:                  return *m_data.rectValue != 0;
   case VariantType::FRect:                 return *m_data.frectValue != 0.0f;
   case VariantType::DRect:                 return *m_data.drectValue != 0.0;
   case VariantType::CharVector:            return !m_data.charVectorValue->IsEmpty();
   case VariantType::ByteVector:            return !m_data.byteVectorValue->IsEmpty();
   case VariantType::IVector:               return !m_data.iVectorValue->IsEmpty();
   case VariantType::UIVector:              return !m_data.uiVectorValue->IsEmpty();
   case VariantType::I64Vector:             return !m_data.i64VectorValue->IsEmpty();
   case VariantType::UI64Vector:            return !m_data.ui64VectorValue->IsEmpty();
   case VariantType::FVector:               return !m_data.fVectorValue->IsEmpty();
   case VariantType::DVector:               return !m_data.dVectorValue->IsEmpty();
   case VariantType::CharMatrix:            return !m_data.charMatrixValue->IsEmpty();
   case VariantType::ByteMatrix:            return !m_data.byteMatrixValue->IsEmpty();
   case VariantType::IMatrix:               return !m_data.iMatrixValue->IsEmpty();
   case VariantType::UIMatrix:              return !m_data.uiVectorValue->IsEmpty();
   case VariantType::I64Matrix:             return !m_data.i64VectorValue->IsEmpty();
   case VariantType::UI64Matrix:            return !m_data.ui64VectorValue->IsEmpty();
   case VariantType::FMatrix:               return !m_data.fMatrixValue->IsEmpty();
   case VariantType::DMatrix:               return !m_data.dMatrixValue->IsEmpty();
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
   case VariantType::Bool:      return int( m_data.boolValue );
   case VariantType::Int8:      return int( m_data.int8Value );
   case VariantType::Int16:     return int( m_data.int16Value );
   case VariantType::Int32:     return int( m_data.int32Value );
   case VariantType::Int64:     return int( m_data.int64Value );
   case VariantType::UInt8:     return int( m_data.uint8Value );
   case VariantType::UInt16:    return int( m_data.uint16Value );
   case VariantType::UInt32:    return int( m_data.uint32Value );
   case VariantType::UInt64:    return int( m_data.uint64Value );
   case VariantType::Float:     return int( m_data.floatValue );
   case VariantType::Double:    return int( m_data.doubleValue );
   case VariantType::FComplex:  return int( m_data.fcomplexValue->Mag() );
   case VariantType::DComplex:  return int( m_data.dcomplexValue->Mag() );
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
   case VariantType::Float:     return int64( m_data.floatValue );
   case VariantType::Double:    return int64( m_data.doubleValue );
   case VariantType::FComplex:  return int64( m_data.fcomplexValue->Mag() );
   case VariantType::DComplex:  return int64( m_data.dcomplexValue->Mag() );
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
   case VariantType::Bool:      return unsigned( m_data.boolValue );
   case VariantType::Int8:      return unsigned( m_data.int8Value );
   case VariantType::Int16:     return unsigned( m_data.int16Value );
   case VariantType::Int32:     return unsigned( m_data.int32Value );
   case VariantType::Int64:     return unsigned( m_data.int64Value );
   case VariantType::UInt8:     return unsigned( m_data.uint8Value );
   case VariantType::UInt16:    return unsigned( m_data.uint16Value );
   case VariantType::UInt32:    return unsigned( m_data.uint32Value );
   case VariantType::UInt64:    return unsigned( m_data.uint64Value );
   case VariantType::Float:     return unsigned( m_data.floatValue );
   case VariantType::Double:    return unsigned( m_data.doubleValue );
   case VariantType::FComplex:  return unsigned( m_data.fcomplexValue->Mag() );
   case VariantType::DComplex:  return unsigned( m_data.dcomplexValue->Mag() );
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
   case VariantType::Float:     return uint64( m_data.floatValue );
   case VariantType::Double:    return uint64( m_data.doubleValue );
   case VariantType::FComplex:  return uint64( m_data.fcomplexValue->Mag() );
   case VariantType::DComplex:  return uint64( m_data.dcomplexValue->Mag() );
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
   case VariantType::Float:     return m_data.floatValue;
   case VariantType::Double:    return float( m_data.doubleValue );
   case VariantType::FComplex:  return float( m_data.fcomplexValue->Mag() );
   case VariantType::DComplex:  return float( m_data.dcomplexValue->Mag() );
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
   case VariantType::Float:     return double( m_data.floatValue );
   case VariantType::Double:    return m_data.doubleValue;
   case VariantType::FComplex:  return double( m_data.fcomplexValue->Mag() );
   case VariantType::DComplex:  return double( m_data.dcomplexValue->Mag() );
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
   case VariantType::Float:     return fcomplex( m_data.floatValue );
   case VariantType::Double:    return fcomplex( float( m_data.doubleValue ) );
   case VariantType::FComplex:  return *m_data.fcomplexValue;
   case VariantType::DComplex:  return fcomplex( *m_data.dcomplexValue );
   case VariantType::String:    return fcomplex( m_data.stringValue->ToFloat() );
   case VariantType::IsoString: return fcomplex( m_data.isoStringValue->ToFloat() );
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
   case VariantType::Float:     return dcomplex( double( m_data.floatValue ) );
   case VariantType::Double:    return dcomplex( m_data.doubleValue );
   case VariantType::FComplex:  return dcomplex( *m_data.fcomplexValue );
   case VariantType::DComplex:  return *m_data.dcomplexValue;
   case VariantType::String:    return dcomplex( m_data.stringValue->ToDouble() );
   case VariantType::IsoString: return dcomplex( m_data.isoStringValue->ToDouble() );
   default:
      throw Error( "Variant::ToDComplex(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

Point Variant::ToPoint() const
{
   switch ( m_type )
   {
   case VariantType::Int8:      return Point( int( m_data.int8Value ) );
   case VariantType::Int16:     return Point( int( m_data.int16Value ) );
   case VariantType::Int32:     return Point( int( m_data.int32Value ) );
   case VariantType::Int64:     return Point( int( m_data.int64Value ) );
   case VariantType::UInt8:     return Point( int( m_data.uint8Value ) );
   case VariantType::UInt16:    return Point( int( m_data.uint16Value ) );
   case VariantType::UInt32:    return Point( int( m_data.uint32Value ) );
   case VariantType::UInt64:    return Point( int( m_data.uint64Value ) );
   case VariantType::Float:     return Point( int( m_data.floatValue ) );
   case VariantType::Double:    return Point( int( m_data.doubleValue ) );
   case VariantType::FComplex:  return Point( int( m_data.fcomplexValue->Real() ), int( m_data.fcomplexValue->Imag() ) );
   case VariantType::DComplex:  return Point( int( m_data.dcomplexValue->Real() ), int( m_data.dcomplexValue->Imag() ) );
   case VariantType::Point:     return *m_data.pointValue;
   case VariantType::FPoint:    return Point( *m_data.fpointValue );
   case VariantType::DPoint:    return Point( *m_data.dpointValue );
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
   case VariantType::Float:     return FPoint( m_data.floatValue );
   case VariantType::Double:    return FPoint( float( m_data.doubleValue ) );
   case VariantType::FComplex:  return FPoint( m_data.fcomplexValue->Real(), m_data.fcomplexValue->Imag() );
   case VariantType::DComplex:  return FPoint( float( m_data.dcomplexValue->Real() ), float( m_data.dcomplexValue->Imag() ) );
   case VariantType::Point:     return FPoint( *m_data.pointValue );
   case VariantType::FPoint:    return *m_data.fpointValue;
   case VariantType::DPoint:    return FPoint( *m_data.dpointValue );
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
   case VariantType::Float:     return DPoint( double( m_data.floatValue ) );
   case VariantType::Double:    return DPoint( m_data.doubleValue );
   case VariantType::FComplex:  return DPoint( double( m_data.fcomplexValue->Real() ), double( m_data.fcomplexValue->Imag() ) );
   case VariantType::DComplex:  return DPoint( m_data.dcomplexValue->Real(), m_data.dcomplexValue->Imag() );
   case VariantType::Point:     return DPoint( *m_data.pointValue );
   case VariantType::FPoint:    return DPoint( *m_data.fpointValue );
   case VariantType::DPoint:    return *m_data.dpointValue;
   default:
      throw Error( "Variant::ToDPoint(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

Rect Variant::ToRect() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return Rect( int( m_data.boolValue ) );
   case VariantType::Int8:      return Rect( int( m_data.int8Value ) );
   case VariantType::Int16:     return Rect( int( m_data.int16Value ) );
   case VariantType::Int32:     return Rect( int( m_data.int32Value ) );
   case VariantType::Int64:     return Rect( int( m_data.int64Value ) );
   case VariantType::UInt8:     return Rect( int( m_data.uint8Value ) );
   case VariantType::UInt16:    return Rect( int( m_data.uint16Value ) );
   case VariantType::UInt32:    return Rect( int( m_data.uint32Value ) );
   case VariantType::UInt64:    return Rect( int( m_data.uint64Value ) );
   case VariantType::Float:     return Rect( int( m_data.floatValue ) );
   case VariantType::Double:    return Rect( int( m_data.doubleValue ) );
   case VariantType::FComplex:  return Rect( int( m_data.fcomplexValue->Mag() ) );
   case VariantType::DComplex:  return Rect( int( m_data.dcomplexValue->Mag() ) );
   case VariantType::Rect:      return *m_data.rectValue;
   case VariantType::FRect:     return Rect( *m_data.frectValue );
   case VariantType::DRect:     return Rect( *m_data.drectValue );
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
   case VariantType::Float:     return FRect( float( m_data.floatValue ) );
   case VariantType::Double:    return FRect( float( m_data.doubleValue ) );
   case VariantType::FComplex:  return FRect( m_data.fcomplexValue->Mag() );
   case VariantType::DComplex:  return FRect( float( m_data.dcomplexValue->Mag() ) );
   case VariantType::Rect:      return FRect( *m_data.rectValue );
   case VariantType::FRect:     return *m_data.frectValue;
   case VariantType::DRect:     return FRect( *m_data.drectValue );
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
   case VariantType::Float:     return DRect( double( m_data.floatValue ) );
   case VariantType::Double:    return DRect( double( m_data.doubleValue ) );
   case VariantType::FComplex:  return DRect( double( m_data.fcomplexValue->Mag() ) );
   case VariantType::DComplex:  return DRect( m_data.dcomplexValue->Mag() );
   case VariantType::Rect:      return DRect( *m_data.rectValue );
   case VariantType::FRect:     return DRect( *m_data.frectValue );
   case VariantType::DRect:     return *m_data.drectValue;
   default:
      throw Error( "Variant::ToDRect(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

CharVector Variant::ToCharVector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return CharVector( int8( m_data.boolValue ), 1 );
   case VariantType::Int8:      return CharVector( m_data.int8Value, 1 );
   case VariantType::Int16:     return CharVector( int8( m_data.int16Value ), 1 );
   case VariantType::Int32:     return CharVector( int8( m_data.int32Value ), 1 );
   case VariantType::Int64:     return CharVector( int8( m_data.int64Value ), 1 );
   case VariantType::UInt8:     return CharVector( int8( m_data.uint8Value ), 1 );
   case VariantType::UInt16:    return CharVector( int8( m_data.uint16Value ), 1 );
   case VariantType::UInt32:    return CharVector( int8( m_data.uint32Value ), 1 );
   case VariantType::UInt64:    return CharVector( int8( m_data.uint64Value ), 1 );
   case VariantType::Float:     return CharVector( int8( m_data.floatValue ), 1 );
   case VariantType::Double:    return CharVector( int8( m_data.doubleValue ), 1 );

   case VariantType::FComplex:
   {
      CharVector v( 2 );
      v[0] = int8( m_data.fcomplexValue->Real() );
      v[1] = int8( m_data.fcomplexValue->Imag() );
      return v;
   }
   case VariantType::DComplex:
   {
      CharVector v( 2 );
      v[0] = int8( m_data.dcomplexValue->Real() );
      v[1] = int8( m_data.dcomplexValue->Imag() );
      return v;
   }

   case VariantType::Point:
   {
      CharVector v( 2 );
      v[0] = int8( m_data.pointValue->x );
      v[1] = int8( m_data.pointValue->y );
      return v;
   }
   case VariantType::FPoint:
   {
      CharVector v( 2 );
      v[0] = int8( m_data.fpointValue->x );
      v[1] = int8( m_data.fpointValue->y );
      return v;
   }
   case VariantType::DPoint:
   {
      CharVector v( 2 );
      v[0] = int8( m_data.dpointValue->x );
      v[1] = int8( m_data.dpointValue->y );
      return v;
   }

   case VariantType::Rect:
   {
      CharVector v( 4 );
      v[0] = int8( m_data.rectValue->x0 );
      v[1] = int8( m_data.rectValue->y0 );
      v[2] = int8( m_data.rectValue->x1 );
      v[3] = int8( m_data.rectValue->y1 );
      return v;
   }
   case VariantType::FRect:
   {
      CharVector v( 4 );
      v[0] = int8( m_data.frectValue->x0 );
      v[1] = int8( m_data.frectValue->y0 );
      v[2] = int8( m_data.frectValue->x1 );
      v[3] = int8( m_data.frectValue->y1 );
      return v;
   }
   case VariantType::DRect:
   {
      CharVector v( 4 );
      v[0] = int8( m_data.drectValue->x0 );
      v[1] = int8( m_data.drectValue->y0 );
      v[2] = int8( m_data.drectValue->x1 );
      v[3] = int8( m_data.drectValue->y1 );
      return v;
   }

   case VariantType::CharVector: return *m_data.charVectorValue;
   case VariantType::ByteVector: return CharVector( m_data.byteVectorValue->Begin(), m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return CharVector( m_data.iVectorValue->Begin(), m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return CharVector( m_data.uiVectorValue->Begin(), m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return CharVector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return CharVector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return CharVector( m_data.fVectorValue->Begin(), m_data.fVectorValue->Length() );
   case VariantType::DVector:    return CharVector( m_data.dVectorValue->Begin(), m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return CharVector( m_data.charMatrixValue->Begin(), int( m_data.charMatrixValue->NumberOfElements() ) );
   case VariantType::ByteMatrix: return CharVector( m_data.byteMatrixValue->Begin(), int( m_data.byteMatrixValue->NumberOfElements() ) );
   case VariantType::IMatrix:    return CharVector( m_data.iMatrixValue->Begin(), int( m_data.iMatrixValue->NumberOfElements() ) );
   case VariantType::UIMatrix:   return CharVector( m_data.uiMatrixValue->Begin(), int( m_data.uiMatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:  return CharVector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix: return CharVector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::FMatrix:    return CharVector( m_data.fMatrixValue->Begin(), int( m_data.fMatrixValue->NumberOfElements() ) );
   case VariantType::DMatrix:    return CharVector( m_data.dMatrixValue->Begin(), int( m_data.dMatrixValue->NumberOfElements() ) );

   case VariantType::ByteArray:  return CharVector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:     return CharVector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:  return CharVector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToCharVector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

ByteVector Variant::ToByteVector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return ByteVector( uint8( m_data.boolValue ), 1 );
   case VariantType::Int8:      return ByteVector( uint8( m_data.int8Value ), 1 );
   case VariantType::Int16:     return ByteVector( uint8( m_data.int16Value ), 1 );
   case VariantType::Int32:     return ByteVector( uint8( m_data.int32Value ), 1 );
   case VariantType::Int64:     return ByteVector( uint8( m_data.int64Value ), 1 );
   case VariantType::UInt8:     return ByteVector( m_data.uint8Value, 1 );
   case VariantType::UInt16:    return ByteVector( uint8( m_data.uint16Value ), 1 );
   case VariantType::UInt32:    return ByteVector( uint8( m_data.uint32Value ), 1 );
   case VariantType::UInt64:    return ByteVector( uint8( m_data.uint64Value ), 1 );
   case VariantType::Float:     return ByteVector( uint8( m_data.floatValue ), 1 );
   case VariantType::Double:    return ByteVector( uint8( m_data.doubleValue ), 1 );

   case VariantType::FComplex:
   {
      ByteVector v( 2 );
      v[0] = uint8( m_data.fcomplexValue->Real() );
      v[1] = uint8( m_data.fcomplexValue->Imag() );
      return v;
   }
   case VariantType::DComplex:
   {
      ByteVector v( 2 );
      v[0] = uint8( m_data.dcomplexValue->Real() );
      v[1] = uint8( m_data.dcomplexValue->Imag() );
      return v;
   }

   case VariantType::Point:
   {
      ByteVector v( 2 );
      v[0] = uint8( m_data.pointValue->x );
      v[1] = uint8( m_data.pointValue->y );
      return v;
   }
   case VariantType::FPoint:
   {
      ByteVector v( 2 );
      v[0] = uint8( m_data.fpointValue->x );
      v[1] = uint8( m_data.fpointValue->y );
      return v;
   }
   case VariantType::DPoint:
   {
      ByteVector v( 2 );
      v[0] = uint8( m_data.dpointValue->x );
      v[1] = uint8( m_data.dpointValue->y );
      return v;
   }

   case VariantType::Rect:
   {
      ByteVector v( 4 );
      v[0] = uint8( m_data.rectValue->x0 );
      v[1] = uint8( m_data.rectValue->y0 );
      v[2] = uint8( m_data.rectValue->x1 );
      v[3] = uint8( m_data.rectValue->y1 );
      return v;
   }
   case VariantType::FRect:
   {
      ByteVector v( 4 );
      v[0] = uint8( m_data.frectValue->x0 );
      v[1] = uint8( m_data.frectValue->y0 );
      v[2] = uint8( m_data.frectValue->x1 );
      v[3] = uint8( m_data.frectValue->y1 );
      return v;
   }
   case VariantType::DRect:
   {
      ByteVector v( 4 );
      v[0] = uint8( m_data.drectValue->x0 );
      v[1] = uint8( m_data.drectValue->y0 );
      v[2] = uint8( m_data.drectValue->x1 );
      v[3] = uint8( m_data.drectValue->y1 );
      return v;
   }

   case VariantType::CharVector: return ByteVector( m_data.charVectorValue->Begin(), m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return *m_data.byteVectorValue;
   case VariantType::IVector:    return ByteVector( m_data.iVectorValue->Begin(), m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return ByteVector( m_data.uiVectorValue->Begin(), m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return ByteVector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return ByteVector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return ByteVector( m_data.fVectorValue->Begin(), m_data.fVectorValue->Length() );
   case VariantType::DVector:    return ByteVector( m_data.dVectorValue->Begin(), m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return ByteVector( m_data.charMatrixValue->Begin(), int( m_data.charMatrixValue->NumberOfElements() ) );
   case VariantType::ByteMatrix: return ByteVector( m_data.byteMatrixValue->Begin(), int( m_data.byteMatrixValue->NumberOfElements() ) );
   case VariantType::IMatrix:    return ByteVector( m_data.iMatrixValue->Begin(), int( m_data.iMatrixValue->NumberOfElements() ) );
   case VariantType::UIMatrix:   return ByteVector( m_data.uiMatrixValue->Begin(), int( m_data.uiMatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:  return ByteVector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix: return ByteVector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::FMatrix:    return ByteVector( m_data.fMatrixValue->Begin(), int( m_data.fMatrixValue->NumberOfElements() ) );
   case VariantType::DMatrix:    return ByteVector( m_data.dMatrixValue->Begin(), int( m_data.dMatrixValue->NumberOfElements() ) );

   case VariantType::ByteArray:  return ByteVector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:     return ByteVector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:  return ByteVector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToByteVector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

IVector Variant::ToIVector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return IVector( int( m_data.boolValue ), 1 );
   case VariantType::Int8:      return IVector( int( m_data.int8Value ), 1 );
   case VariantType::Int16:     return IVector( int( m_data.int16Value ), 1 );
   case VariantType::Int32:     return IVector( m_data.int32Value, 1 );
   case VariantType::Int64:     return IVector( int( m_data.int64Value ), 1 );
   case VariantType::UInt8:     return IVector( int( m_data.uint8Value ), 1 );
   case VariantType::UInt16:    return IVector( int( m_data.uint16Value ), 1 );
   case VariantType::UInt32:    return IVector( int( m_data.uint32Value ), 1 );
   case VariantType::UInt64:    return IVector( int( m_data.uint64Value ), 1 );
   case VariantType::Float:     return IVector( int( m_data.floatValue ), 1 );
   case VariantType::Double:    return IVector( int( m_data.doubleValue ), 1 );

   case VariantType::FComplex:
   {
      IVector v( 2 );
      v[0] = int( m_data.fcomplexValue->Real() );
      v[1] = int( m_data.fcomplexValue->Imag() );
      return v;
   }
   case VariantType::DComplex:
   {
      IVector v( 2 );
      v[0] = int( m_data.dcomplexValue->Real() );
      v[1] = int( m_data.dcomplexValue->Imag() );
      return v;
   }

   case VariantType::Point:
   {
      IVector v( 2 );
      v[0] = m_data.pointValue->x;
      v[1] = m_data.pointValue->y;
      return v;
   }
   case VariantType::FPoint:
   {
      IVector v( 2 );
      v[0] = int( m_data.fpointValue->x );
      v[1] = int( m_data.fpointValue->y );
      return v;
   }
   case VariantType::DPoint:
   {
      IVector v( 2 );
      v[0] = int( m_data.dpointValue->x );
      v[1] = int( m_data.dpointValue->y );
      return v;
   }

   case VariantType::Rect:
   {
      IVector v( 4 );
      v[0] = m_data.rectValue->x0;
      v[1] = m_data.rectValue->y0;
      v[2] = m_data.rectValue->x1;
      v[3] = m_data.rectValue->y1;
      return v;
   }
   case VariantType::FRect:
   {
      IVector v( 4 );
      v[0] = int( m_data.frectValue->x0 );
      v[1] = int( m_data.frectValue->y0 );
      v[2] = int( m_data.frectValue->x1 );
      v[3] = int( m_data.frectValue->y1 );
      return v;
   }
   case VariantType::DRect:
   {
      IVector v( 4 );
      v[0] = int( m_data.drectValue->x0 );
      v[1] = int( m_data.drectValue->y0 );
      v[2] = int( m_data.drectValue->x1 );
      v[3] = int( m_data.drectValue->y1 );
      return v;
   }

   case VariantType::CharVector: return IVector( m_data.charVectorValue->Begin(), m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return IVector( m_data.byteVectorValue->Begin(), m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return *m_data.iVectorValue;
   case VariantType::UIVector:   return IVector( m_data.uiVectorValue->Begin(), m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return IVector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return IVector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return IVector( m_data.fVectorValue->Begin(), m_data.fVectorValue->Length() );
   case VariantType::DVector:    return IVector( m_data.dVectorValue->Begin(), m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return IVector( m_data.charMatrixValue->Begin(), int( m_data.charMatrixValue->NumberOfElements() ) );
   case VariantType::ByteMatrix: return IVector( m_data.byteMatrixValue->Begin(), int( m_data.byteMatrixValue->NumberOfElements() ) );
   case VariantType::IMatrix:    return IVector( m_data.iMatrixValue->Begin(), int( m_data.iMatrixValue->NumberOfElements() ) );
   case VariantType::UIMatrix:   return IVector( m_data.uiMatrixValue->Begin(), int( m_data.uiMatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:  return IVector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix: return IVector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::FMatrix:    return IVector( m_data.fMatrixValue->Begin(), int( m_data.fMatrixValue->NumberOfElements() ) );
   case VariantType::DMatrix:    return IVector( m_data.dMatrixValue->Begin(), int( m_data.dMatrixValue->NumberOfElements() ) );

   case VariantType::ByteArray:  return IVector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:     return IVector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:  return IVector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToIVector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

UIVector Variant::ToUIVector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return UIVector( unsigned( m_data.boolValue ), 1 );
   case VariantType::Int8:      return UIVector( unsigned( m_data.int8Value ), 1 );
   case VariantType::Int16:     return UIVector( unsigned( m_data.int16Value ), 1 );
   case VariantType::Int32:     return UIVector( unsigned( m_data.int32Value ), 1 );
   case VariantType::Int64:     return UIVector( unsigned( m_data.int64Value ), 1 );
   case VariantType::UInt8:     return UIVector( unsigned( m_data.uint8Value ), 1 );
   case VariantType::UInt16:    return UIVector( unsigned( m_data.uint16Value ), 1 );
   case VariantType::UInt32:    return UIVector( m_data.uint32Value, 1 );
   case VariantType::UInt64:    return UIVector( unsigned( m_data.uint64Value ), 1 );
   case VariantType::Float:     return UIVector( unsigned( m_data.floatValue ), 1 );
   case VariantType::Double:    return UIVector( unsigned( m_data.doubleValue ), 1 );

   case VariantType::FComplex:
   {
      UIVector v( 2 );
      v[0] = unsigned( m_data.fcomplexValue->Real() );
      v[1] = unsigned( m_data.fcomplexValue->Imag() );
      return v;
   }
   case VariantType::DComplex:
   {
      UIVector v( 2 );
      v[0] = unsigned( m_data.dcomplexValue->Real() );
      v[1] = unsigned( m_data.dcomplexValue->Imag() );
      return v;
   }

   case VariantType::Point:
   {
      UIVector v( 2 );
      v[0] = unsigned( m_data.pointValue->x );
      v[1] = unsigned( m_data.pointValue->y );
      return v;
   }
   case VariantType::FPoint:
   {
      UIVector v( 2 );
      v[0] = unsigned( m_data.fpointValue->x );
      v[1] = unsigned( m_data.fpointValue->y );
      return v;
   }
   case VariantType::DPoint:
   {
      UIVector v( 2 );
      v[0] = unsigned( m_data.dpointValue->x );
      v[1] = unsigned( m_data.dpointValue->y );
      return v;
   }

   case VariantType::Rect:
   {
      UIVector v( 4 );
      v[0] = unsigned( m_data.rectValue->x0 );
      v[1] = unsigned( m_data.rectValue->y0 );
      v[2] = unsigned( m_data.rectValue->x1 );
      v[3] = unsigned( m_data.rectValue->y1 );
      return v;
   }
   case VariantType::FRect:
   {
      UIVector v( 4 );
      v[0] = unsigned( m_data.frectValue->x0 );
      v[1] = unsigned( m_data.frectValue->y0 );
      v[2] = unsigned( m_data.frectValue->x1 );
      v[3] = unsigned( m_data.frectValue->y1 );
      return v;
   }
   case VariantType::DRect:
   {
      UIVector v( 4 );
      v[0] = unsigned( m_data.drectValue->x0 );
      v[1] = unsigned( m_data.drectValue->y0 );
      v[2] = unsigned( m_data.drectValue->x1 );
      v[3] = unsigned( m_data.drectValue->y1 );
      return v;
   }

   case VariantType::CharVector: return UIVector( m_data.charVectorValue->Begin(), m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return UIVector( m_data.byteVectorValue->Begin(), m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return UIVector( m_data.iVectorValue->Begin(), m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return *m_data.uiVectorValue;
   case VariantType::I64Vector:  return UIVector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return UIVector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return UIVector( m_data.fVectorValue->Begin(), m_data.fVectorValue->Length() );
   case VariantType::DVector:    return UIVector( m_data.dVectorValue->Begin(), m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return UIVector( m_data.charMatrixValue->Begin(), int( m_data.charMatrixValue->NumberOfElements() ) );
   case VariantType::ByteMatrix: return UIVector( m_data.byteMatrixValue->Begin(), int( m_data.byteMatrixValue->NumberOfElements() ) );
   case VariantType::IMatrix:    return UIVector( m_data.iMatrixValue->Begin(), int( m_data.iMatrixValue->NumberOfElements() ) );
   case VariantType::UIMatrix:   return UIVector( m_data.uiMatrixValue->Begin(), int( m_data.uiMatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:  return UIVector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix: return UIVector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::FMatrix:    return UIVector( m_data.fMatrixValue->Begin(), int( m_data.fMatrixValue->NumberOfElements() ) );
   case VariantType::DMatrix:    return UIVector( m_data.dMatrixValue->Begin(), int( m_data.dMatrixValue->NumberOfElements() ) );

   case VariantType::ByteArray:  return UIVector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:     return UIVector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:  return UIVector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToUIVector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

I64Vector Variant::ToI64Vector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return I64Vector( int64( m_data.boolValue ), 1 );
   case VariantType::Int8:      return I64Vector( int64( m_data.int8Value ), 1 );
   case VariantType::Int16:     return I64Vector( int64( m_data.int16Value ), 1 );
   case VariantType::Int32:     return I64Vector( int64( m_data.int32Value ), 1 );
   case VariantType::Int64:     return I64Vector( m_data.int64Value, 1 );
   case VariantType::UInt8:     return I64Vector( int64( m_data.uint8Value ), 1 );
   case VariantType::UInt16:    return I64Vector( int64( m_data.uint16Value ), 1 );
   case VariantType::UInt32:    return I64Vector( int64( m_data.uint32Value ), 1 );
   case VariantType::UInt64:    return I64Vector( int64( m_data.uint64Value ), 1 );
   case VariantType::Float:     return I64Vector( int64( m_data.floatValue ), 1 );
   case VariantType::Double:    return I64Vector( int64( m_data.doubleValue ), 1 );

   case VariantType::FComplex:
   {
      I64Vector v( 2 );
      v[0] = int64( m_data.fcomplexValue->Real() );
      v[1] = int64( m_data.fcomplexValue->Imag() );
      return v;
   }
   case VariantType::DComplex:
   {
      I64Vector v( 2 );
      v[0] = int64( m_data.dcomplexValue->Real() );
      v[1] = int64( m_data.dcomplexValue->Imag() );
      return v;
   }

   case VariantType::Point:
   {
      I64Vector v( 2 );
      v[0] = int64( m_data.pointValue->x );
      v[1] = int64( m_data.pointValue->y );
      return v;
   }
   case VariantType::FPoint:
   {
      I64Vector v( 2 );
      v[0] = int64( m_data.fpointValue->x );
      v[1] = int64( m_data.fpointValue->y );
      return v;
   }
   case VariantType::DPoint:
   {
      I64Vector v( 2 );
      v[0] = int64( m_data.dpointValue->x );
      v[1] = int64( m_data.dpointValue->y );
      return v;
   }

   case VariantType::Rect:
   {
      I64Vector v( 4 );
      v[0] = int64( m_data.rectValue->x0 );
      v[1] = int64( m_data.rectValue->y0 );
      v[2] = int64( m_data.rectValue->x1 );
      v[3] = int64( m_data.rectValue->y1 );
      return v;
   }
   case VariantType::FRect:
   {
      I64Vector v( 4 );
      v[0] = int64( m_data.frectValue->x0 );
      v[1] = int64( m_data.frectValue->y0 );
      v[2] = int64( m_data.frectValue->x1 );
      v[3] = int64( m_data.frectValue->y1 );
      return v;
   }
   case VariantType::DRect:
   {
      I64Vector v( 4 );
      v[0] = int64( m_data.drectValue->x0 );
      v[1] = int64( m_data.drectValue->y0 );
      v[2] = int64( m_data.drectValue->x1 );
      v[3] = int64( m_data.drectValue->y1 );
      return v;
   }

   case VariantType::CharVector: return I64Vector( m_data.charVectorValue->Begin(), m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return I64Vector( m_data.byteVectorValue->Begin(), m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return I64Vector( m_data.iVectorValue->Begin(), m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return I64Vector( m_data.uiVectorValue->Begin(), m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return *m_data.i64VectorValue;
   case VariantType::UI64Vector: return I64Vector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return I64Vector( m_data.fVectorValue->Begin(), m_data.fVectorValue->Length() );
   case VariantType::DVector:    return I64Vector( m_data.dVectorValue->Begin(), m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return I64Vector( m_data.charMatrixValue->Begin(), int( m_data.charMatrixValue->NumberOfElements() ) );
   case VariantType::ByteMatrix: return I64Vector( m_data.byteMatrixValue->Begin(), int( m_data.byteMatrixValue->NumberOfElements() ) );
   case VariantType::IMatrix:    return I64Vector( m_data.iMatrixValue->Begin(), int( m_data.iMatrixValue->NumberOfElements() ) );
   case VariantType::UIMatrix:   return I64Vector( m_data.uiMatrixValue->Begin(), int( m_data.uiMatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:  return I64Vector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix: return I64Vector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::FMatrix:    return I64Vector( m_data.fMatrixValue->Begin(), int( m_data.fMatrixValue->NumberOfElements() ) );
   case VariantType::DMatrix:    return I64Vector( m_data.dMatrixValue->Begin(), int( m_data.dMatrixValue->NumberOfElements() ) );

   case VariantType::ByteArray:  return I64Vector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:     return I64Vector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:  return I64Vector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToI64Vector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

UI64Vector Variant::ToUI64Vector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return UI64Vector( uint64( m_data.boolValue ), 1 );
   case VariantType::Int8:      return UI64Vector( uint64( m_data.int8Value ), 1 );
   case VariantType::Int16:     return UI64Vector( uint64( m_data.int16Value ), 1 );
   case VariantType::Int32:     return UI64Vector( uint64( m_data.int32Value ), 1 );
   case VariantType::Int64:     return UI64Vector( uint64( m_data.int64Value ), 1 );
   case VariantType::UInt8:     return UI64Vector( uint64( m_data.uint8Value ), 1 );
   case VariantType::UInt16:    return UI64Vector( uint64( m_data.uint16Value ), 1 );
   case VariantType::UInt32:    return UI64Vector( uint64( m_data.uint32Value ), 1 );
   case VariantType::UInt64:    return UI64Vector( m_data.uint64Value, 1 );
   case VariantType::Float:     return UI64Vector( uint64( m_data.floatValue ), 1 );
   case VariantType::Double:    return UI64Vector( uint64( m_data.doubleValue ), 1 );

   case VariantType::FComplex:
   {
      UI64Vector v( 2 );
      v[0] = uint64( m_data.fcomplexValue->Real() );
      v[1] = uint64( m_data.fcomplexValue->Imag() );
      return v;
   }
   case VariantType::DComplex:
   {
      UI64Vector v( 2 );
      v[0] = uint64( m_data.dcomplexValue->Real() );
      v[1] = uint64( m_data.dcomplexValue->Imag() );
      return v;
   }

   case VariantType::Point:
   {
      UI64Vector v( 2 );
      v[0] = uint64( m_data.pointValue->x );
      v[1] = uint64( m_data.pointValue->y );
      return v;
   }
   case VariantType::FPoint:
   {
      UI64Vector v( 2 );
      v[0] = uint64( m_data.fpointValue->x );
      v[1] = uint64( m_data.fpointValue->y );
      return v;
   }
   case VariantType::DPoint:
   {
      UI64Vector v( 2 );
      v[0] = uint64( m_data.dpointValue->x );
      v[1] = uint64( m_data.dpointValue->y );
      return v;
   }

   case VariantType::Rect:
   {
      UI64Vector v( 4 );
      v[0] = uint64( m_data.rectValue->x0 );
      v[1] = uint64( m_data.rectValue->y0 );
      v[2] = uint64( m_data.rectValue->x1 );
      v[3] = uint64( m_data.rectValue->y1 );
      return v;
   }
   case VariantType::FRect:
   {
      UI64Vector v( 4 );
      v[0] = uint64( m_data.frectValue->x0 );
      v[1] = uint64( m_data.frectValue->y0 );
      v[2] = uint64( m_data.frectValue->x1 );
      v[3] = uint64( m_data.frectValue->y1 );
      return v;
   }
   case VariantType::DRect:
   {
      UI64Vector v( 4 );
      v[0] = uint64( m_data.drectValue->x0 );
      v[1] = uint64( m_data.drectValue->y0 );
      v[2] = uint64( m_data.drectValue->x1 );
      v[3] = uint64( m_data.drectValue->y1 );
      return v;
   }

   case VariantType::CharVector: return UI64Vector( m_data.charVectorValue->Begin(), m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return UI64Vector( m_data.byteVectorValue->Begin(), m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return UI64Vector( m_data.iVectorValue->Begin(), m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return UI64Vector( m_data.uiVectorValue->Begin(), m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return UI64Vector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return *m_data.ui64VectorValue;
   case VariantType::FVector:    return UI64Vector( m_data.fVectorValue->Begin(), m_data.fVectorValue->Length() );
   case VariantType::DVector:    return UI64Vector( m_data.dVectorValue->Begin(), m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return UI64Vector( m_data.charMatrixValue->Begin(), int( m_data.charMatrixValue->NumberOfElements() ) );
   case VariantType::ByteMatrix: return UI64Vector( m_data.byteMatrixValue->Begin(), int( m_data.byteMatrixValue->NumberOfElements() ) );
   case VariantType::IMatrix:    return UI64Vector( m_data.iMatrixValue->Begin(), int( m_data.iMatrixValue->NumberOfElements() ) );
   case VariantType::UIMatrix:   return UI64Vector( m_data.uiMatrixValue->Begin(), int( m_data.uiMatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:  return UI64Vector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix: return UI64Vector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::FMatrix:    return UI64Vector( m_data.fMatrixValue->Begin(), int( m_data.fMatrixValue->NumberOfElements() ) );
   case VariantType::DMatrix:    return UI64Vector( m_data.dMatrixValue->Begin(), int( m_data.dMatrixValue->NumberOfElements() ) );

   case VariantType::ByteArray:  return UI64Vector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:     return UI64Vector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:  return UI64Vector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToUI64Vector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

FVector Variant::ToFVector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return FVector( float( m_data.boolValue ), 1 );
   case VariantType::Int8:      return FVector( float( m_data.int8Value ), 1 );
   case VariantType::Int16:     return FVector( float( m_data.int16Value ), 1 );
   case VariantType::Int32:     return FVector( float( m_data.int32Value ), 1 );
   case VariantType::Int64:     return FVector( float( m_data.int64Value ), 1 );
   case VariantType::UInt8:     return FVector( float( m_data.uint8Value ), 1 );
   case VariantType::UInt16:    return FVector( float( m_data.uint16Value ), 1 );
   case VariantType::UInt32:    return FVector( float( m_data.uint32Value ), 1 );
   case VariantType::UInt64:    return FVector( float( m_data.uint64Value ), 1 );
   case VariantType::Float:     return FVector( m_data.floatValue, 1 );
   case VariantType::Double:    return FVector( float( m_data.doubleValue ), 1 );

   case VariantType::FComplex:
   {
      FVector v( 2 );
      v[0] = m_data.fcomplexValue->Real();
      v[1] = m_data.fcomplexValue->Imag();
      return v;
   }
   case VariantType::DComplex:
   {
      FVector v( 2 );
      v[0] = float( m_data.dcomplexValue->Real() );
      v[1] = float( m_data.dcomplexValue->Imag() );
      return v;
   }

   case VariantType::Point:
   {
      FVector v( 2 );
      v[0] = float( m_data.pointValue->x );
      v[1] = float( m_data.pointValue->y );
      return v;
   }
   case VariantType::FPoint:
   {
      FVector v( 2 );
      v[0] = m_data.fpointValue->x;
      v[1] = m_data.fpointValue->y;
      return v;
   }
   case VariantType::DPoint:
   {
      FVector v( 2 );
      v[0] = float( m_data.dpointValue->x );
      v[1] = float( m_data.dpointValue->y );
      return v;
   }

   case VariantType::Rect:
   {
      FVector v( 4 );
      v[0] = float( m_data.rectValue->x0 );
      v[1] = float( m_data.rectValue->y0 );
      v[2] = float( m_data.rectValue->x1 );
      v[3] = float( m_data.rectValue->y1 );
      return v;
   }
   case VariantType::FRect:
   {
      FVector v( 4 );
      v[0] = m_data.frectValue->x0;
      v[1] = m_data.frectValue->y0;
      v[2] = m_data.frectValue->x1;
      v[3] = m_data.frectValue->y1;
      return v;
   }
   case VariantType::DRect:
   {
      FVector v( 4 );
      v[0] = float( m_data.drectValue->x0 );
      v[1] = float( m_data.drectValue->y0 );
      v[2] = float( m_data.drectValue->x1 );
      v[3] = float( m_data.drectValue->y1 );
      return v;
   }

   case VariantType::CharVector: return FVector( m_data.charVectorValue->Begin(), m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return FVector( m_data.byteVectorValue->Begin(), m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return FVector( m_data.iVectorValue->Begin(), m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return FVector( m_data.uiVectorValue->Begin(), m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return FVector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return FVector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return *m_data.fVectorValue;
   case VariantType::DVector:    return FVector( m_data.dVectorValue->Begin(), m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return FVector( m_data.charMatrixValue->Begin(), int( m_data.charMatrixValue->NumberOfElements() ) );
   case VariantType::ByteMatrix: return FVector( m_data.byteMatrixValue->Begin(), int( m_data.byteMatrixValue->NumberOfElements() ) );
   case VariantType::IMatrix:    return FVector( m_data.iMatrixValue->Begin(), int( m_data.iMatrixValue->NumberOfElements() ) );
   case VariantType::UIMatrix:   return FVector( m_data.uiMatrixValue->Begin(), int( m_data.uiMatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:  return FVector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix: return FVector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::FMatrix:    return FVector( m_data.fMatrixValue->Begin(), int( m_data.fMatrixValue->NumberOfElements() ) );
   case VariantType::DMatrix:    return FVector( m_data.dMatrixValue->Begin(), int( m_data.dMatrixValue->NumberOfElements() ) );

   case VariantType::ByteArray:  return FVector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:     return FVector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString:  return FVector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToFVector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

DVector Variant::ToDVector() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return DVector( double( m_data.boolValue ), 1 );
   case VariantType::Int8:      return DVector( double( m_data.int8Value ), 1 );
   case VariantType::Int16:     return DVector( double( m_data.int16Value ), 1 );
   case VariantType::Int32:     return DVector( double( m_data.int32Value ), 1 );
   case VariantType::Int64:     return DVector( double( m_data.int64Value ), 1 );
   case VariantType::UInt8:     return DVector( double( m_data.uint8Value ), 1 );
   case VariantType::UInt16:    return DVector( double( m_data.uint16Value ), 1 );
   case VariantType::UInt32:    return DVector( double( m_data.uint32Value ), 1 );
   case VariantType::UInt64:    return DVector( double( m_data.uint64Value ), 1 );
   case VariantType::Float:     return DVector( double( m_data.floatValue ), 1 );
   case VariantType::Double:    return DVector( m_data.doubleValue, 1 );

   case VariantType::FComplex:
   {
      DVector v( 2 );
      v[0] = double( m_data.fcomplexValue->Real() );
      v[1] = double( m_data.fcomplexValue->Imag() );
      return v;
   }
   case VariantType::DComplex:
   {
      DVector v( 2 );
      v[0] = m_data.dcomplexValue->Real();
      v[1] = m_data.dcomplexValue->Imag();
      return v;
   }

   case VariantType::Point:
   {
      DVector v( 2 );
      v[0] = double( m_data.pointValue->x );
      v[1] = double( m_data.pointValue->y );
      return v;
   }
   case VariantType::FPoint:
   {
      DVector v( 2 );
      v[0] = double( m_data.fpointValue->x );
      v[1] = double( m_data.fpointValue->y );
      return v;
   }
   case VariantType::DPoint:
   {
      DVector v( 2 );
      v[0] = m_data.dpointValue->x;
      v[1] = m_data.dpointValue->y;
      return v;
   }

   case VariantType::Rect:
   {
      DVector v( 4 );
      v[0] = double( m_data.rectValue->x0 );
      v[1] = double( m_data.rectValue->y0 );
      v[2] = double( m_data.rectValue->x1 );
      v[3] = double( m_data.rectValue->y1 );
      return v;
   }
   case VariantType::FRect:
   {
      DVector v( 4 );
      v[0] = double( m_data.frectValue->x0 );
      v[1] = double( m_data.frectValue->y0 );
      v[2] = double( m_data.frectValue->x1 );
      v[3] = double( m_data.frectValue->y1 );
      return v;
   }
   case VariantType::DRect:
   {
      DVector v( 4 );
      v[0] = m_data.drectValue->x0;
      v[1] = m_data.drectValue->y0;
      v[2] = m_data.drectValue->x1;
      v[3] = m_data.drectValue->y1;
      return v;
   }

   case VariantType::CharVector: return DVector( m_data.charVectorValue->Begin(), m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return DVector( m_data.byteVectorValue->Begin(), m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return DVector( m_data.iVectorValue->Begin(), m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return DVector( m_data.uiVectorValue->Begin(), m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return DVector( m_data.i64VectorValue->Begin(), m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return DVector( m_data.ui64VectorValue->Begin(), m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return DVector( m_data.fVectorValue->Begin(), m_data.fVectorValue->Length() );
   case VariantType::DVector:    return *m_data.dVectorValue;

   case VariantType::CharMatrix: return DVector( m_data.charMatrixValue->Begin(), int( m_data.charMatrixValue->NumberOfElements() ) );
   case VariantType::ByteMatrix: return DVector( m_data.byteMatrixValue->Begin(), int( m_data.byteMatrixValue->NumberOfElements() ) );
   case VariantType::IMatrix:    return DVector( m_data.iMatrixValue->Begin(), int( m_data.iMatrixValue->NumberOfElements() ) );
   case VariantType::UIMatrix:   return DVector( m_data.uiMatrixValue->Begin(), int( m_data.uiMatrixValue->NumberOfElements() ) );
   case VariantType::I64Matrix:  return DVector( m_data.i64MatrixValue->Begin(), int( m_data.i64MatrixValue->NumberOfElements() ) );
   case VariantType::UI64Matrix: return DVector( m_data.ui64MatrixValue->Begin(), int( m_data.ui64MatrixValue->NumberOfElements() ) );
   case VariantType::FMatrix:    return DVector( m_data.fMatrixValue->Begin(), int( m_data.fMatrixValue->NumberOfElements() ) );
   case VariantType::DMatrix:    return DVector( m_data.dMatrixValue->Begin(), int( m_data.dMatrixValue->NumberOfElements() ) );

   case VariantType::ByteArray: return DVector( m_data.byteArrayValue->Begin(), int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:    return DVector( m_data.stringValue->c_str(), int( m_data.stringValue->Length() ) );
   case VariantType::IsoString: return DVector( m_data.isoStringValue->c_str(), int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToDVector(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

CharMatrix Variant::ToCharMatrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return CharMatrix( int8( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:      return CharMatrix( m_data.int8Value, 1, 1 );
   case VariantType::Int16:     return CharMatrix( int8( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:     return CharMatrix( int8( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:     return CharMatrix( int8( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:     return CharMatrix( int8( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:    return CharMatrix( int8( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:    return CharMatrix( int8( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:    return CharMatrix( int8( m_data.uint64Value ), 1, 1 );
   case VariantType::Float:     return CharMatrix( int8( m_data.floatValue ), 1, 1 );
   case VariantType::Double:    return CharMatrix( int8( m_data.doubleValue ), 1, 1 );

   case VariantType::FComplex:
   {
      CharMatrix m( 1, 2 );
      m[0][0] = int8( m_data.fcomplexValue->Real() );
      m[0][1] = int8( m_data.fcomplexValue->Imag() );
      return m;
   }
   case VariantType::DComplex:
   {
      CharMatrix m( 1, 2 );
      m[0][0] = int8( m_data.dcomplexValue->Real() );
      m[0][1] = int8( m_data.dcomplexValue->Imag() );
      return m;
   }

   case VariantType::Point:
   {
      CharMatrix m( 1, 2 );
      m[0][0] = int8( m_data.pointValue->x );
      m[0][1] = int8( m_data.pointValue->y );
      return m;
   }
   case VariantType::FPoint:
   {
      CharMatrix m( 1, 2 );
      m[0][0] = int8( m_data.fpointValue->x );
      m[0][1] = int8( m_data.fpointValue->y );
      return m;
   }
   case VariantType::DPoint:
   {
      CharMatrix m( 1, 2 );
      m[0][0] = int8( m_data.dpointValue->x );
      m[0][1] = int8( m_data.dpointValue->y );
      return m;
   }

   case VariantType::Rect:
   {
      CharMatrix m( 2, 2 );
      m[0][0] = int8( m_data.rectValue->x0 );
      m[0][1] = int8( m_data.rectValue->y0 );
      m[1][0] = int8( m_data.rectValue->x1 );
      m[1][1] = int8( m_data.rectValue->y1 );
      return m;
   }
   case VariantType::FRect:
   {
      CharMatrix m( 2, 2 );
      m[0][0] = int8( m_data.frectValue->x0 );
      m[0][1] = int8( m_data.frectValue->y0 );
      m[1][0] = int8( m_data.frectValue->x1 );
      m[1][1] = int8( m_data.frectValue->y1 );
      return m;
   }
   case VariantType::DRect:
   {
      CharMatrix m( 2, 2 );
      m[0][0] = int8( m_data.drectValue->x0 );
      m[0][1] = int8( m_data.drectValue->y0 );
      m[1][0] = int8( m_data.drectValue->x1 );
      m[1][1] = int8( m_data.drectValue->y1 );
      return m;
   }

   case VariantType::CharVector: return CharMatrix( m_data.charVectorValue->Begin(), 1, m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return CharMatrix( m_data.byteVectorValue->Begin(), 1, m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return CharMatrix( m_data.iVectorValue->Begin(), 1, m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return CharMatrix( m_data.uiVectorValue->Begin(), 1, m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return CharMatrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return CharMatrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return CharMatrix( m_data.fVectorValue->Begin(), 1, m_data.fVectorValue->Length() );
   case VariantType::DVector:    return CharMatrix( m_data.dVectorValue->Begin(), 1, m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return *m_data.charMatrixValue;
   case VariantType::ByteMatrix: return CharMatrix( m_data.byteMatrixValue->Begin(), m_data.byteMatrixValue->Rows(), m_data.byteMatrixValue->Cols() );
   case VariantType::IMatrix:    return CharMatrix( m_data.iMatrixValue->Begin(), m_data.iMatrixValue->Rows(), m_data.iMatrixValue->Cols() );
   case VariantType::UIMatrix:   return CharMatrix( m_data.uiMatrixValue->Begin(), m_data.uiMatrixValue->Rows(), m_data.uiMatrixValue->Cols() );
   case VariantType::I64Matrix:  return CharMatrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix: return CharMatrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::FMatrix:    return CharMatrix( m_data.fMatrixValue->Begin(), m_data.fMatrixValue->Rows(), m_data.fMatrixValue->Cols() );
   case VariantType::DMatrix:    return CharMatrix( m_data.dMatrixValue->Begin(), m_data.dMatrixValue->Rows(), m_data.dMatrixValue->Cols() );

   case VariantType::ByteArray: return CharMatrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:    return CharMatrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString: return CharMatrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToCharMatrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

ByteMatrix Variant::ToByteMatrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return ByteMatrix( uint8( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:      return ByteMatrix( uint8( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:     return ByteMatrix( uint8( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:     return ByteMatrix( uint8( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:     return ByteMatrix( uint8( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:     return ByteMatrix( m_data.uint8Value, 1, 1 );
   case VariantType::UInt16:    return ByteMatrix( uint8( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:    return ByteMatrix( uint8( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:    return ByteMatrix( uint8( m_data.uint64Value ), 1, 1 );
   case VariantType::Float:     return ByteMatrix( uint8( m_data.floatValue ), 1, 1 );
   case VariantType::Double:    return ByteMatrix( uint8( m_data.doubleValue ), 1, 1 );

   case VariantType::FComplex:
   {
      ByteMatrix m( 1, 2 );
      m[0][0] = uint8( m_data.fcomplexValue->Real() );
      m[0][1] = uint8( m_data.fcomplexValue->Imag() );
      return m;
   }
   case VariantType::DComplex:
   {
      ByteMatrix m( 1, 2 );
      m[0][0] = uint8( m_data.dcomplexValue->Real() );
      m[0][1] = uint8( m_data.dcomplexValue->Imag() );
      return m;
   }

   case VariantType::Point:
   {
      ByteMatrix m( 1, 2 );
      m[0][0] = uint8( m_data.pointValue->x );
      m[0][1] = uint8( m_data.pointValue->y );
      return m;
   }
   case VariantType::FPoint:
   {
      ByteMatrix m( 1, 2 );
      m[0][0] = uint8( m_data.fpointValue->x );
      m[0][1] = uint8( m_data.fpointValue->y );
      return m;
   }
   case VariantType::DPoint:
   {
      ByteMatrix m( 1, 2 );
      m[0][0] = uint8( m_data.dpointValue->x );
      m[0][1] = uint8( m_data.dpointValue->y );
      return m;
   }

   case VariantType::Rect:
   {
      ByteMatrix m( 2, 2 );
      m[0][0] = uint8( m_data.rectValue->x0 );
      m[0][1] = uint8( m_data.rectValue->y0 );
      m[1][0] = uint8( m_data.rectValue->x1 );
      m[1][1] = uint8( m_data.rectValue->y1 );
      return m;
   }
   case VariantType::FRect:
   {
      ByteMatrix m( 2, 2 );
      m[0][0] = uint8( m_data.frectValue->x0 );
      m[0][1] = uint8( m_data.frectValue->y0 );
      m[1][0] = uint8( m_data.frectValue->x1 );
      m[1][1] = uint8( m_data.frectValue->y1 );
      return m;
   }
   case VariantType::DRect:
   {
      ByteMatrix m( 2, 2 );
      m[0][0] = uint8( m_data.drectValue->x0 );
      m[0][1] = uint8( m_data.drectValue->y0 );
      m[1][0] = uint8( m_data.drectValue->x1 );
      m[1][1] = uint8( m_data.drectValue->y1 );
      return m;
   }

   case VariantType::CharVector: return ByteMatrix( m_data.charVectorValue->Begin(), 1, m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return ByteMatrix( m_data.byteVectorValue->Begin(), 1, m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return ByteMatrix( m_data.iVectorValue->Begin(), 1, m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return ByteMatrix( m_data.uiVectorValue->Begin(), 1, m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return ByteMatrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return ByteMatrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return ByteMatrix( m_data.fVectorValue->Begin(), 1, m_data.fVectorValue->Length() );
   case VariantType::DVector:    return ByteMatrix( m_data.dVectorValue->Begin(), 1, m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return ByteMatrix( m_data.charMatrixValue->Begin(), m_data.charMatrixValue->Rows(), m_data.charMatrixValue->Cols() );
   case VariantType::ByteMatrix: return *m_data.byteMatrixValue;
   case VariantType::IMatrix:    return ByteMatrix( m_data.iMatrixValue->Begin(), m_data.iMatrixValue->Rows(), m_data.iMatrixValue->Cols() );
   case VariantType::UIMatrix:   return ByteMatrix( m_data.uiMatrixValue->Begin(), m_data.uiMatrixValue->Rows(), m_data.uiMatrixValue->Cols() );
   case VariantType::I64Matrix:  return ByteMatrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix: return ByteMatrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::FMatrix:    return ByteMatrix( m_data.fMatrixValue->Begin(), m_data.fMatrixValue->Rows(), m_data.fMatrixValue->Cols() );
   case VariantType::DMatrix:    return ByteMatrix( m_data.dMatrixValue->Begin(), m_data.dMatrixValue->Rows(), m_data.dMatrixValue->Cols() );

   case VariantType::ByteArray: return ByteMatrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:    return ByteMatrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString: return ByteMatrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToByteMatrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

IMatrix Variant::ToIMatrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return IMatrix( int( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:      return IMatrix( int( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:     return IMatrix( int( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:     return IMatrix( m_data.int32Value, 1, 1 );
   case VariantType::Int64:     return IMatrix( int( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:     return IMatrix( int( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:    return IMatrix( int( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:    return IMatrix( int( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:    return IMatrix( int( m_data.uint64Value ), 1, 1 );
   case VariantType::Float:     return IMatrix( int( m_data.floatValue ), 1, 1 );
   case VariantType::Double:    return IMatrix( int( m_data.doubleValue ), 1, 1 );

   case VariantType::FComplex:
   {
      IMatrix m( 1, 2 );
      m[0][0] = int( m_data.fcomplexValue->Real() );
      m[0][1] = int( m_data.fcomplexValue->Imag() );
      return m;
   }
   case VariantType::DComplex:
   {
      IMatrix m( 1, 2 );
      m[0][0] = int( m_data.dcomplexValue->Real() );
      m[0][1] = int( m_data.dcomplexValue->Imag() );
      return m;
   }

   case VariantType::Point:
   {
      IMatrix m( 1, 2 );
      m[0][0] = m_data.pointValue->x;
      m[0][1] = m_data.pointValue->y;
      return m;
   }
   case VariantType::FPoint:
   {
      IMatrix m( 1, 2 );
      m[0][0] = int( m_data.fpointValue->x );
      m[0][1] = int( m_data.fpointValue->y );
      return m;
   }
   case VariantType::DPoint:
   {
      IMatrix m( 1, 2 );
      m[0][0] = int( m_data.dpointValue->x );
      m[0][1] = int( m_data.dpointValue->y );
      return m;
   }

   case VariantType::Rect:
   {
      IMatrix m( 2, 2 );
      m[0][0] = m_data.rectValue->x0;
      m[0][1] = m_data.rectValue->y0;
      m[1][0] = m_data.rectValue->x1;
      m[1][1] = m_data.rectValue->y1;
      return m;
   }
   case VariantType::FRect:
   {
      IMatrix m( 2, 2 );
      m[0][0] = int( m_data.frectValue->x0 );
      m[0][1] = int( m_data.frectValue->y0 );
      m[1][0] = int( m_data.frectValue->x1 );
      m[1][1] = int( m_data.frectValue->y1 );
      return m;
   }
   case VariantType::DRect:
   {
      IMatrix m( 2, 2 );
      m[0][0] = int( m_data.drectValue->x0 );
      m[0][1] = int( m_data.drectValue->y0 );
      m[1][0] = int( m_data.drectValue->x1 );
      m[1][1] = int( m_data.drectValue->y1 );
      return m;
   }

   case VariantType::CharVector: return IMatrix( m_data.charVectorValue->Begin(), 1, m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return IMatrix( m_data.byteVectorValue->Begin(), 1, m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return IMatrix( m_data.iVectorValue->Begin(), 1, m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return IMatrix( m_data.uiVectorValue->Begin(), 1, m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return IMatrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return IMatrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return IMatrix( m_data.fVectorValue->Begin(), 1, m_data.fVectorValue->Length() );
   case VariantType::DVector:    return IMatrix( m_data.dVectorValue->Begin(), 1, m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return IMatrix( m_data.charMatrixValue->Begin(), m_data.charMatrixValue->Rows(), m_data.charMatrixValue->Cols() );
   case VariantType::ByteMatrix: return IMatrix( m_data.byteMatrixValue->Begin(), m_data.byteMatrixValue->Rows(), m_data.byteMatrixValue->Cols() );
   case VariantType::IMatrix:    return *m_data.iMatrixValue;
   case VariantType::UIMatrix:   return IMatrix( m_data.uiMatrixValue->Begin(), m_data.uiMatrixValue->Rows(), m_data.uiMatrixValue->Cols() );
   case VariantType::I64Matrix:  return IMatrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix: return IMatrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::FMatrix:    return IMatrix( m_data.fMatrixValue->Begin(), m_data.fMatrixValue->Rows(), m_data.fMatrixValue->Cols() );
   case VariantType::DMatrix:    return IMatrix( m_data.dMatrixValue->Begin(), m_data.dMatrixValue->Rows(), m_data.dMatrixValue->Cols() );

   case VariantType::ByteArray: return IMatrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:    return IMatrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString: return IMatrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToIMatrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

UIMatrix Variant::ToUIMatrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return UIMatrix( unsigned( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:      return UIMatrix( unsigned( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:     return UIMatrix( unsigned( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:     return UIMatrix( unsigned( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:     return UIMatrix( unsigned( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:     return UIMatrix( unsigned( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:    return UIMatrix( unsigned( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:    return UIMatrix( m_data.uint32Value, 1, 1 );
   case VariantType::UInt64:    return UIMatrix( unsigned( m_data.uint64Value ), 1, 1 );
   case VariantType::Float:     return UIMatrix( unsigned( m_data.floatValue ), 1, 1 );
   case VariantType::Double:    return UIMatrix( unsigned( m_data.doubleValue ), 1, 1 );

   case VariantType::FComplex:
   {
      UIMatrix m( 1, 2 );
      m[0][0] = unsigned( m_data.fcomplexValue->Real() );
      m[0][1] = unsigned( m_data.fcomplexValue->Imag() );
      return m;
   }
   case VariantType::DComplex:
   {
      UIMatrix m( 1, 2 );
      m[0][0] = unsigned( m_data.dcomplexValue->Real() );
      m[0][1] = unsigned( m_data.dcomplexValue->Imag() );
      return m;
   }

   case VariantType::Point:
   {
      UIMatrix m( 1, 2 );
      m[0][0] = unsigned( m_data.pointValue->x );
      m[0][1] = unsigned( m_data.pointValue->y );
      return m;
   }
   case VariantType::FPoint:
   {
      UIMatrix m( 1, 2 );
      m[0][0] = unsigned( m_data.fpointValue->x );
      m[0][1] = unsigned( m_data.fpointValue->y );
      return m;
   }
   case VariantType::DPoint:
   {
      UIMatrix m( 1, 2 );
      m[0][0] = unsigned( m_data.dpointValue->x );
      m[0][1] = unsigned( m_data.dpointValue->y );
      return m;
   }

   case VariantType::Rect:
   {
      UIMatrix m( 2, 2 );
      m[0][0] = unsigned( m_data.rectValue->x0 );
      m[0][1] = unsigned( m_data.rectValue->y0 );
      m[1][0] = unsigned( m_data.rectValue->x1 );
      m[1][1] = unsigned( m_data.rectValue->y1 );
      return m;
   }
   case VariantType::FRect:
   {
      UIMatrix m( 2, 2 );
      m[0][0] = unsigned( m_data.frectValue->x0 );
      m[0][1] = unsigned( m_data.frectValue->y0 );
      m[1][0] = unsigned( m_data.frectValue->x1 );
      m[1][1] = unsigned( m_data.frectValue->y1 );
      return m;
   }
   case VariantType::DRect:
   {
      UIMatrix m( 2, 2 );
      m[0][0] = unsigned( m_data.drectValue->x0 );
      m[0][1] = unsigned( m_data.drectValue->y0 );
      m[1][0] = unsigned( m_data.drectValue->x1 );
      m[1][1] = unsigned( m_data.drectValue->y1 );
      return m;
   }

   case VariantType::CharVector: return UIMatrix( m_data.charVectorValue->Begin(), 1, m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return UIMatrix( m_data.byteVectorValue->Begin(), 1, m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return UIMatrix( m_data.iVectorValue->Begin(), 1, m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return UIMatrix( m_data.uiVectorValue->Begin(), 1, m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return UIMatrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return UIMatrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return UIMatrix( m_data.fVectorValue->Begin(), 1, m_data.fVectorValue->Length() );
   case VariantType::DVector:    return UIMatrix( m_data.dVectorValue->Begin(), 1, m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return UIMatrix( m_data.charMatrixValue->Begin(), m_data.charMatrixValue->Rows(), m_data.charMatrixValue->Cols() );
   case VariantType::ByteMatrix: return UIMatrix( m_data.byteMatrixValue->Begin(), m_data.byteMatrixValue->Rows(), m_data.byteMatrixValue->Cols() );
   case VariantType::IMatrix:    return UIMatrix( m_data.iMatrixValue->Begin(), m_data.iMatrixValue->Rows(), m_data.iMatrixValue->Cols() );
   case VariantType::UIMatrix:   return *m_data.uiMatrixValue;
   case VariantType::I64Matrix:  return UIMatrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix: return UIMatrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::FMatrix:    return UIMatrix( m_data.fMatrixValue->Begin(), m_data.fMatrixValue->Rows(), m_data.fMatrixValue->Cols() );
   case VariantType::DMatrix:    return UIMatrix( m_data.dMatrixValue->Begin(), m_data.dMatrixValue->Rows(), m_data.dMatrixValue->Cols() );

   case VariantType::ByteArray: return UIMatrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:    return UIMatrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString: return UIMatrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToUIMatrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

I64Matrix Variant::ToI64Matrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return I64Matrix( int64( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:      return I64Matrix( int64( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:     return I64Matrix( int64( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:     return I64Matrix( int64( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:     return I64Matrix( m_data.int64Value, 1, 1 );
   case VariantType::UInt8:     return I64Matrix( int64( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:    return I64Matrix( int64( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:    return I64Matrix( int64( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:    return I64Matrix( int64( m_data.uint64Value ), 1, 1 );
   case VariantType::Float:     return I64Matrix( int64( m_data.floatValue ), 1, 1 );
   case VariantType::Double:    return I64Matrix( int64( m_data.doubleValue ), 1, 1 );

   case VariantType::FComplex:
   {
      I64Matrix m( 1, 2 );
      m[0][0] = int64( m_data.fcomplexValue->Real() );
      m[0][1] = int64( m_data.fcomplexValue->Imag() );
      return m;
   }
   case VariantType::DComplex:
   {
      I64Matrix m( 1, 2 );
      m[0][0] = int64( m_data.dcomplexValue->Real() );
      m[0][1] = int64( m_data.dcomplexValue->Imag() );
      return m;
   }

   case VariantType::Point:
   {
      I64Matrix m( 1, 2 );
      m[0][0] = int64( m_data.pointValue->x );
      m[0][1] = int64( m_data.pointValue->y );
      return m;
   }
   case VariantType::FPoint:
   {
      I64Matrix m( 1, 2 );
      m[0][0] = int64( m_data.fpointValue->x );
      m[0][1] = int64( m_data.fpointValue->y );
      return m;
   }
   case VariantType::DPoint:
   {
      I64Matrix m( 1, 2 );
      m[0][0] = int64( m_data.dpointValue->x );
      m[0][1] = int64( m_data.dpointValue->y );
      return m;
   }

   case VariantType::Rect:
   {
      I64Matrix m( 2, 2 );
      m[0][0] = int64( m_data.rectValue->x0 );
      m[0][1] = int64( m_data.rectValue->y0 );
      m[1][0] = int64( m_data.rectValue->x1 );
      m[1][1] = int64( m_data.rectValue->y1 );
      return m;
   }
   case VariantType::FRect:
   {
      I64Matrix m( 2, 2 );
      m[0][0] = int64( m_data.frectValue->x0 );
      m[0][1] = int64( m_data.frectValue->y0 );
      m[1][0] = int64( m_data.frectValue->x1 );
      m[1][1] = int64( m_data.frectValue->y1 );
      return m;
   }
   case VariantType::DRect:
   {
      I64Matrix m( 2, 2 );
      m[0][0] = int64( m_data.drectValue->x0 );
      m[0][1] = int64( m_data.drectValue->y0 );
      m[1][0] = int64( m_data.drectValue->x1 );
      m[1][1] = int64( m_data.drectValue->y1 );
      return m;
   }

   case VariantType::CharVector: return I64Matrix( m_data.charVectorValue->Begin(), 1, m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return I64Matrix( m_data.byteVectorValue->Begin(), 1, m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return I64Matrix( m_data.iVectorValue->Begin(), 1, m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return I64Matrix( m_data.uiVectorValue->Begin(), 1, m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return I64Matrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return I64Matrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return I64Matrix( m_data.fVectorValue->Begin(), 1, m_data.fVectorValue->Length() );
   case VariantType::DVector:    return I64Matrix( m_data.dVectorValue->Begin(), 1, m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return I64Matrix( m_data.charMatrixValue->Begin(), m_data.charMatrixValue->Rows(), m_data.charMatrixValue->Cols() );
   case VariantType::ByteMatrix: return I64Matrix( m_data.byteMatrixValue->Begin(), m_data.byteMatrixValue->Rows(), m_data.byteMatrixValue->Cols() );
   case VariantType::IMatrix:    return I64Matrix( m_data.iMatrixValue->Begin(), m_data.iMatrixValue->Rows(), m_data.iMatrixValue->Cols() );
   case VariantType::UIMatrix:   return I64Matrix( m_data.uiMatrixValue->Begin(), m_data.uiMatrixValue->Rows(), m_data.uiMatrixValue->Cols() );
   case VariantType::I64Matrix:  return *m_data.i64MatrixValue;
   case VariantType::UI64Matrix: return I64Matrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::FMatrix:    return I64Matrix( m_data.fMatrixValue->Begin(), m_data.fMatrixValue->Rows(), m_data.fMatrixValue->Cols() );
   case VariantType::DMatrix:    return I64Matrix( m_data.dMatrixValue->Begin(), m_data.dMatrixValue->Rows(), m_data.dMatrixValue->Cols() );

   case VariantType::ByteArray: return I64Matrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:    return I64Matrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString: return I64Matrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToI64Matrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

UI64Matrix Variant::ToUI64Matrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return UI64Matrix( uint64( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:      return UI64Matrix( uint64( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:     return UI64Matrix( uint64( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:     return UI64Matrix( uint64( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:     return UI64Matrix( uint64( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:     return UI64Matrix( uint64( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:    return UI64Matrix( uint64( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:    return UI64Matrix( uint64( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:    return UI64Matrix( m_data.uint64Value, 1, 1 );
   case VariantType::Float:     return UI64Matrix( uint64( m_data.floatValue ), 1, 1 );
   case VariantType::Double:    return UI64Matrix( uint64( m_data.doubleValue ), 1, 1 );

   case VariantType::FComplex:
   {
      UI64Matrix m( 1, 2 );
      m[0][0] = uint64( m_data.fcomplexValue->Real() );
      m[0][1] = uint64( m_data.fcomplexValue->Imag() );
      return m;
   }
   case VariantType::DComplex:
   {
      UI64Matrix m( 1, 2 );
      m[0][0] = uint64( m_data.dcomplexValue->Real() );
      m[0][1] = uint64( m_data.dcomplexValue->Imag() );
      return m;
   }

   case VariantType::Point:
   {
      UI64Matrix m( 1, 2 );
      m[0][0] = uint64( m_data.pointValue->x );
      m[0][1] = uint64( m_data.pointValue->y );
      return m;
   }
   case VariantType::FPoint:
   {
      UI64Matrix m( 1, 2 );
      m[0][0] = uint64( m_data.fpointValue->x );
      m[0][1] = uint64( m_data.fpointValue->y );
      return m;
   }
   case VariantType::DPoint:
   {
      UI64Matrix m( 1, 2 );
      m[0][0] = uint64( m_data.dpointValue->x );
      m[0][1] = uint64( m_data.dpointValue->y );
      return m;
   }

   case VariantType::Rect:
   {
      UI64Matrix m( 2, 2 );
      m[0][0] = uint64( m_data.rectValue->x0 );
      m[0][1] = uint64( m_data.rectValue->y0 );
      m[1][0] = uint64( m_data.rectValue->x1 );
      m[1][1] = uint64( m_data.rectValue->y1 );
      return m;
   }
   case VariantType::FRect:
   {
      UI64Matrix m( 2, 2 );
      m[0][0] = uint64( m_data.frectValue->x0 );
      m[0][1] = uint64( m_data.frectValue->y0 );
      m[1][0] = uint64( m_data.frectValue->x1 );
      m[1][1] = uint64( m_data.frectValue->y1 );
      return m;
   }
   case VariantType::DRect:
   {
      UI64Matrix m( 2, 2 );
      m[0][0] = uint64( m_data.drectValue->x0 );
      m[0][1] = uint64( m_data.drectValue->y0 );
      m[1][0] = uint64( m_data.drectValue->x1 );
      m[1][1] = uint64( m_data.drectValue->y1 );
      return m;
   }

   case VariantType::CharVector: return UI64Matrix( m_data.charVectorValue->Begin(), 1, m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return UI64Matrix( m_data.byteVectorValue->Begin(), 1, m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return UI64Matrix( m_data.iVectorValue->Begin(), 1, m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return UI64Matrix( m_data.uiVectorValue->Begin(), 1, m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return UI64Matrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return UI64Matrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return UI64Matrix( m_data.fVectorValue->Begin(), 1, m_data.fVectorValue->Length() );
   case VariantType::DVector:    return UI64Matrix( m_data.dVectorValue->Begin(), 1, m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return UI64Matrix( m_data.charMatrixValue->Begin(), m_data.charMatrixValue->Rows(), m_data.charMatrixValue->Cols() );
   case VariantType::ByteMatrix: return UI64Matrix( m_data.byteMatrixValue->Begin(), m_data.byteMatrixValue->Rows(), m_data.byteMatrixValue->Cols() );
   case VariantType::IMatrix:    return UI64Matrix( m_data.iMatrixValue->Begin(), m_data.iMatrixValue->Rows(), m_data.iMatrixValue->Cols() );
   case VariantType::UIMatrix:   return UI64Matrix( m_data.uiMatrixValue->Begin(), m_data.uiMatrixValue->Rows(), m_data.uiMatrixValue->Cols() );
   case VariantType::I64Matrix:  return UI64Matrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix: return *m_data.ui64MatrixValue;
   case VariantType::FMatrix:    return UI64Matrix( m_data.fMatrixValue->Begin(), m_data.fMatrixValue->Rows(), m_data.fMatrixValue->Cols() );
   case VariantType::DMatrix:    return UI64Matrix( m_data.dMatrixValue->Begin(), m_data.dMatrixValue->Rows(), m_data.dMatrixValue->Cols() );

   case VariantType::ByteArray: return UI64Matrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:    return UI64Matrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString: return UI64Matrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToUI64Matrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

FMatrix Variant::ToFMatrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return FMatrix( float( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:      return FMatrix( float( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:     return FMatrix( float( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:     return FMatrix( float( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:     return FMatrix( float( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:     return FMatrix( float( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:    return FMatrix( float( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:    return FMatrix( float( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:    return FMatrix( float( m_data.uint64Value ), 1, 1 );
   case VariantType::Float:     return FMatrix( m_data.floatValue, 1, 1 );
   case VariantType::Double:    return FMatrix( float( m_data.doubleValue ), 1, 1 );

   case VariantType::FComplex:
   {
      FMatrix m( 1, 2 );
      m[0][0] = m_data.fcomplexValue->Real();
      m[0][1] = m_data.fcomplexValue->Imag();
      return m;
   }
   case VariantType::DComplex:
   {
      FMatrix m( 1, 2 );
      m[0][0] = float( m_data.dcomplexValue->Real() );
      m[0][1] = float( m_data.dcomplexValue->Imag() );
      return m;
   }

   case VariantType::Point:
   {
      FMatrix m( 1, 2 );
      m[0][0] = float( m_data.pointValue->x );
      m[0][1] = float( m_data.pointValue->y );
      return m;
   }
   case VariantType::FPoint:
   {
      FMatrix m( 1, 2 );
      m[0][0] = m_data.fpointValue->x;
      m[0][1] = m_data.fpointValue->y;
      return m;
   }
   case VariantType::DPoint:
   {
      FMatrix m( 1, 2 );
      m[0][0] = float( m_data.dpointValue->x );
      m[0][1] = float( m_data.dpointValue->y );
      return m;
   }

   case VariantType::Rect:
   {
      FMatrix m( 2, 2 );
      m[0][0] = float( m_data.rectValue->x0 );
      m[0][1] = float( m_data.rectValue->y0 );
      m[1][0] = float( m_data.rectValue->x1 );
      m[1][1] = float( m_data.rectValue->y1 );
      return m;
   }
   case VariantType::FRect:
   {
      FMatrix m( 2, 2 );
      m[0][0] = m_data.frectValue->x0;
      m[0][1] = m_data.frectValue->y0;
      m[1][0] = m_data.frectValue->x1;
      m[1][1] = m_data.frectValue->y1;
      return m;
   }
   case VariantType::DRect:
   {
      FMatrix m( 2, 2 );
      m[0][0] = float( m_data.drectValue->x0 );
      m[0][1] = float( m_data.drectValue->y0 );
      m[1][0] = float( m_data.drectValue->x1 );
      m[1][1] = float( m_data.drectValue->y1 );
      return m;
   }

   case VariantType::CharVector: return FMatrix( m_data.charVectorValue->Begin(), 1, m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return FMatrix( m_data.byteVectorValue->Begin(), 1, m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return FMatrix( m_data.iVectorValue->Begin(), 1, m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return FMatrix( m_data.uiVectorValue->Begin(), 1, m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return FMatrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return FMatrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return FMatrix( m_data.fVectorValue->Begin(), 1, m_data.fVectorValue->Length() );
   case VariantType::DVector:    return FMatrix( m_data.dVectorValue->Begin(), 1, m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return FMatrix( m_data.charMatrixValue->Begin(), m_data.charMatrixValue->Rows(), m_data.charMatrixValue->Cols() );
   case VariantType::ByteMatrix: return FMatrix( m_data.byteMatrixValue->Begin(), m_data.byteMatrixValue->Rows(), m_data.byteMatrixValue->Cols() );
   case VariantType::IMatrix:    return FMatrix( m_data.iMatrixValue->Begin(), m_data.iMatrixValue->Rows(), m_data.iMatrixValue->Cols() );
   case VariantType::UIMatrix:   return FMatrix( m_data.uiMatrixValue->Begin(), m_data.uiMatrixValue->Rows(), m_data.uiMatrixValue->Cols() );
   case VariantType::I64Matrix:  return FMatrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix: return FMatrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::FMatrix:    return *m_data.fMatrixValue;
   case VariantType::DMatrix:    return FMatrix( m_data.dMatrixValue->Begin(), m_data.dMatrixValue->Rows(), m_data.dMatrixValue->Cols() );

   case VariantType::ByteArray: return FMatrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:    return FMatrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString: return FMatrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToFMatrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
   }
}

DMatrix Variant::ToDMatrix() const
{
   switch ( m_type )
   {
   case VariantType::Bool:      return DMatrix( double( m_data.boolValue ), 1, 1 );
   case VariantType::Int8:      return DMatrix( double( m_data.int8Value ), 1, 1 );
   case VariantType::Int16:     return DMatrix( double( m_data.int16Value ), 1, 1 );
   case VariantType::Int32:     return DMatrix( double( m_data.int32Value ), 1, 1 );
   case VariantType::Int64:     return DMatrix( double( m_data.int64Value ), 1, 1 );
   case VariantType::UInt8:     return DMatrix( double( m_data.uint8Value ), 1, 1 );
   case VariantType::UInt16:    return DMatrix( double( m_data.uint16Value ), 1, 1 );
   case VariantType::UInt32:    return DMatrix( double( m_data.uint32Value ), 1, 1 );
   case VariantType::UInt64:    return DMatrix( double( m_data.uint64Value ), 1, 1 );
   case VariantType::Float:     return DMatrix( double( m_data.floatValue ), 1, 1 );
   case VariantType::Double:    return DMatrix( m_data.doubleValue, 1, 1 );

   case VariantType::FComplex:
   {
      DMatrix m( 1, 2 );
      m[0][0] = double( m_data.fcomplexValue->Real() );
      m[0][1] = double( m_data.fcomplexValue->Imag() );
      return m;
   }
   case VariantType::DComplex:
   {
      DMatrix m( 1, 2 );
      m[0][0] = m_data.dcomplexValue->Real();
      m[0][1] = m_data.dcomplexValue->Imag();
      return m;
   }

   case VariantType::Point:
   {
      DMatrix m( 1, 2 );
      m[0][0] = double( m_data.pointValue->x );
      m[0][1] = double( m_data.pointValue->y );
      return m;
   }
   case VariantType::FPoint:
   {
      DMatrix m( 1, 2 );
      m[0][0] = double( m_data.fpointValue->x );
      m[0][1] = double( m_data.fpointValue->y );
      return m;
   }
   case VariantType::DPoint:
   {
      DMatrix m( 1, 2 );
      m[0][0] = m_data.dpointValue->x;
      m[0][1] = m_data.dpointValue->y;
      return m;
   }

   case VariantType::Rect:
   {
      DMatrix m( 2, 2 );
      m[0][0] = double( m_data.rectValue->x0 );
      m[0][1] = double( m_data.rectValue->y0 );
      m[1][0] = double( m_data.rectValue->x1 );
      m[1][1] = double( m_data.rectValue->y1 );
      return m;
   }
   case VariantType::FRect:
   {
      DMatrix m( 2, 2 );
      m[0][0] = double( m_data.frectValue->x0 );
      m[0][1] = double( m_data.frectValue->y0 );
      m[1][0] = double( m_data.frectValue->x1 );
      m[1][1] = double( m_data.frectValue->y1 );
      return m;
   }
   case VariantType::DRect:
   {
      DMatrix m( 2, 2 );
      m[0][0] = m_data.drectValue->x0;
      m[0][1] = m_data.drectValue->y0;
      m[1][0] = m_data.drectValue->x1;
      m[1][1] = m_data.drectValue->y1;
      return m;
   }

   case VariantType::CharVector: return DMatrix( m_data.charVectorValue->Begin(), 1, m_data.charVectorValue->Length() );
   case VariantType::ByteVector: return DMatrix( m_data.byteVectorValue->Begin(), 1, m_data.byteVectorValue->Length() );
   case VariantType::IVector:    return DMatrix( m_data.iVectorValue->Begin(), 1, m_data.iVectorValue->Length() );
   case VariantType::UIVector:   return DMatrix( m_data.uiVectorValue->Begin(), 1, m_data.uiVectorValue->Length() );
   case VariantType::I64Vector:  return DMatrix( m_data.i64VectorValue->Begin(), 1, m_data.i64VectorValue->Length() );
   case VariantType::UI64Vector: return DMatrix( m_data.ui64VectorValue->Begin(), 1, m_data.ui64VectorValue->Length() );
   case VariantType::FVector:    return DMatrix( m_data.fVectorValue->Begin(), 1, m_data.fVectorValue->Length() );
   case VariantType::DVector:    return DMatrix( m_data.dVectorValue->Begin(), 1, m_data.dVectorValue->Length() );

   case VariantType::CharMatrix: return DMatrix( m_data.charMatrixValue->Begin(), m_data.charMatrixValue->Rows(), m_data.charMatrixValue->Cols() );
   case VariantType::ByteMatrix: return DMatrix( m_data.byteMatrixValue->Begin(), m_data.byteMatrixValue->Rows(), m_data.byteMatrixValue->Cols() );
   case VariantType::IMatrix:    return DMatrix( m_data.iMatrixValue->Begin(), m_data.iMatrixValue->Rows(), m_data.iMatrixValue->Cols() );
   case VariantType::UIMatrix:   return DMatrix( m_data.uiMatrixValue->Begin(), m_data.uiMatrixValue->Rows(), m_data.uiMatrixValue->Cols() );
   case VariantType::I64Matrix:  return DMatrix( m_data.i64MatrixValue->Begin(), m_data.i64MatrixValue->Rows(), m_data.i64MatrixValue->Cols() );
   case VariantType::UI64Matrix: return DMatrix( m_data.ui64MatrixValue->Begin(), m_data.ui64MatrixValue->Rows(), m_data.ui64MatrixValue->Cols() );
   case VariantType::FMatrix:    return DMatrix( m_data.fMatrixValue->Begin(), m_data.fMatrixValue->Rows(), m_data.fMatrixValue->Cols() );
   case VariantType::DMatrix:    return *m_data.dMatrixValue;

   case VariantType::ByteArray: return DMatrix( m_data.byteArrayValue->Begin(), 1, int( m_data.byteArrayValue->Length() ) );

   case VariantType::String:    return DMatrix( m_data.stringValue->c_str(), 1, int( m_data.stringValue->Length() ) );
   case VariantType::IsoString: return DMatrix( m_data.isoStringValue->c_str(), 1, int( m_data.isoStringValue->Length() ) );
   default:
      throw Error( "Variant::ToDMatrix(): Invalid conversion from " + String( TypeAsString( m_type ) ) + " type" );
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
      return ByteArray( 1, uint8( m_data.int8Value ) );
   case VariantType::Int16:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.int16Value );
         return ByteArray( v, v+2 );
      }
   case VariantType::Int32:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.int32Value );
         return ByteArray( v, v+4 );
      }
   case VariantType::Int64:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.int64Value );
         return ByteArray( v, v+8 );
      }
   case VariantType::UInt8:
      return ByteArray( 1, m_data.uint8Value );
   case VariantType::UInt16:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.uint16Value );
         return ByteArray( v, v+2 );
      }
   case VariantType::UInt32:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.uint32Value );
         return ByteArray( v, v+4 );
      }
   case VariantType::UInt64:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.uint64Value );
         return ByteArray( v, v+8 );
      }
   case VariantType::Float:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.floatValue );
         return ByteArray( v, v + sizeof( float ) );
      }
   case VariantType::Double:
      {
         const uint8* v = reinterpret_cast<const uint8*>( &m_data.doubleValue );
         return ByteArray( v, v + sizeof( double ) );
      }
   case VariantType::FComplex:
      {
         float r = m_data.fcomplexValue->Real();
         float i = m_data.fcomplexValue->Imag();
         const uint8* v1 = reinterpret_cast<const uint8*>( &r );
         const uint8* v2 = reinterpret_cast<const uint8*>( &i );
         ByteArray a( 2*sizeof( float ) );
         ::memcpy( a.Begin(),                   v1, sizeof( float ) );
         ::memcpy( a.Begin() + sizeof( float ), v2, sizeof( float ) );
         return a;
      }
   case VariantType::DComplex:
      {
         float r = m_data.dcomplexValue->Real();
         float i = m_data.dcomplexValue->Imag();
         const uint8* v1 = reinterpret_cast<const uint8*>( &r );
         const uint8* v2 = reinterpret_cast<const uint8*>( &i );
         ByteArray a( 2*sizeof( double ) );
         ::memcpy( a.Begin(),                    v1, sizeof( double ) );
         ::memcpy( a.Begin() + sizeof( double ), v2, sizeof( double ) );
         return a;
      }
   case VariantType::Point:
      {
         const uint8* v1 = reinterpret_cast<const uint8*>( &m_data.pointValue->x );
         const uint8* v2 = reinterpret_cast<const uint8*>( &m_data.pointValue->y );
         ByteArray a( 2*sizeof( int ) );
         ::memcpy( a.Begin(),                 v1, sizeof( int ) );
         ::memcpy( a.Begin() + sizeof( int ), v2, sizeof( int ) );
         return a;
      }
   case VariantType::FPoint:
      {
         const uint8* v1 = reinterpret_cast<const uint8*>( &m_data.fpointValue->x );
         const uint8* v2 = reinterpret_cast<const uint8*>( &m_data.fpointValue->y );
         ByteArray a( 2*sizeof( float ) );
         ::memcpy( a.Begin(),                   v1, sizeof( float ) );
         ::memcpy( a.Begin() + sizeof( float ), v2, sizeof( float ) );
         return a;
      }
   case VariantType::DPoint:
      {
         const uint8* v1 = reinterpret_cast<const uint8*>( &m_data.dpointValue->x );
         const uint8* v2 = reinterpret_cast<const uint8*>( &m_data.dpointValue->y );
         ByteArray a( 2*sizeof( double ) );
         ::memcpy( a.Begin(),                    v1, sizeof( double ) );
         ::memcpy( a.Begin() + sizeof( double ), v2, sizeof( double ) );
         return a;
      }
   case VariantType::Rect:
      {
         const uint8* v1 = reinterpret_cast<const uint8*>( &m_data.rectValue->x0 );
         const uint8* v2 = reinterpret_cast<const uint8*>( &m_data.rectValue->y0 );
         const uint8* v3 = reinterpret_cast<const uint8*>( &m_data.rectValue->x1 );
         const uint8* v4 = reinterpret_cast<const uint8*>( &m_data.rectValue->y1 );
         ByteArray a( 4*sizeof( int ) );
         ::memcpy( a.Begin(),                   v1, sizeof( int ) );
         ::memcpy( a.Begin() +   sizeof( int ), v2, sizeof( int ) );
         ::memcpy( a.Begin() + 2*sizeof( int ), v3, sizeof( int ) );
         ::memcpy( a.Begin() + 3*sizeof( int ), v4, sizeof( int ) );
         return a;
      }
   case VariantType::FRect:
      {
         const uint8* v1 = reinterpret_cast<const uint8*>( &m_data.frectValue->x0 );
         const uint8* v2 = reinterpret_cast<const uint8*>( &m_data.frectValue->y0 );
         const uint8* v3 = reinterpret_cast<const uint8*>( &m_data.frectValue->x1 );
         const uint8* v4 = reinterpret_cast<const uint8*>( &m_data.frectValue->y1 );
         ByteArray a( 4*sizeof( float ) );
         ::memcpy( a.Begin(),                     v1, sizeof( float ) );
         ::memcpy( a.Begin() +   sizeof( float ), v2, sizeof( float ) );
         ::memcpy( a.Begin() + 2*sizeof( float ), v3, sizeof( float ) );
         ::memcpy( a.Begin() + 3*sizeof( float ), v4, sizeof( float ) );
         return a;
      }
   case VariantType::DRect:
      {
         const uint8* v1 = reinterpret_cast<const uint8*>( &m_data.drectValue->x0 );
         const uint8* v2 = reinterpret_cast<const uint8*>( &m_data.drectValue->y0 );
         const uint8* v3 = reinterpret_cast<const uint8*>( &m_data.drectValue->x1 );
         const uint8* v4 = reinterpret_cast<const uint8*>( &m_data.drectValue->y1 );
         ByteArray a( 4*sizeof( double ) );
         ::memcpy( a.Begin(),                      v1, sizeof( double ) );
         ::memcpy( a.Begin() +   sizeof( double ), v2, sizeof( double ) );
         ::memcpy( a.Begin() + 2*sizeof( double ), v3, sizeof( double ) );
         ::memcpy( a.Begin() + 3*sizeof( double ), v4, sizeof( double ) );
         return a;
      }

   case VariantType::CharVector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.charVectorValue->Begin() );
         return ByteArray( v, v + m_data.charVectorValue->Length()*sizeof( int8 ) );
      }
   case VariantType::ByteVector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.byteVectorValue->Begin() );
         return ByteArray( v, v + m_data.byteVectorValue->Length()*sizeof( uint8 ) );
      }
   case VariantType::IVector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.iVectorValue->Begin() );
         return ByteArray( v, v + m_data.iVectorValue->Length()*sizeof( int ) );
      }
   case VariantType::UIVector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.uiVectorValue->Begin() );
         return ByteArray( v, v + m_data.uiVectorValue->Length()*sizeof( unsigned ) );
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
   case VariantType::FVector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.fVectorValue->Begin() );
         return ByteArray( v, v + m_data.fVectorValue->Length()*sizeof( float ) );
      }
   case VariantType::DVector:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.dVectorValue->Begin() );
         return ByteArray( v, v + m_data.dVectorValue->Length()*sizeof( double ) );
      }
   case VariantType::CharMatrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.charMatrixValue->Begin() );
         return ByteArray( v, v + m_data.charMatrixValue->NumberOfElements()*sizeof( int8 ) );
      }
   case VariantType::ByteMatrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.byteMatrixValue->Begin() );
         return ByteArray( v, v + m_data.byteMatrixValue->NumberOfElements()*sizeof( uint8 ) );
      }
   case VariantType::IMatrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.iMatrixValue->Begin() );
         return ByteArray( v, v + m_data.iMatrixValue->NumberOfElements()*sizeof( int ) );
      }
   case VariantType::UIMatrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.uiMatrixValue->Begin() );
         return ByteArray( v, v + m_data.uiMatrixValue->NumberOfElements()*sizeof( unsigned ) );
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
   case VariantType::FMatrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.fMatrixValue->Begin() );
         return ByteArray( v, v + m_data.fMatrixValue->NumberOfElements()*sizeof( float ) );
      }
   case VariantType::DMatrix:
      {
         const uint8* v = reinterpret_cast<const uint8*>( m_data.dMatrixValue->Begin() );
         return ByteArray( v, v + m_data.dMatrixValue->NumberOfElements()*sizeof( double ) );
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
   case VariantType::Float:
      return String( m_data.floatValue );
   case VariantType::Double:
      return String( m_data.doubleValue );
   case VariantType::FComplex:
      return String().Format( "{%.7g,%.7g}", m_data.fcomplexValue->Real(), m_data.fcomplexValue->Imag() );
   case VariantType::DComplex:
      return String().Format( "{%.16g,%.16g}", m_data.dcomplexValue->Real(), m_data.dcomplexValue->Imag() );
   case VariantType::Point:
      return String().Format( "{%d,%d}", m_data.pointValue->x, m_data.pointValue->y );
   case VariantType::FPoint:
      return String().Format( "{%.7g,%.7g}", m_data.fpointValue->x, m_data.fpointValue->y );
   case VariantType::DPoint:
      return String().Format( "{%.16g,%.16g}", m_data.dpointValue->x, m_data.dpointValue->y );
   case VariantType::Rect:
      return String().Format( "{%d,%d,%d,%d}", m_data.rectValue->x0, m_data.rectValue->y0,
                                               m_data.rectValue->x1, m_data.rectValue->y1 );
   case VariantType::FRect:
      return String().Format( "{%d,%d,%d,%d}", m_data.frectValue->x0, m_data.frectValue->y0,
                                               m_data.frectValue->x1, m_data.frectValue->y1 );
   case VariantType::DRect:
      return String().Format( "{%d,%d,%d,%d}", m_data.drectValue->x0, m_data.drectValue->y0,
                                               m_data.drectValue->x1, m_data.drectValue->y1 );
   case VariantType::CharVector:
      {
         String s = '{';
         if ( !m_data.charVectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( int( (*m_data.charVectorValue)[i] ) ) );
               if ( ++i == m_data.charVectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::ByteVector:
      {
         String s = '{';
         if ( !m_data.byteVectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( unsigned( (*m_data.byteVectorValue)[i] ) ) );
               if ( ++i == m_data.byteVectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::IVector:
      {
         String s = '{';
         if ( !m_data.iVectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( (*m_data.iVectorValue)[i] ) );
               if ( ++i == m_data.iVectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UIVector:
      {
         String s = '{';
         if ( !m_data.uiVectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( (*m_data.uiVectorValue)[i] ) );
               if ( ++i == m_data.uiVectorValue->Length() )
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
   case VariantType::FVector:
      {
         String s = '{';
         if ( !m_data.fVectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( (*m_data.fVectorValue)[i] ) );
               if ( ++i == m_data.fVectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::DVector:
      {
         String s = '{';
         if ( !m_data.dVectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( String( (*m_data.dVectorValue)[i] ) );
               if ( ++i == m_data.dVectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::CharMatrix:
      {
         String s = '{';
         if ( !m_data.charMatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( int( (*m_data.charMatrixValue)[i][j] ) ) );
                  if ( ++j == m_data.charMatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.charMatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::ByteMatrix:
      {
         String s = '{';
         if ( !m_data.byteMatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( unsigned( (*m_data.byteMatrixValue)[i][j] ) ) );
                  if ( ++j == m_data.byteMatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.byteMatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::IMatrix:
      {
         String s = '{';
         if ( !m_data.iMatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( (*m_data.iMatrixValue)[i][j] ) );
                  if ( ++j == m_data.iMatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.iMatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UIMatrix:
      {
         String s = '{';
         if ( !m_data.uiMatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( (*m_data.uiMatrixValue)[i][j] ) );
                  if ( ++j == m_data.uiMatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.uiMatrixValue->Rows() )
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
   case VariantType::FMatrix:
      {
         String s = '{';
         if ( !m_data.fMatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( (*m_data.fMatrixValue)[i][j] ) );
                  if ( ++j == m_data.fMatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.fMatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::DMatrix:
      {
         String s = '{';
         if ( !m_data.dMatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( String( (*m_data.dMatrixValue)[i][j] ) );
                  if ( ++j == m_data.dMatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.dMatrixValue->Rows() )
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
   case VariantType::Float:
      return IsoString( m_data.floatValue );
   case VariantType::Double:
      return IsoString( m_data.doubleValue );
   case VariantType::FComplex:
      return IsoString().Format( "{%.7g,%.7g}", m_data.fcomplexValue->Real(), m_data.fcomplexValue->Imag() );
   case VariantType::DComplex:
      return IsoString().Format( "{%.16g,%.16g}", m_data.dcomplexValue->Real(), m_data.dcomplexValue->Imag() );
   case VariantType::Point:
      return IsoString().Format( "{%d,%d}", m_data.pointValue->x, m_data.pointValue->y );
   case VariantType::FPoint:
      return IsoString().Format( "{%.7g,%.7g}", m_data.fpointValue->x, m_data.fpointValue->y );
   case VariantType::DPoint:
      return IsoString().Format( "{%.16g,%.16g}", m_data.dpointValue->x, m_data.dpointValue->y );
   case VariantType::Rect:
      return IsoString().Format( "{%d,%d,%d,%d}", m_data.rectValue->x0, m_data.rectValue->y0,
                                                  m_data.rectValue->x1, m_data.rectValue->y1 );
   case VariantType::FRect:
      return IsoString().Format( "{%d,%d,%d,%d}", m_data.frectValue->x0, m_data.frectValue->y0,
                                                  m_data.frectValue->x1, m_data.frectValue->y1 );
   case VariantType::DRect:
      return IsoString().Format( "{%d,%d,%d,%d}", m_data.drectValue->x0, m_data.drectValue->y0,
                                                  m_data.drectValue->x1, m_data.drectValue->y1 );
   case VariantType::CharVector:
      {
         IsoString s = '{';
         if ( !m_data.charVectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( int( (*m_data.charVectorValue)[i] ) ) );
               if ( ++i == m_data.charVectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::ByteVector:
      {
         IsoString s = '{';
         if ( !m_data.byteVectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( unsigned( (*m_data.byteVectorValue)[i] ) ) );
               if ( ++i == m_data.byteVectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::IVector:
      {
         IsoString s = '{';
         if ( !m_data.iVectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( (*m_data.iVectorValue)[i] ) );
               if ( ++i == m_data.iVectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UIVector:
      {
         IsoString s = '{';
         if ( !m_data.uiVectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( (*m_data.uiVectorValue)[i] ) );
               if ( ++i == m_data.uiVectorValue->Length() )
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
   case VariantType::FVector:
      {
         IsoString s = '{';
         if ( !m_data.fVectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( (*m_data.fVectorValue)[i] ) );
               if ( ++i == m_data.fVectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::DVector:
      {
         IsoString s = '{';
         if ( !m_data.dVectorValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( IsoString( (*m_data.dVectorValue)[i] ) );
               if ( ++i == m_data.dVectorValue->Length() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::CharMatrix:
      {
         IsoString s = '{';
         if ( !m_data.charMatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( int( (*m_data.charMatrixValue)[i][j] ) ) );
                  if ( ++j == m_data.charMatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.charMatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::ByteMatrix:
      {
         IsoString s = '{';
         if ( !m_data.byteMatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( unsigned( (*m_data.byteMatrixValue)[i][j] ) ) );
                  if ( ++j == m_data.byteMatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.byteMatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::IMatrix:
      {
         IsoString s = '{';
         if ( !m_data.iMatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( (*m_data.iMatrixValue)[i][j] ) );
                  if ( ++j == m_data.iMatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.iMatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::UIMatrix:
      {
         IsoString s = '{';
         if ( !m_data.uiMatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( (*m_data.uiMatrixValue)[i][j] ) );
                  if ( ++j == m_data.uiMatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.uiMatrixValue->Rows() )
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
   case VariantType::FMatrix:
      {
         IsoString s = '{';
         if ( !m_data.fMatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( (*m_data.fMatrixValue)[i][j] ) );
                  if ( ++j == m_data.fMatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.fMatrixValue->Rows() )
                  break;
               s.Append( ',' );
            }
         return s + '}';
      }
   case VariantType::DMatrix:
      {
         IsoString s = '{';
         if ( !m_data.dMatrixValue->IsEmpty() )
            for ( int i = 0; ; )
            {
               s.Append( '{' );
               for ( int j = 0; ; )
               {
                  s.Append( IsoString( (*m_data.dMatrixValue)[i][j] ) );
                  if ( ++j == m_data.dMatrixValue->Cols() )
                     break;
                  s.Append( ',' );
               }
               s.Append( '}' );
               if ( ++i == m_data.dMatrixValue->Rows() )
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
   case VariantType::Float:
      return StringList( 1, String( m_data.floatValue ) );
   case VariantType::Double:
      return StringList( 1, String( m_data.doubleValue ) );
   case VariantType::FComplex:
      {
         StringList s( 2 );
         s[0] = String( m_data.fcomplexValue->Real() );
         s[1] = String( m_data.fcomplexValue->Imag() );
         return s;
      }
   case VariantType::DComplex:
      {
         StringList s( 2 );
         s[0] = String( m_data.dcomplexValue->Real() );
         s[1] = String( m_data.dcomplexValue->Imag() );
         return s;
      }
   case VariantType::Point:
      {
         StringList s( 2 );
         s[0] = String( m_data.pointValue->x );
         s[1] = String( m_data.pointValue->y );
         return s;
      }
   case VariantType::FPoint:
      {
         StringList s( 2 );
         s[0] = String( m_data.fpointValue->x );
         s[1] = String( m_data.fpointValue->y );
         return s;
      }
   case VariantType::DPoint:
      {
         StringList s( 2 );
         s[0] = String( m_data.dpointValue->x );
         s[1] = String( m_data.dpointValue->y );
         return s;
      }
   case VariantType::Rect:
      {
         StringList s( 4 );
         s[0] = String( m_data.rectValue->x0 );
         s[1] = String( m_data.rectValue->y0 );
         s[2] = String( m_data.rectValue->x1 );
         s[3] = String( m_data.rectValue->y1 );
         return s;
      }
   case VariantType::FRect:
      {
         StringList s( 4 );
         s[0] = String( m_data.frectValue->x0 );
         s[1] = String( m_data.frectValue->y0 );
         s[2] = String( m_data.frectValue->x1 );
         s[3] = String( m_data.frectValue->y1 );
         return s;
      }
   case VariantType::DRect:
      {
         StringList s( 4 );
         s[0] = String( m_data.drectValue->x0 );
         s[1] = String( m_data.drectValue->y0 );
         s[2] = String( m_data.drectValue->x1 );
         s[3] = String( m_data.drectValue->y1 );
         return s;
      }
   case VariantType::CharVector:
      {
         StringList s( m_data.charVectorValue->Length() );
         for ( int i = 0; i < m_data.charVectorValue->Length(); ++i )
            s[i] = String( int( (*m_data.charVectorValue)[i] ) );
         return s;
      }
   case VariantType::ByteVector:
      {
         StringList s( m_data.byteVectorValue->Length() );
         for ( int i = 0; i < m_data.byteVectorValue->Length(); ++i )
            s[i] = String( unsigned( (*m_data.byteVectorValue)[i] ) );
         return s;
      }
   case VariantType::IVector:
      {
         StringList s( m_data.iVectorValue->Length() );
         for ( int i = 0; i < m_data.iVectorValue->Length(); ++i )
            s[i] = String( (*m_data.iVectorValue)[i] );
         return s;
      }
   case VariantType::UIVector:
      {
         StringList s( m_data.uiVectorValue->Length() );
         for ( int i = 0; i < m_data.uiVectorValue->Length(); ++i )
            s[i] = String( (*m_data.uiVectorValue)[i] );
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
   case VariantType::FVector:
      {
         StringList s( m_data.fVectorValue->Length() );
         for ( int i = 0; i < m_data.fVectorValue->Length(); ++i )
            s[i] = String( (*m_data.fVectorValue)[i] );
         return s;
      }
   case VariantType::DVector:
      {
         StringList s( m_data.dVectorValue->Length() );
         for ( int i = 0; i < m_data.dVectorValue->Length(); ++i )
            s[i] = String( (*m_data.dVectorValue)[i] );
         return s;
      }
   case VariantType::CharMatrix:
      {
         StringList s( m_data.charMatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.charMatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.charMatrixValue->Cols(); ++j, ++n )
               s[n] = String( int( (*m_data.charMatrixValue)[i][j] ) );
         return s;
      }
   case VariantType::ByteMatrix:
      {
         StringList s( m_data.byteMatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.byteMatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.byteMatrixValue->Cols(); ++j, ++n )
               s[n] = String( unsigned( (*m_data.byteMatrixValue)[i][j] ) );
         return s;
      }
   case VariantType::IMatrix:
      {
         StringList s( m_data.iMatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.iMatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.iMatrixValue->Cols(); ++j, ++n )
               s[n] = String( (*m_data.iMatrixValue)[i][j] );
         return s;
      }
   case VariantType::UIMatrix:
      {
         StringList s( m_data.uiMatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.uiMatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.uiMatrixValue->Cols(); ++j, ++n )
               s[n] = String( (*m_data.uiMatrixValue)[i][j] );
         return s;
      }
   case VariantType::I64Matrix:
      {
         StringList s( m_data.i64MatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.i64MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.i64MatrixValue->Cols(); ++j, ++n )
               s[n] = String( (*m_data.i64MatrixValue)[i][j] );
         return s;
      }
   case VariantType::UI64Matrix:
      {
         StringList s( m_data.ui64MatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.ui64MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.ui64MatrixValue->Cols(); ++j, ++n )
               s[n] = String( (*m_data.ui64MatrixValue)[i][j] );
         return s;
      }
   case VariantType::FMatrix:
      {
         StringList s( m_data.fMatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.fMatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.fMatrixValue->Cols(); ++j, ++n )
               s[n] = String( (*m_data.fMatrixValue)[i][j] );
         return s;
      }
   case VariantType::DMatrix:
      {
         StringList s( m_data.dMatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.dMatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.dMatrixValue->Cols(); ++j, ++n )
               s[n] = String( (*m_data.dMatrixValue)[i][j] );
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
   case VariantType::Float:
      return IsoStringList( 1, IsoString( m_data.floatValue ) );
   case VariantType::Double:
      return IsoStringList( 1, IsoString( m_data.doubleValue ) );
   case VariantType::FComplex:
      {
         IsoStringList s( 2 );
         s[0] = IsoString( m_data.fcomplexValue->Real() );
         s[1] = IsoString( m_data.fcomplexValue->Imag() );
         return s;
      }
   case VariantType::DComplex:
      {
         IsoStringList s( 2 );
         s[0] = IsoString( m_data.dcomplexValue->Real() );
         s[1] = IsoString( m_data.dcomplexValue->Imag() );
         return s;
      }
   case VariantType::Point:
      {
         IsoStringList s( 2 );
         s[0] = IsoString( m_data.pointValue->x );
         s[1] = IsoString( m_data.pointValue->y );
         return s;
      }
   case VariantType::FPoint:
      {
         IsoStringList s( 2 );
         s[0] = IsoString( m_data.fpointValue->x );
         s[1] = IsoString( m_data.fpointValue->y );
         return s;
      }
   case VariantType::DPoint:
      {
         IsoStringList s( 2 );
         s[0] = IsoString( m_data.dpointValue->x );
         s[1] = IsoString( m_data.dpointValue->y );
         return s;
      }
   case VariantType::Rect:
      {
         IsoStringList s( 4 );
         s[0] = IsoString( m_data.rectValue->x0 );
         s[1] = IsoString( m_data.rectValue->y0 );
         s[2] = IsoString( m_data.rectValue->x1 );
         s[3] = IsoString( m_data.rectValue->y1 );
         return s;
      }
   case VariantType::FRect:
      {
         IsoStringList s( 4 );
         s[0] = IsoString( m_data.frectValue->x0 );
         s[1] = IsoString( m_data.frectValue->y0 );
         s[2] = IsoString( m_data.frectValue->x1 );
         s[3] = IsoString( m_data.frectValue->y1 );
         return s;
      }
   case VariantType::DRect:
      {
         IsoStringList s( 4 );
         s[0] = IsoString( m_data.drectValue->x0 );
         s[1] = IsoString( m_data.drectValue->y0 );
         s[2] = IsoString( m_data.drectValue->x1 );
         s[3] = IsoString( m_data.drectValue->y1 );
         return s;
      }
   case VariantType::CharVector:
      {
         IsoStringList s( m_data.charVectorValue->Length() );
         for ( int i = 0; i < m_data.charVectorValue->Length(); ++i )
            s[i] = String( int( (*m_data.charVectorValue)[i] ) );
         return s;
      }
   case VariantType::ByteVector:
      {
         IsoStringList s( m_data.byteVectorValue->Length() );
         for ( int i = 0; i < m_data.byteVectorValue->Length(); ++i )
            s[i] = String( unsigned( (*m_data.byteVectorValue)[i] ) );
         return s;
      }
   case VariantType::IVector:
      {
         IsoStringList s( m_data.iVectorValue->Length() );
         for ( int i = 0; i < m_data.iVectorValue->Length(); ++i )
            s[i] = IsoString( (*m_data.iVectorValue)[i] );
         return s;
      }
   case VariantType::UIVector:
      {
         IsoStringList s( m_data.uiVectorValue->Length() );
         for ( int i = 0; i < m_data.uiVectorValue->Length(); ++i )
            s[i] = IsoString( (*m_data.uiVectorValue)[i] );
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
   case VariantType::FVector:
      {
         IsoStringList s( m_data.fVectorValue->Length() );
         for ( int i = 0; i < m_data.fVectorValue->Length(); ++i )
            s[i] = IsoString( (*m_data.fVectorValue)[i] );
         return s;
      }
   case VariantType::DVector:
      {
         IsoStringList s( m_data.dVectorValue->Length() );
         for ( int i = 0; i < m_data.dVectorValue->Length(); ++i )
            s[i] = IsoString( (*m_data.dVectorValue)[i] );
         return s;
      }
   case VariantType::CharMatrix:
      {
         IsoStringList s( m_data.charMatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.charMatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.charMatrixValue->Cols(); ++j, ++n )
               s[n] = String( int( (*m_data.charMatrixValue)[i][j] ) );
         return s;
      }
   case VariantType::ByteMatrix:
      {
         IsoStringList s( m_data.byteMatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.byteMatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.byteMatrixValue->Cols(); ++j, ++n )
               s[n] = String( unsigned( (*m_data.byteMatrixValue)[i][j] ) );
         return s;
      }
   case VariantType::IMatrix:
      {
         IsoStringList s( m_data.iMatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.iMatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.iMatrixValue->Cols(); ++j, ++n )
               s[n] = IsoString( (*m_data.iMatrixValue)[i][j] );
         return s;
      }
   case VariantType::UIMatrix:
      {
         IsoStringList s( m_data.uiMatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.uiMatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.uiMatrixValue->Cols(); ++j, ++n )
               s[n] = IsoString( (*m_data.uiMatrixValue)[i][j] );
         return s;
      }
   case VariantType::I64Matrix:
      {
         IsoStringList s( m_data.i64MatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.i64MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.i64MatrixValue->Cols(); ++j, ++n )
               s[n] = IsoString( (*m_data.i64MatrixValue)[i][j] );
         return s;
      }
   case VariantType::UI64Matrix:
      {
         IsoStringList s( m_data.ui64MatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.ui64MatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.ui64MatrixValue->Cols(); ++j, ++n )
               s[n] = IsoString( (*m_data.ui64MatrixValue)[i][j] );
         return s;
      }
   case VariantType::FMatrix:
      {
         IsoStringList s( m_data.fMatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.fMatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.fMatrixValue->Cols(); ++j, ++n )
               s[n] = IsoString( (*m_data.fMatrixValue)[i][j] );
         return s;
      }
   case VariantType::DMatrix:
      {
         IsoStringList s( m_data.dMatrixValue->NumberOfElements() );
         for ( int n = 0, i = 0; i < m_data.dMatrixValue->Rows(); ++i )
            for ( int j = 0; j < m_data.dMatrixValue->Cols(); ++j, ++n )
               s[n] = IsoString( (*m_data.dMatrixValue)[i][j] );
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
      return StringKeyValue( *m_data.stringValue, String() );
   case VariantType::IsoString:
      return StringKeyValue( String( *m_data.isoStringValue ), String() );
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
      return IsoStringKeyValue( *m_data.stringValue, String() );
   case VariantType::IsoString:
      return IsoStringKeyValue( String( *m_data.isoStringValue ), String() );
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
   case VariantType::Float:      return &m_data.floatValue;
   case VariantType::Double:     return &m_data.doubleValue;
   case VariantType::CharVector: return  m_data.charVectorValue->Begin();
   case VariantType::ByteVector: return  m_data.byteVectorValue->Begin();
   case VariantType::IVector:    return  m_data.iVectorValue->Begin();
   case VariantType::UIVector:   return  m_data.uiVectorValue->Begin();
   case VariantType::I64Vector:  return  m_data.i64VectorValue->Begin();
   case VariantType::UI64Vector: return  m_data.ui64VectorValue->Begin();
   case VariantType::FVector:    return  m_data.fVectorValue->Begin();
   case VariantType::DVector:    return  m_data.dVectorValue->Begin();
   case VariantType::CharMatrix: return  m_data.charMatrixValue->Begin();
   case VariantType::ByteMatrix: return  m_data.byteMatrixValue->Begin();
   case VariantType::IMatrix:    return  m_data.iMatrixValue->Begin();
   case VariantType::UIMatrix:   return  m_data.uiMatrixValue->Begin();
   case VariantType::I64Matrix:  return  m_data.i64MatrixValue->Begin();
   case VariantType::UI64Matrix: return  m_data.ui64MatrixValue->Begin();
   case VariantType::FMatrix:    return  m_data.fMatrixValue->Begin();
   case VariantType::DMatrix:    return  m_data.dMatrixValue->Begin();
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
   case VariantType::CharVector: return m_data.charVectorValue->Length();
   case VariantType::ByteVector: return m_data.byteVectorValue->Length();
   case VariantType::IVector:    return m_data.iVectorValue->Length();
   case VariantType::UIVector:   return m_data.uiVectorValue->Length();
   case VariantType::I64Vector:  return m_data.i64VectorValue->Length();
   case VariantType::UI64Vector: return m_data.ui64VectorValue->Length();
   case VariantType::FVector:    return m_data.fVectorValue->Length();
   case VariantType::DVector:    return m_data.dVectorValue->Length();
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
   case VariantType::CharMatrix: return Rect( m_data.charMatrixValue->Cols(), m_data.charMatrixValue->Rows() );
   case VariantType::ByteMatrix: return Rect( m_data.byteMatrixValue->Cols(), m_data.byteMatrixValue->Rows() );
   case VariantType::IMatrix:    return Rect( m_data.iMatrixValue->Cols(), m_data.iMatrixValue->Rows() );
   case VariantType::UIMatrix:   return Rect( m_data.uiMatrixValue->Cols(), m_data.uiMatrixValue->Rows() );
   case VariantType::I64Matrix:  return Rect( m_data.i64MatrixValue->Cols(), m_data.i64MatrixValue->Rows() );
   case VariantType::UI64Matrix: return Rect( m_data.ui64MatrixValue->Cols(), m_data.ui64MatrixValue->Rows() );
   case VariantType::FMatrix:    return Rect( m_data.fMatrixValue->Cols(), m_data.fMatrixValue->Rows() );
   case VariantType::DMatrix:    return Rect( m_data.dMatrixValue->Cols(), m_data.dMatrixValue->Rows() );
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
   case VariantType::Float:      return sizeof( m_data.floatValue );
   case VariantType::Double:     return sizeof( m_data.doubleValue );

   // Vectors
   case VariantType::CharVector: return m_data.charVectorValue->Size();
   case VariantType::ByteVector: return m_data.byteVectorValue->Size();
   case VariantType::IVector:    return m_data.iVectorValue->Size();
   case VariantType::UIVector:   return m_data.uiVectorValue->Size();
   case VariantType::I64Vector:  return m_data.i64VectorValue->Size();
   case VariantType::UI64Vector: return m_data.ui64VectorValue->Size();
   case VariantType::FVector:    return m_data.fVectorValue->Size();
   case VariantType::DVector:    return m_data.dVectorValue->Size();
   case VariantType::ByteArray:  return m_data.byteArrayValue->Length() * sizeof( uint8 );
   case VariantType::String:     return m_data.stringValue->Length() * sizeof( char16_type );
   case VariantType::IsoString:  return m_data.isoStringValue->Length() * sizeof( char );

   // Matrices
   case VariantType::CharMatrix: return m_data.charMatrixValue->Size();
   case VariantType::ByteMatrix: return m_data.byteMatrixValue->Size();
   case VariantType::IMatrix:    return m_data.iMatrixValue->Size();
   case VariantType::UIMatrix:   return m_data.uiMatrixValue->Size();
   case VariantType::I64Matrix:  return m_data.i64MatrixValue->Size();
   case VariantType::UI64Matrix: return m_data.ui64MatrixValue->Size();
   case VariantType::FMatrix:    return m_data.fMatrixValue->Size();
   case VariantType::DMatrix:    return m_data.dMatrixValue->Size();

   // Structured
   default:
      throw Error( String( "Variant::BlockSize(): Invalid request for data type \'" ) + TypeAsString( m_type ) + '\'' );
   }
}

// ----------------------------------------------------------------------------

bool Variant::IsScalar() const
{
   switch ( m_type )
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
   case VariantType::Float:
   case VariantType::Double:
      return true;

   default:
      return false;
   }
}

bool Variant::IsVector() const
{
   switch ( m_type )
   {
   case VariantType::CharVector:
   case VariantType::ByteVector:
   case VariantType::IVector:
   case VariantType::UIVector:
   case VariantType::I64Vector:
   case VariantType::UI64Vector:
   case VariantType::FVector:
   case VariantType::DVector:
   case VariantType::ByteArray:
   case VariantType::String:
   case VariantType::IsoString:
      return true;

   default:
      return false;
   }
}

bool Variant::IsMatrix() const
{
   switch ( m_type )
   {
   case VariantType::CharMatrix:
   case VariantType::ByteMatrix:
   case VariantType::IMatrix:
   case VariantType::UIMatrix:
   case VariantType::I64Matrix:
   case VariantType::UI64Matrix:
   case VariantType::FMatrix:
   case VariantType::DMatrix:
      return true;

   default:
      return false;
   }
}

bool Variant::IsStructured() const
{
   switch ( m_type )
   {
   case VariantType::FComplex:
   case VariantType::DComplex:
   case VariantType::Point:
   case VariantType::FPoint:
   case VariantType::DPoint:
   case VariantType::Rect:
   case VariantType::FRect:
   case VariantType::DRect:
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
   case VariantType::Float:      return ToFloat();
   case VariantType::Double:     return ToDouble();
   case VariantType::FComplex:   return ToFComplex();
   case VariantType::DComplex:   return ToDComplex();
   case VariantType::Point:      return ToPoint();
   case VariantType::FPoint:     return ToFPoint();
   case VariantType::DPoint:     return ToDPoint();
   case VariantType::Rect:       return ToRect();
   case VariantType::FRect:      return ToFRect();
   case VariantType::DRect:      return ToDRect();
   case VariantType::CharVector: return ToCharVector();
   case VariantType::ByteVector: return ToByteVector();
   case VariantType::IVector:    return ToIVector();
   case VariantType::UIVector:   return ToUIVector();
   case VariantType::I64Vector:  return ToI64Vector();
   case VariantType::UI64Vector: return ToUI64Vector();
   case VariantType::FVector:    return ToFVector();
   case VariantType::DVector:    return ToDVector();
   case VariantType::CharMatrix: return ToCharMatrix();
   case VariantType::ByteMatrix: return ToByteMatrix();
   case VariantType::IMatrix:    return ToIMatrix();
   case VariantType::UIMatrix:   return ToUIMatrix();
   case VariantType::I64Matrix:  return ToI64Matrix();
   case VariantType::UI64Matrix: return ToUI64Matrix();
   case VariantType::FMatrix:    return ToFMatrix();
   case VariantType::DMatrix:    return ToDMatrix();
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
   case VariantType::Float:
   case VariantType::Double:
      break;
   case VariantType::FComplex:
      if ( m_data.fcomplexValue != 0 )
         delete m_data.fcomplexValue, m_data.fcomplexValue = 0;
      break;
   case VariantType::DComplex:
      if ( m_data.dcomplexValue != 0 )
         delete m_data.dcomplexValue, m_data.dcomplexValue = 0;
      break;
   case VariantType::Point:
      if ( m_data.pointValue != 0 )
         delete m_data.pointValue, m_data.pointValue = 0;
      break;
   case VariantType::FPoint:
      if ( m_data.fpointValue != 0 )
         delete m_data.fpointValue, m_data.fpointValue = 0;
      break;
   case VariantType::DPoint:
      if ( m_data.dpointValue != 0 )
         delete m_data.dpointValue, m_data.dpointValue = 0;
      break;
   case VariantType::Rect:
      if ( m_data.rectValue != 0 )
         delete m_data.rectValue, m_data.rectValue = 0;
      break;
   case VariantType::FRect:
      if ( m_data.frectValue != 0 )
         delete m_data.frectValue, m_data.frectValue = 0;
      break;
   case VariantType::DRect:
      if ( m_data.drectValue != 0 )
         delete m_data.drectValue, m_data.drectValue = 0;
      break;
   case VariantType::CharVector:
      if ( m_data.charVectorValue != 0 )
         delete m_data.charVectorValue, m_data.charVectorValue = 0;
      break;
   case VariantType::ByteVector:
      if ( m_data.byteVectorValue != 0 )
         delete m_data.byteVectorValue, m_data.byteVectorValue = 0;
      break;
   case VariantType::IVector:
      if ( m_data.iVectorValue != 0 )
         delete m_data.iVectorValue, m_data.iVectorValue = 0;
      break;
   case VariantType::UIVector:
      if ( m_data.uiVectorValue != 0 )
         delete m_data.uiVectorValue, m_data.uiVectorValue = 0;
      break;
   case VariantType::I64Vector:
      if ( m_data.i64VectorValue != 0 )
         delete m_data.i64VectorValue, m_data.i64VectorValue = 0;
      break;
   case VariantType::UI64Vector:
      if ( m_data.ui64VectorValue != 0 )
         delete m_data.ui64VectorValue, m_data.ui64VectorValue = 0;
      break;
   case VariantType::FVector:
      if ( m_data.fVectorValue != 0 )
         delete m_data.fVectorValue, m_data.fVectorValue = 0;
      break;
   case VariantType::DVector:
      if ( m_data.dVectorValue != 0 )
         delete m_data.dVectorValue, m_data.dVectorValue = 0;
      break;
   case VariantType::CharMatrix:
      if ( m_data.charMatrixValue != 0 )
         delete m_data.charMatrixValue, m_data.charMatrixValue = 0;
      break;
   case VariantType::ByteMatrix:
      if ( m_data.byteMatrixValue != 0 )
         delete m_data.byteMatrixValue, m_data.byteMatrixValue = 0;
      break;
   case VariantType::IMatrix:
      if ( m_data.iMatrixValue != 0 )
         delete m_data.iMatrixValue, m_data.iMatrixValue = 0;
      break;
   case VariantType::UIMatrix:
      if ( m_data.uiMatrixValue != 0 )
         delete m_data.uiMatrixValue, m_data.uiMatrixValue = 0;
      break;
   case VariantType::I64Matrix:
      if ( m_data.i64MatrixValue != 0 )
         delete m_data.i64MatrixValue, m_data.i64MatrixValue = 0;
      break;
   case VariantType::UI64Matrix:
      if ( m_data.ui64MatrixValue != 0 )
         delete m_data.ui64MatrixValue, m_data.ui64MatrixValue = 0;
      break;
   case VariantType::FMatrix:
      if ( m_data.fMatrixValue != 0 )
         delete m_data.fMatrixValue, m_data.fMatrixValue = 0;
      break;
   case VariantType::DMatrix:
      if ( m_data.dMatrixValue != 0 )
         delete m_data.dMatrixValue, m_data.dMatrixValue = 0;
      break;
   case VariantType::ByteArray:
      if ( m_data.byteArrayValue != 0 )
         delete m_data.byteArrayValue, m_data.byteArrayValue = 0;
      break;
   case VariantType::String:
      if ( m_data.stringValue != 0 )
         delete m_data.stringValue, m_data.stringValue = 0;
      break;
   case VariantType::IsoString:
      if ( m_data.isoStringValue != 0 )
         delete m_data.isoStringValue, m_data.isoStringValue = 0;
      break;
   case VariantType::StringList:
      if ( m_data.stringListValue != 0 )
         delete m_data.stringListValue, m_data.stringListValue = 0;
      break;
   case VariantType::IsoStringList:
      if ( m_data.isoStringListValue != 0 )
         delete m_data.isoStringListValue, m_data.isoStringListValue = 0;
      break;
   case VariantType::StringKeyValue:
      if ( m_data.stringKeyValueValue != 0 )
         delete m_data.stringKeyValueValue, m_data.stringKeyValueValue = 0;
      break;
   case VariantType::IsoStringKeyValue:
      if ( m_data.isoStringKeyValueValue != 0 )
         delete m_data.isoStringKeyValueValue, m_data.isoStringKeyValueValue = 0;
      break;
   case VariantType::StringKeyValueList:
      if ( m_data.stringKeyValueListValue != 0 )
         delete m_data.stringKeyValueListValue, m_data.stringKeyValueListValue = 0;
      break;
   case VariantType::IsoStringKeyValueList:
      if ( m_data.isoStringKeyValueListValue != 0 )
         delete m_data.isoStringKeyValueListValue, m_data.isoStringKeyValueListValue = 0;
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
   case VariantType::Float:
      return COMPARE( floatValue );
   case VariantType::Double:
      return COMPARE( doubleValue );
   case VariantType::FComplex:
      return PTR_COMPARE( fcomplexValue );
   case VariantType::DComplex:
      return PTR_COMPARE( dcomplexValue );
   case VariantType::Point:
      return PTR_COMPARE( pointValue );
   case VariantType::FPoint:
      return PTR_COMPARE( fpointValue );
   case VariantType::DPoint:
      return PTR_COMPARE( dpointValue );
   case VariantType::Rect:
      return PTR_COMPARE( rectValue );
   case VariantType::FRect:
      return PTR_COMPARE( frectValue );
   case VariantType::DRect:
      return PTR_COMPARE( drectValue );
   case VariantType::CharVector:
      return PTR_COMPARE( charVectorValue );
   case VariantType::ByteVector:
      return PTR_COMPARE( byteVectorValue );
   case VariantType::IVector:
      return PTR_COMPARE( iVectorValue );
   case VariantType::UIVector:
      return PTR_COMPARE( uiVectorValue );
   case VariantType::I64Vector:
      return PTR_COMPARE( i64VectorValue );
   case VariantType::UI64Vector:
      return PTR_COMPARE( ui64VectorValue );
   case VariantType::FVector:
      return PTR_COMPARE( fVectorValue );
   case VariantType::DVector:
      return PTR_COMPARE( dVectorValue );
   case VariantType::CharMatrix:
      return PTR_COMPARE( charMatrixValue );
   case VariantType::ByteMatrix:
      return PTR_COMPARE( byteMatrixValue );
   case VariantType::IMatrix:
      return PTR_COMPARE( iMatrixValue );
   case VariantType::UIMatrix:
      return PTR_COMPARE( uiMatrixValue );
   case VariantType::I64Matrix:
      return PTR_COMPARE( i64MatrixValue );
   case VariantType::UI64Matrix:
      return PTR_COMPARE( ui64MatrixValue );
   case VariantType::FMatrix:
      return PTR_COMPARE( fMatrixValue );
   case VariantType::DMatrix:
      return PTR_COMPARE( dMatrixValue );
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
   case VariantType::Float:
      COPY( floatValue );
      break;
   case VariantType::Double:
      COPY( doubleValue );
      break;
   case VariantType::FComplex:
      PTR_COPY( fcomplexValue, fcomplex );
      break;
   case VariantType::DComplex:
      PTR_COPY( dcomplexValue, dcomplex );
      break;
   case VariantType::Point:
      PTR_COPY( pointValue, Point );
      break;
   case VariantType::FPoint:
      PTR_COPY( fpointValue, FPoint );
      break;
   case VariantType::DPoint:
      PTR_COPY( dpointValue, DPoint );
      break;
   case VariantType::Rect:
      PTR_COPY( rectValue, Rect );
      break;
   case VariantType::FRect:
      PTR_COPY( frectValue, FRect );
      break;
   case VariantType::DRect:
      PTR_COPY( drectValue, DRect );
      break;
   case VariantType::CharVector:
      PTR_COPY( charVectorValue, CharVector );
      break;
   case VariantType::ByteVector:
      PTR_COPY( byteVectorValue, ByteVector );
      break;
   case VariantType::IVector:
      PTR_COPY( iVectorValue, IVector );
      break;
   case VariantType::UIVector:
      PTR_COPY( uiVectorValue, UIVector );
      break;
   case VariantType::I64Vector:
      PTR_COPY( i64VectorValue, I64Vector );
      break;
   case VariantType::UI64Vector:
      PTR_COPY( ui64VectorValue, UI64Vector );
      break;
   case VariantType::FVector:
      PTR_COPY( fVectorValue, FVector );
      break;
   case VariantType::DVector:
      PTR_COPY( dVectorValue, DVector );
      break;
   case VariantType::CharMatrix:
      PTR_COPY( charMatrixValue, CharMatrix );
      break;
   case VariantType::ByteMatrix:
      PTR_COPY( byteMatrixValue, ByteMatrix );
      break;
   case VariantType::IMatrix:
      PTR_COPY( iMatrixValue, IMatrix );
      break;
   case VariantType::UIMatrix:
      PTR_COPY( uiMatrixValue, UIMatrix );
      break;
   case VariantType::I64Matrix:
      PTR_COPY( i64MatrixValue, I64Matrix );
      break;
   case VariantType::UI64Matrix:
      PTR_COPY( ui64MatrixValue, UI64Matrix );
      break;
   case VariantType::FMatrix:
      PTR_COPY( fMatrixValue, FMatrix );
      break;
   case VariantType::DMatrix:
      PTR_COPY( dMatrixValue, DMatrix );
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

   case VTYPE_BYTEARRAY:
      {
         const uint8* a = reinterpret_cast<const uint8*>( apiValue.data.blockValue );
         return Variant( ByteArray( a, a+apiValue.dimX ) );
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
   case VTYPE_BYTEMATRIX:
      {
         const uint8* a = reinterpret_cast<const uint8*>( apiValue.data.blockValue );
         return Variant( ByteMatrix( a, int( apiValue.dimY ), int( apiValue.dimX ) ) );
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
   case VTYPE_ISOSTRING:
      return Variant( IsoString( reinterpret_cast<const char*>( apiValue.data.blockValue ) ) );
   case VTYPE_STRING:
      return Variant( String( reinterpret_cast<const char16_type*>( apiValue.data.blockValue ) ) );
   }
}

// ----------------------------------------------------------------------------

// ### Internal
void APIPropertyValueFromVariant( api_property_value& apiValue, const Variant& value )
{
   switch ( value.Type() )
   {
   case VariantType::Invalid:
      apiValue.data.blockValue = 0;
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
   case VariantType::Float:
      apiValue.data.floatValue = value.ToFloat();
      apiValue.type = VTYPE_FLOAT;
      break;
   case VariantType::Double:
      apiValue.data.doubleValue = value.ToDouble();
      apiValue.type = VTYPE_DOUBLE;
      break;
   case VariantType::ByteArray:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_BYTEARRAY;
      break;
   case VariantType::IVector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_IVECTOR;
      break;
   case VariantType::UIVector:
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
   case VariantType::FVector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_FVECTOR;
      break;
   case VariantType::DVector:
      apiValue.data.blockValue = value.InternalBlockAddress();
      apiValue.dimX = value.VectorLength();
      apiValue.type = VTYPE_DVECTOR;
      break;
   case VariantType::ByteMatrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_BYTEMATRIX;
      }
   case VariantType::IMatrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_IMATRIX;
      }
      break;
   case VariantType::UIMatrix:
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
   case VariantType::FMatrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_FMATRIX;
      }
      break;
   case VariantType::DMatrix:
      {
         apiValue.data.blockValue = value.InternalBlockAddress();
         Rect r = value.MatrixDimensions();
         apiValue.dimX = r.Width();
         apiValue.dimY = r.Height();
         apiValue.type = VTYPE_DMATRIX;
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
   case VTYPE_FLOAT:         return VariantType::Float;
   case VTYPE_DOUBLE:        return VariantType::Double;
   case VTYPE_BYTEARRAY:     return VariantType::ByteArray;
   case VTYPE_IVECTOR:       return VariantType::IVector;
   case VTYPE_UIVECTOR:      return VariantType::UIVector;
   case VTYPE_INT64_VECTOR:  return VariantType::I64Vector;
   case VTYPE_UINT64_VECTOR: return VariantType::UI64Vector;
   case VTYPE_FVECTOR:       return VariantType::FVector;
   case VTYPE_DVECTOR:       return VariantType::DVector;
   case VTYPE_BYTEMATRIX:    return VariantType::ByteMatrix;
   case VTYPE_IMATRIX:       return VariantType::IMatrix;
   case VTYPE_UIMATRIX:      return VariantType::UIMatrix;
   case VTYPE_INT64_MATRIX:  return VariantType::I64Matrix;
   case VTYPE_UINT64_MATRIX: return VariantType::UI64Matrix;
   case VTYPE_FMATRIX:       return VariantType::FMatrix;
   case VTYPE_DMATRIX:       return VariantType::DMatrix;
   case VTYPE_ISOSTRING:     return VariantType::IsoString;
   case VTYPE_STRING:        return VariantType::String;
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/Variant.cpp - Released 2014/11/14 17:17:01 UTC
