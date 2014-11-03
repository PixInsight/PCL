// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Geometry Process Module Version 01.01.00.0245
// ****************************************************************************
// CropParameters.cpp - Released 2014/10/29 07:34:55 UTC
// ****************************************************************************
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include "CropParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

LeftMargin*       TheLeftMarginParameter = 0;
TopMargin*        TheTopMarginParameter = 0;
RightMargin*      TheRightMarginParameter = 0;
BottomMargin*     TheBottomMarginParameter = 0;
CroppingMode*     TheCroppingModeParameter = 0;
XResolution*      TheXResolutionCropParameter = 0;
YResolution*      TheYResolutionCropParameter = 0;
MetricResolution* TheMetricResolutionCropParameter = 0;
ForceResolution*  TheForceResolutionCropParameter = 0;
FillRed*          TheFillRedCropParameter = 0;
FillGreen*        TheFillGreenCropParameter = 0;
FillBlue*         TheFillBlueCropParameter = 0;
FillAlpha*        TheFillAlphaCropParameter = 0;

// ----------------------------------------------------------------------------

LeftMargin::LeftMargin( MetaProcess* P ) : MetaDouble( P )
{
   TheLeftMarginParameter = this;
}

IsoString LeftMargin::Id() const
{
   return "leftMargin";
}

int LeftMargin::Precision() const
{
   return -8;
}

// ----------------------------------------------------------------------------

TopMargin::TopMargin( MetaProcess* P ) : MetaDouble( P )
{
   TheTopMarginParameter = this;
}

IsoString TopMargin::Id() const
{
   return "topMargin";
}

int TopMargin::Precision() const
{
   return -8;
}

// ----------------------------------------------------------------------------

RightMargin::RightMargin( MetaProcess* P ) : MetaDouble( P )
{
   TheRightMarginParameter = this;
}

IsoString RightMargin::Id() const
{
   return "rightMargin";
}

int RightMargin::Precision() const
{
   return -8;
}

// ----------------------------------------------------------------------------

BottomMargin::BottomMargin( MetaProcess* P ) : MetaDouble( P )
{
   TheBottomMarginParameter = this;
}

IsoString BottomMargin::Id() const
{
   return "bottomMargin";
}

int BottomMargin::Precision() const
{
   return -8;
}

// ----------------------------------------------------------------------------

CroppingMode::CroppingMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheCroppingModeParameter = this;
}

IsoString CroppingMode::Id() const
{
   return "mode";
}

size_type CroppingMode::NumberOfElements() const
{
   return NumberOfCroppingModes;
}

IsoString CroppingMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case RelativeMargins:      return "RelativeMargins";
   case AbsolutePixels:       return "AbsolutePixels";
   case AbsoluteCentimeters:  return "AbsoluteCentimeters";
   case AbsoluteInches:       return "AbsoluteInches";
   }
}

int CroppingMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type CroppingMode::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF CropParameters.cpp - Released 2014/10/29 07:34:55 UTC
