//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.02.00.0238
// ----------------------------------------------------------------------------
// BackgroundNeutralizationParameters.cpp - Released 2016/02/21 20:22:42 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "BackgroundNeutralizationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

BNBackgroundReferenceViewId* TheBNBackgroundReferenceViewIdParameter = 0;
BNBackgroundLow*             TheBNBackgroundLowParameter = 0;
BNBackgroundHigh*            TheBNBackgroundHighParameter = 0;
BNUseROI*                    TheBNUseROIParameter = 0;
BNROIX0*                     TheBNROIX0Parameter = 0;
BNROIY0*                     TheBNROIY0Parameter = 0;
BNROIX1*                     TheBNROIX1Parameter = 0;
BNROIY1*                     TheBNROIY1Parameter = 0;
BNMode*                      TheBNModeParameter = 0;
BNTargetBackground*          TheBNTargetBackgroundParameter = 0;

// ----------------------------------------------------------------------------

BNBackgroundReferenceViewId::BNBackgroundReferenceViewId( MetaProcess* P ) : MetaString( P )
{
   TheBNBackgroundReferenceViewIdParameter = this;
}

IsoString BNBackgroundReferenceViewId::Id() const
{
   return "backgroundReferenceViewId";
}

// ----------------------------------------------------------------------------

BNBackgroundLow::BNBackgroundLow( MetaProcess* P ) : MetaFloat( P )
{
   TheBNBackgroundLowParameter = this;
}

IsoString BNBackgroundLow::Id() const
{
   return "backgroundLow";
}

int BNBackgroundLow::Precision() const
{
   return 7;
}

double BNBackgroundLow::MinimumValue() const
{
   return 0;
}

double BNBackgroundLow::MaximumValue() const
{
   return 1;
}

double BNBackgroundLow::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

BNBackgroundHigh::BNBackgroundHigh( MetaProcess* P ) : MetaFloat( P )
{
   TheBNBackgroundHighParameter = this;
}

IsoString BNBackgroundHigh::Id() const
{
   return "backgroundHigh";
}

int BNBackgroundHigh::Precision() const
{
   return 7;
}

double BNBackgroundHigh::MinimumValue() const
{
   return 0;
}

double BNBackgroundHigh::MaximumValue() const
{
   return 1;
}

double BNBackgroundHigh::DefaultValue() const
{
   return 0.1;
}

// ----------------------------------------------------------------------------

BNUseROI::BNUseROI( MetaProcess* P ) : MetaBoolean( P )
{
   TheBNUseROIParameter = this;
}

IsoString BNUseROI::Id() const
{
   return "useROI";
}

bool BNUseROI::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

BNROIX0::BNROIX0( MetaProcess* P ) : MetaInt32( P )
{
   TheBNROIX0Parameter = this;
}

IsoString BNROIX0::Id() const
{
   return "roiX0";
}

double BNROIX0::DefaultValue() const
{
   return 0;
}

double BNROIX0::MinimumValue() const
{
   return 0;
}

double BNROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

BNROIY0::BNROIY0( MetaProcess* P ) : MetaInt32( P )
{
   TheBNROIY0Parameter = this;
}

IsoString BNROIY0::Id() const
{
   return "roiY0";
}

double BNROIY0::DefaultValue() const
{
   return 0;
}

double BNROIY0::MinimumValue() const
{
   return 0;
}

double BNROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

BNROIX1::BNROIX1( MetaProcess* P ) : MetaInt32( P )
{
   TheBNROIX1Parameter = this;
}

IsoString BNROIX1::Id() const
{
   return "roiX1";
}

double BNROIX1::DefaultValue() const
{
   return 0;
}

double BNROIX1::MinimumValue() const
{
   return 0;
}

double BNROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

BNROIY1::BNROIY1( MetaProcess* P ) : MetaInt32( P )
{
   TheBNROIY1Parameter = this;
}

IsoString BNROIY1::Id() const
{
   return "roiY1";
}

double BNROIY1::DefaultValue() const
{
   return 0;
}

double BNROIY1::MinimumValue() const
{
   return 0;
}

double BNROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

BNMode::BNMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheBNModeParameter = this;
}

IsoString BNMode::Id() const
{
   return "mode";
}

size_type BNMode::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString BNMode::ElementId( size_type index ) const
{
   switch ( index )
   {
   default:
   case TargetBackground: return "TargetBackground";
   case Rescale:          return "Rescale";
   case RescaleAsNeeded:  return "RescaleAsNeeded";
   case Truncate:         return "Truncate";
   }
}

int BNMode::ElementValue( size_type index ) const
{
   return int( index );
}

size_type BNMode::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

BNTargetBackground::BNTargetBackground( MetaProcess* P ) : MetaFloat( P )
{
   TheBNTargetBackgroundParameter = this;
}

IsoString BNTargetBackground::Id() const
{
   return "targetBackground";
}

int BNTargetBackground::Precision() const
{
   return 7;
}

double BNTargetBackground::MinimumValue() const
{
   return 0;
}

double BNTargetBackground::MaximumValue() const
{
   return 1;
}

double BNTargetBackground::DefaultValue() const
{
   return 0.001;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BackgroundNeutralizationParameters.cpp - Released 2016/02/21 20:22:42 UTC
