//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/Label.cpp - Released 2017-06-21T11:36:44Z
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

#include <pcl/Label.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Label::Label( const String& text, Control& parent ) :
   Frame( (*API->Label->CreateLabel)( ModuleHandle(), this, text.c_str(), parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateLabel" );
}

// ----------------------------------------------------------------------------

String Label::Text() const
{
   size_type len = 0;
   (*API->Label->GetLabelText)( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->Label->GetLabelText)( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetLabelText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void Label::SetText( const String& text )
{
   (*API->Label->SetLabelText)( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

int Label::Margin() const
{
   return (*API->Label->GetLabelMargin)( handle );
}

// ----------------------------------------------------------------------------

void Label::SetMargin( int px )
{
   (*API->Label->SetLabelMargin)( handle, px );
}

// ----------------------------------------------------------------------------

int Label::TextAlignment() const
{
   return (*API->Label->GetLabelAlignment)( handle );
}

// ----------------------------------------------------------------------------

void Label::SetTextAlignment( int align )
{
   (*API->Label->SetLabelAlignment)( handle, align );
}

// ----------------------------------------------------------------------------

bool Label::IsWordWrappingEnabled() const
{
   return (*API->Label->GetLabelWordWrappingEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Label::EnableWordWrapping( bool enable )
{
   (*API->Label->SetLabelWordWrappingEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

bool Label::IsRichTextEnabled() const
{
   return (*API->Label->GetLabelRichTextEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Label::EnableRichText( bool enable )
{
   (*API->Label->SetLabelRichTextEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Label.cpp - Released 2017-06-21T11:36:44Z
