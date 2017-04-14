//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/BitmapBox.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_BitmapBox_h
#define __PCL_BitmapBox_h

/// \file pcl/BitmapBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Bitmap.h>
#include <pcl/Frame.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class BitmapBox
 * \brief Client-side interface to a PixInsight %BitmapBox control.
 *
 * ### TODO: Write a detailed description for %BitmapBox
 */
class PCL_CLASS BitmapBox : public Frame
{
public:

   /*!
    * Constructs a %BitmapBox control that draws a bitmap \a bmp and is a child
    * of \a parent.
    */
   BitmapBox( const Bitmap& bmp = Bitmap::Null(), Control& parent = Control::Null() );

   /*!
    * Destroys a %BitmapBox object.
    */
   virtual ~BitmapBox()
   {
   }

   /*!
    * Returns the bitmap currently drawn by this %BitmapBox control.
    */
   Bitmap CurrentBitmap() const;

   /*!
    * Sets the bitmap \a bmp for this %BitmapBox control.
    */
   void SetBitmap( const Bitmap& bmp );

   /*!
    * Removes the bitmap in this %BitmapBox control. Does not destroy the
    * bitmap (unless it becomes unreferenced); only disassociates it from this
    * %BitmapBox control.
    */
   void Clear()
   {
      SetBitmap( Bitmap::Null() );
   }

   /*!
    * Returns the margin in pixels that this %BitmapBox control reserves around
    * its bitmap.
    */
   int Margin() const;

   /*!
    * Sets a new margin \a m in pixels for this %BitmapBox control.
    */
   void SetMargin( int m );

   /*!
    * Sets the margin of this %BitmapBox control to zero pixels.
    */
   void ClearMargin()
   {
      SetMargin( 0 );
   }

   /*! #
    */
   bool IsAutoFitEnabled() const;

   /*! #
    */
   void EnableAutoFit( bool = true );

   /*! #
    */
   void DisableAutoFit( bool disable = true )
   {
      EnableAutoFit( !disable );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_BitmapBox_h

// ----------------------------------------------------------------------------
// EOF pcl/BitmapBox.h - Released 2017-04-14T23:04:40Z
