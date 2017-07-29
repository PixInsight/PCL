//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/ReadoutOptions.cpp - Released 2017-07-18T16:14:00Z
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

#include <pcl/ReadoutOptions.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ReadoutOptions ReadoutOptions::GetCurrentOptions()
{
   api_readout_options a;
   (*API->Global->GetReadoutOptions)( &a );

   ReadoutOptions o;
   o.data          = ReadoutOptions::readout_data( a.data );
   o.mode          = ReadoutOptions::readout_mode( a.mode );
   o.probeSize     = a.probeSize;
   o.previewSize   = a.previewSize;
   o.previewZoom   = a.previewZoom;
   o.precision     = a.precision;
   o.range         = a.range;
   o.showAlpha     = a.showAlpha;
   o.showMask      = a.showMask;
   o.showPreview   = a.showPreview;
   o.previewCenter = a.previewCenter;
   o.broadcast     = a.broadcast;
   o.real          = a.real;

   return o;
}

// ----------------------------------------------------------------------------

void ReadoutOptions::SetCurrentOptions( const ReadoutOptions& o )
{
   api_readout_options a;
   a.data          = int32( o.data );
   a.mode          = int32( o.mode );
   a.probeSize     = o.probeSize;
   a.previewSize   = o.previewSize;
   a.previewZoom   = o.previewZoom;
   a.precision     = o.precision;
   a.range         = o.range;
   a.showAlpha     = o.showAlpha;
   a.showMask      = o.showMask;
   a.showPreview   = o.showPreview;
   a.previewCenter = o.previewCenter;
   a.broadcast     = o.broadcast;
   a.real          = o.real;
   a.__r__         = 0;

   (*API->Global->SetReadoutOptions)( &a );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ReadoutOptions.cpp - Released 2017-07-18T16:14:00Z
