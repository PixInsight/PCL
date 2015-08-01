//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/SVG.h - Released 2015/07/30 17:15:18 UTC
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

#ifndef __PCL_SVG_h
#define __PCL_SVG_h

/// \file pcl/SVG.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_UIObject_h
#include <pcl/UIObject.h>
#endif

#ifndef __PCL_Rectangle_h
#include <pcl/Rectangle.h>
#endif

#ifndef __PCL_ByteArray_h
#include <pcl/ByteArray.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SVG
 * \brief Scalable %Vector %Graphics generation.
 *
 * The %SVG class performs generation of Scalable %Vector %Graphics output.
 * Generated %SVG data can be sent to either a disk file or to a memory buffer.
 * The Bitmap class can be used to render %SVG objects as images.
 *
 * ### TODO: Write a detailed description for SVG
 *
 * \sa Graphics, Bitmap
 */
class PCL_CLASS SVG : public UIObject
{
public:

   /*!
    * Constructs a %SVG object that will send graphics output to a file at the
    * specified \a filePath.
    *
    * The optional \a width and \a height values, if nonzero, will be written
    * as the corresponding attributes of the &lt;svg&gt; XML element.
    */
   SVG( const String& filePath, int width = 0, int height = 0 );

   /*!
    * Constructs a %SVG object that will send graphics output to a memory
    * buffer.
    *
    * The optional \a width and \a height values, if nonzero, will be written
    * as the corresponding attributes of the &lt;svg&gt; XML element.
    */
   SVG( int width = 0, int height = 0 );

   /*!
    * Destroys a %SVG object.
    */
   virtual ~SVG()
   {
   }

   /*!
    * Returns a reference to a null %SVG instance. A null %SVG does not
    * correspond to an existing SVG object in the PixInsight core application.
    */
   static SVG& Null();

   /*!
    * Obtains the dimensions (width, height) of this %SVG object in pixels.
    */
   void GetDimensions( int& width, int& height ) const;

   /*!
    * Sets the dimensions of this %SVG object, width and height in pixels.
    */
   void SetDimensions( int width, int height ) const;

   /*!
    * Returns the width of this %SVG object.
    */
   int Width() const
   {
      int w, dum; GetDimensions( w, dum ); return w;
   }

   /*!
    * Returns the height of this %SVG object.
    */
   int Height() const
   {
      int dum, h; GetDimensions( dum, h ); return h;
   }

   /*!
    * Returns the bounding rectangle of this %SVG object. The returned
    * rectangle is equal to %Rect( 0, 0, Width(), Height() ).
    */
   Rect Bounds() const
   {
      int w, h; GetDimensions( w, h ); return Rect( w, h );
   }

   /*!
    * Returns the viewBox attribute of this %SVG object. The viewBox defines a
    * clipping rectangle for graphics output. It can be useful when a %SVG
    * drawing will be used as an element of a larger drawing.
    */
   DRect ViewBox() const;

   /*!
    * Sets the viewBox attribute of this %SVG object.
    *
    * \param x0   Left coordinate of the viewBox rectangle.
    * \param y0   Upper coordinate of the viewBox rectangle.
    * \param x1   Right coordinate of the viewBox rectangle.
    * \param y1   Bottom coordinate of the viewBox rectangle.
    *
    * The viewBox defines a clipping rectangle for graphics output. It can be
    * useful when a %SVG drawing will be used as an element of a larger drawing.
    */
   void SetViewBox( double x0, double y0, double x1, double y1 );

   /*!
    * Sets the viewBox attribute of this %SVG object to the specified
    * rectangle \a r.
    */
   template <typename T>
   void SetViewBox( const GenericRectangle<T>& r )
   {
      SetViewBox( double( r.x0 ), double( r.y0 ), double( r.x1 ), double( r.y1 ) );
   }

   /*!
    * Returns the resolution attribute of this %SVG object in dots per inch.
    */
   int Resolution() const;

   /*!
    * Sets the resolution attribute of this %SVG object in dots per inch.
    */
   void SetResolution( int r );

   /*!
    * Returns the output file path of this %SVG object. If this object sends
    * its graphics output to a memory buffer, this function returns an empty
    * string.
    */
   String FilePath() const;

   /*!
    * Returns a copy of the output data buffer generated by this %SVG object
    * as a dynamic array of bytes. If this object sends graphics output to a
    * file buffer, or if a Graphics object is still painting it, this function
    * returns an empty array.
    */
   ByteArray Data() const;

   /*!
    * Returns the title attribute of this %SVG object.
    */
   String Title() const;

   /*!
    * Sets the title attribute of this %SVG object.
    *
    * If no specific value is set by a call to this function, a default title
    * attribute is set to "PixInsight SVG Document" by the PixInsight core
    * application.
    */
   void SetTitle( const String& title );

   /*!
    * Returns the description attribute of this %SVG object.
    */
   String Description() const;

   /*!
    * Sets the description attribute of this %SVG object.
    *
    * If no specific value is set by a call to this function, a default
    * description attribute is set to "Generated with " plus a string
    * representing the version of the PixInsight core application.
    */
   void SetDescription( const String& desc );

   /*!
    * Returns true if this %SVG instance is being actively painted by a
    * Graphics object.
    */
   bool IsPainting() const;

protected:

   SVG( void* );
   virtual void* CloneHandle() const;

   friend class GraphicsContextBase;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_SVG_h

// ----------------------------------------------------------------------------
// EOF pcl/SVG.h - Released 2015/07/30 17:15:18 UTC
