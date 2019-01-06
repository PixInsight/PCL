//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/Pen.cpp - Released 2018-12-12T09:24:30Z
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
#include <pcl/Brush.h>
#include <pcl/Pen.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Pen::Pen( RGBA color, float width, Pen::style style, Pen::cap cap, Pen::join join ) :
   UIObject( (*API->Pen->CreatePen)( ModuleHandle(), color, width, style, cap, join ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreatePen" );
}

// ----------------------------------------------------------------------------

Pen& Pen::Null()
{
   static Pen* nullPen = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullPen == nullptr )
      nullPen = new Pen( reinterpret_cast<void*>( 0 ) );
   return *nullPen;
}

// ----------------------------------------------------------------------------

float Pen::Width() const
{
   float width;
   if ( (*API->Pen->GetPenWidth)( handle, &width ) == api_false )
      throw APIFunctionError( "GetPenWidth" );
   return width;
}

// ----------------------------------------------------------------------------

void Pen::SetWidth( float width )
{
   EnsureUnique();
   (*API->Pen->SetPenWidth)( handle, width );
}

// ----------------------------------------------------------------------------

RGBA Pen::Color() const
{
   return (*API->Pen->GetPenColor)( handle );
}

// ----------------------------------------------------------------------------

void Pen::SetColor( RGBA color )
{
   EnsureUnique();
   (*API->Pen->SetPenColor)( handle, color );
}

// ----------------------------------------------------------------------------

Pen::style Pen::Style() const
{
   return style( (*API->Pen->GetPenStyle)( handle ) );
}

// ----------------------------------------------------------------------------

void Pen::SetStyle( Pen::style style )
{
   EnsureUnique();
   (*API->Pen->SetPenStyle)( handle, style );
}

// ----------------------------------------------------------------------------

Pen::cap Pen::Cap() const
{
   return cap( (*API->Pen->GetPenCap)( handle ) );
}

// ----------------------------------------------------------------------------

void Pen::SetCap( Pen::cap cap )
{
   EnsureUnique();
   (*API->Pen->SetPenCap)( handle, cap );
}

// ----------------------------------------------------------------------------

Pen::join Pen::Join() const
{
   return join( (*API->Pen->GetPenJoin)( handle ) );
}

// ----------------------------------------------------------------------------

void Pen::SetJoin( Pen::join join )
{
   EnsureUnique();
   (*API->Pen->SetPenJoin)( handle, join );
}

// ----------------------------------------------------------------------------

pcl::Brush Pen::Brush() const
{
   return pcl::Brush( (*API->Pen->GetPenBrush)( handle ) );
}

// ----------------------------------------------------------------------------

void Pen::SetBrush( const pcl::Brush& brush )
{
   EnsureUnique();
   (*API->Pen->SetPenBrush)( handle, brush.handle );
}

// ----------------------------------------------------------------------------

void* Pen::CloneHandle() const
{
   return (*API->Pen->ClonePen)( ModuleHandle(), handle );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Pen.cpp - Released 2018-12-12T09:24:30Z
