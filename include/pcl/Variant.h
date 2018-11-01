//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/Variant.h - Released 2018-11-01T11:06:36Z
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

#ifndef __PCL_Variant_h
#define __PCL_Variant_h

/// \file pcl/Variant.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ByteArray.h>
#include <pcl/Complex.h>
#include <pcl/Matrix.h>
#include <pcl/Point.h>
#include <pcl/Rectangle.h>
#include <pcl/StringList.h>
#include <pcl/TimePoint.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::VariantType
 * \brief %Variant data types.
 *
 * Currently %Variant supports the following data types:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>VariantType::Invalid</td>    <td>Symbolic value used to denote an invalid, undefined or unavailable property.</td></tr>
 * <tr><td>VariantType::Bool</td>       <td>A Boolean value, equivalent to the \c bool C++ type.</td></tr>
 * <tr><td>VariantType::Boolean</td>    <td>A synonym for VariantType::Bool.</td></tr>
 * <tr><td>VariantType::Int8</td>       <td>Signed 8-bit integer (int8).</td></tr>
 * <tr><td>VariantType::Char</td>       <td>A synonym for VariantType::Int8.</td></tr>
 * <tr><td>VariantType::Int16</td>      <td>Signed 16-bit integer (int16).</td></tr>
 * <tr><td>VariantType::Short</td>      <td>A synonym for VariantType::Int16.</td></tr>
 * <tr><td>VariantType::Int32</td>      <td>Signed 32-bit integer (int32).</td></tr>
 * <tr><td>VariantType::Integer</td>    <td>A synonym for VariantType::Int32.</td></tr>
 * <tr><td>VariantType::Int64</td>      <td>Signed 64-bit integer (int64).</td></tr>
 * <tr><td>VariantType::UInt8</td>      <td>Unsigned 8-bit integer (uint8).</td></tr>
 * <tr><td>VariantType::Byte</td>       <td>A synonym for VariantType::UInt8.</td></tr>
 * <tr><td>VariantType::UInt16</td>     <td>Unsigned 16-bit integer (uint16).</td></tr>
 * <tr><td>VariantType::UShort</td>     <td>A synonym for VariantType::UInt16.</td></tr>
 * <tr><td>VariantType::UInt32</td>     <td>Unsigned 32-bit integer (uint32).</td></tr>
 * <tr><td>VariantType::UInt</td>       <td>A synonym for VariantType::UInt32.</td></tr>
 * <tr><td>VariantType::UInt64</td>     <td>Unsigned 64-bit integer (uint64).</td></tr>
 * <tr><td>VariantType::Float32</td>    <td>32-bit floating point real value (\c float).</td></tr>
 * <tr><td>VariantType::Float</td>      <td>A synonym for VariantType::Float32.</td></tr>
 * <tr><td>VariantType::Float64</td>    <td>64-bit floating point real value (\c float).</td></tr>
 * <tr><td>VariantType::Double</td>     <td>A synonym for VariantType::Float64.</td></tr>
 * <tr><td>VariantType::Real</td>       <td>A synonym for VariantType::Double.</td></tr>
 * <tr><td>VariantType::Complex32</td>  <td>32-bit floating point complex value (fcomplex).</td></tr>
 * <tr><td>VariantType::FComplex</td>   <td>A synonym for VariantType::Complex32.</td></tr>
 * <tr><td>VariantType::Complex64</td>  <td>64-bit floating point complex value (dcomplex).</td></tr>
 * <tr><td>VariantType::DComplex</td>   <td>A synonym for VariantType::Complex64.</td></tr>
 * <tr><td>VariantType::Complex</td>    <td>A synonym for VariantType::DComplex.</td></tr>
 * <tr><td>VariantType::TimePoint</td>  <td>A time point value (TimePoint).</td></tr>
 * <tr><td>VariantType::I32Point</td>   <td>Two-dimensional point with 32-bit integer coordinates (Point).</td></tr>
 * <tr><td>VariantType::Point</td>      <td>A synonym for VariantType::I32Point.</td></tr>
 * <tr><td>VariantType::F32Point</td>   <td>Two-dimensional point with 32-bit floating point coordinates (FPoint).</td></tr>
 * <tr><td>VariantType::FPoint</td>     <td>A synonym for VariantType::F32Point.</td></tr>
 * <tr><td>VariantType::F64Point</td>   <td>Two-dimensional point with 64-bit floating point coordinates (DPoint).</td></tr>
 * <tr><td>VariantType::DPoint</td>     <td>A synonym for VariantType::F64Point.</td></tr>
 * <tr><td>VariantType::I32Rect</td>    <td>Rectangle with 32-bit integer coordinates (Rect).</td></tr>
 * <tr><td>VariantType::Rect</td>       <td>A synonym for VariantType::I32Rect.</td></tr>
 * <tr><td>VariantType::F32Rect</td>    <td>Rectangle with 32-bit floating point coordinates (FRect).</td></tr>
 * <tr><td>VariantType::FRect</td>      <td>A synonym for VariantType::F32Rect.</td></tr>
 * <tr><td>VariantType::F64Rect</td>    <td>Rectangle with 64-bit floating point coordinates (DRect).</td></tr>
 * <tr><td>VariantType::DRect</td>      <td>A synonym for VariantType::F64Rect.</td></tr>
 * <tr><td>VariantType::I8Vector</td>   <td>Vector of 8-bit signed integer components (I8Vector).</td></tr>
 * <tr><td>VariantType::CharVector</td> <td>A synonym for VariantType::I8Vector</td></tr>
 * <tr><td>VariantType::UI8Vector</td>  <td>Vector of 8-bit unsigned integer components (UI8Vector).</td></tr>
 * <tr><td>VariantType::ByteVector</td> <td>A synonym for VariantType::UI8Vector</td></tr>
 * <tr><td>VariantType::I16Vector</td>  <td>Vector of 16-bit signed integer components (I16Vector).</td></tr>
 * <tr><td>VariantType::UI16Vector</td> <td>Vector of 16-bit unsigned integer components (UI16Vector).</td></tr>
 * <tr><td>VariantType::I32Vector</td>  <td>Vector of 32-bit signed integer components (I32Vector).</td></tr>
 * <tr><td>VariantType::IVector</td>    <td>A synonym for VariantType::I32Vector</td></tr>
 * <tr><td>VariantType::UI32Vector</td> <td>Vector of 32-bit unsigned integer components (UI32Vector).</td></tr>
 * <tr><td>VariantType::UIVector</td>   <td>A synonym for VariantType::UI32Vector</td></tr>
 * <tr><td>VariantType::I64Vector</td>  <td>Vector of 64-bit signed integer components (I64Vector).</td></tr>
 * <tr><td>VariantType::UI64Vector</td> <td>Vector of 64-bit unsigned integer components (UI64Vector).</td></tr>
 * <tr><td>VariantType::F32Vector</td>  <td>Vector of 32-bit floating point real components (F32Vector).</td></tr>
 * <tr><td>VariantType::FVector</td>    <td>A synonym for VariantType::F32Vector</td></tr>
 * <tr><td>VariantType::F64Vector</td>  <td>Vector of 64-bit floating point real components (F64Vector).</td></tr>
 * <tr><td>VariantType::DVector</td>    <td>A synonym for VariantType::F64Vector</td></tr>
 * <tr><td>VariantType::C32Vector</td>  <td>Vector of 32-bit floating point complex components (C32Vector).</td></tr>
 * <tr><td>VariantType::C64Vector</td>  <td>Vector of 64-bit floating point complex components (C64Vector).</td></tr>
 * <tr><td>VariantType::I8Matrix</td>   <td>Matrix of 8-bit signed integer elements (I8Matrix).</td></tr>
 * <tr><td>VariantType::CharMatrix</td> <td>A synonym for VariantType::I8Matrix</td></tr>
 * <tr><td>VariantType::UI8Matrix</td>  <td>Matrix of 8-bit unsigned integer elements (UI8Matrix).</td></tr>
 * <tr><td>VariantType::ByteMatrix</td> <td>A synonym for VariantType::UI8Matrix</td></tr>
 * <tr><td>VariantType::I16Matrix</td>  <td>Matrix of 16-bit signed integer elements (I16Matrix).</td></tr>
 * <tr><td>VariantType::UI16Matrix</td> <td>Matrix of 16-bit unsigned integer elements (UI16Matrix).</td></tr>
 * <tr><td>VariantType::I32Matrix</td>  <td>Matrix of 32-bit signed integer elements (I32Matrix).</td></tr>
 * <tr><td>VariantType::IMatrix</td>    <td>A synonym for VariantType::I32Matrix</td></tr>
 * <tr><td>VariantType::UI32Matrix</td> <td>Matrix of 32-bit unsigned integer elements (UI32Matrix).</td></tr>
 * <tr><td>VariantType::UIMatrix</td>   <td>A synonym for VariantType::UI32Matrix</td></tr>
 * <tr><td>VariantType::I64Matrix</td>  <td>Matrix of 64-bit signed integer elements (I64Matrix).</td></tr>
 * <tr><td>VariantType::UI64Matrix</td> <td>Matrix of 64-bit unsigned integer elements (UI64Matrix).</td></tr>
 * <tr><td>VariantType::F32Matrix</td>  <td>Matrix of 32-bit floating point real elements (F32Matrix).</td></tr>
 * <tr><td>VariantType::FMatrix</td>    <td>A synonym for VariantType::F32Matrix</td></tr>
 * <tr><td>VariantType::F64Matrix</td>  <td>Matrix of 64-bit floating point real elements (F64Matrix).</td></tr>
 * <tr><td>VariantType::DMatrix</td>    <td>A synonym for VariantType::F64Matrix</td></tr>
 * <tr><td>VariantType::C32Matrix</td>  <td>Matrix of 32-bit floating point complex elements (C32Matrix).</td></tr>
 * <tr><td>VariantType::C64Matrix</td>  <td>Matrix of 64-bit floating point complex elements (C64Matrix).</td></tr>
 * <tr><td>VariantType::ByteArray</td>  <td>Dynamic array of unsigned 8-bit integers (ByteArray).</td></tr>
 * <tr><td>VariantType::String</td>     <td>UTF-16 Unicode string (String).</td></tr>
 * <tr><td>VariantType::IsoString</td>  <td>8-bit ISO/IEC 8859-1 or UTF-8 Unicode string (IsoString).</td></tr>
 * <tr><td>VariantType::StringList</td>            <td>Dynamic list of UTF-16 Unicode strings (StringList).</td></tr>
 * <tr><td>VariantType::IsoStringList</td>         <td>Dynamic list of 8-bit strings (IsoStringList).</td></tr>
 * <tr><td>VariantType::StringKeyValue</td>        <td>Key/value pair of UTF-16 Unicode strings (StringKeyValue).</td></tr>
 * <tr><td>VariantType::IsoStringKeyValue</td>     <td>Key/value pair of 8-bit strings (IsoStringKeyValue).</td></tr>
 * <tr><td>VariantType::StringKeyValueList</td>    <td>Dynamic list of key/value pairs of UTF-16 Unicode strings (StringKeyValueList).</td></tr>
 * <tr><td>VariantType::IsoStringKeyValueList</td> <td>Dynamic list of key/value pairs of 8-bit strings (IsoStringKeyValueList).</td></tr>
 * </table>
 */
namespace VariantType
{
   enum value_type
   {
      Invalid = 0,

      Bool,
      Boolean = Bool,

      Int8,
      Char = Int8,
      Int16,
      Short = Int16,
      Int32,
      Integer = Int32,
      Int64,

      UInt8,
      Byte = UInt8,
      UInt16,
      UShort = UInt16,
      UInt32,
      UInt = UInt32,
      UInt64,

      Float32,
      Float = Float32,
      Float64,
      Double = Float64,
      Real = Double,

      Complex32,
      FComplex = Complex32,
      Complex64,
      DComplex = Complex64,
      Complex = DComplex,

      TimePoint,

      I32Point,
      Point = I32Point,
      F32Point,
      FPoint = F32Point,
      F64Point,
      DPoint = F64Point,

      I32Rect,
      Rect = I32Rect,
      F32Rect,
      FRect = F32Rect,
      F64Rect,
      DRect = F64Rect,

      I8Vector,
      CharVector = I8Vector,
      UI8Vector,
      ByteVector = UI8Vector,
      I16Vector,
      UI16Vector,
      I32Vector,
      IVector = I32Vector,
      UI32Vector,
      UIVector = UI32Vector,
      I64Vector,
      UI64Vector,
      F32Vector,
      FVector = F32Vector,
      F64Vector,
      DVector = F64Vector,
      C32Vector,
      C64Vector,

      I8Matrix,
      CharMatrix = I8Matrix,
      UI8Matrix,
      ByteMatrix = UI8Matrix,
      I16Matrix,
      UI16Matrix,
      I32Matrix,
      IMatrix = I32Matrix,
      UI32Matrix,
      UIMatrix = UI32Matrix,
      I64Matrix,
      UI64Matrix,
      F32Matrix,
      FMatrix = F32Matrix,
      F64Matrix,
      DMatrix = F64Matrix,
      C32Matrix,
      C64Matrix,

      ByteArray,

      String,
      IsoString,

      StringList,
      IsoStringList,

      StringKeyValue,
      IsoStringKeyValue,

      StringKeyValueList,
      IsoStringKeyValueList,

      NumberOfVariantTypes,

      FirstCustomType = 1024
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class Variant
 * \brief Acts like a union to store instances of different data types.
 *
 * Sometimes you have to implement a single class or function able to work with
 * several data types. In C++ and other languages supporting template
 * metaprogramming, the obvious way to implement this functionality is by using
 * class or function templates.
 *
 * However, in order to use templates all of the types must be known without
 * ambiguity at compile time, which is not always feasible. When the data types
 * can only be known at runtime, \e variant constructs are the only practical
 * way to implement code able to work with multiple data types dynamically.
 *
 * An instance of the %Variant class can store one object of any of the
 * supported %Variant types, which have been enumerated in the VariantType
 * namespace. In this way %Variant can be used as a sort of envelope to
 * transport objects of different types under a common interface.
 *
 * %Variant supports explicit type conversions through a set of dedicated
 * %ToXXX() member functions. For example, one of the most useful and often
 * used conversion functions is Variant::ToString():
 *
 * \code
 * Variant v( 3 ); // v transports an integer value
 * Console().WriteLn( v.ToString() ); // writes "3" to the console
 * \endcode
 *
 * This conversion function is extremely useful for object serialization. Using
 * this conversion, a function can generate string representations of all the
 * data types supported by %Variant:
 *
 * \code
 * void PrintVariable( const String& name, const Variant& value )
 * {
 *    Console().WriteLn( name + " = " + value.ToString() );
 * }
 * \endcode
 *
 * A %Variant instance owns its stored object and cannot share it with other
 * objects, including other %Variant instances; it can only provide temporary
 * rvalues generated from its stored object, but never references to it. This
 * prevents unexpected object destruction and other problems with the objects
 * transported by %Variant instances.
 *
 * %Variant is an essential component of the introspection mechanisms
 * implemented by the Process, ProcessParameter and ProcessInstance classes.
 * Thanks to %Variant, these classes are able to represent and interface with
 * any installed process on the PixInsight platform.
 */
class PCL_CLASS Variant
{
public:

   /*!
    * An enumeration of all supported %Variant data types.
    */
   typedef VariantType::value_type  data_type;

   /*!
    * Constructs an invalid %Variant instance that stores no object.
    */
   Variant() : m_type( VariantType::Invalid )
   {
      m_data.anyValue = 0;
   }

   /*!
    * Constructs a %Variant instance to store a \c bool value.
    */
   Variant( bool b ) : m_type( VariantType::Bool )
   {
      m_data.boolValue = b;
   }

   /*!
    * Constructs a %Variant instance to store a signed 8-bit integer value.
    */
   Variant( int8 i8 ) : m_type( VariantType::Int8 )
   {
      m_data.int8Value = i8;
   }

   /*!
    * Constructs a %Variant instance to store a signed 16-bit integer value.
    */
   Variant( int16 i16 ) : m_type( VariantType::Int16 )
   {
      m_data.int16Value = i16;
   }

   /*!
    * Constructs a %Variant instance to store a signed 32-bit integer value.
    */
   Variant( int32 i32 ) : m_type( VariantType::Int32 )
   {
      m_data.int32Value = i32;
   }

   /*!
    * Constructs a %Variant instance to store a signed 64-bit integer value.
    */
   Variant( int64 i64 ) : m_type( VariantType::Int64 )
   {
      m_data.int64Value = i64;
   }

   /*!
    * Constructs a %Variant instance to store an unsigned 8-bit integer value.
    */
   Variant( uint8 u8 ) : m_type( VariantType::UInt8 )
   {
      m_data.uint8Value = u8;
   }

   /*!
    * Constructs a %Variant instance to store an unsigned 16-bit integer value.
    */
   Variant( uint16 u16 ) : m_type( VariantType::UInt16 )
   {
      m_data.uint16Value = u16;
   }

   /*!
    * Constructs a %Variant instance to store an unsigned 32-bit integer value.
    */
   Variant( uint32 u32 ) : m_type( VariantType::UInt32 )
   {
      m_data.uint32Value = u32;
   }

   /*!
    * Constructs a %Variant instance to store an unsigned 64-bit integer value.
    */
   Variant( uint64 u64 ) : m_type( VariantType::UInt64 )
   {
      m_data.uint64Value = u64;
   }

   /*!
    * Constructs a %Variant instance to store a \c float value (32-bit floating
    * point).
    */
   Variant( float f ) : m_type( VariantType::Float )
   {
      m_data.float32Value = f;
   }

   /*!
    * Constructs a %Variant instance to store a \c double value (64-bit
    * floating point).
    */
   Variant( double d ) : m_type( VariantType::Double )
   {
      m_data.float64Value = d;
   }

   /*!
    * Constructs a %Variant instance to store a complex number with \c float
    * components (fcomplex).
    */
   Variant( fcomplex fc ) : m_type( VariantType::FComplex )
   {
      m_data.anyValue = 0;
      m_data.complex32Value = new fcomplex( fc );
   }

   /*!
    * Constructs a %Variant instance to store a complex number with \c double
    * components (dcomplex).
    */
   Variant( dcomplex dc ) : m_type( VariantType::DComplex )
   {
      m_data.anyValue = 0;
      m_data.complex64Value = new dcomplex( dc );
   }

   /*!
    * Constructs a %Variant instance to store a time point value (TimePoint).
    */
   Variant( const TimePoint& t ) : m_type( VariantType::TimePoint )
   {
      m_data.anyValue = 0;
      m_data.timePointValue = new TimePoint( t );
   }

   /*!
    * Constructs a %Variant instance to store a two-dimensional point with
    * integer coordinates (Point).
    */
   Variant( const Point& p ) : m_type( VariantType::Point )
   {
      m_data.anyValue = 0;
      m_data.i32PointValue = new Point( p );
   }

   /*!
    * Constructs a %Variant instance to store a two-dimensional point with
    * \c float coordinates (FPoint).
    */
   Variant( const FPoint& fp ) : m_type( VariantType::FPoint )
   {
      m_data.anyValue = 0;
      m_data.f32PointValue = new FPoint( fp );
   }

   /*!
    * Constructs a %Variant instance to store a two-dimensional point with
    * \c double coordinates (DPoint).
    */
   Variant( const DPoint& dp ) : m_type( VariantType::DPoint )
   {
      m_data.anyValue = 0;
      m_data.f64PointValue = new DPoint( dp );
   }

   /*!
    * Constructs a %Variant instance to store a two-dimensional rectangle with
    * integer coordinates (Rect).
    */
   Variant( const Rect& r ) : m_type( VariantType::Rect )
   {
      m_data.anyValue = 0;
      m_data.i32RectValue = new Rect( r );
   }

   /*!
    * Constructs a %Variant instance to store a two-dimensional rectangle with
    * \c float coordinates (FRect).
    */
   Variant( const FRect& fr ) : m_type( VariantType::FRect )
   {
      m_data.anyValue = 0;
      m_data.f32RectValue = new FRect( fr );
   }

   /*!
    * Constructs a %Variant instance to store a two-dimensional rectangle with
    * \c double coordinates (DRect).
    */
   Variant( const DRect& dr ) : m_type( VariantType::DRect )
   {
      m_data.anyValue = 0;
      m_data.f64RectValue = new DRect( dr );
   }

   /*!
    * Constructs a %Variant instance to store a vector of 8-bit signed integer
    * components (CharVector).
    */
   Variant( const CharVector& cv ) : m_type( VariantType::CharVector )
   {
      m_data.anyValue = 0;
      m_data.i8VectorValue = new CharVector( cv );
   }

   /*!
    * Constructs a %Variant instance to store a vector of 8-bit unsigned
    * integer components (ByteVector).
    */
   Variant( const ByteVector& bv ) : m_type( VariantType::ByteVector )
   {
      m_data.anyValue = 0;
      m_data.ui8VectorValue = new ByteVector( bv );
   }

   /*!
    * Constructs a %Variant instance to store a vector of 16-bit signed integer
    * components (I16Vector).
    */
   Variant( const I16Vector& i16v ) : m_type( VariantType::I16Vector )
   {
      m_data.anyValue = 0;
      m_data.i16VectorValue = new I16Vector( i16v );
   }

   /*!
    * Constructs a %Variant instance to store a vector of 16-bit unsigned
    * integer components (UI16Vector).
    */
   Variant( const UI16Vector& ui16v ) : m_type( VariantType::UI16Vector )
   {
      m_data.anyValue = 0;
      m_data.ui16VectorValue = new UI16Vector( ui16v );
   }

   /*!
    * Constructs a %Variant instance to store a vector of 32-bit signed integer
    * components (IVector).
    */
   Variant( const IVector& iv ) : m_type( VariantType::IVector )
   {
      m_data.anyValue = 0;
      m_data.i32VectorValue = new IVector( iv );
   }

   /*!
    * Constructs a %Variant instance to store a vector of 32-bit unsigned
    * integer components (UIVector).
    */
   Variant( const UIVector& uiv ) : m_type( VariantType::UIVector )
   {
      m_data.anyValue = 0;
      m_data.ui32VectorValue = new UIVector( uiv );
   }

   /*!
    * Constructs a %Variant instance to store a vector of 64-bit signed integer
    * components (I64Vector).
    */
   Variant( const I64Vector& i64v ) : m_type( VariantType::I64Vector )
   {
      m_data.anyValue = 0;
      m_data.i64VectorValue = new I64Vector( i64v );
   }

   /*!
    * Constructs a %Variant instance to store a vector of 64-bit unsigned
    * integer components (UI64Vector).
    */
   Variant( const UI64Vector& ui64v ) : m_type( VariantType::UI64Vector )
   {
      m_data.anyValue = 0;
      m_data.ui64VectorValue = new UI64Vector( ui64v );
   }

   /*!
    * Constructs a %Variant instance to store a vector of 32-bit floating point
    * real components (FVector).
    */
   Variant( const FVector& fv ) : m_type( VariantType::FVector )
   {
      m_data.anyValue = 0;
      m_data.f32VectorValue = new FVector( fv );
   }

   /*!
    * Constructs a %Variant instance to store a vector of 64-bit floating point
    * real components (DVector).
    */
   Variant( const DVector& dv ) : m_type( VariantType::DVector )
   {
      m_data.anyValue = 0;
      m_data.f64VectorValue = new DVector( dv );
   }

   /*!
    * Constructs a %Variant instance to store a vector of 32-bit floating point
    * complex components (C32Vector).
    */
   Variant( const C32Vector& c32v ) : m_type( VariantType::C32Vector )
   {
      m_data.anyValue = 0;
      m_data.c32VectorValue = new C32Vector( c32v );
   }

   /*!
    * Constructs a %Variant instance to store a vector of 64-bit floating point
    * complex components (C64Vector).
    */
   Variant( const C64Vector& c64v ) : m_type( VariantType::C64Vector )
   {
      m_data.anyValue = 0;
      m_data.c64VectorValue = new C64Vector( c64v );
   }

   /*!
    * Constructs a %Variant instance to store a matrix of 8-bit signed integer
    * elements (CharMatrix).
    */
   Variant( const CharMatrix& cm ) : m_type( VariantType::CharMatrix )
   {
      m_data.anyValue = 0;
      m_data.i8MatrixValue = new CharMatrix( cm );
   }

   /*!
    * Constructs a %Variant instance to store a matrix of 8-bit unsigned
    * integer elements (ByteMatrix).
    */
   Variant( const ByteMatrix& bm ) : m_type( VariantType::ByteMatrix )
   {
      m_data.anyValue = 0;
      m_data.ui8MatrixValue = new ByteMatrix( bm );
   }

   /*!
    * Constructs a %Variant instance to store a matrix of 16-bit signed integer
    * elements (I16Matrix).
    */
   Variant( const I16Matrix& i16m ) : m_type( VariantType::I16Matrix )
   {
      m_data.anyValue = 0;
      m_data.i16MatrixValue = new I16Matrix( i16m );
   }

   /*!
    * Constructs a %Variant instance to store a matrix of 16-bit unsigned
    * integer elements (UI16Matrix).
    */
   Variant( const UI16Matrix& ui16m ) : m_type( VariantType::UI16Matrix )
   {
      m_data.anyValue = 0;
      m_data.ui16MatrixValue = new UI16Matrix( ui16m );
   }

   /*!
    * Constructs a %Variant instance to store a matrix of 32-bit signed integer
    * elements (IMatrix).
    */
   Variant( const IMatrix& im ) : m_type( VariantType::IMatrix )
   {
      m_data.anyValue = 0;
      m_data.i32MatrixValue = new IMatrix( im );
   }

   /*!
    * Constructs a %Variant instance to store a matrix of 32-bit unsigned
    * integer elements (UIMatrix).
    */
   Variant( const UIMatrix& uim ) : m_type( VariantType::UIMatrix )
   {
      m_data.anyValue = 0;
      m_data.ui32MatrixValue = new UIMatrix( uim );
   }

   /*!
    * Constructs a %Variant instance to store a matrix of 64-bit integer
    * elements (I64Matrix).
    */
   Variant( const I64Matrix& i64m ) : m_type( VariantType::I64Matrix )
   {
      m_data.anyValue = 0;
      m_data.i64MatrixValue = new I64Matrix( i64m );
   }

   /*!
    * Constructs a %Variant instance to store a matrix of 64-bit unsigned
    * integer elements (UI64Matrix).
    */
   Variant( const UI64Matrix& ui64m ) : m_type( VariantType::UI64Matrix )
   {
      m_data.anyValue = 0;
      m_data.ui64MatrixValue = new UI64Matrix( ui64m );
   }

   /*!
    * Constructs a %Variant instance to store a matrix of 32-bit floating point
    * real elements (FMatrix).
    */
   Variant( const FMatrix& fm ) : m_type( VariantType::FMatrix )
   {
      m_data.anyValue = 0;
      m_data.f32MatrixValue = new FMatrix( fm );
   }

   /*!
    * Constructs a %Variant instance to store a matrix of 64-bit floating point
    * real elements (DMatrix).
    */
   Variant( const DMatrix& dm ) : m_type( VariantType::DMatrix )
   {
      m_data.anyValue = 0;
      m_data.f64MatrixValue = new DMatrix( dm );
   }

   /*!
    * Constructs a %Variant instance to store a matrix of 32-bit floating point
    * complex elements (C32Matrix).
    */
   Variant( const C32Matrix& c32m ) : m_type( VariantType::C32Matrix )
   {
      m_data.anyValue = 0;
      m_data.c32MatrixValue = new C32Matrix( c32m );
   }

   /*!
    * Constructs a %Variant instance to store a matrix of 64-bit floating point
    * complex elements (C64Matrix).
    */
   Variant( const C64Matrix& c64m ) : m_type( VariantType::C64Matrix )
   {
      m_data.anyValue = 0;
      m_data.c64MatrixValue = new C64Matrix( c64m );
   }

   /*!
    * Constructs a %Variant instance to store a dynamic array of unsigned 8-bit
    * integers (ByteArray).
    */
   Variant( const ByteArray& ba ) : m_type( VariantType::ByteArray )
   {
      m_data.anyValue = 0;
      m_data.byteArrayValue = new ByteArray( ba );
   }

   /*!
    * Constructs a %Variant instance to store a UTF-16 Unicode string (String).
    */
   Variant( const String& s ) : m_type( VariantType::String )
   {
      m_data.anyValue = 0;
      m_data.stringValue = new String( s );
   }

   /*!
    * Constructs a %Variant instance to store an 8-bit ISO/IEC 8859-1 or UTF-8
    * Unicode string (IsoString).
    */
   Variant( const IsoString& s8 ) : m_type( VariantType::IsoString )
   {
      m_data.anyValue = 0;
      m_data.isoStringValue = new IsoString( s8 );
   }

   /*!
    * Constructs a %Variant instance to store an 8-bit ISO/IEC 8859-1 or UTF-8
    * Unicode string (IsoString), constructed from a null-terminated C string.
    */
   Variant( const char* cp ) : m_type( VariantType::IsoString )
   {
      m_data.anyValue = 0;
      m_data.isoStringValue = new IsoString( cp );
   }

   /*!
    * Constructs a %Variant instance to store a list of UTF-16 Unicode strings
    * (StringList).
    */
   Variant( const StringList& sl ) : m_type( VariantType::StringList )
   {
      m_data.anyValue = 0;
      m_data.stringListValue = new StringList( sl );
   }

   /*!
    * Constructs a %Variant instance to store a list of 8-bit strings
    * (IsoStringList).
    */
   Variant( const IsoStringList& isl ) : m_type( VariantType::IsoStringList )
   {
      m_data.anyValue = 0;
      m_data.isoStringListValue = new IsoStringList( isl );
   }

   /*!
    * Constructs a %Variant instance to store a key/value pair of UTF-16
    * Unicode strings (StringKeyValue).
    */
   Variant( const StringKeyValue& skv ) : m_type( VariantType::StringKeyValue )
   {
      m_data.anyValue = 0;
      m_data.stringKeyValueValue = new StringKeyValue( skv );
   }

   /*!
    * Constructs a %Variant instance to store a key/value pair of 8-bit strings
    * (IsoStringKeyValue).
    */
   Variant( const IsoStringKeyValue& iskv ) : m_type( VariantType::IsoStringKeyValue )
   {
      m_data.anyValue = 0;
      m_data.isoStringKeyValueValue = new IsoStringKeyValue( iskv );
   }

   /*!
    * Constructs a %Variant instance to store a list of key/value pairs of
    * UTF-16 Unicode strings (StringKeyValueList).
    */
   Variant( const StringKeyValueList& skvl ) : m_type( VariantType::StringKeyValueList )
   {
      m_data.anyValue = 0;
      m_data.stringKeyValueListValue = new StringKeyValueList( skvl );
   }

   /*!
    * Constructs a %Variant instance to store a list of key/value pairs of
    * 8-bit strings (IsoStringKeyValueList).
    */
   Variant( const IsoStringKeyValueList& iskvl ) : m_type( VariantType::IsoStringKeyValueList )
   {
      m_data.anyValue = 0;
      m_data.isoStringKeyValueListValue = new IsoStringKeyValueList( iskvl );
   }

   /*!
    * Copy constructor. This %Variant will store a copy of the object stored in
    * another %Variant \a x.
    */
   Variant( const Variant& x ) : m_type( VariantType::Invalid )
   {
      m_data.anyValue = 0;
      Copy( x );
   }

   /*!
    * Move constructor.
    */
   Variant( Variant&& x )
   {
      m_type = x.m_type;
      m_data.anyValue = x.m_data.anyValue;
      x.m_type = VariantType::Invalid;
   }

   /*!
    * Virtual destructor. The object stored in this %Variant will be destroyed.
    */
   virtual ~Variant()
   {
      Clear();
   }

   /*!
    * Copy assignment operator. This %Variant will store a copy of the object
    * stored in another %Variant \a x. Returns a reference to this object.
    */
   Variant& operator =( const Variant& x )
   {
      Assign( x );
      return *this;
   }

   /*!
    * Causes this %Variant to store a copy of the object stored in another
    * %Variant \a x.
    */
   void Assign( const Variant& x )
   {
      if ( &x != this )
      {
         Clear();
         Copy( x );
      }
   }

   /*!
    * Move assignment operator.
    */
   Variant& operator =( Variant&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Transfers the object stored in another %Variant \a x to this %Variant.
    * The source object \a x is left empty/invalid.
    */
   void Transfer( Variant& x )
   {
      if ( &x != this )
      {
         Clear();
         m_type = x.m_type;
         m_data.anyValue = x.m_data.anyValue;
         x.m_type = VariantType::Invalid;
      }
   }

   /*!
    * Transfers the object stored in another %Variant \a x to this %Variant.
    * The source object \a x is left empty/invalid.
    */
   void Transfer( Variant&& x )
   {
      if ( &x != this )
      {
         Clear();
         m_type = x.m_type;
         m_data.anyValue = x.m_data.anyValue;
         x.m_type = VariantType::Invalid;
      }
   }

   /*!
    * Returns true iff this %Variant stores an object. Returns false if this is
    * an invalid %Variant object.
    */
   bool IsValid() const
   {
      return m_type != VariantType::Invalid;
   }

   /*!
    * Returns the type of the object currently stored in this %Variant.
    */
   data_type Type() const
   {
      return data_type( m_type );
   }

   /*!
    * Converts the object currently stored in this %Variant instance to the
    * specified \a type.
    *
    * Returns a %Variant object with the result of the conversion.
    *
    * If this %Variant is invalid, or if the currently stored object cannot be
    * converted to the requested type, this function throws an Error exception
    * with a descriptive message.
    */
   Variant ToType( data_type type ) const;

   /*!
    * Destroys the object stored in this %Variant and leaves this object in an
    * invalid state.
    */
   void Clear();

   /*!
    * Compares the object stored in this %Variant with the object stored in
    * another %Variant \a x.
    *
    * If both %Variant instances store objects of different types, the object
    * in \a x is converted to the type of this %Variant as a temporary object,
    * and the comparison is made between the object in this %variant and the
    * temporary object. If the object in \a x cannot be converted to the type
    * of this %Variant, an Error exception is thrown.
    *
    * On success, returns an integer indicating the comparison result:
    *
    *  0 if the objects are equal. \n
    * -1 if this object precedes the object in \a x. \n
    * +1 if this object postcedes the object in \a x.
    *
    * \sa operator ==(), operator <()
    */
   int Compare( const Variant& x ) const;

   /*!
    * Returns true iff the object stored in this %Variant is equal to the object
    * in another %Variant \a x.
    *
    * This operator is equivalent to:
    *
    * \code Compare( x ) == 0; \endcode
    */
   bool operator ==( const Variant& x ) const
   {
      return Compare( x ) == 0;
   }

   /*!
    * Returns true iff the object stored in this %Variant precedes the object
    * in another %Variant \a x.
    *
    * This operator is equivalent to:
    *
    * \code Compare( x ) < 0; \endcode
    */
   bool operator <( const Variant& x ) const
   {
      return Compare( x ) < 0;
   }

   /*!
    * Converts the object stored in this %Variant to a Boolean value, and
    * returns the result of the conversion.
    *
    * Throws an Error exception if a Boolean conversion is not possible for the
    * object currently stored in this %Variant.
    */
   bool ToBool() const;

   /*!
    * A convenience synonym for ToBool().
    */
   bool ToBoolean() const
   {
      return ToBool();
   }

   /*!
    * Converts the object stored in this %Variant to a 32-bit integer value,
    * and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the \c int type is not
    * possible for the object currently stored in this %Variant.
    */
   int ToInt() const;

   /*!
    * Converts the object stored in this %Variant to a 64-bit integer value,
    * and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the \c int64 type is not
    * possible for the object currently stored in this %Variant.
    */
   int64 ToInt64() const;

   /*!
    * Converts the object stored in this %Variant to an unsigned 32-bit integer
    * value, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the \c unsigned \c int type
    * is not possible for the object currently stored in this %Variant.
    */
   unsigned int ToUInt() const;

   /*!
    * Converts the object stored in this %Variant to an unsigned 64-bit integer
    * value, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the \c uint64 type is not
    * possible for the object currently stored in this %Variant.
    */
   uint64 ToUInt64() const;

   /*!
    * Converts the object stored in this %Variant to a 32-bit floating point
    * value, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the \c float type is not
    * possible for the object currently stored in this %Variant.
    */
   float ToFloat() const;

   /*!
    * Converts the object stored in this %Variant to a 64-bit floating point
    * value, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the \c double type is not
    * possible for the object currently stored in this %Variant.
    */
   double ToDouble() const;

   /*!
    * Converts the object stored in this %Variant to a complex value with
    * \c float components, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the \c fcomplex type is not
    * possible for the object currently stored in this %Variant.
    */
   fcomplex ToFComplex() const;

   /*!
    * Converts the object stored in this %Variant to a complex value with
    * \c double components, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the \c dcomplex type is not
    * possible for the object currently stored in this %Variant.
    */
   dcomplex ToDComplex() const;

   /*!
    * A convenience synonym for ToDComplex().
    */
   complex ToComplex() const
   {
      return ToDComplex();
   }

   /*!
    * Converts the object stored in this %Variant to a time point value, and
    * returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the TimePoint class is not
    * possible for the object currently stored in this %Variant.
    */
   TimePoint ToTimePoint() const;

   /*!
    * Converts the object stored in this %Variant to a two-dimensional point
    * with integer coordinates, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the Point class is not
    * possible for the object currently stored in this %Variant.
    */
   Point ToPoint() const;

   /*!
    * Converts the object stored in this %Variant to a two-dimensional point
    * with \c float coordinates, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the FPoint class is not
    * possible for the object currently stored in this %Variant.
    */
   FPoint ToFPoint() const;

   /*!
    * Converts the object stored in this %Variant to a two-dimensional point
    * with \c double coordinates, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the DPoint class is not
    * possible for the object currently stored in this %Variant.
    */
   DPoint ToDPoint() const;

   /*!
    * Converts the object stored in this %Variant to a two-dimensional
    * rectangle with integer coordinates, and returns the result of the
    * conversion.
    *
    * Throws an Error exception if a conversion to the Rect class is not
    * possible for the object currently stored in this %Variant.
    */
   Rect ToRect() const;

   /*!
    * Converts the object stored in this %Variant to a two-dimensional
    * rectangle with \c float coordinates, and returns the result of the
    * conversion.
    *
    * Throws an Error exception if a conversion to the FRect class is not
    * possible for the object currently stored in this %Variant.
    */
   FRect ToFRect() const;

   /*!
    * Converts the object stored in this %Variant to a two-dimensional
    * rectangle with \c double coordinates, and returns the result of the
    * conversion.
    *
    * Throws an Error exception if a conversion to the DRect class is not
    * possible for the object currently stored in this %Variant.
    */
   DRect ToDRect() const;

   /*!
    * Converts the object stored in this %Variant to a vector of 8-bit signed
    * integer components, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the CharVector class is not
    * possible for the object currently stored in this %Variant.
    */
   CharVector ToCharVector() const;

   /*!
    * Converts the object stored in this %Variant to a vector of 8-bit unsigned
    * integer components, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the ByteVector class is not
    * possible for the object currently stored in this %Variant.
    */
   ByteVector ToByteVector() const;

   /*!
    * Converts the object stored in this %Variant to a vector of 16-bit signed
    * integer components, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the I16Vector class is not
    * possible for the object currently stored in this %Variant.
    */
   I16Vector ToI16Vector() const;

   /*!
    * Converts the object stored in this %Variant to a vector of 16-bit
    * unsigned integer components, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the UI16Vector class is not
    * possible for the object currently stored in this %Variant.
    */
   UI16Vector ToUI16Vector() const;

   /*!
    * Converts the object stored in this %Variant to a vector of integer
    * components, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the IVector class is not
    * possible for the object currently stored in this %Variant.
    */
   IVector ToIVector() const;

   /*!
    * Converts the object stored in this %Variant to a vector of unsigned
    * integer components, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the UIVector class is not
    * possible for the object currently stored in this %Variant.
    */
   UIVector ToUIVector() const;

   /*!
    * Converts the object stored in this %Variant to a vector of 64-bit integer
    * components, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the I64Vector class is not
    * possible for the object currently stored in this %Variant.
    */
   I64Vector ToI64Vector() const;

   /*!
    * Converts the object stored in this %Variant to a vector of 64-bit
    * unsigned integer components, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the UI64Vector class is not
    * possible for the object currently stored in this %Variant.
    */
   UI64Vector ToUI64Vector() const;

   /*!
    * Converts the object stored in this %Variant to a vector of \c float
    * components, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the FVector class is not
    * possible for the object currently stored in this %Variant.
    */
   FVector ToFVector() const;

   /*!
    * Converts the object stored in this %Variant to a vector of \c double
    * components, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the DVector class is not
    * possible for the object currently stored in this %Variant.
    */
   DVector ToDVector() const;

   /*!
    * A convenience synonym for ToDVector().
    */
   Vector ToVector() const
   {
      return ToDVector();
   }

   /*!
    * Converts the object stored in this %Variant to a vector of 32-bit
    * floating point complex components, and returns the result of the
    * conversion.
    *
    * Throws an Error exception if a conversion to the C32Vector class is not
    * possible for the object currently stored in this %Variant.
    */
   C32Vector ToC32Vector() const;

   /*!
    * Converts the object stored in this %Variant to a vector of 64-bit
    * floating point complex components, and returns the result of the
    * conversion.
    *
    * Throws an Error exception if a conversion to the C64Vector class is not
    * possible for the object currently stored in this %Variant.
    */
   C64Vector ToC64Vector() const;

   /*!
    * Converts the object stored in this %Variant to a matrix of 8-bit signed
    * integer elements, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the CharMatrix class is not
    * possible for the object currently stored in this %Variant.
    */
   CharMatrix ToCharMatrix() const;

   /*!
    * Converts the object stored in this %Variant to a matrix of 8-bit unsigned
    * integer elements, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the ByteMatrix class is not
    * possible for the object currently stored in this %Variant.
    */
   ByteMatrix ToByteMatrix() const;

   /*!
    * Converts the object stored in this %Variant to a matrix of 16-bit signed
    * integer elements, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the I16Matrix class is not
    * possible for the object currently stored in this %Variant.
    */
   I16Matrix ToI16Matrix() const;

   /*!
    * Converts the object stored in this %Variant to a matrix of 16-bit
    * unsigned integer elements, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the UI16Matrix class is not
    * possible for the object currently stored in this %Variant.
    */
   UI16Matrix ToUI16Matrix() const;

   /*!
    * Converts the object stored in this %Variant to a matrix of integer
    * elements, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the IMatrix class is not
    * possible for the object currently stored in this %Variant.
    */
   IMatrix ToIMatrix() const;

   /*!
    * Converts the object stored in this %Variant to a matrix of unsigned
    * integer elements, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the UIMatrix class is not
    * possible for the object currently stored in this %Variant.
    */
   UIMatrix ToUIMatrix() const;

   /*!
    * Converts the object stored in this %Variant to a matrix of 64-bit integer
    * elements, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the I64Matrix class is not
    * possible for the object currently stored in this %Variant.
    */
   I64Matrix ToI64Matrix() const;

   /*!
    * Converts the object stored in this %Variant to a matrix of 64-bit
    * unsigned integer elements, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the UI64Matrix class is not
    * possible for the object currently stored in this %Variant.
    */
   UI64Matrix ToUI64Matrix() const;

   /*!
    * Converts the object stored in this %Variant to a matrix of \c float
    * elements, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the FMatrix class is not
    * possible for the object currently stored in this %Variant.
    */
   FMatrix ToFMatrix() const;

   /*!
    * Converts the object stored in this %Variant to a matrix of \c double
    * elements, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the DMatrix class is not
    * possible for the object currently stored in this %Variant.
    */
   DMatrix ToDMatrix() const;

   /*!
    * A convenience synonym for ToDMatrix().
    */
   Matrix ToMatrix() const
   {
      return ToDMatrix();
   }

   /*!
    * Converts the object stored in this %Variant to a matrix of 32-bit
    * floating point complex elements, and returns the result of the
    * conversion.
    *
    * Throws an Error exception if a conversion to the C32Matrix class is not
    * possible for the object currently stored in this %Variant.
    */
   C32Matrix ToC32Matrix() const;

   /*!
    * Converts the object stored in this %Variant to a matrix of 64-bit
    * floating point complex elements, and returns the result of the
    * conversion.
    *
    * Throws an Error exception if a conversion to the C64Matrix class is not
    * possible for the object currently stored in this %Variant.
    */
   C64Matrix ToC64Matrix() const;

   /*!
    * Converts the object stored in this %Variant to a dynamic array of
    * unsigned 8-bit integers, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the ByteArray class is not
    * possible for the object currently stored in this %Variant.
    */
   ByteArray ToByteArray() const;

   /*!
    * Converts the object stored in this %Variant to a UTF-16 Unicode string,
    * and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the String class is not
    * possible for the object currently stored in this %Variant.
    */
   String ToString() const;

   /*!
    * Converts the object stored in this %Variant to an 8-bit ISO/IEC 8859-1 or
    * UTF-8 Unicode string, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the IsoString class is not
    * possible for the object currently stored in this %Variant.
    */
   IsoString ToIsoString() const;

   /*!
    * Converts the object stored in this %Variant to a dynamic list of UTF-16
    * Unicode strings, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the StringList class is not
    * possible for the object currently stored in this %Variant.
    */
   StringList ToStringList() const;

   /*!
    * Converts the object stored in this %Variant to a dynamic list of 8-bit
    * strings, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the IsoStringList class is
    * not possible for the object currently stored in this %Variant.
    */
   IsoStringList ToIsoStringList() const;

   /*!
    * Converts the object stored in this %Variant to a key/value pair of UTF-16
    * Unicode strings, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the StringKeyValue class is
    * not possible for the object currently stored in this %Variant.
    */
   StringKeyValue ToStringKeyValue() const;

   /*!
    * Converts the object stored in this %Variant to a key/value pair of 8-bit
    * strings, and returns the result of the conversion.
    *
    * Throws an Error exception if a conversion to the IsoStringKeyValue class is
    * not possible for the object currently stored in this %Variant.
    */
   IsoStringKeyValue ToIsoStringKeyValue() const;

   /*!
    * Converts the object stored in this %Variant to a dynamic list of
    * key/value pairs of UTF-16 Unicode strings, and returns the result of the
    * conversion.
    *
    * Throws an Error exception if a conversion to the StringKeyValueList class
    * is not possible for the object currently stored in this %Variant.
    */
   StringKeyValueList ToStringKeyValueList() const;

   /*!
    * Converts the object stored in this %Variant to a dynamic list of
    * key/value pairs of 8-bit strings, and returns the result of th
    * conversion.
    *
    * Throws an Error exception if a conversion to the IsoStringKeyValueList
    * class is not possible for the object currently stored in this %Variant.
    */
   IsoStringKeyValueList ToIsoStringKeyValueList() const;

   /*!
    * Returns a pointer to the immutable block of data stored in this %Variant
    * object.
    *
    * For scalar and complex data types (bool, int32, float, fcomplex, etc.),
    * this member function returns a pointer to the transported object.
    *
    * For vector, matrix and string types, this function returns the starting
    * address of the contiguous data block stored by the transported object.
    * For example, if a ByteArray object is being transported by this %Variant
    * object, this function returns ByteArray::Begin().
    *
    * For other structured types such as points, rectangles and lists, this
    * function throws an Error exception.
    */
   const void* InternalBlockAddress() const;

   /*!
    * Returns the length of the vector stored in this %Variant object.
    *
    * If this %Variant does not transport a vector or vector-like object
    * (IVector, FVector, DVector, ByteArray, String or IsoString), this member
    * function throws an Error exception.
    */
   size_type VectorLength() const;

   /*!
    * Returns the dimensions of the matrix stored in this %Variant object.
    *
    * If this %Variant does not transport a matrix object (IMatrix, FMatrix,
    * DMatrix), this member function throws an Error exception.
    *
    * The returned rectangle is anchored at {0,0} and has the same dimensions
    * as the transported matrix: Rect::Height() or Rect::y1 is the number of
    * matrix rows, and Rect::Width() or Rect::x1 is the number of matrix
    * columns.
    */
   Rect MatrixDimensions() const;

   /*!
    * Returns the size in bytes of the scalar, complex, point, rectangle,
    * vector or matrix object transported by this %Variant object.
    *
    * The value returned by this function can be used along with
    * InternalBlockAddress() for object serialization purposes.
    *
    * If this %Variant does not transport a scalar, complex, vector-like or
    * matrix object, this member function throws an Error exception.
    */
   size_type BlockSize() const;

   /*!
    * Returns the length in bytes of a block element for the specified \a type.
    *
    * \a type must be a scalar, complex, vector, matrix or string type.
    * Otherwise this function will throw an Error exception.
    */
   static int BytesPerBlockElementForType( int type );

   /*!
    * Returns the length in bytes of a block element, if this %Variant object
    * transports a scalar, complex, vector, matrix or string object.
    *
    * If this %Variant does not transport a scalar, complex, vector-like or
    * matrix object, this function throws an Error exception.
    */
   int BytesPerBlockElement() const
   {
      return BytesPerBlockElementForType( m_type );
   }

   /*!
    * Returns true only if the specified \a type is a scalar %Variant type:
    * bool, int8, int16, int32, int64, uint8, uint16, uint32, uint64, float or
    * double.
    */
   static bool IsScalarType( int type );

   /*!
    * Returns true only if the object transported by this %Variant is of a
    * scalar type: bool, int8, int16, int32, int64, uint8, uint16, uint32,
    * uint64, float or double.
    */
   bool IsScalar() const
   {
      return IsScalarType( m_type );
   }

   /*!
    * Returns true only if the specified \a type is a complex %Variant type:
    * Complex32 or Complex64.
    */
   static bool IsComplexType( int type )
   {
      return type == VariantType::Complex32 || type == VariantType::Complex64;
   }

   /*!
    * Returns true only if the object transported by this %Variant is of a
    * complex type: Complex32 or Complex64.
    */
   bool IsComplex() const
   {
      return IsComplexType( m_type );
   }

   /*!
    * Returns true only if the specified \a type is VariantType::TimePoint.
    */
   static bool IsTimePointType( int type )
   {
      return type == VariantType::TimePoint;
   }

   /*!
    * Returns true only if the object transported by this %Variant is of the
    * TimePoint type.
    */
   bool IsTimePoint() const
   {
      return IsTimePointType( m_type );
   }

   /*!
    * Returns true only if the specified \a type is a vector or vector-like
    * %Variant type: CharVector, ByteVector, IVector, UIVector, I64Vector,
    * UI64Vector, FVector, DVector, ByteArray, String or IsoString.
    */
   static bool IsVectorType( int type );

   /*!
    * Returns true only if the object transported by this %Variant is of a
    * vector or vector-like type: CharVector, ByteVector, IVector, UIVector,
    * I64Vector, UI64Vector, FVector, DVector, ByteArray, String or IsoString.
    */
   bool IsVector() const
   {
      return IsVectorType( m_type );
   }

   /*!
    * Returns true only if the specified \a type is a matrix %Variant type:
    * CharMatrix, ByteMatrix, IMatrix, UIMatrix, I64Matrix, UI64Matrix, FMatrix
    * or DMatrix.
    */
   static bool IsMatrixType( int type );

   /*!
    * Returns true only if the object transported by this %Variant is of a
    * matrix type: CharMatrix, ByteMatrix, IMatrix, UIMatrix, I64Matrix,
    * UI64Matrix, FMatrix or DMatrix.
    */
   bool IsMatrix() const
   {
      return IsMatrixType( m_type );
   }

   /*!
    * Returns true only if the specified \a type is a character string %Variant
    * type: String or IsoString.
    */
   static bool IsStringType( int type )
   {
      return type == VariantType::String || type == VariantType::IsoString;
   }

   /*!
    * Returns true only if the object transported by this %Variant is of a
    * character string type: String or IsoString.
    */
   bool IsString() const
   {
      return IsStringType( m_type );
   }

   /*!
    * Returns true only if the specified \a type is a structured %Variant type:
    * fcomplex, dcomplex, Point, FPoint, DPoint, Rect, FRect, DRect,
    * StringList, IsoStringList, StringKeyValue, IsoStringKeyValue,
    * StringKeyValueList or IsoStringKeyValueList.
    */
   static bool IsStructuredType( int type );

   /*!
    * Returns true only if the object transported by this %Variant is of a
    * structured type: fcomplex, dcomplex, Point, FPoint, DPoint, Rect, FRect,
    * DRect, StringList, IsoStringList, StringKeyValue, IsoStringKeyValue,
    * StringKeyValueList or IsoStringKeyValueList.
    *
    * For structured objects the InternalBlockAddress() and BlockSize() member
    * functions cannot be used, since these objects cannot be accessed through
    * a unique pointer and a length in bytes.
    */
   bool IsStructured() const
   {
      return IsStructuredType( m_type );
   }

   /*!
    * Returns a static null-terminated string with the name of the specified
    * %Variant \a type.
    */
   static const char* TypeAsString( int type );

private:

   union Data
   {
      bool        boolValue;

      int8        int8Value;
      int16       int16Value;
      int32       int32Value;
      int64       int64Value;

      uint8       uint8Value;
      uint16      uint16Value;
      uint32      uint32Value;
      uint64      uint64Value;

      float       float32Value;
      double      float64Value;

      fcomplex*   complex32Value;
      dcomplex*   complex64Value;

      TimePoint*  timePointValue;

      I32Point*   i32PointValue;
      F32Point*   f32PointValue;
      F64Point*   f64PointValue;

      I32Rect*    i32RectValue;
      F32Rect*    f32RectValue;
      F64Rect*    f64RectValue;

      I8Vector*   i8VectorValue;
      UI8Vector*  ui8VectorValue;
      I16Vector*  i16VectorValue;
      UI16Vector* ui16VectorValue;
      I32Vector*  i32VectorValue;
      UI32Vector* ui32VectorValue;
      I64Vector*  i64VectorValue;
      UI64Vector* ui64VectorValue;
      F32Vector*  f32VectorValue;
      F64Vector*  f64VectorValue;
      C32Vector*  c32VectorValue;
      C64Vector*  c64VectorValue;

      I8Matrix*   i8MatrixValue;
      UI8Matrix*  ui8MatrixValue;
      I16Matrix*  i16MatrixValue;
      UI16Matrix* ui16MatrixValue;
      I32Matrix*  i32MatrixValue;
      UI32Matrix* ui32MatrixValue;
      I64Matrix*  i64MatrixValue;
      UI64Matrix* ui64MatrixValue;
      F32Matrix*  f32MatrixValue;
      F64Matrix*  f64MatrixValue;
      C32Matrix*  c32MatrixValue;
      C64Matrix*  c64MatrixValue;

      ByteArray*  byteArrayValue;

      String*     stringValue;
      IsoString*  isoStringValue;

      StringList*    stringListValue;
      IsoStringList* isoStringListValue;

      StringKeyValue*    stringKeyValueValue;
      IsoStringKeyValue* isoStringKeyValueValue;

      StringKeyValueList*    stringKeyValueListValue;
      IsoStringKeyValueList* isoStringKeyValueListValue;

      uint64 anyValue;
   };

   Data m_data;
   int  m_type;

   // Internal
   void Copy( const Variant& );

   friend class PCL_AssertVariantSize;
};

class PCL_AssertVariantSize
{
#ifndef _MSC_VER
# ifdef __clang__
   _Pragma("clang diagnostic push")
   _Pragma("clang diagnostic ignored \"-Wunused-private-field\"")
# endif
   Variant* v;
   static_assert( sizeof( v->m_data ) == sizeof( uint64 ), "Invalid sizeof( Variant::m_data )" );
   static_assert( sizeof( v->m_data.anyValue ) == sizeof( v->m_data ), "Invalid sizeof( Variant::m_data.anyValue )" );
# ifdef __clang__
   _Pragma("clang diagnostic pop")
# endif
#else
   static_assert( sizeof( Variant::Data ) == sizeof( uint64 ), "Invalid sizeof( Variant::m_data )" );
#endif
};

// ----------------------------------------------------------------------------

} // pcl

/*!
 * \internal
 * API helper routines for internal use.
 */
struct api_property_value;
namespace pcl
{
   Variant VariantFromAPIPropertyValue( const api_property_value& );
   void APIPropertyValueFromVariant( api_property_value&, const Variant& );
   Variant::data_type VariantTypeFromAPIPropertyType( uint64 );
   uint64 APIPropertyTypeFromVariantType( Variant::data_type );
} // pcl

#endif   // __PCL_Variant_h

// ----------------------------------------------------------------------------
// EOF pcl/Variant.h - Released 2018-11-01T11:06:36Z
