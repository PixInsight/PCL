//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.00.0322
// ----------------------------------------------------------------------------
// CropParameters.cpp - Released 2016/11/17 18:14:58 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "CropParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CRLeftMargin*     TheCRLeftMarginParameter = nullptr;
CRTopMargin*      TheCRTopMarginParameter = nullptr;
CRRightMargin*    TheCRRightMarginParameter = nullptr;
CRBottomMargin*   TheCRBottomMarginParameter = nullptr;
CRMode*           TheCRModeParameter = nullptr;
XResolution*      TheCRXResolutionParameter = nullptr;
YResolution*      TheCRYResolutionParameter = nullptr;
MetricResolution* TheCRMetricResolutionParameter = nullptr;
ForceResolution*  TheCRForceResolutionParameter = nullptr;
FillRed*          TheCRFillRedParameter = nullptr;
FillGreen*        TheCRFillGreenParameter = nullptr;
FillBlue*         TheCRFillBlueParameter = nullptr;
FillAlpha*        TheCRFillAlphaParameter = nullptr;

// ----------------------------------------------------------------------------

CRLeftMargin::CRLeftMargin( MetaProcess* P ) : MetaDouble( P )
{
   TheCRLeftMarginParameter = this;
}

IsoString CRLeftMargin::Id() const
{
   return "leftMargin";
}

int CRLeftMargin::Precision() const
{
   return -8;
}

// ----------------------------------------------------------------------------

CRTopMargin::CRTopMargin( MetaProcess* P ) : MetaDouble( P )
{
   TheCRTopMarginParameter = this;
}

IsoString CRTopMargin::Id() const
{
   return "topMargin";
}

int CRTopMargin::Precision() const
{
   return -8;
}

// ----------------------------------------------------------------------------

CRRightMargin::CRRightMargin( MetaProcess* P ) : MetaDouble( P )
{
   TheCRRightMarginParameter = this;
}

IsoString CRRightMargin::Id() const
{
   return "rightMargin";
}

int CRRightMargin::Precision() const
{
   return -8;
}

// ----------------------------------------------------------------------------

CRBottomMargin::CRBottomMargin( MetaProcess* P ) : MetaDouble( P )
{
   TheCRBottomMarginParameter = this;
}

IsoString CRBottomMargin::Id() const
{
   return "bottomMargin";
}

int CRBottomMargin::Precision() const
{
   return -8;
}

// ----------------------------------------------------------------------------

CRMode::CRMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheCRModeParameter = this;
}

IsoString CRMode::Id() const
{
   return "mode";
}

size_type CRMode::NumberOfElements() const
{
   return NumberOfCRModes;
}

IsoString CRMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case RelativeMargins:     return "RelativeMargins";
   case AbsolutePixels:      return "AbsolutePixels";
   case AbsoluteCentimeters: return "AbsoluteCentimeters";
   case AbsoluteInches:      return "AbsoluteInches";
   }
}

int CRMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type CRMode::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CropParameters.cpp - Released 2016/11/17 18:14:58 UTC
