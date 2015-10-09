//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/Brush.h - Released 2015/10/08 11:24:12 UTC
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

#ifndef __PCL_Brush_h
#define __PCL_Brush_h

/// \file pcl/Brush.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_UIObject_h
#include <pcl/UIObject.h>
#endif

#ifndef __PCL_Color_h
#include <pcl/Color.h>
#endif

#ifndef __PCL_Rectangle_h
#include <pcl/Rectangle.h>
#endif

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace BrushStyle
 * \brief Supported brush styles.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>BrushStyle::Empty</td>                <td>Empty brush (transparent brush)</td></tr>
 * <tr><td>BrushStyle::Solid</td>                <td>Solid brush</td></tr>
 * <tr><td>BrushStyle::Dense</td>                <td>Dense pattern brush</td></tr>
 * <tr><td>BrushStyle::HalfTone</td>             <td>50% pattern brush</td></tr>
 * <tr><td>BrushStyle::Sparse</td>               <td>Sparse pattern brush</td></tr>
 * <tr><td>BrushStyle::HorzizontalHatch</td>     <td>-----</td></tr>
 * <tr><td>BrushStyle::VerticalHatch</td>        <td>|||||</td></tr>
 * <tr><td>BrushStyle::CrossHatch</td>           <td>+++++</td></tr>
 * <tr><td>BrushStyle::ForwardDiagonalHatch</td> <td>/////</td></tr>
 * <tr><td>BrushStyle::BackwardDiagonalHatch</td><td>\\\\\\\\\\ </td></tr>
 * <tr><td>BrushStyle::CrossDiagonalHatch</td>   <td>XXXXX</td></tr>
 * <tr><td>BrushStyle::Stipple</td>              <td>Fill with a tiled Bitmap</td></tr>
 * <tr><td>BrushStyle::LinearGradient</td>       <td>Fill with a linear gradient</td></tr>
 * <tr><td>BrushStyle::RadialGradient</td>       <td>Fill with a radial gradient</td></tr>
 * <tr><td>BrushStyle::ConicalGradient</td>      <td>Fill with a conical gradient</td></tr>
 * </table>
 */
namespace BrushStyle
{
   enum value_type
   {
      Empty,                  // empty brush (transparent brush)
      Solid,                  // solid brush
      Dense,                  // dense pattern brush
      HalfTone,               // 50% pattern brush
      Sparse,                 // sparse pattern brush
      HorzizontalHatch,       // -----
      VerticalHatch,          // |||||
      CrossHatch,             // +++++
      ForwardDiagonalHatch,   // /////
      BackwardDiagonalHatch,  // \\\\\. <--- why this dot? :)
      CrossDiagonalHatch,     // XXXXX
      Stipple,                // fill with a tiled Bitmap
      LinearGradient,         // fill with a linear gradient
      RadialGradient,         // fill with a radial gradient
      ConicalGradient,        // fill with a conical gradient

      NumberOfBrushStyles
   };
}

/*!
 * \namespace GradientSpreadMode
 * \brief Supported gradient spread modes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>GradientSpreadMode::Pad</td>     <td>Areas outside the gradient are filled with the nearest (first or last) stop color.</td></tr>
 * <tr><td>GradientSpreadMode::Reflect</td> <td>Gradients are reflected outside gradient areas.</td></tr>
 * <tr><td>GradientSpreadMode::Repeat</td>  <td>Gradients are repeated outside gradient areas.</td></tr>
 * </table>
 */
namespace GradientSpreadMode
{
   enum value_type
   {
      Pad,
      Reflect,
      Repeat,

      NumberOfGradientSpreadModes
   };
}

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

class PCL_CLASS Bitmap;

// ----------------------------------------------------------------------------

/*!
 * \class Brush
 * \brief Client-side interface to a PixInsight %Brush object.
 *
 * ### TODO: Write a detailed description for %Brush.
 *
 * \sa GradientBrush, LinearGradientBrush, RadialGradientBrush,
 * ConicalGradientBrush
 */
class PCL_CLASS Brush : public UIObject
{
public:

   /*!
    * Represents the brush style.
    * Supported values are defined in the BrushStyle namespace.
    */
   typedef BrushStyle::value_type   style;

   /*!
    * Constructs a brush with the specified \a color and \a style.
    */
   Brush( RGBA color = 0xff000000, style = BrushStyle::Solid );

   /*!
    * Constructs a stippled brush with the specified bitmap pattern \a bmp.
    */
   Brush( const Bitmap& bmp );

   /*!
    * Copy constructor. This object will reference the same server-side brush
    * as the specified instance \a b.
    */
   Brush( const Brush& b ) : UIObject( b )
   {
   }

   /*!
    * Move constructor.
    */
   Brush( Brush&& x ) : UIObject( std::move( x ) )
   {
   }

   /*!
    * Destroys a %Brush object. If this object references an existing brush in
    * the PixInsight core application, its reference count is decremented. If
    * it becomes unreferenced, it will be garbage-collected.
    */
   virtual ~Brush()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    *
    * Makes this object reference the same server-side brush as the specified
    * instance \a b. If the previous brush becomes unreferenced, it will be
    * garbage-collected by the PixInsight core application.
    */
   Brush& operator =( const Brush& b )
   {
      Assign( b );
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   Brush& operator =( Brush&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Returns a reference to a <em>null brush</em>. A null %Brush object does
    * not correspond to an existing brush object in the PixInsight core
    * application.
    */
   static Brush& Null();

   /*!
    * Returns the current color of this brush.
    */
   RGBA Color() const;

   /*!
    * Sets the color of this brush.
    */
   void SetColor( RGBA );

   /*!
    * Returns the current style of this brush.
    */
   style Style() const;

   /*!
    * Returns true iff this brush is solid, i.e. if its current style is
    * BrushStyle::Solid.
    */
   bool IsSolid() const
   {
      return Style() == BrushStyle::Solid;
   }

   /*!
    * Returns true iff this brush is empty (or transparent), i.e. if its current
    * style is BrushStyle::Empty. An empty brush produces transparent shapes.
    */
   bool IsEmpty() const
   {
      return Style() == BrushStyle::Empty;
   }

   /*!
    * Returns true iff this brush is transparent. This function is a synonym
    * for IsEmpty().
    */
   bool IsTransparent() const
   {
      return IsEmpty();
   }

   /*!
    * Returns true iff this brush is \e stippled, i.e. if its current style is
    * BrushStyle::Stippled. A stippled brush draws a regular pattern
    * consisting of multiple, adjacent copies of a Bitmap image.
    */
   bool IsStippled() const
   {
      return Style() == BrushStyle::Stipple;
   }

   /*!
    * Sets the style of this brush.
    *
    * Gradient styles (BrushStyle::LinearGradient, BrushStyle::RadialGradient
    * and BrushStyle::ConicalGradient) cannot be set with this function, since
    * gradient brushes are implemented as separate PCL classes
    * (LinearGradientBrush, RadialGradientBrush and ConicalGradientBrush,
    * respectively).
    */
   void SetStyle( style );

   /*!
    * Returns the current \e stipple of this brush. If the current style of
    * this brush is not BrushStyle::Stippled, a null Bitmap object is
    * returned.
    */
   Bitmap Stipple() const;

   /*!
    * Sets the current \e stipple for this brush. This function implicitly
    * forces the style of this brush to be BrushStyle::Stippled.
    */
   void SetStipple( const Bitmap& );

   /*!
    * Returns true iff this object is a gradient brush.
    */
   virtual bool IsGradient() const
   {
      return false;
   }

private:

   Brush( void* h ) : UIObject( h )
   {
   }

   virtual void* CloneHandle() const;

   friend class GraphicsContextBase;
   friend class Graphics;
   friend class VectorGraphics;
   friend class GradientBrush;
   friend class Pen;
};

// ----------------------------------------------------------------------------

/*!
 * \class GradientBrush
 * \brief Base class of PCL gradient brushes.
 *
 * ### TODO: Write a detailed description for %GradientBrush.
 *
 * \sa Brush, LinearGradientBrush, RadialGradientBrush, ConicalGradientBrush
 */
class PCL_CLASS GradientBrush : public Brush
{
public:

   /*!
    * Represents a gradient spread mode.
    * Supported values are defined in the GradientSpreadMode namespace.
    */
   typedef GradientSpreadMode::value_type    spread_mode;

   /*! \struct Stop
    *  \brief Gradient stop.
    *
    * A gradient brush interpolates color values from an ordered list of
    * <em>gradient stops</em>. A gradient stop is formed by a position in the
    * [0,1] range and a RGBA color value.
    */
   struct Stop
   {
      double position;  //!< Stop position in the [0,1] range, where 0 and 1 correspond, respectively, to the starting and end locations of the gradient area.
      RGBA   color;     //!< Stop color encoded as a 32-bit AARRGGBB value.

      /*!
       * Constructs a default gradient stop.
       */
      Stop() : position( 0 ), color( 0xFF000000 )
      {
      }

      /*!
       * Constructs a gradient stop at the specified position \a p with the
       * color \a c.
       */
      Stop( double p, RGBA c ) : position( Range( p, 0.0, 1.0 ) ), color( c )
      {
      }

      /*!
       * Copy constructor.
       */
      Stop( const Stop& x ) : position( x.position ), color( x.color )
      {
      }

      /*!
       * Equality operator. Used to enforce ordering of gradient stop lists.
       */
      bool operator ==( const Stop& x ) const
      {
         return position == x.position && color == x.color;
      }

      /*!
       * Less than relational operator. Used to enforce ordering of gradient
       * stop lists.
       *
       * Gradient stop comparison is exclusively based on stop positions; stop
       * colors are irrelevant for ordering.
       */
      bool operator <( const Stop& x ) const
      {
         return position < x.position; // color is irrelevant to ordering of gradient stops
      }
   };

   /*!
    * Represents a list of gradient stops.
    */
   typedef Array<Stop>  stop_list;

   /*!
    * Returns the list of stops in this gradient brush.
    */
   stop_list Stops() const;

   /*!
    * Returns the current gradient spread mode for this gradient brush.
    */
   spread_mode SpreadMode() const;

   /*!
    */
   virtual bool IsGradient() const
   {
      return true;
   }

protected:

   GradientBrush( void* h ) : Brush( h )
   {
   }

   GradientBrush( const GradientBrush& x ) : Brush( x )
   {
   }

   GradientBrush( GradientBrush&& x ) : Brush( std::move( x ) )
   {
   }
};

/*!
 * \class LinearGradientBrush
 * \brief Linear gradient brush.
 *
 * ### TODO: Write a detailed description for %LinearGradientBrush.
 *
 * \sa Brush, GradientBrush, RadialGradientBrush, ConicalGradientBrush
 */
class PCL_CLASS LinearGradientBrush : public GradientBrush
{
public:

   /*!
    * Represents a gradient spread mode.
    * Supported values are defined in the GradientSpreadMode namespace.
    */
   typedef GradientBrush::spread_mode  spread_mode;

   /*!
    * Represents a list of gradient stops.
    */
   typedef GradientBrush::stop_list    stop_list;

   /*!
    * Constructs a %LinearGradientBrush object.
    *
    * \param x1,y1   Coordinates of the gradient's starting point.
    *
    * \param x2,y2   Coordinates of the gradient's end point.
    *
    * \param stops   Reference to a list of gradient stops. If an empty list is
    *                specified (as by default), two stops will be generated
    *                automatically: {0.0,0x00000000} and {1.0,0xFF000000}.
    *
    * \param spread  Gradient spread mode. Supported spread modes are defined
    *                in the GradientSpreadMode namespace. The default is
    *                GradientSpreadMode::Pad.
    */
   LinearGradientBrush( double x1, double y1, double x2, double y2,
                        const stop_list& stops = stop_list(),
                        spread_mode spread = GradientSpreadMode::Pad );

   /*!
    * Constructs a %LinearGradientBrush object.
    *
    * This is an overloaded constructor, provided for convenience. It behaves
    * like the preceding constructor, except the linear gradient parameters are
    * specified as a rectangle \a r using floating point coordinates.
    */
   LinearGradientBrush( const DRect& r,
                        const stop_list& stops = stop_list(),
                        spread_mode spread = GradientSpreadMode::Pad );

   /*!
    * Constructs a %LinearGradientBrush object.
    *
    * This is an overloaded constructor, provided for convenience. It behaves
    * like the preceding constructor, except the linear gradient parameters are
    * specified as a rectangle \a r using integer coordinates.
    */
   LinearGradientBrush( const Rect& r,
                        const stop_list& stops = stop_list(),
                        spread_mode spread = GradientSpreadMode::Pad );

   /*!
    * Copy constructor. This object will reference the same server-side brush
    * as the specified instance \a b.
    */
   LinearGradientBrush( const LinearGradientBrush& b ) : GradientBrush( b )
   {
   }

   /*!
    * Move constructor.
    */
   LinearGradientBrush( LinearGradientBrush&& x ) : GradientBrush( std::move( x ) )
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    *
    * Makes this object reference the same server-side brush as the specified
    * instance \a b. If the previous brush becomes unreferenced, it will be
    * garbage-collected by the PixInsight core application.
    */
   LinearGradientBrush& operator =( const LinearGradientBrush& b )
   {
      SetHandle( b.handle );
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   LinearGradientBrush& operator =( LinearGradientBrush&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Retrieves the linear gradient parameters of this object.
    *
    * \param[out] x1,y1    Coordinates of the gradient's starting point.
    * \param[out] x2,y2    Coordinates of the gradient's end point.
    *
    * \sa Parameters()
    */
   void GetParameters( double& x1, double& y1, double& x2, double& y2 ) const;

   /*!
    * Returns  the linear gradient parameters of this object as a DRect
    * object.
    *
    * \sa GetParameters( double&, double&, double&, double& )
    */
   DRect Parameters() const
   {
      DRect r;
      GetParameters( r.x0, r.y0, r.x1, r.y1 );
      return r;
   }
};

/*!
 * \class RadialGradientBrush
 * \brief Radial gradient brush.
 *
 * ### TODO: Write a detailed description for %RadialGradientBrush.
 *
 * \sa Brush, GradientBrush, LinearGradientBrush, ConicalGradientBrush
 */
class PCL_CLASS RadialGradientBrush : public GradientBrush
{
public:

   /*!
    * Represents a gradient spread mode.
    * Supported values are defined in the GradientSpreadMode namespace.
    */
   typedef GradientBrush::spread_mode  spread_mode;

   /*!
    * Represents a list of gradient stops.
    */
   typedef GradientBrush::stop_list    stop_list;

   /*!
    * Constructs a %RadialGradientBrush object.
    *
    * \param cx,cy   Coordinates of the gradient's center.
    *
    * \param r       Gradient's radius.
    *
    * \param fx,fy   Coordinates of the gradient's focal point. If these
    *                coordinates are not specified, they will be set at
    *                the specified gradient's center point.
    *
    * \param stops   Reference to a list of gradient stops. If an empty list is
    *                specified (as by default), two stops will be generated
    *                automatically: {0.0,0x00000000} and {1.0,0xFF000000}.
    *
    * \param spread  Gradient spread mode. Supported spread modes are defined
    *                in the GradientSpreadMode namespace. The default is
    *                GradientSpreadMode::Pad.
    */
   RadialGradientBrush( double cx, double cy, double r, double fx = uint32_max, double fy = uint32_max,
                        const stop_list& stops = stop_list(),
                        spread_mode spread = GradientSpreadMode::Pad );

   /*!
    * Constructs a %RadialGradientBrush object.
    *
    * This is an overloaded constructor, provided for convenience. It behaves
    * like the preceding constructor, except the radial gradient parameters are
    * specified as a center point \a c, radius \a r and focal point \a f.
    */
   RadialGradientBrush( const DPoint& c, double r, const DPoint& f = DPoint( uint32_max ),
                        const stop_list& stops = stop_list(),
                        spread_mode spread = GradientSpreadMode::Pad );

   /*!
    * Constructs a %RadialGradientBrush object.
    *
    * This is an overloaded constructor, provided for convenience. It behaves
    * like the preceding constructor, except the radial gradient parameters are
    * specified as a center point \a c, radius \a r and focal point \a f.
    */
   RadialGradientBrush( const Point& c, double r, const Point& f = Point( uint32_max ),
                        const stop_list& stops = stop_list(),
                        spread_mode spread = GradientSpreadMode::Pad );

   /*!
    * Copy constructor. This object will reference the same server-side brush
    * as the specified instance \a b.
    */
   RadialGradientBrush( const RadialGradientBrush& b ) : GradientBrush( b )
   {
   }

   /*!
    * Move constructor.
    */
   RadialGradientBrush( RadialGradientBrush&& x ) : GradientBrush( std::move( x ) )
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    *
    * Makes this object reference the same server-side brush as the specified
    * instance \a b. If the previous brush becomes unreferenced, it will be
    * garbage-collected by the PixInsight core application.
    */
   RadialGradientBrush& operator =( const RadialGradientBrush& b )
   {
      SetHandle( b.handle );
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   RadialGradientBrush& operator =( RadialGradientBrush&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Retrieves the radial gradient parameters of this object.
    *
    * \param[out] cx,cy    Coordinates of the gradient's center.
    * \param[out] r        Gradient's radius.
    * \param[out] fx,fy    Coordinates of the gradient's focal point.
    *
    * \sa GetParameters( DPoint&, double&, DPoint& )
    */
   void GetParameters( double& cx, double& cy, double& r, double& fx, double& fy ) const;

   /*!
    * Returns  the radial gradient parameters of this object.
    *
    * This is an overloaded function, provided for convenience. It behaves like
    * the preceding function, except the center and focal point parameters are
    * retrieved as DPoint objects.
    *
    * \sa GetParameters( double&, double&, double&, double&, double& )
    */
   void GetParameters( DPoint& c, double& r, DPoint& f ) const
   {
      GetParameters( c.x, c.y, r, f.x, f.y );
   }
};

/*!
 * \class ConicalGradientBrush
 * \brief Conical gradient brush.
 *
 * ### TODO: Write a detailed description for %ConicalGradientBrush.
 *
 * \sa Brush, GradientBrush, LinearGradientBrush, RadialGradientBrush
 */
class PCL_CLASS ConicalGradientBrush : public GradientBrush
{
public:

   /*!
    * Represents a list of gradient stops.
    */
   typedef GradientBrush::stop_list    stop_list;

   /*!
    * Constructs a %ConicalGradientBrush object.
    *
    * \param cx,cy   Coordinates of the gradient's center.
    *
    * \param a       Gradient's angle in radians.
    *
    * \param stops   Reference to a list of gradient stops. If an empty list is
    *                specified (as by default), two stops will be generated
    *                automatically: {0.0,0x00000000} and {1.0,0xFF000000}.
    */
   ConicalGradientBrush( double cx, double cy, double a, const stop_list& stops = stop_list() );

   /*!
    * Constructs a %ConicalGradientBrush object.
    *
    * This is an overloaded constructor, provided for convenience. It behaves
    * like the preceding constructor, except the conical gradient parameters
    * specified as a center point \a c and angle \a a in radians.
    */
   ConicalGradientBrush( const DPoint& c, double a, const stop_list& stops = stop_list() );

   /*!
    * Constructs a %ConicalGradientBrush object.
    *
    * This is an overloaded constructor, provided for convenience. It behaves
    * like the preceding constructor, except the conical gradient parameters
    * specified as a center point \a c and angle \a a in radians.
    */
   ConicalGradientBrush( const Point& c, double a, const stop_list& stops = stop_list() );

   /*!
    * Copy constructor. This object will reference the same server-side brush
    * as the specified instance \a b.
    */
   ConicalGradientBrush( const ConicalGradientBrush& b ) : GradientBrush( b )
   {
   }

   /*!
    * Move constructor.
    */
   ConicalGradientBrush( ConicalGradientBrush&& x ) : GradientBrush( std::move( x ) )
   {
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    *
    * Makes this object reference the same server-side brush as the specified
    * instance \a b. If the previous brush becomes unreferenced, it will be
    * garbage-collected by the PixInsight core application.
    */
   ConicalGradientBrush& operator =( const ConicalGradientBrush& b )
   {
      SetHandle( b.handle );
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   ConicalGradientBrush& operator =( ConicalGradientBrush&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Retrieves the conical gradient parameters of this object.
    *
    * \param[out] cx,cy    Coordinates of the gradient's center.
    * \param[out] a        Gradient's angle in radians.
    *
    * \sa GetParameters( DPoint&, double& )
    */
   void GetParameters( double& cx, double& cy, double& a ) const;

   /*!
    * Returns the conical gradient parameters of this object.
    *
    * This is an overloaded function, provided for convenience. It behaves like
    * the preceding function, except the center coordinates are retrieved as a
    * DPoint object.
    *
    * \sa GetParameters( double&, double&, double& )
    */
   void GetParameters( DPoint& c, double& a ) const
   {
      GetParameters( c.x, c.y, a );
   }
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Brush_h

// ----------------------------------------------------------------------------
// EOF pcl/Brush.h - Released 2015/10/08 11:24:12 UTC
