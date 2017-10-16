//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 01.00.00.0117
// ----------------------------------------------------------------------------
// ArcsinhStretchParameters.cpp - Released 2017-10-12T07:54:05Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017 Mark Shelley
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

#include "ArcsinhStretchParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ArcsinhStretch*                  TheArcsinhStretchParameter = nullptr;
ArcsinhStretchBlackPoint*        TheArcsinhStretchBlackPointParameter = nullptr;
ArcsinhStretchProtectHighlights* TheArcsinhStretchProtectHighlightsParameter = nullptr;
ArcsinhStretchUseRgbws*          TheArcsinhStretchUseRgbwsParameter = nullptr;
ArcsinhStretchPreviewClipped*    TheArcsinhStretchPreviewClippedParameter = nullptr;

// ----------------------------------------------------------------------------

ArcsinhStretch::ArcsinhStretch( MetaProcess* P ) : MetaFloat( P )
{
   TheArcsinhStretchParameter = this;
}

IsoString ArcsinhStretch::Id() const
{
   return "stretch";
}

int ArcsinhStretch::Precision() const
{
   return 2;
}

double ArcsinhStretch::DefaultValue() const
{
   return 1;
}

double ArcsinhStretch::MinimumValue() const
{
   return 1;
}

double ArcsinhStretch::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

ArcsinhStretchBlackPoint::ArcsinhStretchBlackPoint( MetaProcess* P ) : MetaFloat( P )
{
   TheArcsinhStretchBlackPointParameter = this;
}

IsoString ArcsinhStretchBlackPoint::Id() const
{
   return "blackPoint";
}

int ArcsinhStretchBlackPoint::Precision() const
{
   return 6;
}

double ArcsinhStretchBlackPoint::DefaultValue() const
{
   return 0;
}

double ArcsinhStretchBlackPoint::MinimumValue() const
{
   return 0;
}

double ArcsinhStretchBlackPoint::MaximumValue() const
{
   return 0.2;
}

// ----------------------------------------------------------------------------

ArcsinhStretchProtectHighlights::ArcsinhStretchProtectHighlights( MetaProcess* P ) : MetaBoolean( P )
{
   TheArcsinhStretchProtectHighlightsParameter = this;
}

IsoString ArcsinhStretchProtectHighlights::Id() const
{
   return "protectHighlights";
}

bool ArcsinhStretchProtectHighlights::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ArcsinhStretchUseRgbws::ArcsinhStretchUseRgbws( MetaProcess* P ) : MetaBoolean( P )
{
   TheArcsinhStretchUseRgbwsParameter = this;
}

IsoString ArcsinhStretchUseRgbws::Id() const
{
   return "useRGBWS";
}

bool ArcsinhStretchUseRgbws::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ArcsinhStretchPreviewClipped::ArcsinhStretchPreviewClipped( MetaProcess* P ) : MetaBoolean( P )
{
   TheArcsinhStretchPreviewClippedParameter = this;
}

IsoString ArcsinhStretchPreviewClipped::Id() const
{
   return "previewClipped";
}

bool ArcsinhStretchPreviewClipped::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchParameters.cpp - Released 2017-10-12T07:54:05Z
