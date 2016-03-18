//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.09.04.0322
// ----------------------------------------------------------------------------
// HDRCompositionParameters.cpp - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#include "HDRCompositionParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

HCImages*                  TheHCImagesParameter = 0;
HCImageEnabled*            TheHCImageEnabledParameter = 0;
HCImagePath*               TheHCImagePathParameter = 0;
HCInputHints*              TheHCInputHintsParameter = 0;
HCMaskBinarizingThreshold* TheHCMaskBinarizingThresholdParameter = 0;
HCMaskSmoothness*          TheHCMaskSmoothnessParameter = 0;
HCMaskGrowth*              TheHCMaskGrowthParameter = 0;
HCAutoExposures*           TheHCAutoExposuresParameter = 0;
HCRejectBlack*             TheHCRejectBlackParameter = 0;
HCUseFittingRegion*        TheHCUseFittingRegionParameter = 0;
HCFittingRectX0*           TheHCFittingRectX0Parameter = 0;
HCFittingRectY0*           TheHCFittingRectY0Parameter = 0;
HCFittingRectX1*           TheHCFittingRectX1Parameter = 0;
HCFittingRectY1*           TheHCFittingRectY1Parameter = 0;
HCGenerate64BitResult*     TheHCGenerate64BitResultParameter = 0;
HCOutputMasks*             TheHCOutputMasksParameter = 0;
HCClosePreviousImages*     TheHCClosePreviousImagesParameter = 0;

// ----------------------------------------------------------------------------

HCImages::HCImages( MetaProcess* P ) : MetaTable( P )
{
   TheHCImagesParameter = this;
}

IsoString HCImages::Id() const
{
   return "images";
}

// ----------------------------------------------------------------------------

HCImageEnabled::HCImageEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheHCImageEnabledParameter = this;
}

IsoString HCImageEnabled::Id() const
{
   return "enabled";
}

bool HCImageEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

HCImagePath::HCImagePath( MetaTable* T ) : MetaString( T )
{
   TheHCImagePathParameter = this;
}

IsoString HCImagePath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------

HCInputHints::HCInputHints( MetaProcess* P ) : MetaString( P )
{
   TheHCInputHintsParameter = this;
}

IsoString HCInputHints::Id() const
{
   return "inputHints";
}

// ----------------------------------------------------------------------------

HCMaskBinarizingThreshold::HCMaskBinarizingThreshold( MetaProcess* P ) : MetaFloat( P )
{
   TheHCMaskBinarizingThresholdParameter = this;
}

IsoString HCMaskBinarizingThreshold::Id() const
{
   return "maskBinarizingThreshold";
}

int HCMaskBinarizingThreshold::Precision() const
{
   return 4;
}

double HCMaskBinarizingThreshold::DefaultValue() const
{
   return 0.8;
}

double HCMaskBinarizingThreshold::MinimumValue() const
{
   return 0;
}

double HCMaskBinarizingThreshold::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

HCMaskSmoothness::HCMaskSmoothness( MetaProcess* P ) : MetaInt32( P )
{
   TheHCMaskSmoothnessParameter = this;
}

IsoString HCMaskSmoothness::Id() const
{
   return "maskSmoothness";
}

double HCMaskSmoothness::DefaultValue() const
{
   return 7;
}

double HCMaskSmoothness::MinimumValue() const
{
   return 1;
}

double HCMaskSmoothness::MaximumValue() const
{
   return 25;
}

// ----------------------------------------------------------------------------

HCMaskGrowth::HCMaskGrowth( MetaProcess* P ) : MetaInt32( P )
{
   TheHCMaskGrowthParameter = this;
}

IsoString HCMaskGrowth::Id() const
{
   return "maskGrowth";
}

double HCMaskGrowth::DefaultValue() const
{
   return 1;
}

double HCMaskGrowth::MinimumValue() const
{
   return 0;
}

double HCMaskGrowth::MaximumValue() const
{
   return 15;
}

// ----------------------------------------------------------------------------

HCAutoExposures::HCAutoExposures( MetaProcess* P ) : MetaBoolean( P )
{
   TheHCAutoExposuresParameter = this;
}

IsoString HCAutoExposures::Id() const
{
   return "autoExposures";
}

bool HCAutoExposures::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

HCRejectBlack::HCRejectBlack( MetaProcess* P ) : MetaBoolean( P )
{
   TheHCRejectBlackParameter = this;
}

IsoString HCRejectBlack::Id() const
{
   return "rejectBlack";
}

bool HCRejectBlack::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

HCUseFittingRegion::HCUseFittingRegion( MetaProcess* P ) : MetaBoolean( P )
{
   TheHCUseFittingRegionParameter = this;
}

IsoString HCUseFittingRegion::Id() const
{
   return "useFittingRegion";
}

bool HCUseFittingRegion::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

HCFittingRectX0::HCFittingRectX0( MetaProcess* P ) : MetaInt32( P )
{
   TheHCFittingRectX0Parameter = this;
}

IsoString HCFittingRectX0::Id() const
{
   return "fittingRectX0";
}

double HCFittingRectX0::DefaultValue() const
{
   return 0;
}

double HCFittingRectX0::MinimumValue() const
{
   return 0;
}

double HCFittingRectX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

HCFittingRectY0::HCFittingRectY0( MetaProcess* P ) : MetaInt32( P )
{
   TheHCFittingRectY0Parameter = this;
}

IsoString HCFittingRectY0::Id() const
{
   return "fittingRectY0";
}

double HCFittingRectY0::DefaultValue() const
{
   return 0;
}

double HCFittingRectY0::MinimumValue() const
{
   return 0;
}

double HCFittingRectY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

HCFittingRectX1::HCFittingRectX1( MetaProcess* P ) : MetaInt32( P )
{
   TheHCFittingRectX1Parameter = this;
}

IsoString HCFittingRectX1::Id() const
{
   return "fittingRectX1";
}

double HCFittingRectX1::DefaultValue() const
{
   return 0;
}

double HCFittingRectX1::MinimumValue() const
{
   return 0;
}

double HCFittingRectX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

HCFittingRectY1::HCFittingRectY1( MetaProcess* P ) : MetaInt32( P )
{
   TheHCFittingRectY1Parameter = this;
}

IsoString HCFittingRectY1::Id() const
{
   return "fittingRectY1";
}

double HCFittingRectY1::DefaultValue() const
{
   return 0;
}

double HCFittingRectY1::MinimumValue() const
{
   return 0;
}

double HCFittingRectY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

HCGenerate64BitResult::HCGenerate64BitResult( MetaProcess* P ) : MetaBoolean( P )
{
   TheHCGenerate64BitResultParameter = this;
}

IsoString HCGenerate64BitResult::Id() const
{
   return "generate64BitResult";
}

bool HCGenerate64BitResult::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

HCOutputMasks::HCOutputMasks( MetaProcess* P ) : MetaBoolean( P )
{
   TheHCOutputMasksParameter = this;
}

IsoString HCOutputMasks::Id() const
{
   return "outputMasks";
}

bool HCOutputMasks::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

HCClosePreviousImages::HCClosePreviousImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheHCClosePreviousImagesParameter = this;
}

IsoString HCClosePreviousImages::Id() const
{
   return "closePreviousImages";
}

bool HCClosePreviousImages::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF HDRCompositionParameters.cpp - Released 2016/02/21 20:22:43 UTC
