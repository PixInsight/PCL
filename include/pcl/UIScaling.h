//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/UIScaling.h - Released 2015/07/30 17:15:18 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_UIScaling_h
#define __PCL_UIScaling_h

/// \file pcl/UIScaling.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Math_h
#include <pcl/Math.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

#ifndef __PCL_GlobalSettings_h
#include <pcl/GlobalSettings.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup ui_scaling_functions User Interface Scaling Functions
 */

#define PCL_UIScaling_LUT_Length 7

extern PCL_DATA const double PCL_UIScalingFactor_LUT[ PCL_UIScaling_LUT_Length ];
extern PCL_DATA const char*  PCL_UIScalingSubdir_LUT[ PCL_UIScaling_LUT_Length ];

/*!
 * Returns the resource scaling index corresponding to the specified display
 * scaling factor.
 *
 * Resource scaling indexes pertain to the set {0,1,2,3,4,5,6}, respectively
 * for the resource scaling factors {1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0}.
 *
 * \ingroup ui_scaling_functions
 */
inline int UIResourceScalingIndex( double scalingFactor )
{
   if ( scalingFactor > 1 )
   {
      int index = Min( RoundInt( 2*scalingFactor ), PCL_UIScaling_LUT_Length+1 );
      if ( index > 2 )
         return index-2;
   }
   return 0;
}

/*!
 * Returns the resource scaling factor corresponding to the specified display
 * scaling factor.
 *
 * A display scaling factor is the ratio of physical device pixels to
 * device-independent logical pixel units for a particular control. A resource
 * scaling factor has the same definition, but is constrained to the finite set
 * {1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0}.
 *
 * On the PixInsight platform, the reference display density is 109 dpi,
 * corresponding to a 27-inch monitor at QHD resolution (2560x1440 physical
 * display pixels). This density corresponds to a display/resource scaling
 * factor of 1.0.
 *
 * \ingroup ui_scaling_functions
 */
inline double UIResourceScalingFactor( double scalingFactor )
{
   return PCL_UIScalingFactor_LUT[UIResourceScalingIndex( scalingFactor )];
}

/*!
 * Returns the resource scaling factor corresponding to the specified resource
 * scaling index.
 *
 * Resource scaling indexes pertain to the set {0,1,2,3,4,5,6}, respectively
 * for the resource scaling factors {1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0}.
 *
 * On the PixInsight platform, the reference display density is 109 dpi,
 * corresponding to a 27-inch monitor at QHD resolution (2560x1440 physical
 * display pixels). This density corresponds to a display/resource scaling
 * factor of 1.0 and a zero resource scaling index.
 *
 * \ingroup ui_scaling_functions
 */
inline double UIResourceScalingFactorForIndex( int index )
{
   return PCL_UIScalingFactor_LUT[Range( index, 0, PCL_UIScaling_LUT_Length-1 )];
}

/*!
 * Applies the specified display scaling factor to convert \a size from
 * device-independent logical pixel units to physical device pixels. The
 * returned value is always rounded to the nearest integer.
 *
 * \ingroup ui_scaling_functions
 */
inline int UIScaled( double scalingFactor, int size )
{
   return (scalingFactor <= 1) ? size : RoundInt( scalingFactor*size );
}

/*!
 * Applies the specified display scaling factor to convert \a size from
 * physical device pixels to device-independent logical pixel units. The
 * returned value is always rounded to the nearest integer.
 *
 * \ingroup ui_scaling_functions
 */
inline int UIUnscaled( double scalingFactor, int size )
{
   return (scalingFactor <= 1) ? size : RoundInt( size/scalingFactor );
}

/*!
 * Returns a user interface resource path corresponding to the specified
 * \a resource path scaled for the specified resource scaling \a index.
 *
 * For example, if this function is called as follows:
 *
 * \code UIScaledResourceForIndex( 3, "/:icons/delete.png" ); \endcode
 *
 * the returned string will be "/:icons/2.5/delete.png" for a resource scaling
 * factor of 2.5.
 *
 * \ingroup ui_scaling_functions
 */
template <class R>
inline String UIScaledResourceForIndex( int index, R resource )
{
   String path( resource );
   index = Range( index, 0, PCL_UIScaling_LUT_Length-1 );
   if ( index > 0 )
   {
      size_type p = path.FindLast( '/' );
      if ( p != String::notFound && p > 0 ) // assume resource starts with ":/"
         return path.Left( p+1 ) + PCL_UIScalingSubdir_LUT[index] + path.Substring( p );
   }
   return path;
}

/*!
 * Returns a user interface resource path corresponding to the specified
 * \a resource path scaled for the specified display scaling factor.
 *
 * For example, if this function is called as follows:
 *
 * \code UIScaledResourceForIndex( 1.47, "/:icons/pencil.png" ); \endcode
 *
 * the returned string will be "/:icons/1.5/pencil.png" for a resource scaling
 * factor of 1.5.
 *
 * \ingroup ui_scaling_functions
 */
template <class R>
inline String UIScaledResource( double scalingFactor, R resource )
{
   return UIScaledResourceForIndex( UIResourceScalingIndex( scalingFactor ), resource );
}

/*
 * Auxiliary structure used by UIScaledStyleSheet().
 */
struct PCL_UIStringSection
{
   size_type pos = 0;
   size_type len = 0;

   PCL_UIStringSection( size_type i, size_type j ) :
      pos( i ),
      len( j - i )
   {
   }

   PCL_UIStringSection() = default;
   PCL_UIStringSection( const PCL_UIStringSection& ) = default;
};

/*!
 * Returns a CSS source code string transformed with scaled dimensions in
 * pixels and scaled resource file paths, and optionally font sizes in points
 * converted to scaled pixels.
 *
 * \param scalingFactor    The display scaling factor to apply.
 *
 * \param styleSheet       A string containing valid input CSS source code. The
 *                         function will return a transformed version of this
 *                         string.
 *
 * \param fontDPI          If greater than zero, this is the font resolution,
 *                         in dots per inch (dpi), for transformation of point
 *                         sizes to scaled pixel sizes. If this parameter is
 *                         zero (the default value), this routine will use the
 *                         font resolution currently selected in core user
 *                         preferences (which is the value of the
 *                         "Application/FontResolution" global integer variable
 *                         - see PixInsightSettings). If this parameter is a
 *                         negative integer, no point-to-pixel conversions will
 *                         be applied.
 *
 * \ingroup ui_scaling_functions
 */
template <class S>
String UIScaledStyleSheet( double scalingFactor, S styleSheet, int fontDPI = 0 )
{
#define PCL_UI_IS_NUMBER_DIGIT( c ) (CharTraits::IsDigit( c ) || CharTraits::IsDecimalSeparator( c ))

   String cssCode( styleSheet );

   /*
    * Scale pixel dimensions.
    */
   Array<PCL_UIStringSection> pxSections;
   for ( size_type q = 0; ; q += 2 )
   {
      q = cssCode.Find( "px", q );
      if ( q == String::notFound )
         break;
      size_type p = q;
      while ( p > 0 && PCL_UI_IS_NUMBER_DIGIT( cssCode[p-1] ) )
         --p;
      if ( p < q )
         pxSections.Add( PCL_UIStringSection( p, q ) );
   }
   for ( Array<PCL_UIStringSection>::const_iterator i = pxSections.ReverseBegin(); i != pxSections.ReverseEnd(); --i )
   {
      double px;
      if ( cssCode.Substring( i->pos, i->len ).TryToDouble( px ) )
         cssCode.Replace( i->pos, i->len, String().Format( "%d", RoundInt( px * scalingFactor ) ) );
   }

   /*
    * Scale resource file paths.
    */
   Array<PCL_UIStringSection> urlSections;
   for ( size_type p = 0; ; )
   {
      p = cssCode.Find( ":/", p );
      if ( p == String::notFound )
         break;
      size_type q = cssCode.Find( ')', p+2 );
      if ( q != String::notFound )
      {
         urlSections.Add( PCL_UIStringSection( p, q ) );
         p = q+1;
      }
      else
         p += 2;
   }
   if ( !urlSections.IsEmpty() )
   {
      int index = UIResourceScalingIndex( scalingFactor );
      for ( Array<PCL_UIStringSection>::const_iterator i = urlSections.ReverseBegin(); i != urlSections.ReverseEnd(); --i )
         cssCode.Replace( i->pos, i->len, String( UIScaledResourceForIndex( index, cssCode.Substring( i->pos, i->len ) ) ) );
   }

   /*
    * If fontDPI=0 (the default value), use core font resolution settings for
    * point-to-pixel conversions.
    */
   if ( fontDPI == 0 )
      fontDPI = PixInsightSettings::GlobalInteger( "Application/FontResolution" );

   /*
    * If a valid font resolution is available, convert points to scaled pixels.
    */
   if ( fontDPI > 0 )
   {
      Array<PCL_UIStringSection> ptSections;
      for ( size_type q = 0; ; q += 2 )
      {
         q = cssCode.Find( "pt", q );
         if ( q == String::notFound )
            break;
         size_type p = q;
         while ( p > 0 && PCL_UI_IS_NUMBER_DIGIT( cssCode[p-1] ) )
            --p;
         if ( p < q )
            ptSections.Add( PCL_UIStringSection( p, q ) );
      }
      for ( Array<PCL_UIStringSection>::const_iterator i = ptSections.ReverseBegin(); i != ptSections.ReverseEnd(); --i )
      {
         double pt;
         if ( cssCode.Substring( i->pos, i->len ).TryToDouble( pt ) )
            cssCode.Replace( i->pos, i->len+2, String().Format( "%dpx", RoundInt( scalingFactor*pt*fontDPI/72 ) ) );
      }
   }

   return cssCode;

#undef PCL_UI_IS_NUMBER_DIGIT
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_UIScaling_h

// ----------------------------------------------------------------------------
// EOF pcl/UIScaling.h - Released 2015/07/30 17:15:18 UTC
