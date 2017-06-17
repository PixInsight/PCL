//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/Pen.h - Released 2017-06-17T10:55:43Z
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

#ifndef __PCL_Pen_h
#define __PCL_Pen_h

/// \file pcl/Pen.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Brush.h>
#include <pcl/Color.h>
#include <pcl/UIObject.h>

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::PenStyle
 * \brief     Pen drawing styles
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>PenStyle::Empty</td>      <td>Nothing is drawn</td></tr>
 * <tr><td>PenStyle::Solid</td>      <td>_____________</td></tr>
 * <tr><td>PenStyle::Dash</td>       <td>_ _ _ _ _ _ _</td></tr>
 * <tr><td>PenStyle::Dot</td>        <td>.............</td></tr>
 * <tr><td>PenStyle::DashDot</td>    <td>_._._._._._._</td></tr>
 * <tr><td>PenStyle::DashDotDot</td> <td>_.._.._.._.._</td></tr>
 * </table>
 */
namespace PenStyle
{
   enum value_type
   {
      Empty,      // Nothing drawn
      Solid,      // _____________
      Dash,       // _ _ _ _ _ _ _
      Dot,        // .............
      DashDot,    // _._._._._._._
      DashDotDot, // _.._.._.._.._

      NumberOfPenStyles
   };
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::PenCap
 * \brief     Pen cap styles
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>PenCap::Flat</td>   <td>Square caps not reaching the end points of lines</td></tr>
 * <tr><td>PenCap::Square</td> <td>Square caps extending beyond end points by half the line width</td></tr>
 * <tr><td>PenCap::Round</td>  <td>Round caps centered at the line end points</td></tr>
 * </table>
 */
namespace PenCap
{
   enum value_type
   {
      Flat,    // Square caps not reaching the end points of lines
      Square,  // Square caps extending beyond end points by half the line width
      Round,   // Round caps centered at the line end points

      NumberOfPenCaps
   };
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::PenJoin
 * \brief     Pen join styles
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>PenJoin::Miter</td> <td>Lines are joined by sharp corners</td></tr>
 * <tr><td>PenJoin::Bevel</td> <td>Lines are joined by flat corners</td></tr>
 * <tr><td>PenJoin::Round</td> <td>Circular arcs are drawn between lines</td></tr>
 * </table>
 */
namespace PenJoin
{
   enum value_type
   {
      Miter,   // Lines are joined by sharp corners
      Bevel,   // Lines are joined by flat corners
      Round,   // Circular arcs are drawn between lines

      NumberOfPenJoins
   };
}

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

class PCL_CLASS Brush;

// ----------------------------------------------------------------------------

/*!
 * \class Pen
 * \brief Client-side interface to a PixInsight %Pen object
 *
 * ### TODO: Write a detailed description for %Pen.
 */
class PCL_CLASS Pen : public UIObject
{
public:

   /*!
    * Represents a pen drawing style.
    */
   typedef PenStyle::value_type  style;

   /*!
    * Represents a pen cap style.
    */
   typedef PenCap::value_type    cap;

   /*!
    * Represents a pen join style.
    */
   typedef PenJoin::value_type   join;

   /*!
    * Constructs a %Pen object with the specified \a color, \a width in pixels,
    * and drawing, cap and join styles.
    */
   Pen( RGBA color = 0xff000000, float width = 0, style s = PenStyle::Solid,
        cap c = PenCap::Square, join j = PenJoin::Miter );

   /*!
    * Copy constructor. This object will reference the same server-side pen
    * as the specified instance \a p.
    */
   Pen( const Pen& p ) : UIObject( p )
   {
   }

   /*!
    * Destroys a %Pen object. If this object references an existing pen in the
    * PixInsight core application, its reference count is decremented. If it
    * becomes unreferenced, it will be garbage-collected.
    */
   virtual ~Pen()
   {
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    *
    * Makes this object reference the same server-side pen as the specified
    * instance \a p. If the previous pen becomes unreferenced, it will be
    * garbage-collected by the PixInsight core application.
    */
   Pen& operator =( const Pen& p )
   {
      SetHandle( p.handle );
      return *this;
   }

   /*!
    * Returns a reference to a <em>null pen</em>. A null %Pen object does not
    * correspond to an existing pen object in the PixInsight core application.
    */
   static Pen& Null();

   /*!
    * Returns the pen width in pixels.
    *
    * A zero width means that this is a <em>cosmetic pen</em>. Cosmetic pens
    * always draw one-pixel strokes, regardless of the transformation applied
    * to the graphics context where they are applied.
    *
    * \sa SetWidth()
    */
   float Width() const;

   /*!
    * Sets the pen width in pixels.
    *
    * A zero width means that this is a <em>cosmetic pen</em>. Cosmetic pens
    * always draw one-pixel strokes, regardless of the transformation applied
    * to the graphics context where they are applied.
    *
    * \sa Width()
    */
   void SetWidth( float w );

   /*!
    * Returns the pen color as a 32-bit AARRGGBB pixel color value.
    *
    * \sa SetColor()
    */
   RGBA Color() const;

   /*!
    * Sets the pen \a color as a 32-bit AARRGGBB pixel color value.
    *
    * \sa Color()
    */
   void SetColor( RGBA color );

   /*!
    * Returns the current pen drawing style.
    *
    * \sa SetStyle(), IsSolid(), IsEmpty()
    */
   style Style() const;

   /*!
    * Returns true iff this pen draws solid strokes, i.e. if the current pen
    * drawing style is PenStyle::Solid.
    *
    * \sa IsEmpty(), Style()
    */
   bool IsSolid() const
   {
      return Style() == PenStyle::Solid;
   }

   /*!
    * Returns true iff this pen draws no strokes, i.e. if the current pen
    * drawing style is PenStyle::Empty.
    *
    * \sa IsSolid(), Style()
    */
   bool IsEmpty() const
   {
      return Style() == PenStyle::Empty;
   }

   /*!
    * Sets the pen drawing style.
    *
    * \sa Style(), IsSolid(), IsEmpty()
    */
   void SetStyle( style );

   /*!
    * Returns the current pen cap style.
    *
    * \sa SetCap()
    */
   cap Cap() const;

   /*!
    * Sets the current pen cap style.
    *
    * \sa Cap()
    */
   void SetCap( cap c );

   /*!
    * Returns the current pen join style.
    *
    * \sa SetJoin()
    */
   join Join() const;

   /*!
    * Sets the current pen join style.
    *
    * \sa Join()
    */
   void SetJoin( join j );

   /*!
    * Returns the current pen brush.
    *
    * \sa SetBrush()
    */
   pcl::Brush Brush() const;

   /*!
    * Returns the current pen brush.
    *
    * \deprecated Use Pen::Brush() in newly produced code.
    * \sa SetBrush()
    */
   pcl::Brush GetBrush() const
   {
      return this->Brush();
   }

   /*!
    * Sets the current pen brush
    *
    * \sa Brush()
    */
   void SetBrush( const pcl::Brush& brush );

private:

   Pen( void* h ) : UIObject( h )
   {
   }

   virtual void* CloneHandle() const;

   friend class GraphicsContextBase;
   friend class Graphics;
   friend class VectorGraphics;
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Pen_h

// ----------------------------------------------------------------------------
// EOF pcl/Pen.h - Released 2017-06-17T10:55:43Z
