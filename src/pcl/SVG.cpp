//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/SVG.cpp - Released 2015/12/17 18:52:18 UTC
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
#include <pcl/SVG.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SVG::SVG( const String& filePath, int width, int height ) :
   UIObject( (*API->SVG->CreateSVGFile)( ModuleHandle(), filePath.c_str(), width, height, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateSVGFile" );
}

// ----------------------------------------------------------------------------

SVG::SVG( int width, int height ) :
   UIObject( (*API->SVG->CreateSVGBuffer)( ModuleHandle(), width, height, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateSVGBuffer" );
}

// ----------------------------------------------------------------------------

SVG& SVG::Null()
{
   static SVG* nullSVG = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullSVG == nullptr )
      nullSVG = new SVG( reinterpret_cast<void*>( 0 ) );
   return *nullSVG;
}

// ----------------------------------------------------------------------------

void SVG::GetDimensions( int& w, int& h ) const
{
   if ( IsNull() )
   {
      w = h = 0;
      return;
   }

   if ( (*API->SVG->GetSVGDimensions)( handle, &w, &h ) == api_false )
      throw APIFunctionError( "GetSVGDimensions" );
}

// ----------------------------------------------------------------------------

void SVG::SetDimensions( int w, int h ) const
{
   if ( (*API->SVG->SetSVGDimensions)( handle, w, h ) == api_false )
      throw APIFunctionError( "SetSVGDimensions" );
}

// ----------------------------------------------------------------------------

DRect SVG::ViewBox() const
{
   DRect r;
   if ( (*API->SVG->GetSVGViewBox)( handle, &r.x0, &r.y0, &r.x1, &r.y1 ) == api_false )
      throw APIFunctionError( "GetSVGViewBox" );
   return r;
}

// ----------------------------------------------------------------------------

void SVG::SetViewBox( double x0, double y0, double x1, double y1 )
{
   if ( (*API->SVG->SetSVGViewBox)( handle, x0, y0, x1, y1 ) == api_false )
      throw APIFunctionError( "SetSVGViewBox" );
}

// ----------------------------------------------------------------------------

int SVG::Resolution() const
{
   return (*API->SVG->GetSVGResolution)( handle );
}

// ----------------------------------------------------------------------------

void SVG::SetResolution( int r )
{
   (*API->SVG->SetSVGResolution)( handle, r );
}

// ----------------------------------------------------------------------------

String SVG::FilePath() const
{
   if ( !IsNull() )
   {
      size_type len = 0;
      (*API->SVG->GetSVGFilePath)( handle, 0, &len );
      if ( len > 0 )
      {
         String path;
         path.SetLength( len );
         if ( (*API->SVG->GetSVGFilePath)( handle, path.c_str(), &len ) == api_false )
            throw APIFunctionError( "GetSVGFilePath" );
         path.ResizeToNullTerminated();
         return path;
      }
   }

   return String();
}

// ----------------------------------------------------------------------------

ByteArray SVG::Data() const
{
   if ( !IsNull() )
   {
      size_type len = 0;
      (*API->SVG->GetSVGDataBuffer)( handle, 0, &len );
      if ( len > 0 )
      {
         ByteArray data( len );
         if ( (*API->SVG->GetSVGDataBuffer)( handle, data.Begin(), &len ) == api_false )
            throw APIFunctionError( "GetSVGDataBuffer" );
         return data;
      }
   }

   return ByteArray();
}

// ----------------------------------------------------------------------------

String SVG::Title() const
{
   if ( !IsNull() )
   {
      size_type len = 0;
      (*API->SVG->GetSVGTitle)( handle, 0, &len );
      if ( len > 0 )
      {
         String title;
         title.SetLength( len );
         if ( (*API->SVG->GetSVGTitle)( handle, title.c_str(), &len ) == api_false )
            throw APIFunctionError( "GetSVGTitle" );
         title.ResizeToNullTerminated();
         return title;
      }
   }

   return String();
}

// ----------------------------------------------------------------------------

void SVG::SetTitle( const String& title )
{
   (*API->SVG->SetSVGTitle)( handle, title.c_str() );
}

// ----------------------------------------------------------------------------

String SVG::Description() const
{
   if ( !IsNull() )
   {
      size_type len = 0;
      (*API->SVG->GetSVGDescription)( handle, 0, &len );
      if ( len > 0 )
      {
         String description;
         description.SetLength( len );
         if ( (*API->SVG->GetSVGDescription)( handle, description.c_str(), &len ) == api_false )
            throw APIFunctionError( "GetSVGDescription" );
         description.ResizeToNullTerminated();
         return description;
      }
   }

   return String();
}

// ----------------------------------------------------------------------------

void SVG::SetDescription( const String& desc )
{
   (*API->SVG->SetSVGDescription)( handle, desc.c_str() );
}

// ----------------------------------------------------------------------------

bool SVG::IsPainting() const
{
   return (*API->SVG->IsSVGPainting)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void* SVG::CloneHandle() const
{
   throw Error( "Cannot clone a SVG handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SVG.cpp - Released 2015/12/17 18:52:18 UTC
