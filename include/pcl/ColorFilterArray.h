//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/ColorFilterArray.h - Released 2017-06-21T11:36:33Z
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

#ifndef __PCL_ColorFilterArray_h
#define __PCL_ColorFilterArray_h

/// \file pcl/ColorFilterArray.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Exception.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup cfa_classes Color Filter Array (CFA) Descriptive Classes
 */

/*!
 * \class ColorFilterArray
 * \brief Color filter array (CFA) structure.
 *
 * %ColorFilterArray describes a color filter array (CFA), such as one of the
 * Bayer filters typically used in raw digital camera images.
 *
 * \ingroup cfa_classes
 * \sa BayerFilterBGGR, BayerFilterGBRG, BayerFilterGRBG, BayerFilterRGGB
 */
class PCL_CLASS ColorFilterArray
{
public:

   /*!
    * Constructs a %ColorFilterArray object.
    *
    * \param pattern    A sequence of ASCII characters defining a CFA matrix.
    *                   Each string character represents an element of the CFA
    *                   matrix, which can be one of:\n
    *                   \n
    *                   <table border="1" cellpadding="4" cellspacing="0">
    *                   <tr><td>0</td> <td>A nonexistent or undefined CFA element.</td></tr>
    *                   <tr><td>R</td> <td>Red</td></tr>
    *                   <tr><td>G</td> <td>Green</td></tr>
    *                   <tr><td>B</td> <td>Blue</td></tr>
    *                   <tr><td>W</td> <td>White or panchromatic</td></tr>
    *                   <tr><td>C</td> <td>Cyan</td></tr>
    *                   <tr><td>M</td> <td>Magenta</td></tr>
    *                   <tr><td>Y</td> <td>Yellow</td></tr>
    *                   </table>\n
    *                   \n
    *                   The length of this string must be equal to the product
    *                   of \a width by \a height. Otherwise an Error exception
    *                   will be thrown. An exception will also be thrown if one
    *                   or more invalid CFA elements are found in \a pattern.
    *
    * \param width      Horizontal dimension of the CFA matrix in pixels.
    *
    * \param height     Vertical dimension of the CFA matrix in pixels.
    *
    * \param name       An optional string identifying the type or model of CFA
    *                   that is being described by this object.
    */
   ColorFilterArray( const IsoString& pattern, int width, int height, const String& name = String() ) :
      m_pattern( pattern.Uppercase() ), m_width( width ), m_height( height ), m_name( name )
   {
      if ( m_width < 1 || m_height < 1 || size_type( NumberOfElements() ) != m_pattern.Length() )
         throw Error( "Malformed CFA pattern" );
      for ( auto element : m_pattern )
         if ( !IsValidCFAElement( element ) )
            throw Error( "Invalid CFA pattern \'" + m_pattern + "\'" );
   }

   /*!
    * Constructs an empty %ColorFilterArray object. An empty CFA has no pattern
    * and zero dimensions.
    */
   ColorFilterArray() = default;

   /*!
    * Copy constructor.
    */
   ColorFilterArray( const ColorFilterArray& ) = default;

   /*!
    * Move constructor.
    */
#ifndef _MSC_VER
   ColorFilterArray( ColorFilterArray&& ) = default;
#endif

   /*!
    * Copy assignment operator.
    */
   ColorFilterArray& operator =( const ColorFilterArray& ) = default;

   /*!
    * Move assignment operator.
    */
#ifndef _MSC_VER
   ColorFilterArray& operator =( ColorFilterArray&& ) = default;
#endif

   /*!
    * Returns a reference to the CFA pattern in this object.
    */
   const IsoString& Pattern() const
   {
      return m_pattern;
   }

   /*!
    * Returns the horizontal dimension of this CFA structure in pixels.
    */
   int Width() const
   {
      return m_width;
   }

   /*!
    * Returns the vertical dimension of this CFA structure in pixels.
    */
   int Height() const
   {
      return m_height;
   }

   /*!
    * Returns the total number of elements (or pixels) in this CFA structure.
    */
   int NumberOfElements() const
   {
      return int( m_pattern.Length() );
   }

   /*!
    * Returns true only if this is an empty %ColorFilterArray object. An empty
    * CFA has no pattern and zero dimensions.
    */
   bool IsEmpty() const
   {
      return m_pattern.IsEmpty();
   }

   /*!
    * Returns a reference to the name of this CFA structure.
    */
   const String& Name() const
   {
      return m_name;
   }

   /*!
    * Returns true only if this object describes an RGB Bayer filter: BGGR,
    * GRBG, GBRG, or RGGB.
    */
   bool IsBayerFilter() const
   {
      return m_pattern == "BGGR" || m_pattern == "GRBG" || m_pattern == "GBRG" || m_pattern == "RGGB";
   }

   /*!
    * Returns a copy of the CFA element at the specified \a x and \a y
    * coordinates (pixel column and row, respectively). The top-left corner of
    * the CFA is by convention located at coordinates x=y=0.
    *
    * For a description of valid CFA elements, see the class constructor.
    */
   char Element( int x, int y ) const
   {
      size_type i = y*m_width + x;
      if ( i < m_pattern.Length() )
         return m_pattern[i];
      return '\0';
   }

   /*
    * Clears all internal structures to yield an empty CFA.
    */
   void Clear()
   {
      m_pattern.Clear();
      m_width = m_height = 0;
      m_name.Clear();
   }

   /*!
    * Returns true iff this object represents the same CFA structure as another
    * object \a x. This operator ignores the Name() components of both objects.
    */
   bool operator ==( const ColorFilterArray& x ) const
   {
      return m_pattern == x.m_pattern && m_width == x.m_width && m_height == x.m_height;
   }

   /*!
    * Returns true only if this %ColorFilterArray object precedes another
    * object \a x. This is a lexicographic comparison performed exclusively on
    * the CFA patterns, ignoring matrix dimensions and filter names.
    */
   bool operator <( const ColorFilterArray& x ) const
   {
      return m_pattern < x.m_pattern;
   }

   /*!
    * Returns true only if the specified character is a valid element of a CFA
    * pattern specification. For a description of valid CFA elements, see the
    * class constructor.
    */
   static bool IsValidCFAElement( char element )
   {
      return strchr( "0RGBWCMY", element ) != nullptr;
   }

private:

   IsoString m_pattern;
   int       m_width  = 0;
   int       m_height = 0;
   String    m_name;
};

// ----------------------------------------------------------------------------

/*!
 * \class BayerFilterRGGB
 * \brief RGGB Bayer filter representation.
 * \ingroup cfa_classes
 * \sa ColorFilterArray, BayerFilterBGGR, BayerFilterGBRG, BayerFilterGRBG
 */
class PCL_CLASS BayerFilterRGGB : public ColorFilterArray
{
public:

   BayerFilterRGGB() : ColorFilterArray( "RGGB", 2, 2, "RGGB Bayer Filter" )
   {
   }
};

/*!
 * \class BayerFilterBGGR
 * \brief BGGR Bayer filter representation.
 * \ingroup cfa_classes
 * \sa ColorFilterArray, BayerFilterGBRG, BayerFilterGRBG, BayerFilterRGGB
 */
class PCL_CLASS BayerFilterBGGR : public ColorFilterArray
{
public:

   BayerFilterBGGR() : ColorFilterArray( "BGGR", 2, 2, "BGGR Bayer Filter" )
   {
   }
};

/*!
 * \class BayerFilterGBRG
 * \brief GBRG Bayer filter representation.
 * \ingroup cfa_classes
 * \sa ColorFilterArray, BayerFilterBGGR, BayerFilterGRBG, BayerFilterRGGB
 */
class PCL_CLASS BayerFilterGBRG : public ColorFilterArray
{
public:

   BayerFilterGBRG() : ColorFilterArray( "GBRG", 2, 2, "GBRG Bayer Filter" )
   {
   }
};

/*!
 * \class BayerFilterGRBG
 * \brief GRBG Bayer filter representation.
 * \ingroup cfa_classes
 * \sa ColorFilterArray, BayerFilterBGGR, BayerFilterGBRG, BayerFilterRGGB
 */
class PCL_CLASS BayerFilterGRBG : public ColorFilterArray
{
public:

   BayerFilterGRBG() : ColorFilterArray( "GRBG", 2, 2, "GRBG Bayer Filter" )
   {
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ColorFilterArray_h

// ----------------------------------------------------------------------------
// EOF pcl/ColorFilterArray.h - Released 2017-06-21T11:36:33Z
