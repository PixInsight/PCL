//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// pcl/Brush.cpp - Released 2015/11/26 15:59:45 UTC
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

#include <pcl/AutoLock.h>
#include <pcl/Bitmap.h>
#include <pcl/Brush.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Brush::Brush( RGBA color, style style ) :
   UIObject( (*API->Brush->CreateBrush)( ModuleHandle(), color, style ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateBrush" );
}

Brush::Brush( const Bitmap& pm ) :
   UIObject( (*API->Brush->CreateBitmapBrush)( ModuleHandle(), pm.handle ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateBitmapBrush" );
}

Brush& Brush::Null()
{
   static Brush* nullBrush = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullBrush == nullptr )
      nullBrush = new Brush( nullptr );
   return *nullBrush;
}

RGBA Brush::Color() const
{
   return (*API->Brush->GetBrushColor)( handle );
}

void Brush::SetColor( RGBA color )
{
   EnsureUnique();
   (*API->Brush->SetBrushColor)( handle, color );
}

Brush::style Brush::Style() const
{
   return style( (*API->Brush->GetBrushStyle)( handle ) );
}

void Brush::SetStyle( Brush::style style )
{
   EnsureUnique();
   (*API->Brush->SetBrushStyle)( handle, style );
}

Bitmap Brush::Stipple() const
{
   return Bitmap( (*API->Brush->GetBrushBitmap)( handle ) );
}

void Brush::SetStipple( const Bitmap& pm )
{
   EnsureUnique();
   (*API->Brush->SetBrushBitmap)( handle, pm.handle );
}

void* Brush::CloneHandle() const
{
   return (*API->Brush->CloneBrush)( ModuleHandle(), handle );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

GradientBrush::stop_list GradientBrush::Stops() const
{
   if ( !IsNull() )
   {
      size_type count = 0;
      (*API->Brush->GetBrushGradientStops)( handle, 0, &count );
      if ( count > 0 )
      {
         stop_list stops( count );
         if ( (*API->Brush->GetBrushGradientStops)( handle, reinterpret_cast<api_gradient_stop*>( stops.Begin() ), &count ) == api_false )
            throw APIFunctionError( "GetBrushGradientStops" );
         return stops;
      }
   }

   return stop_list();
}

GradientBrush::spread_mode GradientBrush::SpreadMode() const
{
   switch ( (*API->Brush->GetBrushGradientSpread)( handle ) )
   {
   case 0: return GradientSpreadMode::Pad;
   case 1: return GradientSpreadMode::Reflect;
   case 2: return GradientSpreadMode::Repeat;
   default:
      throw APIFunctionError( "GetBrushGradientSpread" );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

LinearGradientBrush::LinearGradientBrush( double x1, double y1, double x2, double y2, const stop_list& stops, spread_mode spread ) :
GradientBrush( (*API->Brush->CreateLinearGradientBrush)(
                  ModuleHandle(), x1, y1, x2, y2, spread,
                  reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateLinearGradientBrush" );
}

LinearGradientBrush::LinearGradientBrush( const DRect& r, const stop_list& stops, spread_mode spread ) :
GradientBrush( (*API->Brush->CreateLinearGradientBrush)(
                  ModuleHandle(), r.x0, r.y0, r.x1, r.y1, spread,
                  reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateLinearGradientBrush" );
}

LinearGradientBrush::LinearGradientBrush( const Rect& r, const stop_list& stops, spread_mode spread ) :
GradientBrush( (void*)0 )
{
   DRect d( r );
   void* h = (*API->Brush->CreateLinearGradientBrush)(
                  ModuleHandle(), d.x0, d.y0, d.x1, d.y1, spread,
                  reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() );
   if ( h == 0 )
      throw APIFunctionError( "CreateLinearGradientBrush" );

   SetHandle( h );
}

void LinearGradientBrush::GetParameters( double& x1, double& y1, double& x2, double& y2 ) const
{
   if ( (*API->Brush->GetBrushLinearGradientParameters)( handle, &x1, &y1, &x2, &y2 ) == api_false )
      throw APIFunctionError( "GetBrushLinearGradientParameters" );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

RadialGradientBrush::RadialGradientBrush( double cx, double cy, double r, double fx, double fy, const stop_list& stops, spread_mode spread ) :
GradientBrush( (*API->Brush->CreateRadialGradientBrush)(
                  ModuleHandle(), cx, cy, r,
                  (fx == uint32_max) ? cx : fx, (fy == uint32_max) ? cy : fy,
                  spread, reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateRadialGradientBrush" );
}

RadialGradientBrush::RadialGradientBrush( const DPoint& c, double r, const DPoint& f, const stop_list& stops, spread_mode spread ) :
GradientBrush( (*API->Brush->CreateRadialGradientBrush)(
                  ModuleHandle(), c.x, c.y, r,
                  (f.x == uint32_max) ? c.x : f.x, (f.y == uint32_max) ? c.y : f.y,
                  spread, reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateRadialGradientBrush" );
}

RadialGradientBrush::RadialGradientBrush( const Point& c, double r, const Point& f, const stop_list& stops, spread_mode spread ) :
GradientBrush( (void*)0 )
{
   DPoint dc( c );
   DPoint df( f );
   void* h = (*API->Brush->CreateRadialGradientBrush)(
                  ModuleHandle(), dc.x, dc.y, r,
                  (df.x == uint32_max) ? dc.x : df.x, (df.y == uint32_max) ? dc.y : df.y,
                  spread, reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() );
   if ( h == 0 )
      throw APIFunctionError( "CreateRadialGradientBrush" );

   SetHandle( h );
}

void RadialGradientBrush::GetParameters( double& cx, double& cy, double& r, double& fx, double& fy ) const
{
   if ( (*API->Brush->GetBrushRadialGradientParameters)( handle, &cx, &cy, &r, &fx, &fy ) == api_false )
      throw APIFunctionError( "GetBrushRadialGradientParameters" );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ConicalGradientBrush::ConicalGradientBrush( double cx, double cy, double a, const stop_list& stops ) :
GradientBrush( (*API->Brush->CreateConicalGradientBrush)( ModuleHandle(), cx, cy, a,
                                 reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateConicalGradientBrush" );
}

ConicalGradientBrush::ConicalGradientBrush( const DPoint& c, double a, const stop_list& stops ) :
GradientBrush( (*API->Brush->CreateConicalGradientBrush)( ModuleHandle(), c.x, c.y, a,
                                 reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateConicalGradientBrush" );
}

ConicalGradientBrush::ConicalGradientBrush( const Point& c, double a, const stop_list& stops ) :
GradientBrush( (void*)0 )
{
   DPoint dc( c );
   void* h = (*API->Brush->CreateConicalGradientBrush)( ModuleHandle(), dc.x, dc.y, a,
                                 reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() );
   if ( h == 0 )
      throw APIFunctionError( "CreateConicalGradientBrush" );

   SetHandle( h );
}

void ConicalGradientBrush::GetParameters( double& cx, double& cy, double& a ) const
{
   if ( (*API->Brush->GetBrushConicalGradientParameters)( handle, &cx, &cy, &a ) == api_false )
      throw APIFunctionError( "GetBrushConicalGradientParameters" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Brush.cpp - Released 2015/11/26 15:59:45 UTC
