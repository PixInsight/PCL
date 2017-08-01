//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/FITSHeaderKeyword.h - Released 2017-08-01T14:23:31Z
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

#ifndef __PCL_FITSHeaderKeyword_h
#define __PCL_FITSHeaderKeyword_h

/// \file pcl/FITSHeaderKeyword.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Array.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class FITSHeaderKeyword
 * \brief %FITS header keyword
 */
class PCL_CLASS FITSHeaderKeyword
{
public:

   IsoString name;    //!< Keyword name
   IsoString value;   //!< Keyword value
   IsoString comment; //!< Keyword comment

   /*!
    * Constructs a %FITSHeaderKeyword object with empty name, value and comment
    * components.
    */
   FITSHeaderKeyword() = default;

   /*!
    * Copy constructor.
    */
   FITSHeaderKeyword( const FITSHeaderKeyword& ) = default;

   /*!
    * Move constructor.
    */
   FITSHeaderKeyword( FITSHeaderKeyword&& x ) :
      name( std::move( x.name ) ), value( std::move( x.value ) ), comment( std::move( x.comment ) )
   {
   }

   /*!
    * Constructs a %FITSHeaderKeyword object with the specified name, value and
    * comment components.
    */
   FITSHeaderKeyword( const IsoString& a_name,
                      const IsoString& a_value = IsoString(),
                      const IsoString& a_comment = IsoString() ) :
      name( a_name ), value( a_value ), comment( a_comment )
   {
      Trim();
   }

   template <class S1, class S2, class S3>
   FITSHeaderKeyword( const S1& a_name, const S2& a_value, const S3& a_comment ) :
      name( IsoString( a_name ) ), value( IsoString( a_value ) ), comment( IsoString( a_comment ) )
   {
      Trim();
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   FITSHeaderKeyword& operator =( const FITSHeaderKeyword& ) = default;

   /*!
    * Returns true iff this %FITS keyword has no %value (i.e., if the value
    * member is an empty string).
    */
   bool IsNull() const
   {
      return value.IsEmpty();
   }

   /*!
    * Returns true iff this %FITS keyword has a string %value.
    *
    * A string %value is assumed if the value component begins with a single
    * quote (').
    */
   bool IsString() const
   {
      return value.StartsWith( '\'' );
   }

   /*!
    * Returns true iff this %FITS keyword has a boolean %value.
    *
    * A boolean %value is assumed if the value component is equal to 'T' or 'F'
    * for \c true and \c false, respectively.
    */
   bool IsBoolean() const
   {
      return value == 'T' || value == 'F';
   }

   /*!
    * Returns true iff this %FITS keyword has a numeric %value.
    *
    * A numeric %value is assumed if the value component is a valid
    * representation of an integer or floating point number.
    */
   bool IsNumeric() const
   {
      return value.IsNumeral();
   }

   /*!
    * Gets the numeric %value of this %FITS keyword in the specified variable
    * \a v, or zero if this keyword has no numeric %value.
    *
    * Returns true iff this keyword has a valid numeric %value. This member
    * function does not throw exceptions, even if the keyword's value contains
    * an illegal numeric representation.
    */
   bool GetNumericValue( double& v ) const
   {
      if ( value.TryToDouble( v ) )
         return true;
      v = 0;
      return false;
   }

   /*!
    * Returns the string %value (irrespective of its data type) of this %FITS
    * header keyword, with delimiters pulled off and leading/trailing
    * whitespace trimmed out.
    */
   IsoString StripValueDelimiters() const
   {
      IsoString stripped;

      if ( !value.IsEmpty() && *value == '\'' ) // leading delimiter ?
      {
         size_type n = value.Length()-1;
         if ( value[n] == '\'' ) // trailing delimiter ?
            --n;
         stripped = value.Substring( 1, n );
      }
      else
         stripped = value;

      return stripped.Trimmed();
   }

   /*!
    * If this keyword has a string value without leading and trailing quotes,
    * this member function adds them, as required by the FITS standard. Returns
    * true if the value was fixed, false if the value was not changed.
    */
   bool FixValueDelimiters()
   {
      double dum;
      if ( !IsNull() )
         if ( !IsString() )
            if ( !IsBoolean() )
               if ( !IsNumeric() || !value.TryToDouble( dum ) )
               {
                  value.EnsureSingleQuoted();
                  return true;
               }
      return false;
   }

   /*!
    * Trims leading and trailing spaces in the name, value and comment
    * components of this %FITS header keyword.
    */
   void Trim()
   {
      name.Trim();
      value.Trim();
      comment.Trim();
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class pcl::FITSKeywordArray
 * \brief Dynamic array of %FITS header keywords
 *
 * %FITSKeywordArray is a template instantiation of Array for
 * FITSHeaderKeyword.
 */
typedef Array<FITSHeaderKeyword> FITSKeywordArray;

// ----------------------------------------------------------------------------

/*!
 * \defgroup fits_keyword_comparison FITS Keyword Comparison Functions
 */

/*!
 * Returns true iff two %FITS header keywords, \a h1 and \a h2, are equal.
 *
 * Two %FITS keywords are equal if their three components (name, value,
 * comment) are equal. However, keyword name comparison is case-insensitive as
 * per the %FITS standard.
 *
 * \ingroup fits_keyword_comparison
 */
inline bool operator ==( const FITSHeaderKeyword& h1, const FITSHeaderKeyword& h2 )
{
   // Keyword name comparison is case-insensitive as per the FITS standard.
   return h1.name.CompareIC( h2.name ) == 0 && h1.value == h2.value && h1.comment == h2.comment;
}

/*!
 * Returns true iff a %FITS header keyword \a h1 is less than other keyword
 * \a h2.
 *
 * This function compares the components of both keywords. The precedence order
 * is name, value and comment. Keyword name comparison is case-insensitive as
 * per the %FITS standard.
 *
 * \ingroup fits_keyword_comparison
 */
inline bool operator <( const FITSHeaderKeyword& h1, const FITSHeaderKeyword& h2 )
{
   int i = h1.name.CompareIC( h2.name );
   return i < 0 || i == 0 && (h1.value < h2.value || h1.value == h2.value && h1.comment < h2.comment);
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FITSHeaderKeyword_h

// ----------------------------------------------------------------------------
// EOF pcl/FITSHeaderKeyword.h - Released 2017-08-01T14:23:31Z
