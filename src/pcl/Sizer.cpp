//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// pcl/Sizer.cpp - Released 2017-05-02T10:39:13Z
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

#include <pcl/AutoLock.h>
#include <pcl/Control.h>
#include <pcl/Sizer.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Sizer::Sizer( bool vertical ) :
   UIObject( (*API->Sizer->CreateSizer)( ModuleHandle(), vertical ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateSizer" );
}

// ----------------------------------------------------------------------------

Sizer& Sizer::Null()
{
   static Sizer* nullSizer = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullSizer == nullptr )
      nullSizer = new Sizer( reinterpret_cast<void*>( 0 ) );
   return *nullSizer;
}

// ----------------------------------------------------------------------------

Control& Sizer::ParentControl() const
{
   control_handle hParent = (*API->Sizer->GetSizerParentControl)( handle ); // N.B.: client handle
   return (hParent != 0) ? *reinterpret_cast<Control*>( hParent ) : Control::Null();
}

// ----------------------------------------------------------------------------

bool Sizer::IsVertical() const
{
   return (*API->Sizer->GetSizerOrientation)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

int Sizer::NumberOfItems() const
{
   return (*API->Sizer->GetSizerCount)( handle );
}

// ----------------------------------------------------------------------------

int Sizer::IndexOf( const Sizer& s ) const
{
   return s.IsNull() ? -1 : (*API->Sizer->GetSizerIndex)( handle, s.handle );
}

// ----------------------------------------------------------------------------

int Sizer::IndexOf( const Control& c ) const
{
   return c.IsNull() ? -1 : (*API->Sizer->GetSizerControlIndex)( handle, c.handle );
}

// ----------------------------------------------------------------------------

void Sizer::Add( Sizer& s, int stretchFactor )
{
   if ( !s.IsNull() )
      (*API->Sizer->InsertSizer)( handle, -1, s.handle, stretchFactor );
}

// ----------------------------------------------------------------------------

void Sizer::Add( Control& c, int stretchFactor, Sizer::item_alignment align )
{
   if ( !c.IsNull() )
      (*API->Sizer->InsertSizerControl)( handle, -1, c.handle, stretchFactor, align );
}

// ----------------------------------------------------------------------------

void Sizer::AddSpacing( int size, bool autoScaling )
{
   (*API->Sizer->InsertSizerSpacing)( handle, -1, autoScaling ? LogicalPixelsToPhysical( size ) : size );
}

// ----------------------------------------------------------------------------

void Sizer::AddStretch( int stretchFactor )
{
   (*API->Sizer->InsertSizerStretch)( handle, -1, stretchFactor );
}

// ----------------------------------------------------------------------------

void Sizer::Insert( int index, Sizer& s, int stretchFactor )
{
   if ( !s.IsNull() )
      (*API->Sizer->InsertSizer)( handle, index, s.handle, stretchFactor );
}

// ----------------------------------------------------------------------------

void Sizer::Insert( int index, Control& c, int stretchFactor, Sizer::item_alignment align )
{
   if ( !c.IsNull() )
      (*API->Sizer->InsertSizerControl)( handle, index, c.handle, stretchFactor, align );
}

// ----------------------------------------------------------------------------

void Sizer::InsertSpacing( int index, int size, bool autoScaling )
{
   (*API->Sizer->InsertSizerSpacing)( handle, index, autoScaling ? LogicalPixelsToPhysical( size ) : size );
}

// ----------------------------------------------------------------------------

void Sizer::InsertStretch( int index, int stretchFactor )
{
   (*API->Sizer->InsertSizerStretch)( handle, index, stretchFactor );
}

// ----------------------------------------------------------------------------

void Sizer::Remove( Sizer& s )
{
   if ( !s.IsNull() )
      (*API->Sizer->RemoveSizer)( handle, s.handle );
}

// ----------------------------------------------------------------------------

void Sizer::Remove( Control& c )
{
   if ( !c.IsNull() )
      (*API->Sizer->RemoveSizerControl)( handle, c.handle );
}

// ----------------------------------------------------------------------------

void Sizer::SetStretchFactor( Sizer& s, int stretchFactor )
{
   if ( !s.IsNull() )
      (*API->Sizer->SetSizerStretchFactor)( handle, s.handle, stretchFactor );
}

// ----------------------------------------------------------------------------

void Sizer::SetStretchFactor( Control& c, int stretchFactor )
{
   if ( !c.IsNull() )
      (*API->Sizer->SetSizerControlStretchFactor)( handle, c.handle, stretchFactor );
}

// ----------------------------------------------------------------------------

void Sizer::SetAlignment( Sizer& s, Sizer::item_alignment align )
{
   if ( !s.IsNull() )
      (*API->Sizer->SetSizerAlignment)( handle, s.handle, align );
}

// ----------------------------------------------------------------------------

void Sizer::SetAlignment( Control& c, Sizer::item_alignment align )
{
   if ( !c.IsNull() )
      (*API->Sizer->SetSizerControlAlignment)( handle, c.handle, align );
}

// ----------------------------------------------------------------------------

int Sizer::Margin( bool autoScaling ) const
{
   int margin = (*API->Sizer->GetSizerMargin)( handle );
   return autoScaling ? PhysicalPixelsToLogical( margin ) : margin;
}

// ----------------------------------------------------------------------------

void Sizer::SetMargin( int size, bool autoScaling )
{
   (*API->Sizer->SetSizerMargin)( handle, autoScaling ? LogicalPixelsToPhysical( size ) : size );
}

// ----------------------------------------------------------------------------

int Sizer::Spacing( bool autoScaling ) const
{
   int spacing = (*API->Sizer->GetSizerSpacing)( handle );
   return autoScaling ? PhysicalPixelsToLogical( spacing ) : spacing;
}

// ----------------------------------------------------------------------------

void Sizer::SetSpacing( int size, bool autoScaling )
{
   (*API->Sizer->SetSizerSpacing)( handle, autoScaling ? LogicalPixelsToPhysical( size ) : size );
}

// ----------------------------------------------------------------------------

double Sizer::DisplayPixelRatio() const
{
   double r;
   if ( (*API->Sizer->GetSizerDisplayPixelRatio)( handle, &r ) == api_false )
      throw APIFunctionError( "GetSizerDisplayPixelRatio" );
   return r;
}

// ----------------------------------------------------------------------------

void* Sizer::CloneHandle() const
{
   throw Error( "Cannot clone a Sizer handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Sizer.cpp - Released 2017-05-02T10:39:13Z
