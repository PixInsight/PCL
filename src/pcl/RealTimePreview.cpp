//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/RealTimePreview.cpp - Released 2015/12/17 18:52:18 UTC
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

#include <pcl/RealTimePreview.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Exception.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

bool RealTimePreview::SetOwner( ProcessInterface& iface )
{
   interface_handle h = iface.IsNull() ? 0 : reinterpret_cast<interface_handle>( &iface );
   return (*API->RealTimePreview->SetRealTimePreviewOwner)( h, 0/*flags*/ ) != api_false;
}

// ----------------------------------------------------------------------------

void RealTimePreview::Update()
{
   (*API->RealTimePreview->UpdateRealTimePreview)();
}

// ----------------------------------------------------------------------------

bool RealTimePreview::IsUpdating()
{
   return (*API->RealTimePreview->IsRealTimePreviewUpdating)() != api_false;
}

// ----------------------------------------------------------------------------

void RealTimePreview::ShowProgressDialog( const String& title, const String& text, size_type totalCount )
{
   (*API->RealTimePreview->ShowRealTimePreviewProgressDialog)( title.c_str(), text.c_str(), totalCount, 0/*flags*/ );
}

// ----------------------------------------------------------------------------

void RealTimePreview::CloseProgressDialog()
{
   (*API->RealTimePreview->CloseRealTimePreviewProgressDialog)();
}

// ----------------------------------------------------------------------------

bool RealTimePreview::IsProgressDialogVisible()
{
   return (*API->RealTimePreview->IsRealTimePreviewProgressDialogVisible)() != api_false;
}

// ----------------------------------------------------------------------------

void RealTimePreview::SetProgressCount( size_type count )
{
   (*API->RealTimePreview->SetRealTimePreviewProgressCount)( count, 0/*flags*/ );
}

// ----------------------------------------------------------------------------

void RealTimePreview::SetProgressText( const String& text )
{
   (*API->RealTimePreview->SetRealTimePreviewProgressText)( text.c_str(), 0/*flags*/ );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/RealTimePreview.cpp - Released 2015/12/17 18:52:18 UTC
