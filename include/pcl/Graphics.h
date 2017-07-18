//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/Graphics.h - Released 2017-07-18T16:13:52Z
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

#ifndef __PCL_Graphics_h
#define __PCL_Graphics_h

/// \file pcl/Graphics.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Brush.h>
#include <pcl/Font.h>
#include <pcl/Matrix.h>
#include <pcl/Pen.h>
#include <pcl/Rectangle.h>
#include <pcl/TextAlign.h>
#include <pcl/UIObject.h>

#ifndef __PCL_GRAPHICS_DONT_REMOVE_PREVIOUS_DECLARATIONS

// Remove conflicting identifiers from Win32 SDK headers.
// If more conflicts arise, fix *all* of them *here only*.
#ifdef DrawText
#undef DrawText
#endif

#endif   // !__PCL_GRAPHICS_DONT_REMOVE_PREVIOUS_DECLARATIONS

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::CompositionOp
 * \brief Image composition operators.
 *
 * The following are the <em>Porter-Duff alpha composition operators</em>,
 * supported by all versions of PixInsight:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>CompositionOp::Clear</td>            <td>Clear</td></tr>
 * <tr><td>CompositionOp::Source</td>           <td>A</td></tr>
 * <tr><td>CompositionOp::Destination</td>      <td>B</td></tr>
 * <tr><td>CompositionOp::SourceOver</td>       <td>A over B</td></tr>
 * <tr><td>CompositionOp::DestinationOver</td>  <td>B over A</td></tr>
 * <tr><td>CompositionOp::SourceIn</td>         <td>A in B</td></tr>
 * <tr><td>CompositionOp::DestinationIn</td>    <td>B in A</td></tr>
 * <tr><td>CompositionOp::SourceOut</td>        <td>A out B</td></tr>
 * <tr><td>CompositionOp::DestinationOut</td>   <td>B out A</td></tr>
 * <tr><td>CompositionOp::SourceAtop</td>       <td>A atop B</td></tr>
 * <tr><td>CompositionOp::DestinationAtop</td>  <td>B atop A</td></tr>
 * <tr><td>CompositionOp::Xor</td>              <td>A xor B</td></tr>
 * </table>
 *
 * See T. Porter & T. Duff, <em>Compositing Digital Images</em>, Computer
 * Graphics Vol. 18, Num. 3, July 1984, pp 253-259.
 *
 * In addition, the following operators are also supported starting from
 * PixInsight Core version 1.7.0.702:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>CompositionOp::Min</td>              <td>Min( A, B )</td></tr>
 * <tr><td>CompositionOp::Max</td>              <td>Max( A, B )</td></tr>
 * <tr><td>CompositionOp::Add</td>              <td>A + B</td></tr>
 * <tr><td>CompositionOp::Multiply</td>         <td>A * B</td></tr>
 * <tr><td>CompositionOp::Screen</td>           <td>~A * ~B</td></tr>
 * <tr><td>CompositionOp::Overlay</td>          <td>(A > 0.5) ? ~(~(2*(A - 0.5)) * ~B) : 2*A*B</td></tr>
 * <tr><td>CompositionOp::ColorDodge</td>       <td>A/~B</td></tr>
 * <tr><td>CompositionOp::ColorBurn</td>        <td>~(~A/B)</td></tr>
 * <tr><td>CompositionOp::HardLight</td>        <td>(B > 0.5) ? ~(~A * ~(2*(B - 0.5))) : A*2*B</td></tr>
 * <tr><td>CompositionOp::SoftLight</td>        <td>(B > 0.5) ? ~(~A * ~(B - 0.5)) : A*(B + 0.5)</td></tr>
 * <tr><td>CompositionOp::Difference</td>       <td>Abs( A - B )</td></tr>
 * <tr><td>CompositionOp::Exclusion</td>        <td>0.5 - 2*(A - 0.5)*(B - 0.5)</td></tr>
 * </table>
 *
 * In the tables above, A represents a target pixel and B represents a source
 * pixel. For example, if a Graphics object is being used to paint on a Bitmap
 * object, bitmap pixels are target pixels, and any graphical elements drawn
 * using the %Graphics object are considered as source pixels.
 */
namespace CompositionOp
{
   enum value_type
   {
      Clear,            // 0
      Source,           // A
      Destination,      // B
      SourceOver,       // A over B
      DestinationOver,  // B over A
      SourceIn,         // A in B
      DestinationIn,    // B in A
      SourceOut,        // A out B
      DestinationOut,   // B out A
      SourceAtop,       // A atop B
      DestinationAtop,  // B atop A
      Xor,              // A xor B
      Min,              // Min( A, B )
      Max,              // Max( A, B )
      Add,              // A + B
      Multiply,         // A * B
      Screen,           // ~A * ~B
      Overlay,          // (A > 0.5) ? ~(~(2*(A - 0.5)) * ~B) : 2*A*B
      ColorDodge,       // A/~B
      ColorBurn,        // ~(~A/B)
      HardLight,        // (B > 0.5) ? ~(~A * ~(2*(B - 0.5))) : A*2*B
      SoftLight,        // (B > 0.5) ? ~(~A * ~(B - 0.5)) : A*(B + 0.5)
      Difference,       // Abs( A - B )
      Exclusion,        // 0.5 - 2*(A - 0.5)*(B - 0.5)

      NumberOfCompositionOps
   };
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::FillRule
 * \brief Polygon fill rules
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>FillRule::OddEven</td> <td>Odd-even (or parity, alternate) polygon fill rule</td></tr>
 * <tr><td>FillRule::Winding</td> <td>Nonzero winding polygon fill rule</td></tr>
 * </table>
 */
namespace FillRule
{
   enum value_type
   {
      OddEven, // Odd-even (or parity, alternate) polygon fill rule
      Winding, // Nonzero winding polygon fill rule

      NumberOfFillRules
   };
}

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

class Control;
class Bitmap;
class SVG;

// ----------------------------------------------------------------------------

/*!
 * \brief Base class of client-side interface classes to PixInsight <em>graphics contexts</em>
 *
 * ### TODO: Write a detailed description for %GraphicsContextBase.
 *
 * ### TODO: Include a number of examples for drawing primitives, composition
 * operators, rendering performance, transformations, etc.
 */
class PCL_CLASS GraphicsContextBase : public UIObject
{
public:

   /*!
    * Represents a Porter-Duff alpha composition operator.
    */
   typedef CompositionOp::value_type   composition_op;

   /*!
    * Represents a polygon fill rule.
    */
   typedef FillRule::value_type        fill_rule;

   /*!
    * Constructs an inactive graphics context.
    */
   GraphicsContextBase();

   /*!
    * Constructs an active graphics context, ready to draw on the specified
    * \a control.
    *
    * \warning Invoking this constructor outside a Control::OnPaint() event
    * handler is illegal on all supported platforms.
    */
   GraphicsContextBase( Control& control ); // ### illegal if used outside Control::OnPaint()

   /*!
    * Constructs an active graphics context, ready to draw on the specified
    * \a bitmap.
    */
   GraphicsContextBase( Bitmap& bitmap );

   /*!
    * Constructs an active graphics context, ready to draw on the specified
    * \a svg object.
    */
   GraphicsContextBase( SVG& svg );

   /*!
    * Move constructor.
    */
   GraphicsContextBase( GraphicsContextBase&& x ) : UIObject( std::move( x ) )
   {
   }

   /*!
    * Destroys a %GraphicsContextBase object. If this graphics context is
    * active, it is terminated upon destruction.
    */
   virtual ~GraphicsContextBase()
   {
      if ( !IsNull() )
         if ( IsPainting() )
            EndPaint();
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   GraphicsContextBase& operator =( GraphicsContextBase&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since graphics contexts are unique objects, calling this member function
    * has no effect.
    */
   virtual void EnsureUnique()
   {
   }

   /*!
    * Activates this graphics context to draw on the specified \a control.
    *
    * \warning Calling this function outside a Control::OnPaint() event handler
    * is illegal on all supported platforms.
    */
   bool BeginPaint( Control& control ); // ### illegal if used outside Control::OnPaint()

   /*!
    * Activates this graphics context to draw on the specified \a bitmap.
    */
   bool BeginPaint( Bitmap& bitmap );

   /*!
    * Activates this graphics context to draw on the specified \a svg object.
    */
   bool BeginPaint( SVG& svg );

   /*!
    * Returns true iff this graphics context is currently active.
    */
   bool IsPainting() const;

   /*!
    * Terminates a graphics context drawing operation.
    */
   void EndPaint();

   /*! #
    */
   bool IsTransformationEnabled() const;

   /*! #
    */
   void EnableTransformation( bool = true );

   /*! #
    */
   void DisableTransformation( bool disable = true )
   {
      EnableTransformation( !disable );
   }

   /*! #
    */
   Matrix TransformationMatrix() const;

   /*! #
    */
   void SetTransformationMatrix( const Matrix& M );

   /*! #
    */
   void SetTransformationMatrix( double m11, double m12, double m13,
                                 double m21, double m22, double m23,
                                 double m31, double m32, double m33 )
   {
      SetTransformationMatrix( Matrix( m11, m12, m13, m21, m22, m23, m31, m32, m33 ) );
   }

   /*! #
    */
   void MultiplyTransformationMatrix( const Matrix& M );

   /*! #
    */
   void MultiplyTransformationMatrix( double m11, double m12, double m13,
                                      double m21, double m22, double m23,
                                      double m31, double m32, double m33 )
   {
      MultiplyTransformationMatrix( Matrix( m11, m12, m13, m21, m22, m23, m31, m32, m33 ) );
   }

   /*! #
    */
   void RotateTransformation( double angleRadians );

   /*! #
    */
   void ScaleTransformation( double xScale, double yScale );

   /*! #
    */
   void ScaleTransformation( double scale )
   {
      ScaleTransformation( scale, scale );
   }

   /*! #
    */
   void TranslateTransformation( double dx, double dy );

   /*! #
    */
   void TranslateTransformation( double d )
   {
      TranslateTransformation( d, d );
   }

   /*! #
    */
   void ShearTransformation( double sx, double sy );

   /*! #
    */
   void ShearTransformation( double s )
   {
      ShearTransformation( s, s );
   }

   /*! #
    */
   void ResetTransformation();

   /*! #
    */
   void Transform( double& x, double& y ) const;

   /*! #
    */
   void Transform( DPoint* p, size_type n ) const;

   /*! #
    */
   void Transform( Array<DPoint>& a ) const
   {
      Transform( a.Begin(), a.Length() );
   }

   /*! #
    */
   DPoint Transformed( const DPoint& p ) const
   {
      DPoint p1( p );
      Transform( p1.x, p1.y );
      return p1;
   }

   /*! #
    */
   DRect Transformed( const DRect& r ) const
   {
      DRect r1( r );
      Transform( r1.x0, r1.y0 );
      Transform( r1.x1, r1.y1 );
      return r1;
   }

   /*! #
    */
   Array<DPoint> Transformed( const Array<DPoint>& a ) const
   {
      Array<DPoint> a1( a );
      Transform( a1 );
      return a1;
   }

   /*! #
    */
   bool IsClippingEnabled() const;

   /*! #
    */
   void EnableClipping( bool = true );

   /*! #
    */
   void DisableClipping( bool disable = true )
   {
      EnableClipping( !disable );
   }

   /*! #
    */
   bool IsAntialiasingEnabled() const;

   /*! #
    */
   void EnableAntialiasing( bool = true );

   /*! #
    */
   void DisableAntialiasing( bool disable = true )
   {
      EnableAntialiasing( !disable );
   }

   /*! #
    */
   bool IsTextAntialiasingEnabled() const;

   /*! #
    */
   void EnableTextAntialiasing( bool = true );

   /*! #
    */
   void DisableTextAntialiasing( bool disable = true )
   {
      EnableTextAntialiasing( !disable );
   }

   /*! #
    */
   bool IsSmoothInterpolationEnabled() const;

   /*! #
    */
   void EnableSmoothInterpolation( bool = true );

   /*! #
    */
   void DisableSmoothInterpolation( bool disable = true )
   {
      EnableSmoothInterpolation( !disable );
   }

   /*! #
    */
   composition_op CompositionOperator() const;

   /*! #
    */
   void SetCompositionOperator( composition_op );

   /*! #
    */
   void ResetCompositionOperator()
   {
      SetCompositionOperator( CompositionOp::SourceOver );
   }

   /*! #
    */
   double Opacity() const;

   /*! #
    */
   void SetOpacity( double op );

   /*! #
    */
   pcl::Brush BackgroundBrush() const;

   /*! #
    */
   void SetBackgroundBrush( const pcl::Brush& );

   /*! #
    */
   bool IsTransparentBackground() const;

   /*! #
    */
   bool IsOpaqueBackground() const
   {
      return !IsTransparentBackground();
   }

   /*! #
    */
   void SetTransparentBackground( bool = true );

   /*! #
    */
   void SetOpaqueBackground( bool opaque = true )
   {
      SetTransparentBackground( !opaque );
   }

   /*! #
    */
   pcl::Pen Pen() const;

   /*!
    * \deprecated Use Graphics::Pen() in newly produced code.
    */
   pcl::Pen GetPen() const
   {
      return this->Pen();
   }

   /*! #
    */
   void SetPen( const pcl::Pen& );

   /*! #
    */
   void SetPen( RGBA color, float width )
   {
      SetPen( pcl::Pen( color, width ) );
   }

   /*! #
    */
   pcl::Brush Brush() const;

   /*!
    * \deprecated Use Graphics::Brush() in newly produced code.
    */
   pcl::Brush GetBrush() const
   {
      return this->Brush();
   }

   /*! #
    */
   void SetBrush( const pcl::Brush& );

   /*! #
    */
   pcl::Font Font() const;

   /*!
    * \deprecated Use Graphics::Font() in newly produced code.
    */
   pcl::Font GetFont() const
   {
      return this->Font();
   }

   /*! #
    */
   void SetFont( const pcl::Font& );

   /*! #
    */
   void PushState();

   /*! #
    */
   void PopState();

protected:

   /*!
    * \internal
    */
   GraphicsContextBase( void* h ) : UIObject( h )
   {
   }

private:

   virtual void* CloneHandle() const;
};

// ----------------------------------------------------------------------------

/*!
 * \class Graphics
 * \brief Client-side interface to a PixInsight <em>graphics context</em> object
 *
 * ### TODO: Write a detailed description for %Graphics.
 *
 * ### TODO: Include a number of examples for drawing primitives, composition
 * operators, rendering performance, transformations, etc.
 */
class PCL_CLASS Graphics : public GraphicsContextBase
{
public:

   /*!
    * Represents a Porter-Duff alpha composition operator.
    */
   typedef GraphicsContextBase::composition_op  composition_op;

   /*!
    * Represents a polygon fill rule.
    */
   typedef GraphicsContextBase::fill_rule       fill_rule;

   /*!
    * Constructs an inactive graphics context.
    */
   Graphics() : GraphicsContextBase()
   {
   }

   /*!
    * Constructs an active graphics context, ready to draw on the specified
    * \a control.
    *
    * \warning Invoking this constructor outside a Control::OnPaint() event
    * handler is illegal on all supported platforms.
    */
   Graphics( Control& control ) : GraphicsContextBase( control ) // ### illegal outside Control::OnPaint()
   {
   }

   /*!
    * Constructs an active graphics context, ready to draw on the specified
    * \a bitmap.
    */
   Graphics( Bitmap& bitmap ) : GraphicsContextBase( bitmap )
   {
   }

   /*!
    * Constructs an active graphics context, ready to draw on the specified
    * \a svg object.
    */
   Graphics( SVG& svg ) : GraphicsContextBase( svg )
   {
   }

   /*!
    * Move constructor.
    */
   Graphics( Graphics&& x ) : GraphicsContextBase( std::move( x ) )
   {
   }

   /*!
    * Destroys a %Graphics object. If this graphics context is active, it is
    * terminated upon destruction.
    */
   virtual ~Graphics()
   {
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   Graphics& operator =( Graphics&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Returns a reference to a null %Graphics instance. A null %Graphics does
    * not correspond to an existing graphics context in the PixInsight core
    * application.
    */
   static Graphics& Null();

   /*! #
    */
   pcl::Rect ClipRect() const;

   /*! #
    */
   void SetClipRect( const pcl::Rect& r )
   {
      SetClipRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void SetClipRect( int x0, int y0, int x1, int y1 );

   /*! #
    */
   void IntersectClipRect( const pcl::Rect& r )
   {
      SetClipRect( ClipRect().Intersection( r ) );
   }

   /*! #
    */
   void IntersectClipRect( int x0, int y0, int x1, int y1 )
   {
      IntersectClipRect( pcl::Rect( x0, y0, x1, y1 ) );
   }

   /*! #
    */
   void UniteClipRect( const pcl::Rect& r )
   {
      SetClipRect( ClipRect().Union( r ) );
   }

   /*! #
    */
   void UniteClipRect( int x0, int y0, int x1, int y1 )
   {
      UniteClipRect( pcl::Rect( x0, y0, x1, y1 ) );
   }

   /*! #
    */
   pcl::Point BrushOrigin() const;

   /*! #
    */
   void SetBrushOrigin( const pcl::Point& p )
   {
      SetBrushOrigin( p.x, p.y );
   }

   /*! #
    */
   void SetBrushOrigin( int x, int y );

   /*! #
    */
   void DrawPoint( const pcl::Point& p )
   {
      DrawPoint( p.x, p.y );
   }

   /*! #
    */
   void DrawPoint( int x, int y );

   /*! #
    */
   void DrawLine( const pcl::Point& p0, const pcl::Point& p1 )
   {
      DrawLine( p0.x, p0.y, p1.x, p1.y );
   }

   /*! #
    */
   void DrawLine( int x0, int y0, int x1, int y1 );

   /*! #
    */
   void DrawRect( int x0, int y0, int x1, int y1 );

   /*! #
    */
   void DrawRect( const pcl::Rect& r )
   {
      DrawRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void StrokeRect( int x0, int y0, int x1, int y1, const pcl::Pen& pen = pcl::Pen::Null() );

   /*! #
    */
   void StrokeRect( const pcl::Rect& r, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeRect( r.x0, r.y0, r.x1, r.y1, pen );
   }

   /*! #
    */
   void FillRect( int x0, int y0, int x1, int y1, const pcl::Brush& brush = pcl::Brush::Null() );

   /*! #
    */
   void FillRect( const pcl::Rect& r, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillRect( r.x0, r.y0, r.x1, r.y1, brush );
   }

   /*! #
    */
   void EraseRect( int x0, int y0, int x1, int y1 )
   {
      FillRect( x0, y0, x1, y1, BackgroundBrush() );
   }

   /*! #
    */
   void EraseRect( const pcl::Rect& r )
   {
      EraseRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void DrawRoundedRect( int x0, int y0, int x1, int y1, double rx, double ry );

   /*! #
    */
   void DrawRoundedRect( int x0, int y0, int x1, int y1, double rxy = 25 )
   {
      DrawRoundedRect( x0, y0, x1, y1, rxy, rxy );
   }

   /*! #
    */
   void DrawRoundedRect( const pcl::Rect& r, double rx, double ry )
   {
      DrawRoundedRect( r.x0, r.y0, r.x1, r.y1, rx, ry );
   }

   /*! #
    */
   void DrawRoundedRect( const pcl::Rect& r, double rxy = 25 )
   {
      DrawRoundedRect( r.x0, r.y0, r.x1, r.y1, rxy, rxy );
   }

   /*! #
    */
   void StrokeRoundedRect( int x0, int y0, int x1, int y1, double rx, double ry, const pcl::Pen& pen = pcl::Pen::Null() );

   /*! #
    */
   void StrokeRoundedRect( int x0, int y0, int x1, int y1, double rxy = 25, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeRoundedRect( x0, y0, x1, y1, rxy, rxy, pen );
   }

   /*! #
    */
   void StrokeRoundedRect( const pcl::Rect& r, double rx, double ry, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeRoundedRect( r.x0, r.y0, r.x1, r.y1, rx, ry, pen );
   }

   /*! #
    */
   void StrokeRoundedRect( const pcl::Rect& r, double rxy = 25, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeRoundedRect( r.x0, r.y0, r.x1, r.y1, rxy, rxy, pen );
   }

   /*! #
    */
   void FillRoundedRect( int x0, int y0, int x1, int y1, double rx, double ry, const pcl::Brush& brush = pcl::Brush::Null() );

   /*! #
    */
   void FillRoundedRect( int x0, int y0, int x1, int y1, double rxy = 25, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillRoundedRect( x0, y0, x1, y1, rxy, rxy, brush );
   }

   /*! #
    */
   void FillRoundedRect( const pcl::Rect& r, double rx, double ry, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillRoundedRect( r.x0, r.y0, r.x1, r.y1, rx, ry, brush );
   }

   /*! #
    */
   void FillRoundedRect( const pcl::Rect& r, double rxy = 25, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillRoundedRect( r.x0, r.y0, r.x1, r.y1, rxy, rxy, brush );
   }

   /*! #
    */
   void EraseRoundedRect( int x0, int y0, int x1, int y1, double rx, double ry )
   {
      FillRoundedRect( x0, y0, x1, y1, rx, ry, BackgroundBrush() );
   }

   /*! #
    */
   void EraseRoundedRect( int x0, int y0, int x1, int y1, double rxy = 25 )
   {
      EraseRoundedRect( x0, y0, x1, y1, rxy, rxy );
   }

   /*! #
    */
   void EraseRoundedRect( const pcl::Rect& r, double rx, double ry )
   {
      EraseRoundedRect( r.x0, r.y0, r.x1, r.y1, rx, ry );
   }

   /*! #
    */
   void EraseRoundedRect( const pcl::Rect& r, double rxy = 25 )
   {
      EraseRoundedRect( r.x0, r.y0, r.x1, r.y1, rxy, rxy );
   }

   /*! #
    */
   void DrawEllipse( int x0, int y0, int x1, int y1 );

   /*! #
    */
   void DrawEllipse( const pcl::Rect& r )
   {
      DrawEllipse( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void StrokeEllipse( int x0, int y0, int x1, int y1, const pcl::Pen& pen = pcl::Pen::Null() );

   /*! #
    */
   void StrokeEllipse( const pcl::Rect& r, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeEllipse( r.x0, r.y0, r.x1, r.y1, pen );
   }

   /*! #
    */
   void FillEllipse( int x0, int y0, int x1, int y1, const pcl::Brush& brush = pcl::Brush::Null() );

   /*! #
    */
   void FillEllipse( const pcl::Rect& r, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillEllipse( r.x0, r.y0, r.x1, r.y1, brush );
   }

   /*! #
    */
   void EraseEllipse( int x0, int y0, int x1, int y1 )
   {
      FillEllipse( x0, y0, x1, y1, BackgroundBrush() );
   }

   /*! #
    */
   void EraseEllipse( const pcl::Rect& r )
   {
      EraseEllipse( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void DrawCircle( int xc, int yc, int r )
   {
      DrawEllipse( xc-r, yc-r, xc+r+1, yc+r+1 );
   }

   /*! #
    */
   void DrawCircle( const pcl::Point& c, int r )
   {
      DrawCircle( c.x, c.y, r );
   }

   /*! #
    */
   void StrokeCircle( int xc, int yc, int r, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeEllipse( xc-r, yc-r, xc+r+1, yc+r+1, pen );
   }

   /*! #
    */
   void StrokeCircle( const pcl::Point& c, int r, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeCircle( c.x, c.y, r, pen );
   }

   /*! #
    */
   void FillCircle( int xc, int yc, int r, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillEllipse( xc-r, yc-r, xc+r+1, yc+r+1, brush );
   }

   /*! #
    */
   void FillCircle( const pcl::Point& c, int r, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillCircle( c.x, c.y, r, brush );
   }

   /*! #
    */
   void EraseCircle( int xc, int yc, int r )
   {
      FillCircle( xc, yc, r, BackgroundBrush() );
   }

   /*! #
    */
   void EraseCircle( const pcl::Point& c, int r )
   {
      EraseCircle( c.x, c.y, r );
   }

   /*! #
    */
   void DrawPolygon( const pcl::Point* p, size_type n, fill_rule rule = FillRule::OddEven );

   /*! #
    */
   template <class C>
   void DrawPolygon( const C& c, fill_rule rule = FillRule::OddEven )
   {
      DrawPolygon( c.Begin(), c.Length(), rule );
   }

   /*! #
    */
   void StrokePolygon( const pcl::Point* p, size_type n, const pcl::Pen& pen = pcl::Pen::Null(), fill_rule rule = FillRule::OddEven );

   /*! #
    */
   template <class C>
   void StrokePolygon( const C& c, const pcl::Pen& pen = pcl::Pen::Null(), fill_rule rule = FillRule::OddEven )
   {
      StrokePolygon( c.Begin(), c.Length(), pen, rule );
   }

   /*! #
    */
   void FillPolygon( const pcl::Point* p, size_type n, const pcl::Brush& brush = pcl::Brush::Null(), fill_rule rule = FillRule::OddEven );

   /*! #
    */
   template <class C>
   void FillPolygon( const C& c, const pcl::Brush& brush = pcl::Brush::Null(), fill_rule rule = FillRule::OddEven )
   {
      FillPolygon( c.Begin(), c.Length(), brush, rule );
   }

   /*! #
    */
   void ErasePolygon( const pcl::Point* p, size_type n, fill_rule rule = FillRule::OddEven )
   {
      FillPolygon( p, n, BackgroundBrush(), rule );
   }

   /*! #
    */
   template <class C>
   void ErasePolygon( const C& c, fill_rule rule = FillRule::OddEven )
   {
      ErasePolygon( c.Begin(), c.Length(), rule );
   }

   /*! #
    */
   void DrawPolyline( const pcl::Point*, size_type n );

   /*! #
    */
   template <class C>
   void DrawPolyline( const C& c )
   {
      DrawPolyline( c.Begin(), c.Length() );
   }

   /*! #
    */
   void DrawEllipticArc( int x0, int y0, int x1, int y1, double start, double span );

   /*! #
    */
   void DrawEllipticArc( const pcl::Rect& r, double start, double span )
   {
      DrawEllipticArc( r.x0, r.y0, r.x1, r.y1, start, span );
   }

   /*! #
    */
   void DrawArc( int xc, int yc, int r, double start, double span )
   {
      DrawEllipticArc( xc-r, yc-r, xc+r+1, yc+r+1, start, span );
   }

   /*! #
    */
   void DrawArc( const pcl::Point& c, int r, double start, double span )
   {
      DrawArc( c.x, c.y, r, start, span );
   }

   /*! #
    */
   void DrawEllipticChord( int x0, int y0, int x1, int y1, double start, double span );

   /*! #
    */
   void DrawEllipticChord( const pcl::Rect& r, double start, double span )
   {
      DrawEllipticChord( r.x0, r.y0, r.x1, r.y1, start, span );
   }

   /*! #
    */
   void StrokeEllipticChord( int x0, int y0, int x1, int y1, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() );

   /*! #
    */
   void StrokeEllipticChord( const pcl::Rect& r, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeEllipticChord( r.x0, r.y0, r.x1, r.y1, start, span, pen );
   }

   /*! #
    */
   void FillEllipticChord( int x0, int y0, int x1, int y1, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() );

   /*! #
    */
   void FillEllipticChord( const pcl::Rect& r, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillEllipticChord( r.x0, r.y0, r.x1, r.y1, start, span, brush );
   }

   /*! #
    */
   void EraseEllipticChord( int x0, int y0, int x1, int y1, double start, double span )
   {
      FillEllipticChord( x0, y0, x1, y1, start, span, BackgroundBrush() );
   }

   /*! #
    */
   void EraseEllipticChord( const pcl::Rect& r, double start, double span )
   {
      EraseEllipticChord( r.x0, r.y0, r.x1, r.y1, start, span );
   }

   /*! #
    */
   void DrawChord( int xc, int yc, int r, double start, double span )
   {
      DrawEllipticChord( xc-r, yc-r, xc+r+1, yc+r+1, start, span );
   }

   /*! #
    */
   void DrawChord( const pcl::Point& c, int r, double start, double span )
   {
      DrawChord( c.x, c.y, r, start, span );
   }

   /*! #
    */
   void StrokeChord( int xc, int yc, int r, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeEllipticChord( xc-r, yc-r, xc+r+1, yc+r+1, start, span, pen );
   }

   /*! #
    */
   void StrokeChord( const pcl::Point& c, int r, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeChord( c.x, c.y, r, start, span, pen );
   }

   /*! #
    */
   void FillChord( int xc, int yc, int r, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillEllipticChord( xc-r, yc-r, xc+r+1, yc+r+1, start, span, brush );
   }

   /*! #
    */
   void FillChord( const pcl::Point& c, int r, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillChord( c.x, c.y, r, start, span, brush );
   }

   /*! #
    */
   void EraseChord( int xc, int yc, int r, double start, double span )
   {
      FillChord( xc, yc, r, start, span, BackgroundBrush() );
   }

   /*! #
    */
   void EraseChord( const pcl::Point& c, int r, double start, double span )
   {
      EraseChord( c.x, c.y, r, start, span );
   }

   /*! #
    */
   void DrawEllipticPie( int x0, int y0, int x1, int y1, double start, double span );

   /*! #
    */
   void DrawEllipticPie( const pcl::Rect& r, double start, double span )
   {
      DrawEllipticPie( r.x0, r.y0, r.x1, r.y1, start, span );
   }

   /*! #
    */
   void StrokeEllipticPie( int x0, int y0, int x1, int y1, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() );

   /*! #
    */
   void StrokeEllipticPie( const pcl::Rect& r, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeEllipticPie( r.x0, r.y0, r.x1, r.y1, start, span, pen );
   }

   /*! #
    */
   void FillEllipticPie( int x0, int y0, int x1, int y1, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() );

   /*! #
    */
   void FillEllipticPie( const pcl::Rect& r, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillEllipticPie( r.x0, r.y0, r.x1, r.y1, start, span, brush );
   }

   /*! #
    */
   void EraseEllipticPie( int x0, int y0, int x1, int y1, double start, double span )
   {
      FillEllipticPie( x0, y0, x1, y1, start, span, BackgroundBrush() );
   }

   /*! #
    */
   void EraseEllipticPie( const pcl::Rect& r, double start, double span )
   {
      EraseEllipticPie( r.x0, r.y0, r.x1, r.y1, start, span );
   }

   /*! #
    */
   void DrawPie( int xc, int yc, int r, double start, double span )
   {
      DrawEllipticPie( xc-r, yc-r, xc+r+1, yc+r+1, start, span );
   }

   /*! #
    */
   void DrawPie( const pcl::Point& c, int r, double start, double span )
   {
      DrawPie( c.x, c.y, r, start, span );
   }

   /*! #
    */
   void StrokePie( int xc, int yc, int r, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeEllipticPie( xc-r, yc-r, xc+r+1, yc+r+1, start, span, pen );
   }

   /*! #
    */
   void StrokePie( const pcl::Point& c, int r, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokePie( c.x, c.y, r, start, span, pen );
   }

   /*! #
    */
   void FillPie( int xc, int yc, int r, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillEllipticPie( xc-r, yc-r, xc+r+1, yc+r+1, start, span, brush );
   }

   /*! #
    */
   void FillPie( const pcl::Point& c, int r, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillPie( c.x, c.y, r, start, span, brush );
   }

   /*! #
    */
   void ErasePie( int xc, int yc, int r, double start, double span )
   {
      FillPie( xc, yc, r, start, span, BackgroundBrush() );
   }

   /*! #
    */
   void ErasePie( const pcl::Point& c, int r, double start, double span )
   {
      ErasePie( c.x, c.y, r, start, span );
   }

   /*! #
    */
   void DrawBitmap( int x, int y, const Bitmap& );

   /*! #
    */
   void DrawBitmap( const pcl::Point& p, const Bitmap& bm )
   {
      DrawBitmap( p.x, p.y, bm );
   }

   /*! #
    */
   void DrawBitmapRect( int x, int y, const Bitmap&, int x0, int y0, int x1, int y1 );

   /*! #
    */
   void DrawBitmapRect( const pcl::Point& p, const Bitmap& bm, const pcl::Rect& r )
   {
      DrawBitmapRect( p.x, p.y, bm, r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void DrawScaledBitmap( int x0, int y0, int x1, int y1, const Bitmap& );

   /*! #
    */
   void DrawScaledBitmap( const pcl::Rect& r, const Bitmap& bm )
   {
      DrawScaledBitmap( r.x0, r.y0, r.x1, r.y1, bm );
   }

   /*! #
    */
   void DrawScaledBitmapRect( int dx0, int dy0, int dx1, int dy1, const Bitmap&, int sx0, int sy0, int sx1, int sy1 );

   /*! #
    */
   void DrawScaledBitmapRect( const pcl::Rect& dst, const Bitmap& bm, const pcl::Rect& src )
   {
      DrawScaledBitmapRect( dst.x0, dst.y0, dst.x1, dst.y1, bm, src.x0, src.y0, src.x1, src.y1 );
   }

   /*! #
    */
   void DrawTiledBitmap( int x0, int y0, int x1, int y1, const Bitmap&, int xOffset = 0, int yOffset = 0 );

   /*! #
    */
   void DrawTiledBitmap( const pcl::Rect& r, const Bitmap& bm, const pcl::Point& offset = pcl::Point() )
   {
      DrawTiledBitmap( r.x0, r.y0, r.x1, r.y1, bm, offset.x, offset.y );
   }

   /*! #
    */
   void DrawText( int x, int y, const String& );

   /*! #
    */
   void DrawText( const pcl::Point& p, const String& s )
   {
      DrawText( p.x, p.y, s );
   }

   /*! #
    */
   void DrawTextRect( int x0, int y0, int x1, int y1, const String&, TextAlignmentFlags = TextAlign::Default );

   /*! #
    */
   void DrawTextRect( const pcl::Rect r, const String& s, TextAlignmentFlags a = TextAlign::Default )
   {
      DrawTextRect( r.x0, r.y0, r.x1, r.y1, s, a );
   }

   /*! #
    */
   pcl::Rect TextRect( int x0, int y0, int x1, int y1, const String&, TextAlignmentFlags = TextAlign::Default ) const;

   /*! #
    */
   pcl::Rect TextRect( const pcl::Rect r, const String& s, TextAlignmentFlags a = TextAlign::Default )
   {
      return TextRect( r.x0, r.y0, r.x1, r.y1, s, a );
   }

private:

   Graphics( void* h ) : GraphicsContextBase( h )
   {
   }

   friend class InterfaceDispatcher;
};

// ----------------------------------------------------------------------------

/*!
 * \class VectorGraphics
 * \brief Client-side interface to a PixInsight <em>vector graphics context</em>.
 *
 * ### TODO: Write a detailed description for %VectorGraphics.
 *
 * ### TODO: Include a number of examples for drawing primitives, composition
 * operators, rendering performance, transformations, etc.
 */
class PCL_CLASS VectorGraphics : public GraphicsContextBase
{
public:

   /*!
    * Represents a Porter-Duff alpha composition operator.
    */
   typedef GraphicsContextBase::composition_op  composition_op;

   /*!
    * Represents a polygon fill rule.
    */
   typedef GraphicsContextBase::fill_rule       fill_rule;

   /*!
    * Constructs an inactive vector graphics context.
    */
   VectorGraphics() : GraphicsContextBase()
   {
   }

   /*!
    * Constructs an active vector graphics context, ready to draw on the
    * specified \a control.
    *
    * \warning Invoking this constructor outside a Control::OnPaint() event
    * handler is illegal on all supported platforms.
    */
   VectorGraphics( Control& control ) : GraphicsContextBase( control ) // ### illegal outside Control::OnPaint()
   {
   }

   /*!
    * Constructs an active vector graphics context, ready to draw on the
    * specified \a bitmap.
    */
   VectorGraphics( Bitmap& bitmap ) : GraphicsContextBase( bitmap )
   {
   }

   /*!
    * Constructs an active vector graphics context, ready to draw on the
    * specified \a svg object.
    */
   VectorGraphics( SVG& svg ) : GraphicsContextBase( svg )
   {
   }

   /*!
    * Move constructor.
    */
   VectorGraphics( VectorGraphics&& x ) : GraphicsContextBase( std::move( x ) )
   {
   }

   /*!
    * Destroys a %VectorGraphics object. If this graphics context is active, it
    * is terminated upon destruction.
    */
   virtual ~VectorGraphics()
   {
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   VectorGraphics& operator =( VectorGraphics&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Returns a reference to a null %VectorGraphics instance. A null
    * %VectorGraphics does not correspond to an existing vector graphics
    * context in the PixInsight core application.
    */
   static VectorGraphics& Null();

   /*! #
    */
   pcl::DRect ClipRect() const;

   /*! #
    */
   template <class R>
   void SetClipRect( const R& r )
   {
      SetClipRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void SetClipRect( double x0, double y0, double x1, double y1 );

   /*! #
    */
   template <class R>
   void IntersectClipRect( const R& r )
   {
      SetClipRect( ClipRect().Intersection( r ) );
   }

   /*! #
    */
   void IntersectClipRect( double x0, double y0, double x1, double y1 )
   {
      IntersectClipRect( pcl::DRect( x0, y0, x1, y1 ) );
   }

   /*! #
    */
   template <class R>
   void UniteClipRect( const R& r )
   {
      SetClipRect( ClipRect().Union( r ) );
   }

   /*! #
    */
   void UniteClipRect( double x0, double y0, double x1, double y1 )
   {
      UniteClipRect( pcl::DRect( x0, y0, x1, y1 ) );
   }

   /*! #
    */
   pcl::DPoint BrushOrigin() const;

   /*! #
    */
   template <class P>
   void SetBrushOrigin( const P& p )
   {
      SetBrushOrigin( p.x, p.y );
   }

   /*! #
    */
   void SetBrushOrigin( double x, double y );

   /*! #
    */
   template <class P>
   void DrawPoint( const P& p )
   {
      DrawPoint( p.x, p.y );
   }

   /*! #
    */
   void DrawPoint( double x, double y );

   /*! #
    */
   template <class P0, class P1>
   void DrawLine( const P0& p0, const P1& p1 )
   {
      DrawLine( p0.x, p0.y, p1.x, p1.y );
   }

   /*! #
    */
   void DrawLine( double x0, double y0, double x1, double y1 );

   /*! #
    */
   void DrawRect( double x0, double y0, double x1, double y1 );

   /*! #
    */
   template <class R>
   void DrawRect( const R& r )
   {
      DrawRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void StrokeRect( double x0, double y0, double x1, double y1, const pcl::Pen& pen = pcl::Pen::Null() );

   /*! #
    */
   template <class R>
   void StrokeRect( const R& r, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeRect( r.x0, r.y0, r.x1, r.y1, pen );
   }

   /*! #
    */
   void FillRect( double x0, double y0, double x1, double y1, const pcl::Brush& brush = pcl::Brush::Null() );

   /*! #
    */
   template <class R>
   void FillRect( const R& r, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillRect( r.x0, r.y0, r.x1, r.y1, brush );
   }

   /*! #
    */
   void EraseRect( double x0, double y0, double x1, double y1 )
   {
      FillRect( x0, y0, x1, y1, BackgroundBrush() );
   }

   /*! #
    */
   template <class R>
   void EraseRect( const R& r )
   {
      EraseRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void DrawRoundedRect( double x0, double y0, double x1, double y1, double rx, double ry );

   /*! #
    */
   void DrawRoundedRect( double x0, double y0, double x1, double y1, double rxy = 25 )
   {
      DrawRoundedRect( x0, y0, x1, y1, rxy, rxy );
   }

   /*! #
    */
   template <class R>
   void DrawRoundedRect( const R& r, double rx, double ry )
   {
      DrawRoundedRect( r.x0, r.y0, r.x1, r.y1, rx, ry );
   }

   /*! #
    */
   template <class R>
   void DrawRoundedRect( const R& r, double rxy = 25 )
   {
      DrawRoundedRect( r.x0, r.y0, r.x1, r.y1, rxy, rxy );
   }

   /*! #
    */
   void StrokeRoundedRect( double x0, double y0, double x1, double y1, double rx, double ry, const pcl::Pen& pen = pcl::Pen::Null() );

   /*! #
    */
   void StrokeRoundedRect( double x0, double y0, double x1, double y1, double rxy = 25, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeRoundedRect( x0, y0, x1, y1, rxy, rxy, pen );
   }

   /*! #
    */
   template <class R>
   void StrokeRoundedRect( const R& r, double rx, double ry, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeRoundedRect( r.x0, r.y0, r.x1, r.y1, rx, ry, pen );
   }

   /*! #
    */
   template <class R>
   void StrokeRoundedRect( const R& r, double rxy = 25, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeRoundedRect( r.x0, r.y0, r.x1, r.y1, rxy, rxy, pen );
   }

   /*! #
    */
   void FillRoundedRect( double x0, double y0, double x1, double y1, double rx, double ry, const pcl::Brush& brush = pcl::Brush::Null() );

   /*! #
    */
   void FillRoundedRect( double x0, double y0, double x1, double y1, double rxy = 25, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillRoundedRect( x0, y0, x1, y1, rxy, rxy, brush );
   }

   /*! #
    */
   template <class R>
   void FillRoundedRect( const R& r, double rx, double ry, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillRoundedRect( r.x0, r.y0, r.x1, r.y1, rx, ry, brush );
   }

   /*! #
    */
   template <class R>
   void FillRoundedRect( const R& r, double rxy = 25, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillRoundedRect( r.x0, r.y0, r.x1, r.y1, rxy, rxy, brush );
   }

   /*! #
    */
   void EraseRoundedRect( double x0, double y0, double x1, double y1, double rx, double ry )
   {
      FillRoundedRect( x0, y0, x1, y1, rx, ry, BackgroundBrush() );
   }

   /*! #
    */
   void EraseRoundedRect( double x0, double y0, double x1, double y1, double rxy = 25 )
   {
      EraseRoundedRect( x0, y0, x1, y1, rxy, rxy );
   }

   /*! #
    */
   template <class R>
   void EraseRoundedRect( const R& r, double rx, double ry )
   {
      EraseRoundedRect( r.x0, r.y0, r.x1, r.y1, rx, ry );
   }

   /*! #
    */
   template <class R>
   void EraseRoundedRect( const R& r, double rxy = 25 )
   {
      EraseRoundedRect( r.x0, r.y0, r.x1, r.y1, rxy, rxy );
   }

   /*! #
    */
   void DrawEllipse( double x0, double y0, double x1, double y1 );

   /*! #
    */
   template <class R>
   void DrawEllipse( const R& r )
   {
      DrawEllipse( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void StrokeEllipse( double x0, double y0, double x1, double y1, const pcl::Pen& pen = pcl::Pen::Null() );

   /*! #
    */
   template <class R>
   void StrokeEllipse( const R& r, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeEllipse( r.x0, r.y0, r.x1, r.y1, pen );
   }

   /*! #
    */
   void FillEllipse( double x0, double y0, double x1, double y1, const pcl::Brush& brush = pcl::Brush::Null() );

   /*! #
    */
   template <class R>
   void FillEllipse( const R& r, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillEllipse( r.x0, r.y0, r.x1, r.y1, brush );
   }

   /*! #
    */
   void EraseEllipse( double x0, double y0, double x1, double y1 )
   {
      FillEllipse( x0, y0, x1, y1, BackgroundBrush() );
   }

   /*! #
    */
   template <class R>
   void EraseEllipse( const R& r )
   {
      EraseEllipse( r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void DrawCircle( double xc, double yc, double r )
   {
      DrawEllipse( xc-r, yc-r, xc+r, yc+r );
   }

   /*! #
    */
   template <class P>
   void DrawCircle( const P& c, double r )
   {
      DrawCircle( c.x, c.y, r );
   }

   /*! #
    */
   void StrokeCircle( double xc, double yc, double r, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeEllipse( xc-r, yc-r, xc+r, yc+r, pen );
   }

   /*! #
    */
   template <class P>
   void StrokeCircle( const P& c, double r, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeCircle( c.x, c.y, r, pen );
   }

   /*! #
    */
   void FillCircle( double xc, double yc, double r, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillEllipse( xc-r, yc-r, xc+r, yc+r, brush );
   }

   /*! #
    */
   template <class P>
   void FillCircle( const P& c, double r, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillCircle( c.x, c.y, r, brush );
   }

   /*! #
    */
   void EraseCircle( double xc, double yc, double r )
   {
      FillCircle( xc, yc, r, BackgroundBrush() );
   }

   /*! #
    */
   template <class P>
   void EraseCircle( const P& c, double r )
   {
      EraseCircle( c.x, c.y, r );
   }

   /*! #
    */
   void DrawPolygon( const pcl::DPoint* p, size_type n, fill_rule rule = FillRule::OddEven );

   /*! #
    */
   template <class C>
   void DrawPolygon( const C& c, fill_rule rule = FillRule::OddEven )
   {
      DrawPolygon( c.Begin(), c.Length(), rule );
   }

   /*! #
    */
   void StrokePolygon( const pcl::DPoint* p, size_type n, const pcl::Pen& pen = pcl::Pen::Null(), fill_rule rule = FillRule::OddEven );

   /*! #
    */
   template <class C>
   void StrokePolygon( const C& c, const pcl::Pen& pen = pcl::Pen::Null(), fill_rule rule = FillRule::OddEven )
   {
      StrokePolygon( c.Begin(), c.Length(), pen, rule );
   }

   /*! #
    */
   void FillPolygon( const pcl::DPoint* p, size_type n, const pcl::Brush& brush = pcl::Brush::Null(), fill_rule rule = FillRule::OddEven );

   /*! #
    */
   template <class C>
   void FillPolygon( const C& c, const pcl::Brush& brush = pcl::Brush::Null(), fill_rule rule = FillRule::OddEven )
   {
      FillPolygon( c.Begin(), c.Length(), brush, rule );
   }

   /*! #
    */
   void ErasePolygon( const pcl::DPoint* p, size_type n, fill_rule rule = FillRule::OddEven )
   {
      FillPolygon( p, n, BackgroundBrush(), rule );
   }

   /*! #
    */
   template <class C>
   void ErasePolygon( const C& c, fill_rule rule = FillRule::OddEven )
   {
      ErasePolygon( c.Begin(), c.Length(), rule );
   }

   /*! #
    */
   void DrawPolyline( const pcl::DPoint*, size_type n );

   /*! #
    */
   template <class C>
   void DrawPolyline( const C& c )
   {
      DrawPolyline( c.Begin(), c.Length() );
   }

   /*! #
    */
   void DrawEllipticArc( double x0, double y0, double x1, double y1, double start, double span );

   /*! #
    */
   template <class R>
   void DrawEllipticArc( const R& r, double start, double span )
   {
      DrawEllipticArc( r.x0, r.y0, r.x1, r.y1, start, span );
   }

   /*! #
    */
   void DrawArc( double xc, double yc, double r, double start, double span )
   {
      DrawEllipticArc( xc-r, yc-r, xc+r, yc+r, start, span );
   }

   /*! #
    */
   template <class P>
   void DrawArc( const P& c, double r, double start, double span )
   {
      DrawArc( c.x, c.y, r, start, span );
   }

   /*! #
    */
   void DrawEllipticChord( double x0, double y0, double x1, double y1, double start, double span );

   /*! #
    */
   template <class R>
   void DrawEllipticChord( const R& r, double start, double span )
   {
      DrawEllipticChord( r.x0, r.y0, r.x1, r.y1, start, span );
   }

   /*! #
    */
   void StrokeEllipticChord( double x0, double y0, double x1, double y1, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() );

   /*! #
    */
   template <class R>
   void StrokeEllipticChord( const R& r, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeEllipticChord( r.x0, r.y0, r.x1, r.y1, start, span, pen );
   }

   /*! #
    */
   void FillEllipticChord( double x0, double y0, double x1, double y1, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() );

   /*! #
    */
   template <class R>
   void FillEllipticChord( const R& r, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillEllipticChord( r.x0, r.y0, r.x1, r.y1, start, span, brush );
   }

   /*! #
    */
   void EraseEllipticChord( double x0, double y0, double x1, double y1, double start, double span )
   {
      FillEllipticChord( x0, y0, x1, y1, start, span, BackgroundBrush() );
   }

   /*! #
    */
   template <class R>
   void EraseEllipticChord( const R& r, double start, double span )
   {
      EraseEllipticChord( r.x0, r.y0, r.x1, r.y1, start, span );
   }

   /*! #
    */
   void DrawChord( double xc, double yc, double r, double start, double span )
   {
      DrawEllipticChord( xc-r, yc-r, xc+r, yc+r, start, span );
   }

   /*! #
    */
   template <class P>
   void DrawChord( const P& c, double r, double start, double span )
   {
      DrawChord( c.x, c.y, r, start, span );
   }

   /*! #
    */
   void StrokeChord( double xc, double yc, double r, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeEllipticChord( xc-r, yc-r, xc+r, yc+r, start, span, pen );
   }

   /*! #
    */
   template <class P>
   void StrokeChord( const P& c, double r, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeChord( c.x, c.y, r, start, span, pen );
   }

   /*! #
    */
   void FillChord( double xc, double yc, double r, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillEllipticChord( xc-r, yc-r, xc+r, yc+r, start, span, brush );
   }

   /*! #
    */
   template <class P>
   void FillChord( const P& c, double r, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillChord( c.x, c.y, r, start, span, brush );
   }

   /*! #
    */
   void EraseChord( double xc, double yc, double r, double start, double span )
   {
      FillChord( xc, yc, r, start, span, BackgroundBrush() );
   }

   /*! #
    */
   template <class P>
   void EraseChord( const P& c, double r, double start, double span )
   {
      EraseChord( c.x, c.y, r, start, span );
   }

   /*! #
    */
   void DrawEllipticPie( double x0, double y0, double x1, double y1, double start, double span );

   /*! #
    */
   template <class R>
   void DrawEllipticPie( const R& r, double start, double span )
   {
      DrawEllipticPie( r.x0, r.y0, r.x1, r.y1, start, span );
   }

   /*! #
    */
   void StrokeEllipticPie( double x0, double y0, double x1, double y1, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() );

   /*! #
    */
   template <class R>
   void StrokeEllipticPie( const R& r, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeEllipticPie( r.x0, r.y0, r.x1, r.y1, start, span, pen );
   }

   /*! #
    */
   void FillEllipticPie( double x0, double y0, double x1, double y1, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() );

   /*! #
    */
   template <class R>
   void FillEllipticPie( const R& r, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillEllipticPie( r.x0, r.y0, r.x1, r.y1, start, span, brush );
   }

   /*! #
    */
   void EraseEllipticPie( double x0, double y0, double x1, double y1, double start, double span )
   {
      FillEllipticPie( x0, y0, x1, y1, start, span, BackgroundBrush() );
   }

   /*! #
    */
   template <class R>
   void EraseEllipticPie( const R& r, double start, double span )
   {
      EraseEllipticPie( r.x0, r.y0, r.x1, r.y1, start, span );
   }

   /*! #
    */
   void DrawPie( double xc, double yc, double r, double start, double span )
   {
      DrawEllipticPie( xc-r, yc-r, xc+r, yc+r, start, span );
   }

   /*! #
    */
   template <class P>
   void DrawPie( const P& c, double r, double start, double span )
   {
      DrawPie( c.x, c.y, r, start, span );
   }

   /*! #
    */
   void StrokePie( double xc, double yc, double r, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokeEllipticPie( xc-r, yc-r, xc+r, yc+r, start, span, pen );
   }

   /*! #
    */
   template <class P>
   void StrokePie( const P& c, double r, double start, double span, const pcl::Pen& pen = pcl::Pen::Null() )
   {
      StrokePie( c.x, c.y, r, start, span, pen );
   }

   /*! #
    */
   void FillPie( double xc, double yc, double r, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillEllipticPie( xc-r, yc-r, xc+r, yc+r, start, span, brush );
   }

   /*! #
    */
   template <class P>
   void FillPie( const P& c, double r, double start, double span, const pcl::Brush& brush = pcl::Brush::Null() )
   {
      FillPie( c.x, c.y, r, start, span, brush );
   }

   /*! #
    */
   void ErasePie( double xc, double yc, double r, double start, double span )
   {
      FillPie( xc, yc, r, start, span, BackgroundBrush() );
   }

   /*! #
    */
   template <class P>
   void ErasePie( const P& c, double r, double start, double span )
   {
      ErasePie( c.x, c.y, r, start, span );
   }

   /*! #
    */
   void DrawBitmap( double x, double y, const Bitmap& );

   /*! #
    */
   template <class P>
   void DrawBitmap( const P& p, const Bitmap& bm )
   {
      DrawBitmap( p.x, p.y, bm );
   }

   /*! #
    */
   void DrawBitmapRect( double x, double y, const Bitmap&, double x0, double y0, double x1, double y1 );

   /*! #
    */
   template <class P, class R>
   void DrawBitmapRect( const P& p, const Bitmap& bm, const R& r )
   {
      DrawBitmapRect( p.x, p.y, bm, r.x0, r.y0, r.x1, r.y1 );
   }

   /*! #
    */
   void DrawScaledBitmap( double x0, double y0, double x1, double y1, const Bitmap& );

   /*! #
    */
   template <class R>
   void DrawScaledBitmap( const R& r, const Bitmap& bm )
   {
      DrawScaledBitmap( r.x0, r.y0, r.x1, r.y1, bm );
   }

   /*! #
    */
   void DrawScaledBitmapRect( double dx0, double dy0, double dx1, double dy1, const Bitmap&, double sx0, double sy0, double sx1, double sy1 );

   /*! #
    */
   template <class R1, class R2>
   void DrawScaledBitmapRect( const R1& dst, const Bitmap& bm, const R2& src )
   {
      DrawScaledBitmapRect( dst.x0, dst.y0, dst.x1, dst.y1, bm, src.x0, src.y0, src.x1, src.y1 );
   }

   /*! #
    */
   void DrawTiledBitmap( double x0, double y0, double x1, double y1, const Bitmap&, double xOffset = 0, double yOffset = 0 );

   /*! #
    */
   template <class R, class P>
   void DrawTiledBitmap( const R& r, const Bitmap& bm, const P& offset = P() )
   {
      DrawTiledBitmap( r.x0, r.y0, r.x1, r.y1, bm, offset.x, offset.y );
   }

   /*! #
    */
   void DrawText( double x, double y, const String& );

   /*! #
    */
   template <class P>
   void DrawText( const P& p, const String& s )
   {
      DrawText( p.x, p.y, s );
   }

   /*! #
    */
   void DrawTextRect( double x0, double y0, double x1, double y1, const String&, TextAlignmentFlags = TextAlign::Default );

   /*! #
    */
   template <class R>
   void DrawTextRect( const R& r, const String& s, TextAlignmentFlags a = TextAlign::Default )
   {
      DrawTextRect( r.x0, r.y0, r.x1, r.y1, s, a );
   }

   /*! #
    */
   pcl::DRect TextRect( double x0, double y0, double x1, double y1, const String&, TextAlignmentFlags = TextAlign::Default ) const;

   /*! #
    */
   template <class R>
   pcl::DRect TextRect( const R& r, const String& s, TextAlignmentFlags a = TextAlign::Default )
   {
      return TextRect( r.x0, r.y0, r.x1, r.y1, s, a );
   }

private:

   VectorGraphics( void* h ) : GraphicsContextBase( h )
   {
   }

   friend class InterfaceDispatcher;
};

// ----------------------------------------------------------------------------

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Graphics_h

// ----------------------------------------------------------------------------
// EOF pcl/Graphics.h - Released 2017-07-18T16:13:52Z
