//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/Graphics.cpp - Released 2018-11-01T11:06:52Z
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

#include <pcl/AutoLock.h>
#include <pcl/Bitmap.h>
#include <pcl/Control.h>
#include <pcl/Graphics.h>
#include <pcl/SVG.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

GraphicsContextBase::GraphicsContextBase() :
UIObject( (*API->Graphics->CreateGraphics)( ModuleHandle() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateGraphics" );
}

GraphicsContextBase::GraphicsContextBase( Control& w ) :
UIObject( (*API->Graphics->CreateGraphics)( ModuleHandle() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateGraphics" );

   if ( (*API->Graphics->BeginControlPaint)( handle, w.handle ) == api_false )
      throw APIFunctionError( "BeginControlPaint" );
}

GraphicsContextBase::GraphicsContextBase( Bitmap& bmp ) :
UIObject( (*API->Graphics->CreateGraphics)( ModuleHandle() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateGraphics" );

   if ( (*API->Graphics->BeginBitmapPaint)( handle, bmp.handle ) == api_false )
      throw APIFunctionError( "BeginBitmapPaint" );
}

GraphicsContextBase::GraphicsContextBase( SVG& svg ) :
UIObject( (*API->Graphics->CreateGraphics)( ModuleHandle() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateGraphics" );

   if ( (*API->Graphics->BeginSVGPaint)( handle, svg.handle ) == api_false )
      throw APIFunctionError( "BeginSVGPaint" );
}

// ----------------------------------------------------------------------------

bool GraphicsContextBase::BeginPaint( Control& w )
{
   return (*API->Graphics->BeginControlPaint)( handle, w.handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool GraphicsContextBase::BeginPaint( Bitmap& pm )
{
   return (*API->Graphics->BeginBitmapPaint)( handle, pm.handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool GraphicsContextBase::BeginPaint( SVG& svg )
{
   return (*API->Graphics->BeginSVGPaint)( handle, svg.handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool GraphicsContextBase::IsPainting() const
{
   return (*API->Graphics->GetGraphicsStatus)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::EndPaint()
{
   (*API->Graphics->EndPaint)( handle );
}

// ----------------------------------------------------------------------------

bool GraphicsContextBase::IsTransformationEnabled() const
{
   return (*API->Graphics->GetGraphicsTransformationEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::EnableTransformation( bool enable )
{
   (*API->Graphics->EnableGraphicsTransformation)( handle, api_bool( enable ) );
}

// ----------------------------------------------------------------------------

Matrix GraphicsContextBase::TransformationMatrix() const
{
   Matrix M( 3, 3 );
   (*API->Graphics->GetGraphicsTransformationMatrix)(
                                       handle,
                                       &M[0][0], &M[0][1], &M[0][2],
                                       &M[1][0], &M[1][1], &M[1][2],
                                       &M[2][0], &M[2][1], &M[2][2] );
   return M;
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::SetTransformationMatrix( const Matrix& M )
{
   (*API->Graphics->SetGraphicsTransformationMatrix)( handle,
                                       M[0][0], M[0][1], M[0][2],
                                       M[1][0], M[1][1], M[1][2],
                                       M[2][0], M[2][1], M[2][2] );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::MultiplyTransformationMatrix( const Matrix& M )
{
   (*API->Graphics->MultiplyGraphicsTransformationMatrix)( handle,
                                       M[0][0], M[0][1], M[0][2],
                                       M[1][0], M[1][1], M[1][2],
                                       M[2][0], M[2][1], M[2][2] );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::RotateTransformation( double radians )
{
   (*API->Graphics->RotateGraphicsTransformation)( handle, radians );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::ScaleTransformation( double xScale, double yScale )
{
   (*API->Graphics->ScaleGraphicsTransformation)( handle, xScale, yScale );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::TranslateTransformation( double dx, double dy )
{
   (*API->Graphics->TranslateGraphicsTransformation)( handle, dx, dy );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::ShearTransformation( double sx, double sy )
{
   (*API->Graphics->ShearGraphicsTransformation)( handle, sx, sy );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::ResetTransformation()
{
   (*API->Graphics->ResetGraphicsTransformation)( handle );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::Transform( double& x, double& y ) const
{
   DPoint p( x, y );
   (*API->Graphics->TransformPoints)( handle, reinterpret_cast<double*>( &p ), size_type( 1 ) );
   x = p.x;
   y = p.y;
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::Transform( DPoint* p, size_type n ) const
{
   (*API->Graphics->TransformPoints)( handle, reinterpret_cast<double*>( p ), n );
}

// ----------------------------------------------------------------------------

bool GraphicsContextBase::IsClippingEnabled() const
{
   return (*API->Graphics->GetGraphicsClippingEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::EnableClipping( bool enable )
{
   (*API->Graphics->EnableGraphicsClipping)( handle, api_bool( enable ) );
}

// ----------------------------------------------------------------------------

bool GraphicsContextBase::IsAntialiasingEnabled() const
{
   return (*API->Graphics->GetGraphicsAntialiasingEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::EnableAntialiasing( bool enable )
{
   (*API->Graphics->EnableGraphicsAntialiasing)( handle, api_bool( enable ) );
}

// ----------------------------------------------------------------------------

bool GraphicsContextBase::IsTextAntialiasingEnabled() const
{
   return (*API->Graphics->GetGraphicsTextAntialiasingEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::EnableTextAntialiasing( bool enable )
{
   (*API->Graphics->EnableGraphicsTextAntialiasing)( handle, api_bool( enable ) );
}

// ----------------------------------------------------------------------------

bool GraphicsContextBase::IsSmoothInterpolationEnabled() const
{
   return (*API->Graphics->GetGraphicsSmoothInterpolationEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::EnableSmoothInterpolation( bool enable )
{
   (*API->Graphics->EnableGraphicsSmoothInterpolation)( handle, api_bool( enable ) );
}

// ----------------------------------------------------------------------------

GraphicsContextBase::composition_op GraphicsContextBase::CompositionOperator() const
{
   return composition_op( (*API->Graphics->GetGraphicsCompositionOperator)( handle ) );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::SetCompositionOperator( GraphicsContextBase::composition_op op )
{
   (*API->Graphics->SetGraphicsCompositionOperator)( handle, op );
}

// ----------------------------------------------------------------------------

double GraphicsContextBase::Opacity() const
{
   double op;
   (*API->Graphics->GetGraphicsOpacity)( handle, &op );
   return op;
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::SetOpacity( double opc )
{
   (*API->Graphics->SetGraphicsOpacity)( handle, opc );
}

// ----------------------------------------------------------------------------

pcl::Brush GraphicsContextBase::BackgroundBrush() const
{
   return pcl::Brush( (*API->Graphics->GetGraphicsBackgroundBrush)( handle ) );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::SetBackgroundBrush( const pcl::Brush& brush )
{
   (*API->Graphics->SetGraphicsBackgroundBrush)( handle, brush.handle );
}

// ----------------------------------------------------------------------------

bool GraphicsContextBase::IsTransparentBackground() const
{
   return (*API->Graphics->GetGraphicsTransparentBackgroundEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::SetTransparentBackground( bool enable )
{
   (*API->Graphics->SetGraphicsTransparentBackground)( handle, api_bool( enable ) );
}

// ----------------------------------------------------------------------------

pcl::Pen GraphicsContextBase::Pen() const
{
   return pcl::Pen( (*API->Graphics->GetGraphicsPen)( handle ) );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::SetPen( const pcl::Pen& pen )
{
   (*API->Graphics->SetGraphicsPen)( handle, pen.handle );
}

// ----------------------------------------------------------------------------

pcl::Brush GraphicsContextBase::Brush() const
{
   return pcl::Brush( (*API->Graphics->GetGraphicsBrush)( handle ) );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::SetBrush( const pcl::Brush& brush )
{
   (*API->Graphics->SetGraphicsBrush)( handle, brush.handle );
}

// ----------------------------------------------------------------------------

pcl::Font GraphicsContextBase::Font() const
{
   return pcl::Font( (*API->Graphics->GetGraphicsFont)( handle ) );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::SetFont( const pcl::Font& font )
{
   (*API->Graphics->SetGraphicsFont)( handle, font.handle );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::PushState()
{
   (*API->Graphics->PushGraphicsState)( handle );
}

// ----------------------------------------------------------------------------

void GraphicsContextBase::PopState()
{
   (*API->Graphics->PopGraphicsState)( handle );
}

// ----------------------------------------------------------------------------

void* GraphicsContextBase::CloneHandle() const
{
   throw Error( "Cannot clone a graphics context handle" );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Graphics& Graphics::Null()
{
   static Graphics* nullGraphics = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullGraphics == nullptr )
      nullGraphics = new Graphics( nullptr );
   return *nullGraphics;
}

// ----------------------------------------------------------------------------

pcl::Rect Graphics::ClipRect() const
{
   pcl::Rect r;
   (*API->Graphics->GetGraphicsClipRect)( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

void Graphics::SetClipRect( int x0, int y0, int x1, int y1 )
{
   (*API->Graphics->SetGraphicsClipRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

pcl::Point Graphics::BrushOrigin() const
{
   pcl::Point p;
   (*API->Graphics->GetGraphicsBrushOrigin)( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

void Graphics::SetBrushOrigin( int x, int y )
{
   (*API->Graphics->SetGraphicsBrushOrigin)( handle, x, y );
}

// ----------------------------------------------------------------------------

void Graphics::DrawPoint( int x, int y )
{
   (*API->Graphics->DrawPoint)( handle, x, y );
}

// ----------------------------------------------------------------------------

void Graphics::DrawLine( int x0, int y0, int x1, int y1 )
{
   (*API->Graphics->DrawLine)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void Graphics::DrawRect( int x0, int y0, int x1, int y1 )
{
   (*API->Graphics->DrawRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void Graphics::StrokeRect( int x0, int y0, int x1, int y1, const pcl::Pen& pen )
{
   (*API->Graphics->StrokeRect)( handle, x0, y0, x1, y1, pen.handle );
}

// ----------------------------------------------------------------------------

void Graphics::FillRect( int x0, int y0, int x1, int y1, const pcl::Brush& brush )
{
   (*API->Graphics->FillRect)( handle, x0, y0, x1, y1, brush.handle );
}

// ----------------------------------------------------------------------------

void Graphics::DrawRoundedRect( int x0, int y0, int x1, int y1, double rx, double ry )
{
   (*API->Graphics->DrawRoundedRect)( handle, x0, y0, x1, y1, rx, ry );
}

// ----------------------------------------------------------------------------

void Graphics::StrokeRoundedRect( int x0, int y0, int x1, int y1, double rx, double ry, const pcl::Pen& pen )
{
   (*API->Graphics->StrokeRoundedRect)( handle, x0, y0, x1, y1, rx, ry, pen.handle );
}

// ----------------------------------------------------------------------------

void Graphics::FillRoundedRect( int x0, int y0, int x1, int y1, double rx, double ry, const pcl::Brush& brush )
{
   (*API->Graphics->StrokeRoundedRect)( handle, x0, y0, x1, y1, rx, ry, brush.handle );
}

// ----------------------------------------------------------------------------

void Graphics::DrawEllipse( int x0, int y0, int x1, int y1 )
{
   (*API->Graphics->DrawEllipse)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void Graphics::StrokeEllipse( int x0, int y0, int x1, int y1, const pcl::Pen& pen )
{
   (*API->Graphics->StrokeEllipse)( handle, x0, y0, x1, y1, pen.handle );
}

// ----------------------------------------------------------------------------

void Graphics::FillEllipse( int x0, int y0, int x1, int y1, const pcl::Brush& brush )
{
   (*API->Graphics->FillEllipse)( handle, x0, y0, x1, y1, brush.handle );
}

// ----------------------------------------------------------------------------

void Graphics::DrawPolygon( const pcl::Point* p, size_type n, fill_rule rule )
{
   (*API->Graphics->DrawPolygon)( handle, reinterpret_cast<const int32*>( p ), n, rule );
}

// ----------------------------------------------------------------------------

void Graphics::StrokePolygon( const pcl::Point* p, size_type n, const pcl::Pen& pen, fill_rule rule )
{
   (*API->Graphics->StrokePolygon)( handle, reinterpret_cast<const int32*>( p ), n, rule, pen.handle );
}

// ----------------------------------------------------------------------------

void Graphics::FillPolygon( const pcl::Point* p, size_type n, const pcl::Brush& brush, fill_rule rule )
{
   (*API->Graphics->FillPolygon)( handle, reinterpret_cast<const int32*>( p ), n, rule, brush.handle );
}

// ----------------------------------------------------------------------------

void Graphics::DrawPolyline( const pcl::Point* points, size_type n )
{
   (*API->Graphics->DrawPolyline)( handle, reinterpret_cast<const int32*>( points ), n );
}

// ----------------------------------------------------------------------------

void Graphics::DrawEllipticArc( int x0, int y0, int x1, int y1, double start, double span )
{
   (*API->Graphics->DrawArc)( handle, x0, y0, x1, y1, start, span );
}

// ----------------------------------------------------------------------------

void Graphics::DrawEllipticChord( int x0, int y0, int x1, int y1, double start, double span )
{
   (*API->Graphics->DrawChord)( handle, x0, y0, x1, y1, start, span );
}

// ----------------------------------------------------------------------------

void Graphics::StrokeEllipticChord( int x0, int y0, int x1, int y1, double start, double span, const pcl::Pen& pen )
{
   (*API->Graphics->StrokeChord)( handle, x0, y0, x1, y1, start, span, pen.handle );
}

// ----------------------------------------------------------------------------

void Graphics::FillEllipticChord( int x0, int y0, int x1, int y1, double start, double span, const pcl::Brush& brush )
{
   (*API->Graphics->FillChord)( handle, x0, y0, x1, y1, start, span, brush.handle );
}

// ----------------------------------------------------------------------------

void Graphics::DrawEllipticPie( int x0, int y0, int x1, int y1, double start, double span )
{
   (*API->Graphics->DrawPie)( handle, x0, y0, x1, y1, start, span );
}

// ----------------------------------------------------------------------------

void Graphics::StrokeEllipticPie( int x0, int y0, int x1, int y1, double start, double span, const pcl::Pen& pen )
{
   (*API->Graphics->StrokePie)( handle, x0, y0, x1, y1, start, span, pen.handle );
}

// ----------------------------------------------------------------------------

void Graphics::FillEllipticPie( int x0, int y0, int x1, int y1, double start, double span, const pcl::Brush& brush )
{
   (*API->Graphics->FillPie)( handle, x0, y0, x1, y1, start, span, brush.handle );
}

// ----------------------------------------------------------------------------

void Graphics::DrawBitmap( int x, int y, const Bitmap& bm )
{
   (*API->Graphics->DrawBitmap)( handle, x, y, bm.handle );
}

// ----------------------------------------------------------------------------

void Graphics::DrawBitmapRect( int x, int y, const Bitmap& bm, int x0, int y0, int x1, int y1 )
{
   (*API->Graphics->DrawBitmapRect)( handle, x, y, bm.handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void Graphics::DrawScaledBitmap( int x0, int y0, int x1, int y1, const Bitmap& bm )
{
   (*API->Graphics->DrawScaledBitmap)( handle, x0, y0, x1, y1, bm.handle );
}

// ----------------------------------------------------------------------------

void Graphics::DrawScaledBitmapRect( int dx0, int dy0, int dx1, int dy1,
                           const Bitmap& bm, int sx0, int sy0, int sx1, int sy1 )
{
   (*API->Graphics->DrawScaledBitmapRect)( handle, dx0, dy0, dx1, dy1, bm.handle, sx0, sy0, sx1, sy1 );
}

// ----------------------------------------------------------------------------

void Graphics::DrawTiledBitmap( int x0, int y0, int x1, int y1, const Bitmap& bm, int ox, int oy )
{
   (*API->Graphics->DrawTiledBitmap)( handle, x0, y0, x1, y1, bm.handle, ox, oy );
}

// ----------------------------------------------------------------------------

void Graphics::DrawText( int x, int y, const String& s )
{
   (*API->Graphics->DrawText)( handle, x, y, s.c_str() );
}

// ----------------------------------------------------------------------------

void Graphics::DrawTextRect( int x0, int y0, int x1, int y1, const String& s, TextAlignmentFlags align )
{
   (*API->Graphics->DrawTextRect)( handle, x0, y0, x1, y1, s.c_str(), align );
}

// ----------------------------------------------------------------------------

pcl::Rect Graphics::TextRect( int x0, int y0, int x1, int y1, const String& s, TextAlignmentFlags align ) const
{
   pcl::Rect r;
   (*API->Graphics->GetTextRect)( handle, x0, y0, x1, y1, s.c_str(), align, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

VectorGraphics& VectorGraphics::Null()
{
   static VectorGraphics* nullGraphics = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullGraphics == nullptr )
      nullGraphics = new VectorGraphics( nullptr );
   return *nullGraphics;
}

// ----------------------------------------------------------------------------

pcl::DRect VectorGraphics::ClipRect() const
{
   pcl::DRect r;
   (*API->Graphics->GetGraphicsClipRectD)( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

void VectorGraphics::SetClipRect( double x0, double y0, double x1, double y1 )
{
   (*API->Graphics->SetGraphicsClipRectD)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

pcl::DPoint VectorGraphics::BrushOrigin() const
{
   pcl::DPoint p;
   (*API->Graphics->GetGraphicsBrushOriginD)( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

void VectorGraphics::SetBrushOrigin( double x, double y )
{
   (*API->Graphics->SetGraphicsBrushOriginD)( handle, x, y );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawPoint( double x, double y )
{
   (*API->Graphics->DrawPointD)( handle, x, y );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawLine( double x0, double y0, double x1, double y1 )
{
   (*API->Graphics->DrawLineD)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawRect( double x0, double y0, double x1, double y1 )
{
   (*API->Graphics->DrawRectD)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void VectorGraphics::StrokeRect( double x0, double y0, double x1, double y1, const pcl::Pen& pen )
{
   (*API->Graphics->StrokeRectD)( handle, x0, y0, x1, y1, pen.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::FillRect( double x0, double y0, double x1, double y1, const pcl::Brush& brush )
{
   (*API->Graphics->FillRectD)( handle, x0, y0, x1, y1, brush.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawRoundedRect( double x0, double y0, double x1, double y1, double rx, double ry )
{
   (*API->Graphics->DrawRoundedRectD)( handle, x0, y0, x1, y1, rx, ry );
}

// ----------------------------------------------------------------------------

void VectorGraphics::StrokeRoundedRect( double x0, double y0, double x1, double y1, double rx, double ry, const pcl::Pen& pen )
{
   (*API->Graphics->StrokeRoundedRectD)( handle, x0, y0, x1, y1, rx, ry, pen.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::FillRoundedRect( double x0, double y0, double x1, double y1, double rx, double ry, const pcl::Brush& brush )
{
   (*API->Graphics->StrokeRoundedRectD)( handle, x0, y0, x1, y1, rx, ry, brush.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawEllipse( double x0, double y0, double x1, double y1 )
{
   (*API->Graphics->DrawEllipseD)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void VectorGraphics::StrokeEllipse( double x0, double y0, double x1, double y1, const pcl::Pen& pen )
{
   (*API->Graphics->StrokeEllipseD)( handle, x0, y0, x1, y1, pen.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::FillEllipse( double x0, double y0, double x1, double y1, const pcl::Brush& brush )
{
   (*API->Graphics->FillEllipseD)( handle, x0, y0, x1, y1, brush.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawPolygon( const pcl::DPoint* p, size_type n, fill_rule rule )
{
   (*API->Graphics->DrawPolygonD)( handle, reinterpret_cast<const double*>( p ), n, rule );
}

// ----------------------------------------------------------------------------

void VectorGraphics::StrokePolygon( const pcl::DPoint* p, size_type n, const pcl::Pen& pen, fill_rule rule )
{
   (*API->Graphics->StrokePolygonD)( handle, reinterpret_cast<const double*>( p ), n, rule, pen.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::FillPolygon( const pcl::DPoint* p, size_type n, const pcl::Brush& brush, fill_rule rule )
{
   (*API->Graphics->FillPolygonD)( handle, reinterpret_cast<const double*>( p ), n, rule, brush.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawPolyline( const pcl::DPoint* points, size_type n )
{
   (*API->Graphics->DrawPolylineD)( handle, reinterpret_cast<const double*>( points ), n );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawEllipticArc( double x0, double y0, double x1, double y1, double start, double span )
{
   (*API->Graphics->DrawArcD)( handle, x0, y0, x1, y1, start, span );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawEllipticChord( double x0, double y0, double x1, double y1, double start, double span )
{
   (*API->Graphics->DrawChordD)( handle, x0, y0, x1, y1, start, span );
}

// ----------------------------------------------------------------------------

void VectorGraphics::StrokeEllipticChord( double x0, double y0, double x1, double y1, double start, double span, const pcl::Pen& pen )
{
   (*API->Graphics->StrokeChordD)( handle, x0, y0, x1, y1, start, span, pen.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::FillEllipticChord( double x0, double y0, double x1, double y1, double start, double span, const pcl::Brush& brush )
{
   (*API->Graphics->FillChordD)( handle, x0, y0, x1, y1, start, span, brush.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawEllipticPie( double x0, double y0, double x1, double y1, double start, double span )
{
   (*API->Graphics->DrawPieD)( handle, x0, y0, x1, y1, start, span );
}

// ----------------------------------------------------------------------------

void VectorGraphics::StrokeEllipticPie( double x0, double y0, double x1, double y1, double start, double span, const pcl::Pen& pen )
{
   (*API->Graphics->StrokePieD)( handle, x0, y0, x1, y1, start, span, pen.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::FillEllipticPie( double x0, double y0, double x1, double y1, double start, double span, const pcl::Brush& brush )
{
   (*API->Graphics->FillPieD)( handle, x0, y0, x1, y1, start, span, brush.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawBitmap( double x, double y, const Bitmap& bm )
{
   (*API->Graphics->DrawBitmapD)( handle, x, y, bm.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawBitmapRect( double x, double y, const Bitmap& bm, double x0, double y0, double x1, double y1 )
{
   (*API->Graphics->DrawBitmapRectD)( handle, x, y, bm.handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawScaledBitmap( double x0, double y0, double x1, double y1, const Bitmap& bm )
{
   (*API->Graphics->DrawScaledBitmapD)( handle, x0, y0, x1, y1, bm.handle );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawScaledBitmapRect( double dx0, double dy0, double dx1, double dy1,
                           const Bitmap& bm, double sx0, double sy0, double sx1, double sy1 )
{
   (*API->Graphics->DrawScaledBitmapRectD)( handle, dx0, dy0, dx1, dy1, bm.handle, sx0, sy0, sx1, sy1 );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawTiledBitmap( double x0, double y0, double x1, double y1, const Bitmap& bm, double ox, double oy )
{
   (*API->Graphics->DrawTiledBitmapD)( handle, x0, y0, x1, y1, bm.handle, ox, oy );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawText( double x, double y, const String& s )
{
   (*API->Graphics->DrawTextD)( handle, x, y, s.c_str() );
}

// ----------------------------------------------------------------------------

void VectorGraphics::DrawTextRect( double x0, double y0, double x1, double y1, const String& s, TextAlignmentFlags align )
{
   (*API->Graphics->DrawTextRectD)( handle, x0, y0, x1, y1, s.c_str(), align );
}

// ----------------------------------------------------------------------------

pcl::DRect VectorGraphics::TextRect( double x0, double y0, double x1, double y1, const String& s, TextAlignmentFlags align ) const
{
   pcl::DRect r;
   (*API->Graphics->GetTextRectD)( handle, x0, y0, x1, y1, s.c_str(), align, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Graphics.cpp - Released 2018-11-01T11:06:52Z
