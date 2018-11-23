//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/PixelTraits.h - Released 2018-11-23T16:14:19Z
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

#ifndef __PCL_PixelTraits_h
#define __PCL_PixelTraits_h

/// \file pcl/PixelTraits.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Complex.h>
#include <pcl/Math.h>

#include <memory.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct PixelTraitsLUT
{
    const float*  pFLUT8;   // uint8 -> float
    const float*  pFLUTA;   // i/255^2 (direct 8-bit normalization)
    const float*  p1FLUT8;  // 1 - pFLUT8
    const float*  pFLUT16;  // uint16 -> float
    const float*  pFLUT20;  // uint20 -> float
    const double* pDLUT8;   // uint8 -> double
    const double* pDLUTA;   // i/255^2 (direct 8-bit normalization)
    const double* p1DLUT8;  // 1- pDLUT8
    const double* pDLUT16;  // uint16 -> double
    const double* pDLUT20;  // uint20 -> double
    const uint8*  p8LUT16;  // uint16 -> uint8
    const uint8*  p8LUT20;  // uint20 -> uint8
    const uint16* p16LUT8;  // uint8 -> uint16
    const uint16* p16LUT20; // uint20 -> uint16
    const uint32* p20LUT8;  // uint8 -> uint20
    const uint32* p20LUT16; // uint16 -> uint20
    const uint32* p24LUT8;  // uint8 -> uint24
    const uint32* p24LUT16; // uint16 -> uint24
    const uint32* p32LUT8;  // uint8 -> uint32
    const uint32* p32LUT16; // uint16 -> uint32
    const uint32* p32LUT20; // uint20 -> uint32
};

extern PCL_DATA const PixelTraitsLUT* PTLUT;

#ifndef __PCL_DONT_USE_PIXTRAITS_LUT
# define pFLUT8   PTLUT->pFLUT8
# define pFLUTA   PTLUT->pFLUTA
# define p1FLUT8  PTLUT->p1FLUT8
# define pFLUT16  PTLUT->pFLUT16
# define pFLUT20  PTLUT->pFLUT20
# define pDLUT8   PTLUT->pDLUT8
# define pDLUTA   PTLUT->pDLUTA
# define p1DLUT8  PTLUT->p1DLUT8
# define pDLUT16  PTLUT->pDLUT16
# define pDLUT20  PTLUT->pDLUT20
# define p8LUT16  PTLUT->p8LUT16
# define p8LUT20  PTLUT->p8LUT20
# define p16LUT8  PTLUT->p16LUT8
# define p16LUT20 PTLUT->p16LUT20
# define p20LUT8  PTLUT->p20LUT8
# define p20LUT16 PTLUT->p20LUT16
# define p24LUT8  PTLUT->p24LUT8
# define p24LUT16 PTLUT->p24LUT16
# define p32LUT8  PTLUT->p32LUT8
# define p32LUT16 PTLUT->p32LUT16
# define p32LUT20 PTLUT->p32LUT20
#endif

#define EPSILON_D 1.0e-16
#define EPSILON_F 1.0e-8F

// ----------------------------------------------------------------------------

/*!
 * \class GenericPixelTraits
 * \brief Base class of all pixel traits classes.
 *
 * %GenericPixelTraits defines fundamental properties and functionality common
 * to pixel sample types.
 *
 * The purpose of a <em>pixel traits</em> class is to characterize a data type
 * to represent a single component of a pixel, or <em>pixel sample</em>. For
 * example, a pixel traits class must provide primitive routines to copy
 * samples between pixels and images, and to transform sample values between
 * different numeric data types, among many other critical, low-level tasks.
 *
 * %GenericPixelTraits is in the core of PixInsight's <em>scalable design</em>:
 * by isolating low-level pixel sample manipulations from medium-level pixel
 * management and high-level image transformations, the whole platform achieves
 * optimum efficiency and versatility for multiple pixel data types.
 *
 * %GenericPixelTraits is a template class that must be instantiated for
 * suitable data types. Seven instantiations of %GenericPixelTraits have
 * already been predefined in PCL, namely:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>FloatPixelTraits</td>    <td>32-bit IEEE 754 floating point real pixel samples</td></tr>
 * <tr><td>DoublePixelTraits</td>   <td>64-bit IEEE 754 floating point real pixel samples</td></tr>
 * <tr><td>ComplexPixelTraits</td>  <td>32-bit IEEE 754 floating point complex pixel samples</td></tr>
 * <tr><td>DComplexPixelTraits</td> <td>64-bit IEEE 754 floating point complex pixel samples</td></tr>
 * <tr><td>UInt8PixelTraits</td>    <td>8-bit unsigned integer pixel samples</td></tr>
 * <tr><td>UInt16PixelTraits</td>   <td>16-bit unsigned integer pixel samples</td></tr>
 * <tr><td>UInt32PixelTraits</td>   <td>32-bit unsigned integer pixel samples</td></tr>
 * </table>
 *
 * In coordination with the GenericImage<P> class, these template
 * instantiations originate the corresponding seven fundamental two-dimensional
 * image classes that have been predefined in PCL:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>Image</td>         <td>32-bit IEEE 754 floating point real image</td></tr>
 * <tr><td>DImage</td>        <td>64-bit IEEE 754 floating point real image</td></tr>
 * <tr><td>ComplexImage</td>  <td>32-bit IEEE 754 floating point complex image</td></tr>
 * <tr><td>DComplexImage</td> <td>64-bit IEEE 754 floating point complex image</td></tr>
 * <tr><td>UInt8Image</td>    <td>8-bit unsigned integer image</td></tr>
 * <tr><td>UInt16Image</td>   <td>16-bit unsigned integer image</td></tr>
 * <tr><td>UInt32Image</td>   <td>32-bit unsigned integer image</td></tr>
 * </table>
 *
 * \sa FloatPixelTraits, DoublePixelTraits, ComplexPixelTraits, DComplexPixelTraits,
 * UInt8PixelTraits, UInt16PixelTraits, UInt32PixelTraits, GenericImage, SharedPixelData
 */
template <class S>
class PCL_CLASS GenericPixelTraits
{
public:

   /*!
    * Represents a pixel sample value.
    */
   typedef S   sample;

   enum { bytesPerSample = sizeof( sample ) };
   enum { bitsPerSample = bytesPerSample << 3 };

   /*!
    * Returns the number of bytes necessary to store a pixel sample.
    */
   static constexpr int BytesPerSample()
   {
      return bytesPerSample;
   }

   /*!
    * Returns the number of bits in a pixel sample.
    */
   static constexpr int BitsPerSample()
   {
      return bitsPerSample;
   }

   /*!
    * Returns the minimum valid sample value. This is usually a value of zero
    * in the numeric data type represented by \c sample.
    */
   static constexpr sample MinSampleValue()
   {
      return sample( 0 );
   }
};

#define IMPLEMENT_TRANSFER_OPERATIONS                             \
                                                                  \
   template <typename T>                                          \
   static void Fill( sample* f, T x, size_type n )                \
   {                                                              \
      PCL_PRECONDITION( f != 0 )                                  \
      sample v = ToSample( x );                                   \
      for ( sample* fn = f+n; f != fn; ++f )                      \
         *f = v;                                                  \
   }                                                              \
                                                                  \
   static void Fill( sample* f, sample x, size_type n )           \
   {                                                              \
      PCL_PRECONDITION( f != 0 )                                  \
      for ( sample* fn = f+n; f < fn; ++f )                       \
         *f = x;                                                  \
   }                                                              \
                                                                  \
   template <typename T>                                          \
   static void Get( T* f, const sample* g, size_type n )          \
   {                                                              \
      PCL_PRECONDITION( f != 0 && g != 0 )                        \
      for ( T* fn = f+n; f < fn; ++f, ++g )                       \
         FromSample( *f, *g );                                    \
   }                                                              \
                                                                  \
   static void Get( sample* f, const sample* g, size_type n )     \
   {                                                              \
      PCL_PRECONDITION( f != 0 && g != 0 )                        \
      ::memcpy( f, g, n*BytesPerSample() );                       \
   }                                                              \
                                                                  \
   template <typename T>                                          \
   static void Copy( sample* f, const T* g, size_type n )         \
   {                                                              \
      PCL_PRECONDITION( f != 0 && g != 0 )                        \
      for ( sample* fn = f+n; f < fn; ++f, ++g )                  \
         *f = ToSample( *g );                                     \
   }                                                              \
                                                                  \
   static void Copy( sample* f, const sample* g, size_type n )    \
   {                                                              \
      PCL_PRECONDITION( f != 0 && g != 0 )                        \
      ::memcpy( f, g, n*BytesPerSample() );                       \
   }                                                              \
                                                                  \
   template <typename T>                                          \
   static void GetMin( T* f, const sample* g, size_type n )       \
   {                                                              \
      PCL_PRECONDITION( f != 0 && g != 0 )                        \
      for ( T* fn = f+n; f < fn; ++f, ++g )                       \
      {                                                           \
         T h; FromSample( h, *g );                                \
         if ( h < *f )                                            \
            *f = h;                                               \
      }                                                           \
   }                                                              \
                                                                  \
   static void GetMin( sample* f, const sample* g, size_type n )  \
   {                                                              \
      PCL_PRECONDITION( f != 0 && g != 0 )                        \
      for ( sample* fn = f+n; f < fn; ++f, ++g )                  \
         if ( *g < *f )                                           \
            *f = *g;                                              \
   }                                                              \
                                                                  \
   template <typename T>                                          \
   static void CopyMin( sample* f, const T* g, size_type n )      \
   {                                                              \
      PCL_PRECONDITION( f != 0 && g != 0 )                        \
      for ( sample* fn = f+n; f < fn; ++f, ++g )                  \
      {                                                           \
         sample h = ToSample( *g );                               \
         if ( h < *f )                                            \
            *f = h;                                               \
      }                                                           \
   }                                                              \
                                                                  \
   static void CopyMin( sample* f, const sample* g, size_type n ) \
   {                                                              \
      PCL_PRECONDITION( f != 0 && g != 0 )                        \
      for ( sample* fn = f+n; f < fn; ++f, ++g )                  \
         if ( *g < *f )                                           \
            *f = *g;                                              \
   }                                                              \
                                                                  \
   template <typename T>                                          \
   static void GetMax( T* f, const sample* g, size_type n )       \
   {                                                              \
      PCL_PRECONDITION( f != 0 && g != 0 )                        \
      for ( T* fn = f+n; f < fn; ++f, ++g )                       \
      {                                                           \
         T h; FromSample( h, *g );                                \
         if ( *f < h )                                            \
            *f = h;                                               \
      }                                                           \
   }                                                              \
                                                                  \
   static void GetMax( sample* f, const sample* g, size_type n )  \
   {                                                              \
      PCL_PRECONDITION( f != 0 && g != 0 )                        \
      for ( sample* fn = f+n; f < fn; ++f, ++g )                  \
         if ( *f < *g )                                           \
            *f = *g;                                              \
   }                                                              \
                                                                  \
   template <typename T>                                          \
   static void CopyMax( sample* f, const T* g, size_type n )      \
   {                                                              \
      PCL_PRECONDITION( f != 0 && g != 0 )                        \
      for ( sample* fn = f+n; f < fn; ++f, ++g )                  \
      {                                                           \
         sample h = ToSample( *g );                               \
         if ( *f < h )                                            \
            *f = h;                                               \
      }                                                           \
   }                                                              \
                                                                  \
   static void CopyMax( sample* f, const sample* g, size_type n ) \
   {                                                              \
      PCL_PRECONDITION( f != 0 && g != 0 )                        \
      for ( sample* fn = f+n; f < fn; ++f, ++g )                  \
         if ( *f < *g )                                           \
            *f = *g;                                              \
   }

// ----------------------------------------------------------------------------

/*!
 * \class FloatPixelTraits
 * \brief 32-bit IEEE 754 normalized floating point real pixel traits.
 *
 * %FloatPixelTraits is a template instantiation of GenericPixelTraits for the
 * \c float type. It defines the characteristic properties and functionality of
 * 32-bit IEEE 754 floating point real pixel samples.
 *
 * \sa GenericPixelTraits, GenericImage, Image
 */
class PCL_CLASS FloatPixelTraits : public GenericPixelTraits<float>
{
public:

   /*!
    * Represents this template instantiation.
    */
   typedef GenericPixelTraits<float>   traits_type;

   /*!
    * Represents a pixel sample value.
    */
   typedef traits_type::sample         sample;

   /*!
    * Returns true iff this pixel traits class corresponds to a floating point
    * pixel sample type.
    */
   static constexpr bool IsFloatSample()
   {
      return true;
   }

   /*!
    * Returns true if this pixel traits class corresponds to a complex pixel
    * sample type; false if it represents a real pixel sample type.
    */
   static constexpr bool IsComplexSample()
   {
      return false;
   }

   /*!
    * Returns the address of a static null-terminated string identifying the
    * sample data type represented by this pixel traits class.
    *
    * For %FloatPixelTraits, this member function returns "Float32".
    */
   static constexpr const char* SampleFormat()
   {
      return "Float32";
   }

   /*!
    * Returns the maximum valid pixel sample value.
    *
    * For %FloatPixelTraits, this member function returns 1.0F.
    */
   static constexpr sample MaxSampleValue()
   {
      return 1.0F;
   }

   /*!
    * Conversion of any floating point value to a pixel sample value.
    */
   template <typename T>
   static constexpr sample FloatToSample( T x )
   {
      return sample( x );
   }

   /*!
    * Conversion of an 8-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( x )/uint8_max;
#else
      return pFLUT8[x];
#endif
   }

   /*!
    * Conversion of an 8-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return (sample( x ) - int8_min)/uint8_max;
#else
      return pFLUT8[int( x ) - int8_min];
#endif
   }

   /*!
    * Conversion of a 16-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( x )/uint16_max;
#else
      return pFLUT16[x];
#endif
   }

   /*!
    * Conversion of a 16-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return (sample( x ) - int16_min)/uint16_max;
#else
      return pFLUT16[int( x ) - int16_min];
#endif
   }

   /*!
    * Conversion of a 32-bit unsigned integer value to a pixel sample value.
    */
   static constexpr sample ToSample( uint32 x )
   {
      return sample( double( x )/uint32_max );
   }

   /*!
    * Conversion of a 32-bit signed integer value to a pixel sample value.
    */
   static constexpr sample ToSample( int32 x )
   {
      return sample( (double( x ) - int32_min)/uint32_max );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value.
    */
   static constexpr sample ToSample( float x )
   {
      return sample( x );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value.
    */
   static constexpr sample ToSample( double x )
   {
      return sample( x );
   }

   /*!
    * Conversion of any complex value to a pixel sample value.
    */
   template <typename T>
   static constexpr sample ToSample( const Complex<T>& x )
   {
      return sample( pcl::Abs( x ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit unsigned integer.
    */
   static void FromSample( uint8& a, sample b )
   {
      a = uint8( RoundInt( b*uint8_max ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit signed integer.
    */
   static void FromSample( int8& a, sample b )
   {
      a = int8( RoundInt( b*uint8_max ) + int8_min );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit unsigned integer.
    */
   static void FromSample( uint16& a, sample b )
   {
      a = uint16( RoundInt( b*uint16_max ) );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit signed integer.
    */
   static void FromSample( int16& a, sample b )
   {
      a = int16( RoundInt( b*uint16_max ) + int16_min );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit unsigned integer.
    */
   static void FromSample( uint32& a, sample b )
   {
      a = uint32( Round( double( b )*uint32_max ) );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit signed integer.
    */
   static void FromSample( int32& a, sample b )
   {
      a = int32( Round( double( b )*uint32_max ) + int32_min );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit floating point real.
    */
   static void FromSample( float& a, sample b )
   {
      a = float( b );
   }

   /*!
    * Conversion of a pixel sample value to a 64-bit floating point real.
    */
   static void FromSample( double& a, sample b )
   {
      a = double( b );
   }

   /*!
    * Conversion of a pixel sample value to any complex type.
    */
   template <typename T>
   static void FromSample( Complex<T>& a, sample b )
   {
      a = typename Complex<T>::component( b );
   }

   /*!
    * Copies a T value \a b to a pixel sample variable \a a, with implicit
    * conversion from the source data type T to the pixel sample type.
    */
   template <typename T>
   static void Mov( sample& a, T b )
   {
      a = ToSample( b );
   }

   /*!
    * Adds a T value \a b to a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Add( sample& a, T b )
   {
      a += ToSample( b );
   }

   /*!
    * Subtracts a T value \a b from a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Sub( sample& a, T b )
   {
      a -= ToSample( b );
   }

   /*!
    * Multiplies a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Mul( sample& a, T b )
   {
      a *= ToSample( b );
   }

   /*!
    * Divides a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Div( sample& a, T b )
   {
      a /= ToSample( b );
   }

   /*!
    * Raises a pixel sample variable \a a to a T exponent value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Pow( sample& a, T b )
   {
      a = pcl::Pow( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a its absolute difference with a T
    * value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Dif( sample& a, T b )
   {
      a = pcl::Abs( a - ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the minimum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Min( sample& a, T b )
   {
      a = pcl::Min( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the maximum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Max( sample& a, T b )
   {
      a = pcl::Max( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive OR
    * operation with a T value \a b. The bitwise OR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Or( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ia | ib ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive NOR
    * operation with a T value \a b. The bitwise NOR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Nor( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~(ia | ib) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise AND operation with a
    * T value \a b. The bitwise AND operation is performed after converting
    * both operands to 8-bit unsigned integers, then the result is converted to
    * the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void And( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ia & ib ) );
   }

   /*!
    * Negates (bitwise NOT operation) a pixel sample variable \a a. Negation is
    * performed after converting the operand to an 8-bit unsigned integer,
    * then the result is converted to the pixel sample type before assignment.
    */
   static void Not( sample& a )
   {
      uint8 ia; FromSample( ia, a );
      a = ToSample( uint8( ~ia ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise negation (NOT
    * operation) of a T value \a b. Bitwise negation is performed after
    * converting both operands to 8-bit unsigned integers, then the result is
    * converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Not( sample& a, T b )
   {
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~ib ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise NAND operation with a
    * T value \a b. The bitwise NAND operation is performed after converting
    * both operands to 8-bit unsigned integers, then the result is converted to
    * the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Nand( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~(ia & ib) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive OR (XOR)
    * operation with a T value \a b. The bitwise XOR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Xor( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ia ^ ib ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive NOR (XNOR)
    * operation with a T value \a b. The bitwise XNOR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Xnor( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~(ia ^ ib) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorBurn( sample& a, T b )
   {
      a = 1 - pcl::Min( (1 - a)/pcl::Max( EPSILON_F, ToSample( b ) ), 1.0F );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void LinearBurn( sample& a, T b )
   {
      a = a + ToSample( b ) - 1;
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>screen</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Screen( sample& a, T b )
   {
      a = 1 - (1 - a)*(1 - ToSample( b ));
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color dodge</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorDodge( sample& a, T b )
   {
      a = pcl::Min( a/pcl::Max( EPSILON_F, (1 - ToSample( b )) ), 1.0F );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>overlay</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Overlay( sample& a, T b )
   {
      a = (a > 0.5F) ? 1 - ((1 - 2*(a - 0.5F)) * (1 - ToSample( b ))) : 2*a*ToSample( b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>soft light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void SoftLight( sample& a, T b )
   {
      sample fb = ToSample( b );
      a = (fb > 0.5F) ? 1 - (1 - a)*(1 - fb - 0.5F) : a*(fb + 0.5F);
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>hard light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void HardLight( sample& a, T b )
   {
      sample fb = ToSample( b );
      a = (fb > 0.5F) ? 1 - (1 - a)*(1 - 2*(fb - 0.5F)) : 2*a*fb;
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>vivid light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void VividLight( sample& a, T b )
   {
      sample fb = ToSample( b );
      a = (fb > 0.5F) ? 1 - pcl::Max( (1 - a)/(fb - 0.5F)/2, 1.0F ) : pcl::Min( a/pcl::Max( EPSILON_F, 1 - 2*fb ), 1.0F );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear light</em>
    * standard composition operation of its current value and a T value \a b,
    * with implicit data type conversion.
    */
   template <typename T>
   static void LinearLight( sample& a, T b )
   {
      sample fb = ToSample( b );
      a = (fb > 0.5F) ? pcl::Max( a + 2*(fb - 0.5F), 1.0F ) : pcl::Max( a + 2*fb - 1, 1.0F );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>pin light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void PinLight( sample& a, T b )
   {
      sample fb = ToSample( b );
      a = (fb > 0.5F) ? pcl::Max( a, 2*(fb - 0.5F) ) : pcl::Min( a, 2*fb );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>exclusion</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Exclusion( sample& a, T b )
   {
      a = pcl::Range( 0.5F - 2*(a - 0.5F)*(ToSample( b ) - 0.5F), 0.0F, 1.0F );
   }

   // -------------------------------------------------------------------------

   IMPLEMENT_TRANSFER_OPERATIONS
};

// ----------------------------------------------------------------------------

/*!
 * \class DoublePixelTraits
 * \brief 64-bit IEEE 754 normalized floating point real pixel traits.
 *
 * %DoublePixelTraits is a template instantiation of GenericPixelTraits for the
 * \c double type. It defines the characteristic properties and functionality
 * of 64-bit IEEE 754 floating point real pixel samples.
 *
 * \sa GenericPixelTraits, GenericImage, DImage
 */
class PCL_CLASS DoublePixelTraits : public GenericPixelTraits<double>
{
public:

   /*!
    * Represents this template instantiation.
    */
   typedef GenericPixelTraits<double>  traits_type;

   /*!
    * Represents a pixel sample value.
    */
   typedef traits_type::sample         sample;

   /*!
    * Returns true iff this pixel traits class corresponds to a floating point
    * pixel sample type.
    */
   static constexpr bool IsFloatSample()
   {
      return true;
   }

   /*!
    * Returns true if this pixel traits class corresponds to a complex pixel
    * sample type; false if it represents a real pixel sample type.
    */
   static constexpr bool IsComplexSample()
   {
      return false;
   }

   /*!
    * Returns the address of a static null-terminated string identifying the
    * sample data type represented by this pixel traits class.
    *
    * For %DoublePixelTraits, this member function returns "Float64".
    */
   static constexpr const char* SampleFormat()
   {
      return "Float64";
   }

   /*!
    * Returns the maximum valid pixel sample value.
    *
    * For %DoublePixelTraits, this member function returns 1.0.
    */
   static constexpr sample MaxSampleValue()
   {
      return 1.0;
   }

   /*!
    * Conversion of any floating point value to a pixel sample value.
    */
   template <typename T>
   static constexpr sample FloatToSample( T x )
   {
      return sample( x );
   }

   /*!
    * Conversion of an 8-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( x )/uint8_max;
#else
      return pDLUT8[x];
#endif
   }

   /*!
    * Conversion of an 8-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return (sample( x ) - int8_min)/uint8_max;
#else
      return pDLUT8[int( x ) - int8_min];
#endif
   }

   /*!
    * Conversion of a 16-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( x )/uint16_max;
#else
      return pDLUT16[x];
#endif
   }

   /*!
    * Conversion of a 16-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return (sample( x ) - int16_min)/uint16_max;
#else
      return pDLUT16[int( x ) - int16_min];
#endif
   }

   /*!
    * Conversion of a 32-bit unsigned integer value to a pixel sample value.
    */
   static constexpr sample ToSample( uint32 x )
   {
      return sample( x )/uint32_max;
   }

   /*!
    * Conversion of a 32-bit signed integer value to a pixel sample value.
    */
   static constexpr sample ToSample( int32 x )
   {
      return (sample( x ) - int32_min)/uint32_max;
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value.
    */
   static constexpr sample ToSample( float x )
   {
      return sample( x );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value.
    */
   static constexpr sample ToSample( double x )
   {
      return sample( x );
   }

   /*!
    * Conversion of any complex value to a pixel sample value.
    */
   template <typename T>
   static constexpr sample ToSample( const Complex<T>& x )
   {
      return sample( pcl::Abs( x ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit unsigned integer.
    */
   static void FromSample( uint8& a, sample b )
   {
      a = uint8( RoundInt( b*uint8_max ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit signed integer.
    */
   static void FromSample( int8& a, sample b )
   {
      a = int8( RoundInt( b*uint8_max ) + int8_min );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit unsigned integer.
    */
   static void FromSample( uint16& a, sample b )
   {
      a = uint16( RoundInt( b*uint16_max ) );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit signed integer.
    */
   static void FromSample( int16& a, sample b )
   {
      a = int16( RoundInt( b*uint16_max ) + int16_min );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit unsigned integer.
    */
   static void FromSample( uint32& a, sample b )
   {
      a = uint32( Round( sample( b )*uint32_max ) );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit signed integer.
    */
   static void FromSample( int32& a, sample b )
   {
      a = int32( Round( sample( b )*uint32_max ) + int32_min );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit floating point real.
    */
   static void FromSample( float& a, sample b )
   {
      a = float( b );
   }

   /*!
    * Conversion of a pixel sample value to a 64-bit floating point real.
    */
   static void FromSample( double& a, sample b )
   {
      a = double( b );
   }

   /*!
    * Conversion of a pixel sample value to any complex type.
    */
   template <typename T>
   static void FromSample( Complex<T>& a, sample b )
   {
      a = typename Complex<T>::component( b );
   }

   /*!
    * Copies a T value \a b to a pixel sample variable \a a, with implicit
    * conversion from the source data type T to the pixel sample type.
    */
   template <typename T>
   static void Mov( sample& a, T b )
   {
      a = ToSample( b );
   }

   /*!
    * Adds a T value \a b to a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Add( sample& a, T b )
   {
      a += ToSample( b );
   }

   /*!
    * Subtracts a T value \a b from a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Sub( sample& a, T b )
   {
      a -= ToSample( b );
   }

   /*!
    * Multiplies a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Mul( sample& a, T b )
   {
      a *= ToSample( b );
   }

   /*!
    * Divides a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Div( sample& a, T b )
   {
      a /= ToSample( b );
   }

   /*!
    * Raises a pixel sample variable \a a to a T exponent value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Pow( sample& a, T b )
   {
      a = pcl::Pow( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a its absolute difference with a T
    * value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Dif( sample& a, T b )
   {
      a = pcl::Abs( a - ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the minimum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Min( sample& a, T b )
   {
      a = pcl::Min( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the maximum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Max( sample& a, T b )
   {
      a = pcl::Max( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive OR
    * operation with a T value \a b. The bitwise OR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Or( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ia | ib ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive NOR
    * operation with a T value \a b. The bitwise NOR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Nor( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~(ia | ib) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise AND operation with a
    * T value \a b. The bitwise AND operation is performed after converting
    * both operands to 8-bit unsigned integers, then the result is converted to
    * the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void And( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ia & ib ) );
   }

   /*!
    * Negates (bitwise NOT operation) a pixel sample variable \a a. Negation is
    * performed after converting the operand to an 8-bit unsigned integer,
    * then the result is converted to the pixel sample type before assignment.
    */
   static void Not( sample& a )
   {
      uint8 ia; FromSample( ia, a );
      a = ToSample( uint8( ~ia ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise negation (NOT
    * operation) of a T value \a b. Bitwise negation is performed after
    * converting both operands to 8-bit unsigned integers, then the result is
    * converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Not( sample& a, T b )
   {
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~ib ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise NAND operation with a
    * T value \a b. The bitwise NAND operation is performed after converting
    * both operands to 8-bit unsigned integers, then the result is converted to
    * the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Nand( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~(ia & ib) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive OR (XOR)
    * operation with a T value \a b. The bitwise XOR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Xor( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ia ^ ib ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive NOR (XNOR)
    * operation with a T value \a b. The bitwise XNOR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Xnor( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~(ia ^ ib) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorBurn( sample& a, T b )
   {
      a = 1 - pcl::Min( (1 - a)/pcl::Max( EPSILON_D, ToSample( b ) ), 1.0 );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void LinearBurn( sample& a, T b )
   {
      a = a + ToSample( b ) - 1;
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>screen</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Screen( sample& a, T b )
   {
      a = 1 - (1 - a)*(1 - ToSample( b ));
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color dodge</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorDodge( sample& a, T b )
   {
      a = pcl::Min( a/pcl::Max( EPSILON_D, (1 - ToSample( b )) ), 1.0 );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>overlay</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Overlay( sample& a, T b )
   {
      a = (a > 0.5) ? 1 - ((1 - 2*(a - 0.5)) * (1 - ToSample( b ))) : 2*a*ToSample( b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>soft light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void SoftLight( sample& a, T b )
   {
      sample fb = ToSample( b );
      a = (fb > 0.5) ? 1 - (1 - a)*(1 - fb - 0.5) : a*(fb + 0.5);
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>hard light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void HardLight( sample& a, T b )
   {
      sample fb = ToSample( b );
      a = (fb > 0.5) ? 1 - (1 - a)*(1 - 2*(fb - 0.5)) : 2*a*fb;
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>vivid light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void VividLight( sample& a, T b )
   {
      sample fb = ToSample( b );
      a = (fb > 0.5) ? 1 - pcl::Max( (1 - a)/(fb - 0.5)/2, 1.0 ) : pcl::Min( a/pcl::Max( EPSILON_D, 1 - 2*fb ), 1.0 );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear light</em>
    * standard composition operation of its current value and a T value \a b,
    * with implicit data type conversion.
    */
   template <typename T>
   static void LinearLight( sample& a, T b )
   {
      sample fb = ToSample( b );
      a = (fb > 0.5) ? pcl::Max( a + 2*(fb - 0.5), 1.0 ) : pcl::Max( a + 2*fb - 1, 1.0 );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>pin light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void PinLight( sample& a, T b )
   {
      sample fb = ToSample( b );
      a = (fb > 0.5) ? pcl::Max( a, 2*(fb - 0.5) ) : pcl::Min( a, 2*fb );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>exclusion</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Exclusion( sample& a, T b )
   {
      a = pcl::Range( 0.5 - 2*(a - 0.5)*(ToSample( b ) - 0.5), 0.0, 1.0 );
   }

   // -------------------------------------------------------------------------

   IMPLEMENT_TRANSFER_OPERATIONS
};

// ----------------------------------------------------------------------------

/*!
 * \class ComplexPixelTraits
 * \brief 32-bit IEEE 754 normalized floating point complex pixel traits.
 *
 * %ComplexPixelTraits is a template instantiation of GenericPixelTraits for
 * the \c Complex<float> type. It defines the characteristic properties and
 * functionality of 32-bit IEEE 754 floating point complex pixel samples.
 *
 * \sa GenericPixelTraits, GenericImage, ComplexImage
 */
class PCL_CLASS ComplexPixelTraits : public GenericPixelTraits<Complex<float> >
{
public:

   /*!
    * Represents this template instantiation.
    */
   typedef GenericPixelTraits<Complex<float> >  traits_type;

   /*!
    * Represents a pixel sample value.
    */
   typedef traits_type::sample                  sample;

   /*!
    * Represents a component of a complex pixel sample.
    */
   typedef sample::component                    component;

   static constexpr int BitsPerSample()
   {
      return sizeof( component ) << 3;
   }

   /*!
    * Returns true iff this pixel traits class corresponds to a floating point
    * real pixel sample type.
    */
   static constexpr bool IsFloatSample()
   {
      return false;
   }

   /*!
    * Returns true if this pixel traits class corresponds to a complex pixel
    * sample type; false if it represents a real pixel sample type.
    */
   static constexpr bool IsComplexSample()
   {
      return true;
   }

   /*!
    * Returns the address of a static null-terminated string identifying the
    * sample data type represented by this pixel traits class.
    *
    * For %ComplexPixelTraits, this member function returns "Complex32".
    */
   static constexpr const char* SampleFormat()
   {
      return "Complex32";
   }

   /*!
    * Returns the maximum valid pixel sample value.
    *
    * For %ComplexPixelTraits, this member function returns {1.0F, 0.0F}.
    */
   static sample MaxSampleValue()
   {
      return sample( component( 1 ) );
   }

   /*!
    * Conversion of any floating point value to a pixel sample value.
    */
   template <typename T>
   static sample FloatToSample( T x )
   {
      return sample( component( x ) );
   }

   template <typename T>
   static sample FloatToSample( sample x )
   {
      return sample( x );
   }

   /*!
    * Conversion of an 8-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( component( x )/uint8_max );
#else
      return sample( component( pFLUT8[x] ) );
#endif
   }

   /*!
    * Conversion of an 8-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( (component( x ) - int8_min)/uint8_max );
#else
      return sample( component( pFLUT8[int( x ) - int8_min] ) );
#endif
   }

   /*!
    * Conversion of a 16-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( component( x )/uint16_max );
#else
      return sample( component( pFLUT16[x] ) );
#endif
   }

   /*!
    * Conversion of a 16-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( (component( x ) - int16_min)/uint16_max );
#else
      return sample( component( pFLUT16[int( x ) - int16_min] ) );
#endif
   }

   /*!
    * Conversion of a 32-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint32 x )
   {
      return sample( component( double( x )/uint32_max ) );
   }

   /*!
    * Conversion of a 32-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int32 x )
   {
      return sample( component( (double( x ) - int32_min)/uint32_max ) );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value.
    */
   static sample ToSample( float x )
   {
      return sample( component( x ) );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value.
    */
   static sample ToSample( double x )
   {
      return sample( component( x ) );
   }

   /*!
    * Conversion of any complex value to a pixel sample value.
    */
   template <typename T>
   static sample ToSample( const Complex<T>& x )
   {
      return sample( x );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit unsigned integer.
    */
   static void FromSample( uint8& a, sample b )
   {
      a = uint8( RoundInt( pcl::Abs( b )*uint8_max ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit signed integer.
    */
   static void FromSampl( int8& a, sample b )
   {
      a = int8( RoundInt( pcl::Abs( b )*uint8_max ) + int8_min );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit unsigned integer.
    */
   static void FromSample( uint16& a, sample b )
   {
      a = uint16( RoundInt( pcl::Abs( b )*uint16_max ) );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit signed integer.
    */
   static void FromSample( int16& a, sample b )
   {
      a = int16( RoundInt( pcl::Abs( b )*uint16_max ) + int16_min );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit unsigned integer.
    */
   static void FromSample( uint32& a, sample b )
   {
      a = uint32( Round( double( pcl::Abs( b ) )*uint32_max ) );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit signed integer.
    */
   static void FromSample( int32& a, sample b )
   {
      a = int32( Round( double( pcl::Abs( b ) )*uint32_max ) + int32_min );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit floating point real.
    */
   static void FromSample( float& a, sample b )
   {
      a = float( pcl::Abs( b ) );
   }

   /*!
    * Conversion of a pixel sample value to a 64-bit floating point real.
    */
   static void FromSample( double& a, sample b )
   {
      a = double( pcl::Abs( b ) );
   }

   /*!
    * Conversion of a pixel sample value to any complex type.
    */
   template <typename T>
   static void FromSample( Complex<T>& a, sample b )
   {
      a = Complex<T>( b );
   }

   /*!
    * Copies a T value \a b to a pixel sample variable \a a, with implicit
    * conversion from the source data type T to the pixel sample type.
    */
   template <typename T>
   static void Mov( sample& a, T b )
   {
      a = ToSample( b );
   }

   /*!
    * Adds a T value \a b to a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Add( sample& a, T b )
   {
      a += ToSample( b );
   }

   /*!
    * Subtracts a T value \a b from a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Sub( sample& a, T b )
   {
      a -= ToSample( b );
   }

   /*!
    * Multiplies a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Mul( sample& a, T b )
   {
      a *= ToSample( b );
   }

   /*!
    * Divides a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Div( sample& a, T b )
   {
      a /= ToSample( b );
   }

   /*!
    * Raises a pixel sample variable \a a to a T exponent value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Pow( sample& a, T b )
   {
      a = pcl::Pow( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a its absolute difference with a T
    * value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Dif( sample& a, T b )
   {
      a = pcl::Abs( a - ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the minimum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Min( sample& a, T b )
   {
      a = pcl::Min( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the maximum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Max( sample& a, T b )
   {
      a = pcl::Max( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive OR
    * operation with a T value \a b. The bitwise OR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Or( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ia | ib ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive NOR
    * operation with a T value \a b. The bitwise NOR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Nor( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~(ia | ib) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise AND operation with a
    * T value \a b. The bitwise AND operation is performed after converting
    * both operands to 8-bit unsigned integers, then the result is converted to
    * the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void And( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ia & ib ) );
   }

   /*!
    * Negates (bitwise NOT operation) a pixel sample variable \a a. Negation is
    * performed after converting the operand to an 8-bit unsigned integer,
    * then the result is converted to the pixel sample type before assignment.
    */
   static void Not( sample& a )
   {
      uint8 ia; FromSample( ia, a );
      a = ToSample( uint8( ~ia ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise negation (NOT
    * operation) of a T value \a b. Bitwise negation is performed after
    * converting both operands to 8-bit unsigned integers, then the result is
    * converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Not( sample& a, T b )
   {
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~ib ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise NAND operation with a
    * T value \a b. The bitwise NAND operation is performed after converting
    * both operands to 8-bit unsigned integers, then the result is converted to
    * the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Nand( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~(ia & ib) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive OR (XOR)
    * operation with a T value \a b. The bitwise XOR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Xor( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ia ^ ib ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive NOR (XNOR)
    * operation with a T value \a b. The bitwise XNOR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Xnor( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~(ia ^ ib) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorBurn( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_F, fb ), 1.0F ) );
   }

   static void ColorBurn( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_F, b ), 1.0F ) );
   }

   static void ColorBurn( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_D, b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void LinearBurn( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( fa + fb - 1 );
   }

   static void LinearBurn( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( fa + b - 1 );
   }

   static void LinearBurn( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa + b - 1 );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>screen</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Screen( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( 1 - (1 - fa)*(1 - fb) );
   }

   static void Screen( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( 1 - (1 - fa)*(1 - b) );
   }

   static void Screen( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - (1 - fa)*(1 - b) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color dodge</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorDodge( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_F, 1 - fb ), 1.0F ) );
   }

   static void ColorDodge( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_F, 1 - b ), 1.0F ) );
   }

   static void ColorDodge( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_D, 1 - b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>overlay</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Overlay( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fa > 0.5F) ? 1 - ((1 - 2*(fa - 0.5F)) * (1 - fb)) : 2*a*fb );
   }

   static void Overlay( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (fa > 0.5F) ? 1 - ((1 - 2*(fa - 0.5F)) * (1 - b)) : 2*a*b );
   }

   static void Overlay( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (fa > 0.5) ? 1 - ((1 - 2*(fa - 0.5)) * (1 - b)) : 2*a*b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>soft light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void SoftLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? 1 - (1 - fa)*(1 - fb - 0.5F) : a*(fb + 0.5F) );
   }

   static void SoftLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? 1 - (1 - fa)*(1 - b - 0.5F) : a*(b + 0.5F) );
   }

   static void SoftLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - (1 - fa)*(1 - b - 0.5) : a*(b + 0.5) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>hard light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void HardLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? 1 - (1 - fa)*(1 - 2*(fb - 0.5F)) : 2*fa*fb );
   }

   static void HardLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? 1 - (1 - fa)*(1 - 2*(b - 0.5F)) : 2*fa*b );
   }

   static void HardLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - (1 - fa)*(1 - 2*(b - 0.5)) : 2*fa*b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>vivid light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void VividLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? 1 - pcl::Max( (1 - fa)/(fb - 0.5F)/2, 1.0F ) : pcl::Min( fa/pcl::Max( EPSILON_F, 1 - 2*fb ), 1.0F ) );
   }

   static void VividLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? 1 - pcl::Max( (1 - fa)/(b - 0.5F)/2, 1.0F ) : pcl::Min( fa/pcl::Max( EPSILON_F, 1 - 2*b ), 1.0F ) );
   }

   static void VividLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - pcl::Max( (1 - fa)/(b - 0.5)/2, 1.0 ) : pcl::Min( fa/pcl::Max( EPSILON_D, 1 - 2*b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear light</em>
    * standard composition operation of its current value and a T value \a b,
    * with implicit data type conversion.
    */
   template <typename T>
   static void LinearLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? pcl::Max( fa + 2*(fb - 0.5F), 1.0F ) : pcl::Max( fa + 2*fb - 1, 1.0F ) );
   }

   static void LinearLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? pcl::Max( fa + 2*(b - 0.5F), 1.0F ) : pcl::Max( fa + 2*b - 1, 1.0F ) );
   }

   static void LinearLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? pcl::Max( fa + 2*(b - 0.5), 1.0 ) : pcl::Max( fa + 2*b - 1, 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>pin light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void PinLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? pcl::Max( fa, 2*(fb - 0.5F) ) : pcl::Min( fa, 2*fb ) );
   }

   static void PinLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? pcl::Max( fa, 2*(b - 0.5F) ) : pcl::Min( fa, 2*b ) );
   }

   static void PinLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? pcl::Max( fa, 2*(b - 0.5) ) : pcl::Min( fa, 2*b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>exclusion</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Exclusion( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Range( 0.5F - 2*(fa - 0.5F)*(fb - 0.5F), 0.0F, 1.0F ) );
   }

   static void Exclusion( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( pcl::Range( 0.5F - 2*(fa - 0.5F)*(b - 0.5F), 0.0F, 1.0F ) );
   }

   static void Exclusion( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Range( 0.5 - 2*(fa - 0.5)*(b - 0.5), 0.0, 1.0 ) );
   }

   // -------------------------------------------------------------------------

   IMPLEMENT_TRANSFER_OPERATIONS
};

// ----------------------------------------------------------------------------

/*!
 * \class DComplexPixelTraits
 * \brief 64-bit IEEE 754 normalized floating point complex pixel traits.
 *
 * %DComplexPixelTraits is a template instantiation of GenericPixelTraits for
 * the \c Complex<double> type. It defines the characteristic properties and
 * functionality of 64-bit IEEE 754 floating point complex pixel samples.
 *
 * \sa GenericPixelTraits, GenericImage, DComplexImage
 */
class PCL_CLASS DComplexPixelTraits : public GenericPixelTraits<Complex<double> >
{
public:

   /*!
    * Represents this template instantiation.
    */
   typedef GenericPixelTraits<Complex<double> > traits_type;

   /*!
    * Represents a pixel sample value.
    */
   typedef traits_type::sample                  sample;

   /*!
    * Represents a component of a complex pixel sample.
    */
   typedef sample::component                    component;

   /*!
    */
   static constexpr int BitsPerSample()
   {
      return sizeof( component ) << 3;
   }

   /*!
    * Returns true iff this pixel traits class corresponds to a floating point
    * real pixel sample type.
    */
   static constexpr bool IsFloatSample()
   {
      return false;
   }

   /*!
    * Returns true if this pixel traits class corresponds to a complex pixel
    * sample type; false if it represents a real pixel sample type.
    */
   static constexpr bool IsComplexSample()
   {
      return true;
   }

   /*!
    * Returns the address of a static null-terminated string identifying the
    * sample data type represented by this pixel traits class.
    *
    * For %DComplexPixelTraits, this member function returns "Complex64".
    */
   static constexpr const char* SampleFormat()
   {
      return "Complex64";
   }

   /*!
    * Returns the maximum valid pixel sample value.
    *
    * For %DComplexPixelTraits, this member function returns {1.0, 0.0F}.
    */
   static sample MaxSampleValue()
   {
      return sample( component( 1 ) );
   }

   /*!
    * Conversion of any floating point value to a pixel sample value.
    */
   template <typename T>
   static sample FloatToSample( T x )
   {
      return sample( component( x ) );
   }

   template <typename T>
   static sample FloatToSample( sample x )
   {
      return sample( x );
   }

   /*!
    * Conversion of an 8-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( component( x )/uint8_max );
#else
      return sample( component( pDLUT8[x] ) );
#endif
   }

   /*!
    * Conversion of an 8-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( (component( x ) - int8_min)/uint8_max );
#else
      return sample( component( pDLUT8[int( x ) - int8_min] ) );
#endif
   }

   /*!
    * Conversion of a 16-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( component( x )/uint16_max );
#else
      return sample( component( pDLUT16[x] ) );
#endif
   }

   /*!
    * Conversion of a 16-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( (component( x ) - int16_min)/uint16_max );
#else
      return sample( component( pDLUT16[int( x ) - int16_min] ) );
#endif
   }

   /*!
    * Conversion of a 32-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint32 x )
   {
      return sample( component( x )/uint32_max );
   }

   /*!
    * Conversion of a 32-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int32 x )
   {
      return sample( (component( x ) - int32_min)/uint32_max );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value.
    */
   static sample ToSample( float x )
   {
      return sample( component( x ) );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value.
    */
   static sample ToSample( double x )
   {
      return sample( component( x ) );
   }

   /*!
    * Conversion of any complex value to a pixel sample value.
    */
   template <typename T>
   static sample ToSample( const Complex<T>& x )
   {
      return sample( x );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit unsigned integer.
    */
   static void FromSample( uint8& a, sample b )
   {
      a = uint8( RoundInt( pcl::Abs( b )*uint8_max ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit signed integer.
    */
   static void FromSample( int8& a, sample b )
   {
      a = int8( RoundInt( pcl::Abs( b )*uint8_max ) + int8_min );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit signed integer.
    */
   static void FromSample( uint16& a, sample b )
   {
      a = uint16( RoundInt( pcl::Abs( b )*uint16_max ) );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit signed integer.
    */
   static void FromSample( int16& a, sample b )
   {
      a = int16( RoundInt( pcl::Abs( b )*uint16_max ) + int16_min );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit unsigned integer.
    */
   static void FromSample( uint32& a, sample b )
   {
      a = uint32( Round( component( pcl::Abs( b ) )*uint32_max ) );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit signed integer.
    */
   static void FromSample( int32& a, sample b )
   {
      a = int32( Round( component( pcl::Abs( b ) )*uint32_max ) + int32_min );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit floating point real.
    */
   static void FromSample( float& a, sample b )
   {
      a = float( pcl::Abs( b ) );
   }

   /*!
    * Conversion of a pixel sample value to a 64-bit floating point real.
    */
   static void FromSample( double& a, sample b )
   {
      a = double( pcl::Abs( b ) );
   }

   /*!
    * Conversion of a pixel sample value to any complex type.
    */
   template <typename T>
   static void FromSample( Complex<T>& a, sample b )
   {
      a = Complex<T>( b );
   }

   /*!
    * Copies a T value \a b to a pixel sample variable \a a, with implicit
    * conversion from the source data type T to the pixel sample type.
    */
   template <typename T>
   static void Mov( sample& a, T b )
   {
      a = ToSample( b );
   }

   /*!
    * Adds a T value \a b to a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Add( sample& a, T b )
   {
      a += ToSample( b );
   }

   /*!
    * Subtracts a T value \a b from a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Sub( sample& a, T b )
   {
      a -= ToSample( b );
   }

   /*!
    * Multiplies a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Mul( sample& a, T b )
   {
      a *= ToSample( b );
   }

   /*!
    * Divides a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Div( sample& a, T b )
   {
      a /= ToSample( b );
   }

   /*!
    * Raises a pixel sample variable \a a to a T exponent value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Pow( sample& a, T b )
   {
      a = pcl::Pow( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a its absolute difference with a T
    * value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Dif( sample& a, T b )
   {
      a = pcl::Abs( a - ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the minimum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Min( sample& a, T b )
   {
      a = pcl::Min( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the maximum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Max( sample& a, T b )
   {
      a = pcl::Max( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive OR
    * operation with a T value \a b. The bitwise OR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Or( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ia | ib ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive NOR
    * operation with a T value \a b. The bitwise NOR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Nor( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~(ia | ib) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise AND operation with a
    * T value \a b. The bitwise AND operation is performed after converting
    * both operands to 8-bit unsigned integers, then the result is converted to
    * the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void And( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ia & ib ) );
   }

   /*!
    * Negates (bitwise NOT operation) a pixel sample variable \a a. Negation is
    * performed after converting the operand to an 8-bit unsigned integer,
    * then the result is converted to the pixel sample type before assignment.
    */
   static void Not( sample& a )
   {
      uint8 ia; FromSample( ia, a );
      a = ToSample( uint8( ~ia ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise negation (NOT
    * operation) of a T value \a b. Bitwise negation is performed after
    * converting both operands to 8-bit unsigned integers, then the result is
    * converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Not( sample& a, T b )
   {
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~ib ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise NAND operation with a
    * T value \a b. The bitwise NAND operation is performed after converting
    * both operands to 8-bit unsigned integers, then the result is converted to
    * the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Nand( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~(ia & ib) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive OR (XOR)
    * operation with a T value \a b. The bitwise XOR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Xor( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ia ^ ib ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive NOR (XNOR)
    * operation with a T value \a b. The bitwise XNOR operation is performed
    * after converting both operands to 8-bit unsigned integers, then the
    * result is converted to the pixel sample type and assigned to \a a.
    */
   template <typename T>
   static void Xnor( sample& a, T b )
   {
      uint8 ia; FromSample( ia, a );
      uint8 ib; FromSample( ib, ToSample( b ) );
      a = ToSample( uint8( ~(ia ^ ib) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorBurn( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_D, fb ), 1.0 ) );
   }

   static void ColorBurn( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_D, double( b ) ), 1.0 ) );
   }

   static void ColorBurn( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_D, b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void LinearBurn( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( fa + fb - 1 );
   }

   static void LinearBurn( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa + double( b ) - 1 );
   }

   static void LinearBurn( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa + b - 1 );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>screen</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Screen( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( 1 - (1 - fa)*(1 - fb) );
   }

   static void Screen( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - (1 - fa)*(1 - double( b )) );
   }

   static void Screen( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - (1 - fa)*(1 - b) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color dodge</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorDodge( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_D, 1 - fb ), 1.0 ) );
   }

   static void ColorDodge( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_D, 1 - double( b ) ), 1.0 ) );
   }

   static void ColorDodge( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_D, 1 - b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>overlay</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Overlay( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fa > 0.5) ? 1 - ((1 - 2*(fa - 0.5)) * (1 - fb)) : 2*a*fb );
   }

   static void Overlay( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (fa > 0.5) ? 1 - ((1 - 2*(fa - 0.5)) * (1 - double( b ))) : 2*a*double( b ) );
   }

   static void Overlay( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (fa > 0.5) ? 1 - ((1 - 2*(fa - 0.5)) * (1 - b)) : 2*a*b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>soft light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void SoftLight( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5) ? 1 - (1 - fa)*(1 - fb - 0.5) : a*(fb + 0.5) );
   }

   static void SoftLight( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      double fb = double( b );
      a = ToSample( (fb > 0.5) ? 1 - (1 - fa)*(1 - fb - 0.5) : a*(fb + 0.5) );
   }

   static void SoftLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - (1 - fa)*(1 - b - 0.5) : a*(b + 0.5) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>hard light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void HardLight( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5) ? 1 - (1 - fa)*(1 - 2*(fb - 0.5)) : 2*fa*fb );
   }

   static void HardLight( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      double fb = double( b );
      a = ToSample( (fb > 0.5) ? 1 - (1 - fa)*(1 - 2*(fb - 0.5)) : 2*fa*fb );
   }

   static void HardLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - (1 - fa)*(1 - 2*(b - 0.5)) : 2*fa*b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>vivid light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void VividLight( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5) ? 1 - pcl::Max( (1 - fa)/(fb - 0.5)/2, 1.0 ) : pcl::Min( fa/pcl::Max( EPSILON_D, 1 - 2*fb ), 1.0 ) );
   }

   static void VividLight( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      double fb = double( b );
      a = ToSample( (fb > 0.5) ? 1 - pcl::Max( (1 - fa)/(fb - 0.5)/2, 1.0 ) : pcl::Min( fa/pcl::Max( EPSILON_D, 1 - 2*fb ), 1.0 ) );
   }

   static void VividLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - pcl::Max( (1 - fa)/(b - 0.5)/2, 1.0 ) : pcl::Min( fa/pcl::Max( EPSILON_D, 1 - 2*b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear light</em>
    * standard composition operation of its current value and a T value \a b,
    * with implicit data type conversion.
    */
   template <typename T>
   static void LinearLight( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5) ? pcl::Max( fa + 2*(fb - 0.5), 1.0 ) : pcl::Max( fa + 2*fb - 1, 1.0 ) );
   }

   static void LinearLight( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      double fb = double( b );
      a = ToSample( (fb > 0.5) ? pcl::Max( fa + 2*(fb - 0.5), 1.0 ) : pcl::Max( fa + 2*fb - 1, 1.0 ) );
   }

   static void LinearLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? pcl::Max( fa + 2*(b - 0.5), 1.0 ) : pcl::Max( fa + 2*b - 1, 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>pin light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void PinLight( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5) ? pcl::Max( fa, 2*(fb - 0.5) ) : pcl::Min( fa, 2*fb ) );
   }

   static void PinLight( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      double fb = double( b );
      a = ToSample( (fb > 0.5) ? pcl::Max( fa, 2*(fb - 0.5) ) : pcl::Min( fa, 2*fb ) );
   }

   static void PinLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? pcl::Max( fa, 2*(b - 0.5) ) : pcl::Min( fa, 2*b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>exclusion</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Exclusion( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Range( 0.5 - 2*(fa - 0.5)*(fb - 0.5), 0.0, 1.0 ) );
   }

   static void Exclusion( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Range( 0.5 - 2*(fa - 0.5)*(double( b ) - 0.5), 0.0, 1.0 ) );
   }

   static void Exclusion( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Range( 0.5 - 2*(fa - 0.5)*(b - 0.5), 0.0, 1.0 ) );
   }

   // -------------------------------------------------------------------------

   IMPLEMENT_TRANSFER_OPERATIONS
};

// ----------------------------------------------------------------------------

/*!
 * \class UInt8PixelTraits
 * \brief 8-bit unsigned integer pixel traits.
 *
 * %UInt8PixelTraits is a template instantiation of GenericPixelTraits for the
 * \c uint8 type. It defines the characteristic properties and functionality of
 * 8-bit unsigned integer pixel samples.
 *
 * \sa GenericPixelTraits, GenericImage, UInt8Image
 */
class PCL_CLASS UInt8PixelTraits : public GenericPixelTraits<uint8>
{
public:

   /*!
    * Represents this template instantiation.
    */
   typedef GenericPixelTraits<uint8>   traits_type;

   /*!
    * Represents a pixel sample value.
    */
   typedef traits_type::sample         sample;

   /*!
    * Returns true iff this pixel traits class corresponds to a floating point
    * real pixel sample type.
    */
   static constexpr bool IsFloatSample()
   {
      return false;
   }

   /*!
    * Returns true if this pixel traits class corresponds to a complex pixel
    * sample type; false if it represents a real pixel sample type.
    */
   static constexpr bool IsComplexSample()
   {
      return false;
   }

   /*!
    * Returns the address of a static null-terminated string identifying the
    * sample data type represented by this pixel traits class.
    *
    * For %UInt8PixelTraits, this member function returns "UInt8".
    */
   static constexpr const char* SampleFormat()
   {
      return "UInt8";
   }

   /*!
    * Returns the maximum valid pixel sample value.
    *
    * For %UInt8PixelTraits, this member function returns 255.
    */
   static constexpr sample MaxSampleValue()
   {
      return uint8_max;
   }

   /*!
    * Conversion of any floating point value to a pixel sample value.
    */
   template <typename T>
   static sample FloatToSample( T x )
   {
#ifdef __PCL_ENFORCE_PIXTRAITS_FLOAT_RANGE
      return sample( pcl::Range( Round( x ), T( 0 ), T( uint8_max ) ) );
#else
      // ### N.B.: x must be in the range [0,uint8_max].
      return sample( RoundInt( x ) );
#endif
   }

   /*!
    * Conversion of an 8-bit unsigned integer value to a pixel sample value.
    */
   static constexpr sample ToSample( uint8 x )
   {
      return sample( x );
   }

   /*!
    * Conversion of an 8-bit signed integer value to a pixel sample value.
    */
   static constexpr sample ToSample( int8 x )
   {
      return sample( int32( x ) - int32( int8_min ) );
   }

   /*!
    * Conversion of a 16-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( Round( double( x )*uint16_to_uint8 ) );
#else
      return p8LUT16[x];
#endif
   }

   /*!
    * Conversion of a 16-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( Round( (double( x ) - int16_min)*uint16_to_uint8 ) );
#else
      return p8LUT16[int( x ) - int16_min];
#endif
   }

   /*!
    * Conversion of a 32-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint32 x )
   {
      return sample( Round( double( x )*uint32_to_uint8 ) );
   }

   /*!
    * Conversion of a 32-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int32 x )
   {
      return sample( Round( (double( x ) - int32_min)*uint32_to_uint8 ) );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value.
    */
   static sample ToSample( float x )
   {
      return FloatToSample( x*uint8_max );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value. This
    * function guarantees that the result will never overflow as a result of an
    * out-of-range argument value.
    */
   static sample ToSampleConstrained( float x )
   {
      return FloatToSample( pcl::Range( x, 0.0F, 1.0F )*uint8_max );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value.
    */
   static sample ToSample( double x )
   {
      return FloatToSample( x*uint8_max );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value. This
    * function guarantees that the result will never overflow as a result of an
    * out-of-range argument value.
    */
   static sample ToSampleConstrained( double x )
   {
      return FloatToSample( pcl::Range( x, 0.0, 1.0 )*uint8_max );
   }

   /*!
    * Conversion of any complex value to a pixel sample value.
    */
   template <typename T>
   static sample ToSample( const Complex<T>& x )
   {
      return ToSample( pcl::Abs( x ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit unsigned integer.
    */
   static void FromSample( uint8& a, sample b )
   {
      a = uint8( b );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit signed integer.
    */
   static void FromSample( int8& a, sample b )
   {
      a = int8( int32( b ) + int32( int8_min ) );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit unsigned integer.
    */
   static void FromSample( uint16& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = uint16( b )*uint8_to_uint16;
#else
      a = p16LUT8[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit signed integer.
    */
   static void FromSample( int16& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = int16( int32( uint16( b )*uint8_to_uint16 ) + int32( int16_min ) );
#else
      a = int16( int32( p16LUT8[b] ) + int32( int16_min ) );
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit unsigned integer.
    */
   static void FromSample( uint32& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = uint32( b )*uint8_to_uint32;
#else
      a = p32LUT8[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit signed integer.
    */
   static void FromSample( int32& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = int32( double( uint32( b )*uint8_to_uint32 ) + int32_min );
#else
      a = int32( int64( p32LUT8[b] ) + int64( int32_min ) );
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit floating point real.
    */
   static void FromSample( float& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = float( b )/uint8_max;
#else
      a = pFLUT8[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 64-bit floating point real.
    */
   static void FromSample( double& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = double( b )/uint8_max;
#else
      a = pDLUT8[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to any complex type.
    */
   template <typename T>
   static void FromSample( Complex<T>& a, sample b )
   {
      typename Complex<T>::component c;
      FromSample( c, b );
      a = c;
   }

   /*!
    * Copies a T value \a b to a pixel sample variable \a a, with implicit
    * conversion from the source data type T to the pixel sample type.
    */
   template <typename T>
   static void Mov( sample& a, T b )
   {
      a = ToSample( b );
   }

   /*!
    * Adds a T value \a b to a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Add( sample& a, T b )
   {
      a = sample( pcl::Range( uint32( a ) + uint32( ToSample( b ) ),
                  uint32( 0 ), uint32( uint8_max ) ) );
   }

   /*!
    * Subtracts a T value \a b from a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Sub( sample& a, T b )
   {
      a = sample( pcl::Range( int32( a ) - int32( ToSample( b ) ),
                  int32( 0 ), int32( uint8_max ) ) );
   }

   /*!
    * Multiplies a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Mul( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( fa * fb );
   }

   /*! #
    */
   static void Mul( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( fa * b );
   }

   /*! #
    */
   static void Mul( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa * b );
   }

   /*! #
    */
   static void Mul( sample& a, pcl::Complex<float> b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( fa * b );
   }

   /*! #
    */
   static void Mul( sample& a, pcl::Complex<double> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa * b );
   }

   /*!
    * Divides a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Div( sample& a, T b )
   {
      a = FloatToSample( float( a )/float( ToSample( b ) ) );
   }

   /*! #
    */
   static void Div( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( fa / b );
   }

   /*! #
    */
   static void Div( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa / b );
   }

   /*! #
    */
   static void Div( sample& a, pcl::Complex<float> b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( fa / b );
   }

   /*! #
    */
   static void Div( sample& a, pcl::Complex<double> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa / b );
   }

   /*!
    * Raises a pixel sample variable \a a to a T exponent value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Pow( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Pow( fa, fb ) );
   }

   /*! #
    */
   static void Pow( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( pcl::Pow( fa, b ) );
   }

   /*! #
    */
   static void Pow( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Pow( fa, b ) );
   }

   /*! #
    */
   static void Pow( sample& a, pcl::Complex<float> b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( pcl::Pow( fa, b ) );
   }

   /*! #
    */
   static void Pow( sample& a, pcl::Complex<double> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Pow( fa, b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a its absolute difference with a T
    * value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Dif( sample& a, T b )
   {
      a = sample( pcl::Range( pcl::Abs( int32( a ) - int32( ToSample( b ) ) ),
                  int32( 0 ), int32( uint8_max ) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the minimum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Min( sample& a, T b )
   {
      a = pcl::Min( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the maximum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Max( sample& a, T b )
   {
      a = pcl::Max( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive OR
    * operation with a T value \a b. The bitwise OR operation is performed
    * after converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Or( sample& a, T b )
   {
      a |= ToSample( b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive NOR
    * operation with a T value \a b. The bitwise NOR operation is performed
    * after converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Nor( sample& a, T b )
   {
      a = ~(a | ToSample( b ));
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise AND operation with a
    * T value \a b. The bitwise AND operation is performed after converting
    * the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void And( sample& a, T b )
   {
      a &= ToSample( b );
   }

   /*!
    * Negates (bitwise NOT operation) a pixel sample variable \a a.
    */
   static void Not( sample& a )
   {
      a = sample( ~a );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise negation (NOT
    * operation) of a T value \a b. Bitwise negation is performed after
    * converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Not( sample& a, T b )
   {
      a = sample( ~ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise NAND operation with a
    * T value \a b. The bitwise NAND operation is performed after converting
    * the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Nand( sample& a, T b )
   {
      a = sample( ~(a & ToSample( b )) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive OR (XOR)
    * operation with a T value \a b. The bitwise XOR operation is performed
    * after converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Xor( sample& a, T b )
   {
      a ^= ToSample( b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive NOR (XNOR)
    * operation with a T value \a b. The bitwise XNOR operation is performed
    * after converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Xnor( sample& a, T b )
   {
      a = sample( ~(a ^ ToSample( b )) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorBurn( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_F, fb ), 1.0F ) );
   }

   static void ColorBurn( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_F, b ), 1.0F ) );
   }

   static void ColorBurn( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_D, b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void LinearBurn( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( fa + fb - 1 );
   }

   static void LinearBurn( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( fa + b - 1 );
   }

   static void LinearBurn( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa + b - 1 );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>screen</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Screen( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( 1 - (1 - fa)*(1 - fb) );
   }

   static void Screen( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( 1 - (1 - fa)*(1 - b) );
   }

   static void Screen( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - (1 - fa)*(1 - b) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color dodge</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorDodge( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_F, 1 - fb ), 1.0F ) );
   }

   static void ColorDodge( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_F, 1 - b ), 1.0F ) );
   }

   static void ColorDodge( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_D, 1 - b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>overlay</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Overlay( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fa > 0.5F) ? 1 - ((1 - 2*(fa - 0.5F)) * (1 - fb)) : 2*a*fb );
   }

   static void Overlay( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (fa > 0.5F) ? 1 - ((1 - 2*(fa - 0.5F)) * (1 - b)) : 2*a*b );
   }

   static void Overlay( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (fa > 0.5) ? 1 - ((1 - 2*(fa - 0.5)) * (1 - b)) : 2*a*b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>soft light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void SoftLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? 1 - (1 - fa)*(1 - fb - 0.5F) : a*(fb + 0.5F) );
   }

   static void SoftLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? 1 - (1 - fa)*(1 - b - 0.5F) : a*(b + 0.5F) );
   }

   static void SoftLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - (1 - fa)*(1 - b - 0.5) : a*(b + 0.5) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>hard light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void HardLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? 1 - (1 - fa)*(1 - 2*(fb - 0.5F)) : 2*fa*fb );
   }

   static void HardLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? 1 - (1 - fa)*(1 - 2*(b - 0.5F)) : 2*fa*b );
   }

   static void HardLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - (1 - fa)*(1 - 2*(b - 0.5)) : 2*fa*b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>vivid light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void VividLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? 1 - pcl::Max( (1 - fa)/(fb - 0.5F)/2, 1.0F ) : pcl::Min( fa/pcl::Max( EPSILON_F, 1 - 2*fb ), 1.0F ) );
   }

   static void VividLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? 1 - pcl::Max( (1 - fa)/(b - 0.5F)/2, 1.0F ) : pcl::Min( fa/pcl::Max( EPSILON_F, 1 - 2*b ), 1.0F ) );
   }

   static void VividLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - pcl::Max( (1 - fa)/(b - 0.5)/2, 1.0 ) : pcl::Min( fa/pcl::Max( EPSILON_D, 1 - 2*b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear light</em>
    * standard composition operation of its current value and a T value \a b,
    * with implicit data type conversion.
    */
   template <typename T>
   static void LinearLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? pcl::Max( fa + 2*(fb - 0.5F), 1.0F ) : pcl::Max( fa + 2*fb - 1, 1.0F ) );
   }

   static void LinearLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? pcl::Max( fa + 2*(b - 0.5F), 1.0F ) : pcl::Max( fa + 2*b - 1, 1.0F ) );
   }

   static void LinearLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? pcl::Max( fa + 2*(b - 0.5), 1.0 ) : pcl::Max( fa + 2*b - 1, 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>pin light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void PinLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? pcl::Max( fa, 2*(fb - 0.5F) ) : pcl::Min( fa, 2*fb ) );
   }

   static void PinLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? pcl::Max( fa, 2*(b - 0.5F) ) : pcl::Min( fa, 2*b ) );
   }

   static void PinLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? pcl::Max( fa, 2*(b - 0.5) ) : pcl::Min( fa, 2*b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>exclusion</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Exclusion( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Range( 0.5F - 2*(fa - 0.5F)*(fb - 0.5F), 0.0F, 1.0F ) );
   }

   static void Exclusion( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( pcl::Range( 0.5F - 2*(fa - 0.5F)*(b - 0.5F), 0.0F, 1.0F ) );
   }

   static void Exclusion( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Range( 0.5 - 2*(fa - 0.5)*(b - 0.5), 0.0, 1.0 ) );
   }

   // -------------------------------------------------------------------------

   IMPLEMENT_TRANSFER_OPERATIONS
};

// ----------------------------------------------------------------------------

/*!
 * \class UInt16PixelTraits
 * \brief 16-bit unsigned integer pixel traits.
 *
 * %UInt16PixelTraits is a template instantiation of GenericPixelTraits for the
 * \c uint16 type. It defines the characteristic properties and functionality
 * of 16-bit unsigned integer pixel samples.
 *
 * \sa GenericPixelTraits, GenericImage, UInt16Image
 */
class PCL_CLASS UInt16PixelTraits : public GenericPixelTraits<uint16>
{
public:

   /*!
    * Represents this template instantiation.
    */
   typedef GenericPixelTraits<uint16>  traits_type;

   /*!
    * Represents a pixel sample value.
    */
   typedef traits_type::sample         sample;

   /*!
    * Returns true iff this pixel traits class corresponds to a floating point
    * real pixel sample type.
    */
   static constexpr bool IsFloatSample()
   {
      return false;
   }

   /*!
    * Returns true if this pixel traits class corresponds to a complex pixel
    * sample type; false if it represents a real pixel sample type.
    */
   static constexpr bool IsComplexSample()
   {
      return false;
   }

   /*!
    * Returns the address of a static null-terminated string identifying the
    * sample data type represented by this pixel traits class.
    *
    * For %UInt16PixelTraits, this member function returns "UInt16".
    */
   static constexpr const char* SampleFormat()
   {
      return "UInt16";
   }

   /*!
    * Returns the maximum valid pixel sample value.
    *
    * For %UInt16PixelTraits, this member function returns 65535.
    */
   static constexpr sample MaxSampleValue()
   {
      return uint16_max;
   }

   /*!
    * Conversion of any floating point value to a pixel sample value.
    */
   template <typename T>
   static sample FloatToSample( T x )
   {
#ifdef __PCL_ENFORCE_PIXTRAITS_FLOAT_RANGE
      return sample( pcl::Range( Round( x ), T( 0 ), T( uint16_max ) ) );
#else
      // ### N.B.: x must be in the range [0,uint16_max].
      return sample( RoundInt( x ) );
#endif
   }

   /*!
    * Conversion of an 8-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( x )*sample( uint8_to_uint16 );
#else
      return p16LUT8[x];
#endif
   }

   /*!
    * Conversion of an 8-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( (int32( x ) - int32( int8_min ))*int32( uint8_to_uint16 ) );
#else
      return p16LUT8[int( x ) - int( int8_min )];
#endif
   }

   /*!
    * Conversion of a 16-bit unsigned integer value to a pixel sample value.
    */
   static constexpr sample ToSample( uint16 x )
   {
      return sample( x );
   }

   /*!
    * Conversion of a 16-bit signed integer value to a pixel sample value.
    */
   static constexpr sample ToSample( int16 x )
   {
      return sample( int32( x ) - int32( int16_min ) );
   }

   /*!
    * Conversion of a 32-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint32 x )
   {
      return sample( Round( double( x )*uint32_to_uint16 ) );
   }

   /*!
    * Conversion of a 32-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int32 x )
   {
      return sample( Round( (double( x ) - int32_min)*uint32_to_uint16 ) );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value.
    */
   static sample ToSample( float x )
   {
      return FloatToSample( x*uint16_max );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value. This
    * function guarantees that the result will never overflow as a result of an
    * out-of-range argument value.
    */
   static sample ToSampleConstrained( float x )
   {
      return FloatToSample( pcl::Range( x, 0.0F, 1.0F )*uint16_max );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value.
    */
   static sample ToSample( double x )
   {
      return FloatToSample( x*uint16_max );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value. This
    * function guarantees that the result will never overflow as a result of an
    * out-of-range argument value.
    */
   static sample ToSampleConstrained( double x )
   {
      return FloatToSample( pcl::Range( x, 0.0, 1.0 )*uint16_max );
   }

   /*!
    * Conversion of any complex value to a pixel sample value.
    */
   template <typename T>
   static constexpr sample ToSample( const Complex<T>& x )
   {
      return ToSample( pcl::Abs( x ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit unsigned integer.
    */
   static void FromSample( uint8& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = uint8( Round( double( b )*uint16_to_uint8 ) );
#else
      a = p8LUT16[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit signed integer.
    */
   static void FromSample( int8& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = int8( Round( double( b )*uint16_to_uint8 ) + int8_min );
#else
      a = int8( int( p8LUT16[b] ) + int( int8_min ) );
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit unsigned integer.
    */
   static void FromSample( uint16& a, sample b )
   {
      a = uint16( b );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit signed integer.
    */
   static void FromSample( int16& a, sample b )
   {
      a = int16( int32( b ) + int32( int16_min ) );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit unsigned integer.
    */
   static void FromSample( uint32& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = uint32( b )*uint16_to_uint32;
#else
      a = p32LUT16[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit signed integer.
    */
   static void FromSample( int32& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = int32( double( uint32( b )*uint16_to_uint32 ) + int32_min );
#else
      a = int32( int64( p32LUT16[b] ) + int64( int32_min ) );
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit floating point real.
    */
   static void FromSample( float& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = float( b )/uint16_max;
#else
      a = pFLUT16[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 64-bit floating point real.
    */
   static void FromSample( double& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = double( b )/uint16_max;
#else
      a = pDLUT16[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to any complex type.
    */
   template <typename T>
   static void FromSample( Complex<T>& a, sample b )
   {
      typename Complex<T>::component c;
      FromSample( c, b );
      a = c;
   }

   /*!
    * Copies a T value \a b to a pixel sample variable \a a, with implicit
    * conversion from the source data type T to the pixel sample type.
    */
   template <typename T>
   static void Mov( sample& a, T b )
   {
      a = ToSample( b );
   }

   /*!
    * Adds a T value \a b to a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Add( sample& a, T b )
   {
      a = sample( pcl::Range( uint32( a ) + uint32( ToSample( b ) ),
                  uint32( 0 ), uint32( uint16_max ) ) );
   }

   /*!
    * Subtracts a T value \a b from a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Sub( sample& a, T b )
   {
      a = sample( pcl::Range( int32( a ) - int32( ToSample( b ) ),
                  int32( 0 ), int32( uint16_max ) ) );
   }

   /*!
    * Multiplies a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Mul( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( fa * fb );
   }

   /*! #
    */
   static void Mul( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa * b );
   }

   /*! #
    */
   static void Mul( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa * b );
   }

   /*! #
    */
   static void Mul( sample& a, pcl::Complex<float> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa * b );
   }

   /*! #
    */
   static void Mul( sample& a, pcl::Complex<double> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa * b );
   }

   /*!
    * Divides a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Div( sample& a, T b )
   {
      a = FloatToSample( double( a )/double( ToSample( b ) ) );
   }

   /*! #
    */
   static void Div( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa / b );
   }

   /*! #
    */
   static void Div( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa / b );
   }

   /*! #
    */
   static void Div( sample& a, pcl::Complex<float> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa / b );
   }

   /*! #
    */
   static void Div( sample& a, pcl::Complex<double> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa / b );
   }

   /*!
    * Raises a pixel sample variable \a a to a T exponent value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Pow( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Pow( fa, fb ) );
   }

   /*! #
    */
   static void Pow( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Pow( fa, double( b ) ) );
   }

   /*! #
    */
   static void Pow( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Pow( fa, b ) );
   }

   /*! #
    */
   static void Pow( sample& a, pcl::Complex<float> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Pow( fa, pcl::Complex<double>( b ) ) );
   }

   /*! #
    */
   static void Pow( sample& a, pcl::Complex<double> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Pow( fa, b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a its absolute difference with a T
    * value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Dif( sample& a, T b )
   {
      a = sample( pcl::Range( pcl::Abs( int32( a ) - int32( ToSample( b ) ) ),
                  int32( 0 ), int32( uint16_max ) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the minimum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Min( sample& a, T b )
   {
      a = pcl::Min( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the maximum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Max( sample& a, T b )
   {
      a = pcl::Max( a, ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive OR
    * operation with a T value \a b. The bitwise OR operation is performed
    * after converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Or( sample& a, T b )
   {
      a |= ToSample( b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive NOR
    * operation with a T value \a b. The bitwise NOR operation is performed
    * after converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Nor( sample& a, T b )
   {
      a = ~(a | ToSample( b ));
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise AND operation with a
    * T value \a b. The bitwise AND operation is performed after converting
    * the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void And( sample& a, T b )
   {
      a &= ToSample( b );
   }

   /*!
    * Negates (bitwise NOT operation) a pixel sample variable \a a.
    */
   static void Not( sample& a )
   {
      a = sample( ~a );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise negation (NOT
    * operation) of a T value \a b. Bitwise negation is performed after
    * converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Not( sample& a, T b )
   {
      a = sample( ~ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise NAND operation with a
    * T value \a b. The bitwise NAND operation is performed after converting
    * the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Nand( sample& a, T b )
   {
      a = sample( ~(a & ToSample( b )) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive OR (XOR)
    * operation with a T value \a b. The bitwise XOR operation is performed
    * after converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Xor( sample& a, T b )
   {
      a ^= ToSample( b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive NOR (XNOR)
    * operation with a T value \a b. The bitwise XNOR operation is performed
    * after converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Xnor( sample& a, T b )
   {
      a = sample( ~(a ^ ToSample( b )) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorBurn( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_F, fb ), 1.0F ) );
   }

   static void ColorBurn( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_F, b ), 1.0F ) );
   }

   static void ColorBurn( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_D, b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void LinearBurn( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( fa + fb - 1 );
   }

   static void LinearBurn( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( fa + b - 1 );
   }

   static void LinearBurn( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa + b - 1 );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>screen</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Screen( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( 1 - (1 - fa)*(1 - fb) );
   }

   static void Screen( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( 1 - (1 - fa)*(1 - b) );
   }

   static void Screen( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - (1 - fa)*(1 - b) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color dodge</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorDodge( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_F, 1 - fb ), 1.0F ) );
   }

   static void ColorDodge( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_F, 1 - b ), 1.0F ) );
   }

   static void ColorDodge( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_D, 1 - b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>overlay</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Overlay( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fa > 0.5F) ? 1 - ((1 - 2*(fa - 0.5F)) * (1 - fb)) : 2*a*fb );
   }

   static void Overlay( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (fa > 0.5F) ? 1 - ((1 - 2*(fa - 0.5F)) * (1 - b)) : 2*a*b );
   }

   static void Overlay( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (fa > 0.5) ? 1 - ((1 - 2*(fa - 0.5)) * (1 - b)) : 2*a*b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>soft light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void SoftLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? 1 - (1 - fa)*(1 - fb - 0.5F) : a*(fb + 0.5F) );
   }

   static void SoftLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? 1 - (1 - fa)*(1 - b - 0.5F) : a*(b + 0.5F) );
   }

   static void SoftLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - (1 - fa)*(1 - b - 0.5) : a*(b + 0.5) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>hard light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void HardLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? 1 - (1 - fa)*(1 - 2*(fb - 0.5F)) : 2*fa*fb );
   }

   static void HardLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? 1 - (1 - fa)*(1 - 2*(b - 0.5F)) : 2*fa*b );
   }

   static void HardLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - (1 - fa)*(1 - 2*(b - 0.5)) : 2*fa*b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>vivid light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void VividLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? 1 - pcl::Max( (1 - fa)/(fb - 0.5F)/2, 1.0F ) : pcl::Min( fa/pcl::Max( EPSILON_F, 1 - 2*fb ), 1.0F ) );
   }

   static void VividLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? 1 - pcl::Max( (1 - fa)/(b - 0.5F)/2, 1.0F ) : pcl::Min( fa/pcl::Max( EPSILON_F, 1 - 2*b ), 1.0F ) );
   }

   static void VividLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - pcl::Max( (1 - fa)/(b - 0.5)/2, 1.0 ) : pcl::Min( fa/pcl::Max( EPSILON_D, 1 - 2*b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear light</em>
    * standard composition operation of its current value and a T value \a b,
    * with implicit data type conversion.
    */
   template <typename T>
   static void LinearLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? pcl::Max( fa + 2*(fb - 0.5F), 1.0F ) : pcl::Max( fa + 2*fb - 1, 1.0F ) );
   }

   static void LinearLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? pcl::Max( fa + 2*(b - 0.5F), 1.0F ) : pcl::Max( fa + 2*b - 1, 1.0F ) );
   }

   static void LinearLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? pcl::Max( fa + 2*(b - 0.5), 1.0 ) : pcl::Max( fa + 2*b - 1, 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>pin light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void PinLight( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5F) ? pcl::Max( fa, 2*(fb - 0.5F) ) : pcl::Min( fa, 2*fb ) );
   }

   static void PinLight( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( (b > 0.5F) ? pcl::Max( fa, 2*(b - 0.5F) ) : pcl::Min( fa, 2*b ) );
   }

   static void PinLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? pcl::Max( fa, 2*(b - 0.5) ) : pcl::Min( fa, 2*b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>exclusion</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Exclusion( sample& a, T b )
   {
      float fa; FromSample( fa, a );
      float fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Range( 0.5F - 2*(fa - 0.5F)*(fb - 0.5F), 0.0F, 1.0F ) );
   }

   static void Exclusion( sample& a, float b )
   {
      float fa; FromSample( fa, a );
      a = ToSample( pcl::Range( 0.5F - 2*(fa - 0.5F)*(b - 0.5F), 0.0F, 1.0F ) );
   }

   static void Exclusion( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Range( 0.5 - 2*(fa - 0.5)*(b - 0.5), 0.0, 1.0 ) );
   }

   // -------------------------------------------------------------------------

   IMPLEMENT_TRANSFER_OPERATIONS
};

// ----------------------------------------------------------------------------

/*!
 * \class UInt32PixelTraits
 * \brief 32-bit unsigned integer pixel traits.
 *
 * %UInt32PixelTraits is a template instantiation of GenericPixelTraits for the
 * \c uint32 type. It defines the characteristic properties and functionality
 * of 32-bit unsigned integer pixel samples.
 *
 * \sa GenericPixelTraits, GenericImage, UInt32Image
 */
class PCL_CLASS UInt32PixelTraits : public GenericPixelTraits<uint32>
{
public:

   /*!
    * Represents this template instantiation.
    */
   typedef GenericPixelTraits<uint32>  traits_type;

   /*!
    * Represents a pixel sample value.
    */
   typedef traits_type::sample         sample;

   /*!
    * Returns true iff this pixel traits class corresponds to a floating point
    * real pixel sample type.
    */
   static constexpr bool IsFloatSample()
   {
      return false;
   }

   /*!
    * Returns true if this pixel traits class corresponds to a complex pixel
    * sample type; false if it represents a real pixel sample type.
    */
   static constexpr bool IsComplexSample()
   {
      return false;
   }

   /*!
    * Returns the address of a static null-terminated string identifying the
    * sample data type represented by this pixel traits class.
    *
    * For %UInt32PixelTraits, this member function returns "UInt32".
    */
   static constexpr const char* SampleFormat()
   {
      return "UInt32";
   }

   /*!
    * Returns the maximum valid pixel sample value.
    *
    * For %UInt32PixelTraits, this member function returns 4294967295.
    */
   static constexpr sample MaxSampleValue()
   {
      return uint32_max;
   }

   /*!
    * Conversion of any floating point value to a pixel sample value.
    */
   template <typename T>
   static sample FloatToSample( T x )
   {
#ifdef __PCL_ENFORCE_PIXTRAITS_FLOAT_RANGE
      return sample( pcl::Range( Round( x ), T( 0 ), T( uint32_max ) ) );
#else
      // ### N.B.: x must be in the range [0,uint32_max].
      return sample( RoundI64( x ) );
#endif
   }

   /*!
    * Conversion of an 8-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( x )*sample( uint8_to_uint32 );
#else
      return p32LUT8[x];
#endif
   }

   /*!
    * Conversion of an 8-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( uint32( int32( x ) - int32( int8_min ) )*uint8_to_uint32 );
#else
      return p32LUT8[int( x ) - int( int8_min )];
#endif
   }

   /*!
    * Conversion of a 16-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( x )*sample( uint16_to_uint32 );
#else
      return p32LUT16[x];
#endif
   }

   /*!
    * Conversion of a 16-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( uint32( int32( x ) - int32( int16_min ) )*uint16_to_uint32 );
#else
      return p32LUT16[int( x ) - int( int16_min )];
#endif
   }

   /*!
    * Conversion of a 32-bit unsigned integer value to a pixel sample value.
    */
   static constexpr sample ToSample( uint32 x )
   {
      return sample( x );
   }

   /*!
    * Conversion of a 32-bit signed integer value to a pixel sample value.
    */
   static constexpr sample ToSample( int32 x )
   {
      return sample( double( x ) - double( int32_min ) );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value.
    */
   static sample ToSample( float x )
   {
      return FloatToSample( double( x )*uint32_max );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value. This
    * function guarantees that the result will never overflow as a result of an
    * out-of-range argument value.
    */
   static sample ToSampleConstrained( float x )
   {
      return FloatToSample( pcl::Range( double( x ), 0.0, 1.0 )*uint32_max );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value.
    */
   static sample ToSample( double x )
   {
      return FloatToSample( x*uint32_max );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value. This
    * function guarantees that the result will never overflow as a result of an
    * out-of-range argument value.
    */
   static sample ToSampleConstrained( double x )
   {
      return FloatToSample( pcl::Range( x, 0.0, 1.0 )*uint32_max );
   }

   /*!
    * Conversion of any complex value to a pixel sample value.
    */
   template <typename T>
   static sample ToSample( const Complex<T>& x )
   {
      return ToSample( pcl::Abs( x ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit unsigned integer.
    */
   static void FromSample( uint8& a, sample b )
   {
      a = uint8( RoundInt( double( b )*uint32_to_uint8 ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit signed integer.
    */
   static void FromSample( int8& a, sample b )
   {
      a = int8( RoundInt( double( b )*uint32_to_uint8 ) + int8_min );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit unsigned integer.
    */
   static void FromSample( uint16& a, sample b )
   {
      a = uint16( RoundInt( double( b )*uint32_to_uint16 ) );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit signed integer.
    */
   static void FromSample( int16& a, sample b )
   {
      a = int16( RoundInt( double( b )*uint32_to_uint16 ) + int16_min );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit unsigned integer.
    */
   static void FromSample( uint32& a, sample b )
   {
      a = uint32( b );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit signed integer.
    */
   static void FromSample( int32& a, sample b )
   {
      a = TruncInt( double( b ) + int32_min );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit floating point real.
    */
   static void FromSample( float& a, sample b )
   {
      a = float( double( b )/uint32_max );
   }

   /*!
    * Conversion of a pixel sample value to a 64-bit floating point real.
    */
   static void FromSample( double& a, sample b )
   {
      a = double( b )/uint32_max;
   }

   /*!
    * Conversion of a pixel sample value to any complex type.
    */
   template <typename T>
   static void FromSample( Complex<T>& a, sample b )
   {
      typename Complex<T>::component c;
      FromSample( c, b );
      a = c;
   }

   /*!
    * Copies a T value \a b to a pixel sample variable \a a, with implicit
    * conversion from the source data type T to the pixel sample type.
    */
   template <typename T>
   static void Mov( sample& a, T b )
   {
      a = ToSample( b );
   }

   /*!
    * Adds a T value \a b to a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Add( sample& a, T b )
   {
      a = sample( pcl::Range( double( a ) + double( ToSample( b ) ),
                  0.0, double( uint32_max ) ) );
   }

   /*!
    * Subtracts a T value \a b from a pixel sample variable \a a, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Sub( sample& a, T b )
   {
      a = sample( pcl::Range( double( a ) - double( ToSample( b ) ),
                  0.0, double( uint32_max ) ) );
   }

   /*!
    * Multiplies a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Mul( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( fa * fb );
   }

   /*! #
    */
   static void Mul( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa * b );
   }

   /*! #
    */
   static void Mul( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa * b );
   }

   /*! #
    */
   static void Mul( sample& a, pcl::Complex<float> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa * b );
   }

   /*! #
    */
   static void Mul( sample& a, pcl::Complex<double> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa * b );
   }

   /*!
    * Divides a pixel sample variable \a a by a T value \a b, with implicit
    * data type conversion.
    */
   template <typename T>
   static void Div( sample& a, T b )
   {
      a = FloatToSample( double( a )/double( ToSample( b ) ) );
   }

   /*! #
    */
   static void Div( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa / b );
   }

   /*! #
    */
   static void Div( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa / b );
   }

   /*! #
    */
   static void Div( sample& a, pcl::Complex<float> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa / b );
   }

   /*! #
    */
   static void Div( sample& a, pcl::Complex<double> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa / b );
   }

   /*!
    * Raises a pixel sample variable \a a to a T exponent value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Pow( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Pow( fa, fb ) );
   }

   /*! #
    */
   static void Pow( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Pow( fa, double( b ) ) );
   }

   /*! #
    */
   static void Pow( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Pow( fa, b ) );
   }

   /*! #
    */
   static void Pow( sample& a, pcl::Complex<float> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Pow( fa, pcl::Complex<double>( b ) ) );
   }

   /*! #
    */
   static void Pow( sample& a, pcl::Complex<double> b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Pow( fa, b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a its absolute difference with a T
    * value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Dif( sample& a, T b )
   {
      a = sample( pcl::Range( pcl::Abs( double( a ) - double( ToSample( b ) ) ),
                  0.0, double( uint32_max ) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the minimum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Min( sample& a, T b )
   {
      a = sample( pcl::Min( a, ToSample( b ) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the maximum of its current value
    * and a T value \a b, with implicit data type conversion.
    */
   template <typename T>
   static void Max( sample& a, T b )
   {
      a = sample( pcl::Max( a, ToSample( b ) ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive OR
    * operation with a T value \a b. The bitwise OR operation is performed
    * after converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Or( sample& a, T b )
   {
      a |= ToSample( b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise inclusive NOR
    * operation with a T value \a b. The bitwise NOR operation is performed
    * after converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Nor( sample& a, T b )
   {
      a = ~(a | ToSample( b ));
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise AND operation with a
    * T value \a b. The bitwise AND operation is performed after converting
    * the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void And( sample& a, T b )
   {
      a &= ToSample( b );
   }

   /*!
    * Negates (bitwise NOT operation) a pixel sample variable \a a.
    */
   static void Not( sample& a )
   {
      a = sample( ~a );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise negation (NOT
    * operation) of a T value \a b. Bitwise negation is performed after
    * converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Not( sample& a, T b )
   {
      a = sample( ~ToSample( b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise NAND operation with a
    * T value \a b. The bitwise NAND operation is performed after converting
    * the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Nand( sample& a, T b )
   {
      a = sample( ~(a & ToSample( b )) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive OR (XOR)
    * operation with a T value \a b. The bitwise XOR operation is performed
    * after converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Xor( sample& a, T b )
   {
      a ^= ToSample( b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the bitwise exclusive NOR (XNOR)
    * operation with a T value \a b. The bitwise XNOR operation is performed
    * after converting the right-hand side operand \a b to the pixel sample type.
    */
   template <typename T>
   static void Xnor( sample& a, T b )
   {
      a = sample( ~(a ^ ToSample( b )) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorBurn( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_D, fb ), 1.0 ) );
   }

   static void ColorBurn( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_D, double( b ) ), 1.0 ) );
   }

   static void ColorBurn( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - pcl::Min( (1 - fa)/pcl::Max( EPSILON_D, b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear burn</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void LinearBurn( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( fa + fb - 1 );
   }

   static void LinearBurn( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa + double( b ) - 1 );
   }

   static void LinearBurn( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( fa + b - 1 );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>screen</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Screen( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( 1 - (1 - fa)*(1 - fb) );
   }

   static void Screen( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - (1 - fa)*(1 - double( b )) );
   }

   static void Screen( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( 1 - (1 - fa)*(1 - b) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>color dodge</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void ColorDodge( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_D, 1 - fb ), 1.0 ) );
   }

   static void ColorDodge( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_D, 1 - double( b ) ), 1.0 ) );
   }

   static void ColorDodge( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Min( fa/pcl::Max( EPSILON_D, 1 - b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>overlay</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Overlay( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fa > 0.5) ? 1 - ((1 - 2*(fa - 0.5)) * (1 - fb)) : 2*a*fb );
   }

   static void Overlay( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (fa > 0.5) ? 1 - ((1 - 2*(fa - 0.5)) * (1 - double( b ))) : 2*a*double( b ) );
   }

   static void Overlay( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (fa > 0.5) ? 1 - ((1 - 2*(fa - 0.5)) * (1 - b)) : 2*a*b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>soft light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void SoftLight( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5) ? 1 - (1 - fa)*(1 - fb - 0.5) : a*(fb + 0.5) );
   }

   static void SoftLight( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      double fb = double( b );
      a = ToSample( (fb > 0.5) ? 1 - (1 - fa)*(1 - fb - 0.5) : a*(fb + 0.5) );
   }

   static void SoftLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - (1 - fa)*(1 - b - 0.5) : a*(b + 0.5) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>hard light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void HardLight( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5) ? 1 - (1 - fa)*(1 - 2*(fb - 0.5)) : 2*fa*fb );
   }

   static void HardLight( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      double fb = double( b );
      a = ToSample( (fb > 0.5) ? 1 - (1 - fa)*(1 - 2*(fb - 0.5)) : 2*fa*fb );
   }

   static void HardLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - (1 - fa)*(1 - 2*(b - 0.5)) : 2*fa*b );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>vivid light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void VividLight( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5) ? 1 - pcl::Max( (1 - fa)/(fb - 0.5)/2, 1.0 ) : pcl::Min( fa/pcl::Max( EPSILON_D, 1 - 2*fb ), 1.0 ) );
   }

   static void VividLight( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      double fb = double( b );
      a = ToSample( (fb > 0.5) ? 1 - pcl::Max( (1 - fa)/(fb - 0.5)/2, 1.0 ) : pcl::Min( fa/pcl::Max( EPSILON_D, 1 - 2*fb ), 1.0 ) );
   }

   static void VividLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? 1 - pcl::Max( (1 - fa)/(b - 0.5)/2, 1.0 ) : pcl::Min( fa/pcl::Max( EPSILON_D, 1 - 2*b ), 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>linear light</em>
    * standard composition operation of its current value and a T value \a b,
    * with implicit data type conversion.
    */
   template <typename T>
   static void LinearLight( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5) ? pcl::Max( fa + 2*(fb - 0.5), 1.0 ) : pcl::Max( fa + 2*fb - 1, 1.0 ) );
   }

   static void LinearLight( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      double fb = double( b );
      a = ToSample( (fb > 0.5) ? pcl::Max( fa + 2*(fb - 0.5), 1.0 ) : pcl::Max( fa + 2*fb - 1, 1.0 ) );
   }

   static void LinearLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? pcl::Max( fa + 2*(b - 0.5), 1.0 ) : pcl::Max( fa + 2*b - 1, 1.0 ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>pin light</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void PinLight( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( (fb > 0.5) ? pcl::Max( fa, 2*(fb - 0.5) ) : pcl::Min( fa, 2*fb ) );
   }

   static void PinLight( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      double fb = double( b );
      a = ToSample( (fb > 0.5) ? pcl::Max( fa, 2*(fb - 0.5) ) : pcl::Min( fa, 2*fb ) );
   }

   static void PinLight( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( (b > 0.5) ? pcl::Max( fa, 2*(b - 0.5) ) : pcl::Min( fa, 2*b ) );
   }

   /*!
    * Assigns to a pixel sample variable \a a the <em>exclusion</em> standard
    * composition operation of its current value and a T value \a b, with
    * implicit data type conversion.
    */
   template <typename T>
   static void Exclusion( sample& a, T b )
   {
      double fa; FromSample( fa, a );
      double fb; FromSample( fb, ToSample( b ) );
      a = ToSample( pcl::Range( 0.5 - 2*(fa - 0.5)*(fb - 0.5), 0.0, 1.0 ) );
   }

   static void Exclusion( sample& a, float b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Range( 0.5 - 2*(fa - 0.5)*(double( b ) - 0.5), 0.0, 1.0 ) );
   }

   static void Exclusion( sample& a, double b )
   {
      double fa; FromSample( fa, a );
      a = ToSample( pcl::Range( 0.5 - 2*(fa - 0.5)*(b - 0.5), 0.0, 1.0 ) );
   }

   // -------------------------------------------------------------------------

   IMPLEMENT_TRANSFER_OPERATIONS
};

// ----------------------------------------------------------------------------

/*!
 * \class UInt20PixelTraits
 * \brief 20-bit unsigned integer pixel traits.
 *
 * %UInt20PixelTraits is a template instantiation of GenericPixelTraits for
 * unsigned 20-bit integers (stored as \c uint32 values).
 *
 * This class does not represent an instantiable pixel sample type. It is used
 * mainly for generation and management of 20-bit lookup tables (LUTs).
 *
 * \sa GenericPixelTraits
 */
class PCL_CLASS UInt20PixelTraits : public GenericPixelTraits<uint32>
{
public:

   /*!
    * Represents this template instantiation.
    */
   typedef GenericPixelTraits<uint32>  traits_type;

   /*!
    * Represents a pixel sample value.
    */
   typedef traits_type::sample         sample;

   /*!
    * Returns true iff this pixel traits class corresponds to a floating point
    * real pixel sample type.
    */
   static constexpr bool IsFloatSample()
   {
      return false;
   }

   /*!
    * Returns true if this pixel traits class corresponds to a complex pixel
    * sample type; false if it represents a real pixel sample type.
    */
   static constexpr bool IsComplexSample()
   {
      return false;
   }

   /*!
    * Returns the address of a static null-terminated string identifying the
    * sample data type represented by this pixel traits class.
    *
    * For %UInt20PixelTraits, this member function returns "UInt20".
    */
   static constexpr const char* SampleFormat()
   {
      return "UInt20";
   }

   /*!
    * Returns the maximum valid pixel sample value.
    *
    * For %UInt20PixelTraits, this member function returns 1048576.
    */
   static constexpr sample MaxSampleValue()
   {
      return uint20_max;
   }

   /*!
    * Conversion of any floating point value to a pixel sample value.
    */
   template <typename T>
   static sample FloatToSample( T x )
   {
#ifdef __PCL_ENFORCE_PIXTRAITS_FLOAT_RANGE
      return sample( pcl::Range( Round( x ), T( 0 ), T( uint20_max ) ) );
#else
      // ### N.B.: x must be in the range [0,uint20_max].
      return sample( RoundInt( x ) );
#endif
   }

   /*!
    * Conversion of an 8-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( RoundInt( x * uint8_to_uint20 ) );
#else
      return p20LUT8[x];
#endif
   }

   /*!
    * Conversion of an 8-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( RoundInt( (int32( x ) - int32( int8_min ))*uint8_to_uint20 ) );
#else
      return p20LUT8[int( x ) - int( int8_min )];
#endif
   }

   /*!
    * Conversion of a 16-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( RoundInt( x * uint16_to_uint20 ) );
#else
      return p20LUT16[x];
#endif
   }

   /*!
    * Conversion of a 16-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( RoundInt( (int32( x ) - int32( int16_min ))*uint16_to_uint20 ) );
#else
      return p20LUT16[int( x ) - int( int16_min )];
#endif
   }

   /*!
    * Conversion of a 32-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint32 x )
   {
      return sample( RoundInt( x * uint32_to_uint20 ) );
   }

   /*!
    * Conversion of a 32-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int32 x )
   {
      return sample( RoundInt( (double( x ) - int32_min)*uint32_to_uint20 ) );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value.
    */
   static sample ToSample( float x )
   {
      return FloatToSample( x*uint20_max );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value. This
    * function guarantees that the result will never overflow as a result of an
    * out-of-range argument value.
    */
   static sample ToSampleConstrained( float x )
   {
      return FloatToSample( pcl::Range( x, 0.0F, 1.0F )*uint20_max );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value.
    */
   static sample ToSample( double x )
   {
      return FloatToSample( x*uint20_max );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value. This
    * function guarantees that the result will never overflow as a result of an
    * out-of-range argument value.
    */
   static sample ToSampleConstrained( double x )
   {
      return FloatToSample( pcl::Range( x, 0.0, 1.0 )*uint20_max );
   }

   /*!
    * Conversion of any complex value to a pixel sample value.
    */
   template <typename T>
   static constexpr sample ToSample( const Complex<T>& x )
   {
      return ToSample( pcl::Abs( x ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit unsigned integer.
    */
   static void FromSample( uint8& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = uint8( RoundInt( b * uint20_to_uint8 ) );
#else
      a = p8LUT20[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit signed integer.
    */
   static void FromSample( int8& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = int8( RoundInt( b * uint20_to_uint8 ) + int8_min );
#else
      a = int8( int( p8LUT20[b] ) + int( int8_min ) );
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit unsigned integer.
    */
   static void FromSample( uint16& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = uint16( RoundInt( b * uint20_to_uint16 ) );
#else
      a = p16LUT20[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit signed integer.
    */
   static void FromSample( int16& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = int16( RoundInt( b * uint20_to_uint16 ) + int16_min );
#else
      a = int16( int( p16LUT20[b] ) + int( int16_min ) );
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit unsigned integer.
    */
   static void FromSample( uint32& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = uint32( RoundInt( b * uint20_to_uint32 ) );
#else
      a = p32LUT20[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit signed integer.
    */
   static void FromSample( int32& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = int32( RoundInt( b * uint20_to_uint32 ) + int32_min );
#else
      a = int32( int64( p32LUT20[b] ) + int64( int32_min ) );
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit floating point real.
    */
   static void FromSample( float& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = float( b )/uint20_max;
#else
      a = pFLUT20[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to a 64-bit floating point real.
    */
   static void FromSample( double& a, sample b )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      a = double( b )/uint20_max;
#else
      a = pDLUT20[b];
#endif
   }

   /*!
    * Conversion of a pixel sample value to any complex type.
    */
   template <typename T>
   static void FromSample( Complex<T>& a, sample b )
   {
      typename Complex<T>::component c;
      FromSample( c, b );
      a = c;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class UInt24PixelTraits
 * \brief 24-bit unsigned integer pixel traits.
 *
 * %UInt24PixelTraits is a template instantiation of GenericPixelTraits for
 * unsigned 24-bit integers (stored as \c uint32 values).
 *
 * This class does not represent an instantiable pixel sample type. It is used
 * mainly for generation and management of 24-bit lookup tables (LUTs).
 *
 * \sa GenericPixelTraits
 */
class PCL_CLASS UInt24PixelTraits : public GenericPixelTraits<uint32>
{
public:

   /*!
    * Represents this template instantiation.
    */
   typedef GenericPixelTraits<uint32>  traits_type;

   /*!
    * Represents a pixel sample value.
    */
   typedef traits_type::sample         sample;

   /*!
    * Returns true iff this pixel traits class corresponds to a floating point
    * real pixel sample type.
    */
   static constexpr bool IsFloatSample()
   {
      return false;
   }

   /*!
    * Returns true if this pixel traits class corresponds to a complex pixel
    * sample type; false if it represents a real pixel sample type.
    */
   static constexpr bool IsComplexSample()
   {
      return false;
   }

   /*!
    * Returns the address of a static null-terminated string identifying the
    * sample data type represented by this pixel traits class.
    *
    * For %UInt24PixelTraits, this member function returns "UInt24".
    */
   static constexpr const char* SampleFormat()
   {
      return "UInt24";
   }

   /*!
    * Returns the maximum valid pixel sample value.
    *
    * For %UInt24PixelTraits, this member function returns 1048576.
    */
   static constexpr sample MaxSampleValue()
   {
      return uint24_max;
   }

   /*!
    * Conversion of any floating point value to a pixel sample value.
    */
   template <typename T>
   static sample FloatToSample( T x )
   {
#ifdef __PCL_ENFORCE_PIXTRAITS_FLOAT_RANGE
      return sample( pcl::Range( Round( x ), T( 0 ), T( uint24_max ) ) );
#else
      // ### N.B.: x must be in the range [0,uint24_max].
      return sample( RoundInt( x ) );
#endif
   }

   /*!
    * Conversion of an 8-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( x * uint8_to_uint24 );
#else
      return p24LUT8[x];
#endif
   }

   /*!
    * Conversion of an 8-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int8 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( RoundInt( (int32( x ) - int32( int8_min ))*uint8_to_uint24 ) );
#else
      return p24LUT8[int( x ) - int( int8_min )];
#endif
   }

   /*!
    * Conversion of a 16-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( RoundInt( x * uint16_to_uint24 ) );
#else
      return p24LUT16[x];
#endif
   }

   /*!
    * Conversion of a 16-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int16 x )
   {
#ifdef __PCL_DONT_USE_PIXTRAITS_LUT
      return sample( RoundInt( (int32( x ) - int32( int16_min ))*uint16_to_uint24 ) );
#else
      return p24LUT16[int( x ) - int( int16_min )];
#endif
   }

   /*!
    * Conversion of a 32-bit unsigned integer value to a pixel sample value.
    */
   static sample ToSample( uint32 x )
   {
      return sample( RoundInt( x * uint32_to_uint24 ) );
   }

   /*!
    * Conversion of a 32-bit signed integer value to a pixel sample value.
    */
   static sample ToSample( int32 x )
   {
      return sample( RoundInt( (double( x ) - int32_min)*uint32_to_uint24 ) );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value.
    */
   static sample ToSample( float x )
   {
      return FloatToSample( double( x )*uint24_max );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value. This
    * function guarantees that the result will never overflow as a result of an
    * out-of-range argument value.
    */
   static sample ToSampleConstrained( float x )
   {
      return FloatToSample( pcl::Range( double( x ), 0.0, 1.0 )*uint24_max );
   }

   /*!
    * Conversion of a 64-bit floating point value to a pixel sample value.
    */
   static sample ToSample( double x )
   {
      return FloatToSample( x*uint24_max );
   }

   /*!
    * Conversion of a 32-bit floating point value to a pixel sample value. This
    * function guarantees that the result will never overflow as a result of an
    * out-of-range argument value.
    */
   static sample ToSampleConstrained( double x )
   {
      return FloatToSample( pcl::Range( x, 0.0, 1.0 )*uint24_max );
   }

   /*!
    * Conversion of any complex value to a pixel sample value.
    */
   template <typename T>
   static sample ToSample( const Complex<T>& x )
   {
      return ToSample( pcl::Abs( x ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit unsigned integer.
    */
   static void FromSample( uint8& a, sample b )
   {
      a = uint8( RoundInt( b * uint24_to_uint8 ) );
   }

   /*!
    * Conversion of a pixel sample value to an 8-bit signed integer.
    */
   static void FromSample( int8& a, sample b )
   {
      a = int8( RoundInt( b * uint24_to_uint8 ) + int8_min );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit unsigned integer.
    */
   static void FromSample( uint16& a, sample b )
   {
      a = uint16( RoundInt( b * uint24_to_uint16 ) );
   }

   /*!
    * Conversion of a pixel sample value to a 16-bit signed integer.
    */
   static void FromSample( int16& a, sample b )
   {
      a = int16( RoundInt( b * uint24_to_uint16 ) + int16_min );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit unsigned integer.
    */
   static void FromSample( uint32& a, sample b )
   {
      a = uint32( RoundInt( b * uint24_to_uint32 ) );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit signed integer.
    */
   static void FromSample( int32& a, sample b )
   {
      a = int32( RoundInt( b * uint24_to_uint32 ) + int32_min );
   }

   /*!
    * Conversion of a pixel sample value to a 32-bit floating point real.
    */
   static void FromSample( float& a, sample b )
   {
      a = float( b )/uint24_max;
   }

   /*!
    * Conversion of a pixel sample value to a 64-bit floating point real.
    */
   static void FromSample( double& a, sample b )
   {
      a = double( b )/uint24_max;
   }

   /*!
    * Conversion of a pixel sample value to any complex type.
    */
   template <typename T>
   static void FromSample( Complex<T>& a, sample b )
   {
      typename Complex<T>::component c;
      FromSample( c, b );
      a = c;
   }
};

// ----------------------------------------------------------------------------

#undef IMPLEMENT_TRANSFER_OPERATIONS

#undef EPSILON_D
#undef EPSILON_F

#ifndef __PCL_DONT_USE_PIXTRAITS_LUT
#undef pFLUT8
#undef pFLUTA
#undef p1FLUT8
#undef pFLUT16
#undef pFLUT20
#undef pDLUT8
#undef pDLUTA
#undef p1DLUT8
#undef pDLUT16
#undef pDLUT20
#undef p8LUT16
#undef p8LUT20
#undef p16LUT8
#undef p16LUT20
#undef p20LUT8
#undef p20LUT16
#undef p24LUT8
#undef p24LUT16
#undef p32LUT8
#undef p32LUT16
#undef p32LUT20
#endif

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PixelTraits_h

// ----------------------------------------------------------------------------
// EOF pcl/PixelTraits.h - Released 2018-11-23T16:14:19Z
