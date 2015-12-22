//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0351
// ----------------------------------------------------------------------------
// MaskedStretchParameters.cpp - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "MaskedStretchParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

MSTargetBackground*          TheMSTargetBackgroundParameter = 0;
MSNumberOfIterations*        TheMSNumberOfIterationsParameter = 0;
MSClippingFraction*          TheMSClippingFractionParameter = 0;
MSBackgroundReferenceViewId* TheMSBackgroundReferenceViewIdParameter = 0;
MSBackgroundLow*             TheMSBackgroundLowParameter = 0;
MSBackgroundHigh*            TheMSBackgroundHighParameter = 0;
MSUseROI*                    TheMSUseROIParameter = 0;
MSROIX0*                     TheMSROIX0Parameter = 0;
MSROIY0*                     TheMSROIY0Parameter = 0;
MSROIX1*                     TheMSROIX1Parameter = 0;
MSROIY1*                     TheMSROIY1Parameter = 0;
MSMaskType*                  TheMSMaskTypeParameter = 0;

// ----------------------------------------------------------------------------

MSTargetBackground::MSTargetBackground( MetaProcess* P ) : MetaDouble( P )
{
   TheMSTargetBackgroundParameter = this;
}

IsoString MSTargetBackground::Id() const
{
   return "targetBackground";
}

int MSTargetBackground::Precision() const
{
   return 8;
}

double MSTargetBackground::MinimumValue() const
{
   return 0;
}

double MSTargetBackground::MaximumValue() const
{
   return 1;
}

double MSTargetBackground::DefaultValue() const
{
   return 0.125;
}

// ----------------------------------------------------------------------------

MSNumberOfIterations::MSNumberOfIterations( MetaProcess* P ) : MetaInt32( P )
{
   TheMSNumberOfIterationsParameter = this;
}

IsoString MSNumberOfIterations::Id() const
{
   return "numberOfIterations";
}

double MSNumberOfIterations::DefaultValue() const
{
   return 100;
}

double MSNumberOfIterations::MinimumValue() const
{
   return 2;
}

double MSNumberOfIterations::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

MSClippingFraction::MSClippingFraction( MetaProcess* P ) : MetaDouble( P )
{
   TheMSClippingFractionParameter = this;
}

IsoString MSClippingFraction::Id() const
{
   return "clippingFraction";
}

int MSClippingFraction::Precision() const
{
   return 8;
}

double MSClippingFraction::MinimumValue() const
{
   return 0;
}

double MSClippingFraction::MaximumValue() const
{
   return 1;
}

double MSClippingFraction::DefaultValue() const
{
   return 0.0005;
}

// ----------------------------------------------------------------------------

MSBackgroundReferenceViewId::MSBackgroundReferenceViewId( MetaProcess* P ) : MetaString( P )
{
   TheMSBackgroundReferenceViewIdParameter = this;
}

IsoString MSBackgroundReferenceViewId::Id() const
{
   return "backgroundReferenceViewId";
}

// ----------------------------------------------------------------------------

MSBackgroundLow::MSBackgroundLow( MetaProcess* P ) : MetaDouble( P )
{
   TheMSBackgroundLowParameter = this;
}

IsoString MSBackgroundLow::Id() const
{
   return "backgroundLow";
}

int MSBackgroundLow::Precision() const
{
   return 8;
}

double MSBackgroundLow::MinimumValue() const
{
   return 0;
}

double MSBackgroundLow::MaximumValue() const
{
   return 1;
}

double MSBackgroundLow::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

MSBackgroundHigh::MSBackgroundHigh( MetaProcess* P ) : MetaDouble( P )
{
   TheMSBackgroundHighParameter = this;
}

IsoString MSBackgroundHigh::Id() const
{
   return "backgroundHigh";
}

int MSBackgroundHigh::Precision() const
{
   return 8;
}

double MSBackgroundHigh::MinimumValue() const
{
   return 0;
}

double MSBackgroundHigh::MaximumValue() const
{
   return 1;
}

double MSBackgroundHigh::DefaultValue() const
{
   return 0.05;
}

// ----------------------------------------------------------------------------

MSUseROI::MSUseROI( MetaProcess* P ) : MetaBoolean( P )
{
   TheMSUseROIParameter = this;
}

IsoString MSUseROI::Id() const
{
   return "useROI";
}

bool MSUseROI::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

MSROIX0::MSROIX0( MetaProcess* P ) : MetaInt32( P )
{
   TheMSROIX0Parameter = this;
}

IsoString MSROIX0::Id() const
{
   return "roiX0";
}

double MSROIX0::DefaultValue() const
{
   return 0;
}

double MSROIX0::MinimumValue() const
{
   return 0;
}

double MSROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

MSROIY0::MSROIY0( MetaProcess* P ) : MetaInt32( P )
{
   TheMSROIY0Parameter = this;
}

IsoString MSROIY0::Id() const
{
   return "roiY0";
}

double MSROIY0::DefaultValue() const
{
   return 0;
}

double MSROIY0::MinimumValue() const
{
   return 0;
}

double MSROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

MSROIX1::MSROIX1( MetaProcess* P ) : MetaInt32( P )
{
   TheMSROIX1Parameter = this;
}

IsoString MSROIX1::Id() const
{
   return "roiX1";
}

double MSROIX1::DefaultValue() const
{
   return 0;
}

double MSROIX1::MinimumValue() const
{
   return 0;
}

double MSROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

MSROIY1::MSROIY1( MetaProcess* P ) : MetaInt32( P )
{
   TheMSROIY1Parameter = this;
}

IsoString MSROIY1::Id() const
{
   return "roiY1";
}

double MSROIY1::DefaultValue() const
{
   return 0;
}

double MSROIY1::MinimumValue() const
{
   return 0;
}

double MSROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

MSMaskType::MSMaskType( MetaProcess* P ) : MetaEnumeration( P )
{
   TheMSMaskTypeParameter = this;
}

IsoString MSMaskType::Id() const
{
   return "maskType";
}

size_type MSMaskType::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString MSMaskType::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Intensity: return "MaskType_Intensity";
   case Value:     return "MaskType_Value";
   }
}

int MSMaskType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type MSMaskType::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MaskedStretchParameters.cpp - Released 2015/12/18 08:55:08 UTC
