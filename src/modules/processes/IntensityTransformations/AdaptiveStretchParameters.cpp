// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard IntensityTransformations Process Module Version 01.07.00.0287
// ****************************************************************************
// AdaptiveStretchParameters.cpp - Released 2014/11/14 17:19:22 UTC
// ****************************************************************************
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "AdaptiveStretchParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ASNoiseThreshold* TheASNoiseThresholdParameter = 0;
ASProtection*     TheASProtectionParameter = 0;
ASUseProtection*  TheASUseProtectionParameter = 0;
ASMaxCurvePoints* TheASMaxCurvePointsParameter = 0;
ASUseROI*         TheASUseROIParameter = 0;
ASROIX0*          TheASROIX0Parameter = 0;
ASROIY0*          TheASROIY0Parameter = 0;
ASROIX1*          TheASROIX1Parameter = 0;
ASROIY1*          TheASROIY1Parameter = 0;

// ----------------------------------------------------------------------------

ASNoiseThreshold::ASNoiseThreshold( MetaProcess* P ) : MetaDouble( P )
{
   TheASNoiseThresholdParameter = this;
}

IsoString ASNoiseThreshold::Id() const
{
   return "noiseThreshold";
}

int ASNoiseThreshold::Precision() const
{
   return 8;
}

double ASNoiseThreshold::MinimumValue() const
{
   return 0;
}

double ASNoiseThreshold::MaximumValue() const
{
   return 1;
}

double ASNoiseThreshold::DefaultValue() const
{
   return 0.001;
}

// ------------------------------------------------------------------------

ASProtection::ASProtection( MetaProcess* P ) : MetaDouble( P )
{
   TheASProtectionParameter = this;
}

IsoString ASProtection::Id() const
{
   return "protection";
}

int ASProtection::Precision() const
{
   return 8;
}

double ASProtection::MinimumValue() const
{
   return 0;
}

double ASProtection::MaximumValue() const
{
   return 1;
}

double ASProtection::DefaultValue() const
{
   return 0.01;
}

// ----------------------------------------------------------------------------

ASUseProtection::ASUseProtection( MetaProcess* P ) : MetaBoolean( P )
{
   TheASUseProtectionParameter = this;
}

IsoString ASUseProtection::Id() const
{
   return "useProtection";
}

bool ASUseProtection::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ASMaxCurvePoints::ASMaxCurvePoints( MetaProcess* P ) : MetaInt32( P )
{
   TheASMaxCurvePointsParameter = this;
}

IsoString ASMaxCurvePoints::Id() const
{
   return "maxCurvePoints";
}

double ASMaxCurvePoints::DefaultValue() const
{
   return 1000000;
}

double ASMaxCurvePoints::MinimumValue() const
{
   return 32;
}

double ASMaxCurvePoints::MaximumValue() const
{
   return 100000000;
}

// ----------------------------------------------------------------------------

ASUseROI::ASUseROI( MetaProcess* P ) : MetaBoolean( P )
{
   TheASUseROIParameter = this;
}

IsoString ASUseROI::Id() const
{
   return "useROI";
}

bool ASUseROI::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ASROIX0::ASROIX0( MetaProcess* P ) : MetaInt32( P )
{
   TheASROIX0Parameter = this;
}

IsoString ASROIX0::Id() const
{
   return "roiX0";
}

double ASROIX0::DefaultValue() const
{
   return 0;
}

double ASROIX0::MinimumValue() const
{
   return 0;
}

double ASROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

ASROIY0::ASROIY0( MetaProcess* P ) : MetaInt32( P )
{
   TheASROIY0Parameter = this;
}

IsoString ASROIY0::Id() const
{
   return "roiY0";
}

double ASROIY0::DefaultValue() const
{
   return 0;
}

double ASROIY0::MinimumValue() const
{
   return 0;
}

double ASROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

ASROIX1::ASROIX1( MetaProcess* P ) : MetaInt32( P )
{
   TheASROIX1Parameter = this;
}

IsoString ASROIX1::Id() const
{
   return "roiX1";
}

double ASROIX1::DefaultValue() const
{
   return 0;
}

double ASROIX1::MinimumValue() const
{
   return 0;
}

double ASROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

ASROIY1::ASROIY1( MetaProcess* P ) : MetaInt32( P )
{
   TheASROIY1Parameter = this;
}

IsoString ASROIY1::Id() const
{
   return "roiY1";
}

double ASROIY1::DefaultValue() const
{
   return 0;
}

double ASROIY1::MinimumValue() const
{
   return 0;
}

double ASROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF AdaptiveStretchParameters.cpp - Released 2014/11/14 17:19:22 UTC
