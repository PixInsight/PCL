//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/Cursor.cpp - Released 2017-07-09T18:07:16Z
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
#include <pcl/Bitmap.h>
#include <pcl/Cursor.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Cursor::Cursor( std_cursor stdShape ) :
   UIObject( (*API->Cursor->CreateCursor)( ModuleHandle(), stdShape ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateCursor" );
}

Cursor::Cursor( const Bitmap& p, const pcl::Point& hotSpot ) :
   UIObject( (*API->Cursor->CreateBitmapCursor)( ModuleHandle(), p.handle, hotSpot.x, hotSpot.y ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateBitmapCursor" );
}

Cursor::Cursor( const Bitmap& p, int hotSpotX, int hotSpotY ) :
   UIObject( (*API->Cursor->CreateBitmapCursor)( ModuleHandle(), p.handle, hotSpotX, hotSpotY ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateBitmapCursor" );
}

// ----------------------------------------------------------------------------

Cursor& Cursor::Null()
{
   static Cursor* nullCursor = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullCursor == nullptr )
      nullCursor = new Cursor( nullptr );
   return *nullCursor;
}

// ----------------------------------------------------------------------------

pcl::Point Cursor::HotSpot() const
{
   pcl::Point p;
   (*API->Cursor->GetCursorHotSpot)( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

pcl::Point Cursor::Position()
{
   pcl::Point p;
   (*API->Global->GetCursorPosition)( &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

void Cursor::SetPosition( int x, int y )
{
   (*API->Global->SetCursorPosition)( x, y );
}

// ----------------------------------------------------------------------------

void* Cursor::CloneHandle() const
{
   return (*API->Cursor->CloneCursor)( ModuleHandle(), handle );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Cursor.cpp - Released 2017-07-09T18:07:16Z
