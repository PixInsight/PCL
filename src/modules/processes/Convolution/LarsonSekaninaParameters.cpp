//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0265
// ----------------------------------------------------------------------------
// LarsonSekaninaParameters.cpp - Released 2018-11-01T11:07:20Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "LarsonSekaninaParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

LSInterpolation*     TheLSInterpolationParameter = 0;
LSRadiusDiff*        TheLSRadiusDiffParameter = 0;
LSAngleDiff*         TheLSAngleDiffParameter = 0;
LSCenterX*           TheLSCenterXParameter = 0;
LSCenterY*           TheLSCenterYParameter = 0;
LSAmount*            TheLSAmountParameter = 0;
LSThreshold*         TheLSThresholdParameter = 0;
LSDeringing*         TheLSDeringingParameter = 0;
LSRangeLow*          TheLSRangeLowParameter = 0;
LSRangeHigh*         TheLSRangeHighParameter = 0;
LSUseLuminance*      TheLSUseLuminanceParameter = 0;
LSHighPass*          TheLSHighPassParameter = 0;
LSDisableExtension*  TheLSDisableExtensionParameter = 0;

// ----------------------------------------------------------------------------

LSInterpolation::LSInterpolation( MetaProcess* p ) : MetaEnumeration( p )
{
   TheLSInterpolationParameter = this;
}

IsoString LSInterpolation::Id() const
{
   return "interpolation";
}

size_type LSInterpolation::NumberOfElements() const
{
   return NumberOfInterpolationModes;
}

IsoString LSInterpolation::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Bilinear:       return "Bilinear";
   case Bicubic:        return "Bicubic";
   case BicubicSpline:  return "BicubicSpline";
   case BicubicBSpline: return "BicubicBSpline";
   }
}

int LSInterpolation::ElementValue( size_type i ) const
{
   return int( i );
}

size_type LSInterpolation::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

LSRadiusDiff::LSRadiusDiff( MetaProcess* P ) : MetaFloat( P )
{
   TheLSRadiusDiffParameter = this;
}

IsoString LSRadiusDiff::Id() const
{
   return "radiusIncrement";
}

int LSRadiusDiff::Precision() const
{
   return 3;
}

double LSRadiusDiff::MinimumValue() const
{
   return 0;
}

double LSRadiusDiff::MaximumValue() const
{
   return 100;
}

double LSRadiusDiff::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

LSAngleDiff::LSAngleDiff( MetaProcess* P ) : MetaFloat( P )
{
   TheLSAngleDiffParameter = this;
}

IsoString LSAngleDiff::Id() const
{
   return "angleIncrement";
}

int LSAngleDiff::Precision() const
{
   return 3;
}

double LSAngleDiff::MinimumValue() const
{
   return 0;
}

double LSAngleDiff::MaximumValue() const
{
   return 90;
}

double LSAngleDiff::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

LSCenterX::LSCenterX( MetaProcess* P ) : MetaFloat( P )
{
   TheLSCenterXParameter = this;
}

IsoString LSCenterX::Id() const
{
   return "centerX";
}

int LSCenterX::Precision() const
{
   return 3;
}

double LSCenterX::MinimumValue() const
{
   return -100000.0;
}

double LSCenterX::MaximumValue() const
{
   return 100000.0;
}

double LSCenterX::DefaultValue() const
{
   return 0.0;
}

// ----------------------------------------------------------------------------

LSCenterY::LSCenterY( MetaProcess* P ) : MetaFloat( P )
{
   TheLSCenterYParameter = this;
}

IsoString LSCenterY::Id() const
{
   return "centerY";
}

int LSCenterY::Precision() const
{
   return 3;
}

double LSCenterY::MinimumValue() const
{
   return -100000.0;
}

double LSCenterY::MaximumValue() const
{
   return 100000.0;
}

double LSCenterY::DefaultValue() const
{
   return 0.0;
}

// ----------------------------------------------------------------------------

LSAmount::LSAmount( MetaProcess* P ) : MetaFloat( P )
{
   TheLSAmountParameter = this;
}

IsoString LSAmount::Id() const
{
   return "amount";
}

int LSAmount::Precision() const
{
   return 3;
}

double LSAmount::MinimumValue() const
{
   return 0.001;
}

double LSAmount::MaximumValue() const
{
   return 1;
}

double LSAmount::DefaultValue() const
{
   return 0.10;
}

// ----------------------------------------------------------------------------

LSThreshold::LSThreshold( MetaProcess* P ) : MetaFloat( P )
{
   TheLSThresholdParameter = this;
}

IsoString LSThreshold::Id() const
{
   return "threshold";
}

int LSThreshold::Precision() const
{
   return 7;
}

double LSThreshold::MinimumValue() const
{
   return 0;
}

double LSThreshold::MaximumValue() const
{
   return 1;
}

double LSThreshold::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

LSDeringing::LSDeringing( MetaProcess* P ) : MetaFloat( P )
{
   TheLSDeringingParameter = this;
}

IsoString LSDeringing::Id() const
{
   return "deringing";
}

int LSDeringing::Precision() const
{
   return 2;
}

double LSDeringing::MinimumValue() const
{
   return 0;
}

double LSDeringing::MaximumValue() const
{
   return 1;
}

double LSDeringing::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

LSRangeLow::LSRangeLow( MetaProcess* P ) : MetaFloat( P )
{
   TheLSRangeLowParameter = this;
}

IsoString LSRangeLow::Id() const
{
   return "rangeLow";
}

int LSRangeLow::Precision() const
{
   return 7;
}

double LSRangeLow::MinimumValue() const
{
   return 0;
}

double LSRangeLow::MaximumValue() const
{
   return 1;
}

double LSRangeLow::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

LSRangeHigh::LSRangeHigh( MetaProcess* P ) : MetaFloat( P )
{
   TheLSRangeHighParameter = this;
}

IsoString LSRangeHigh::Id() const
{
   return "rangeHigh";
}

int LSRangeHigh::Precision() const
{
   return 7;
}

double LSRangeHigh::MinimumValue() const
{
   return 0;
}

double LSRangeHigh::MaximumValue() const
{
   return 1;
}

double LSRangeHigh::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

LSUseLuminance::LSUseLuminance( MetaProcess* P ) : MetaBoolean( P )
{
   TheLSUseLuminanceParameter = this;
}

IsoString LSUseLuminance::Id() const
{
   return "useLuminance";
}

bool LSUseLuminance::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LSHighPass::LSHighPass( MetaProcess* P ) : MetaBoolean( P )
{
   TheLSHighPassParameter = this;
}

IsoString LSHighPass::Id() const
{
   return "highPass";
}

bool LSHighPass::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LSDisableExtension::LSDisableExtension( MetaProcess* P ) : MetaBoolean( P )
{
   TheLSDisableExtensionParameter = this;
}

IsoString LSDisableExtension::Id() const
{
   return "disableExtension";
}

bool LSDisableExtension::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LarsonSekaninaParameters.cpp - Released 2018-11-01T11:07:20Z
