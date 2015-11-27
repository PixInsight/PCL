//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard AssistedColorCalibration Process Module Version 01.00.00.0155
// ----------------------------------------------------------------------------
// AssistedColorCalibrationParameters.cpp - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard AssistedColorCalibration PixInsight module.
//
// Copyright (c) 2010-2015 Zbynek Vrastil
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#include "AssistedColorCalibrationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ACCRedCorrectionFactor*    TheACCRedCorrectionFactor = 0;
ACCGreenCorrectionFactor*  TheACCGreenCorrectionFactor = 0;
ACCBlueCorrectionFactor*   TheACCBlueCorrectionFactor = 0;
ACCBackgroundReference*    TheACCBackgroundReference = 0;
ACCHistogramShadows*       TheACCHistogramShadows = 0;
ACCHistogramHighlights*    TheACCHistogramHighlights = 0;
ACCHistogramMidtones*      TheACCHistogramMidtones = 0;
ACCSaturationBoost*        TheACCSaturationBoost = 0;

// ----------------------------------------------------------------------------

ACCRedCorrectionFactor::ACCRedCorrectionFactor( MetaProcess* P ) : MetaFloat( P )
{
   TheACCRedCorrectionFactor = this;
}

IsoString ACCRedCorrectionFactor::Id() const
{
   return "redCorrectionFactor";
}

int ACCRedCorrectionFactor::Precision() const
{
   return 3;
}

double ACCRedCorrectionFactor::DefaultValue() const
{
   return 1;
}

double ACCRedCorrectionFactor::MinimumValue() const
{
   return 0;
}

double ACCRedCorrectionFactor::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

ACCGreenCorrectionFactor::ACCGreenCorrectionFactor( MetaProcess* P ) : MetaFloat( P )
{
   TheACCGreenCorrectionFactor = this;
}

IsoString ACCGreenCorrectionFactor::Id() const
{
   return "GreenCorrectionFactor";
}

int ACCGreenCorrectionFactor::Precision() const
{
   return 3;
}

double ACCGreenCorrectionFactor::DefaultValue() const
{
   return 1;
}

double ACCGreenCorrectionFactor::MinimumValue() const
{
   return 0;
}

double ACCGreenCorrectionFactor::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

ACCBlueCorrectionFactor::ACCBlueCorrectionFactor( MetaProcess* P ) : MetaFloat( P )
{
   TheACCBlueCorrectionFactor = this;
}

IsoString ACCBlueCorrectionFactor::Id() const
{
   return "BlueCorrectionFactor";
}

int ACCBlueCorrectionFactor::Precision() const
{
   return 3;
}

double ACCBlueCorrectionFactor::DefaultValue() const
{
   return 1;
}

double ACCBlueCorrectionFactor::MinimumValue() const
{
   return 0;
}

double ACCBlueCorrectionFactor::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

ACCBackgroundReference::ACCBackgroundReference( MetaProcess* P ) : MetaString( P )
{
   TheACCBackgroundReference = this;
}

IsoString ACCBackgroundReference::Id() const
{
   return "backgroundRef";
}

size_type ACCBackgroundReference::MinLength() const
{
   return 0;
}

String ACCBackgroundReference::DefaultValue() const
{
   return "";
}

// ----------------------------------------------------------------------------

ACCHistogramShadows::ACCHistogramShadows( MetaProcess* P ) : MetaFloat( P )
{
   TheACCHistogramShadows = this;
}

IsoString ACCHistogramShadows::Id() const
{
   return "HistogramShadows";
}

int ACCHistogramShadows::Precision() const
{
   return 5;
}

double ACCHistogramShadows::DefaultValue() const
{
   return 0;
}

double ACCHistogramShadows::MinimumValue() const
{
   return 0;
}

double ACCHistogramShadows::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

ACCHistogramHighlights::ACCHistogramHighlights( MetaProcess* P ) : MetaFloat( P )
{
   TheACCHistogramHighlights = this;
}

IsoString ACCHistogramHighlights::Id() const
{
   return "HistogramHighlights";
}

int ACCHistogramHighlights::Precision() const
{
   return 5;
}

double ACCHistogramHighlights::DefaultValue() const
{
   return 1;
}

double ACCHistogramHighlights::MinimumValue() const
{
   return 0;
}

double ACCHistogramHighlights::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

ACCHistogramMidtones::ACCHistogramMidtones( MetaProcess* P ) : MetaFloat( P )
{
   TheACCHistogramMidtones = this;
}

IsoString ACCHistogramMidtones::Id() const
{
   return "HistogramMidtones";
}

int ACCHistogramMidtones::Precision() const
{
   return 5;
}

double ACCHistogramMidtones::DefaultValue() const
{
   return 0.5;
}

double ACCHistogramMidtones::MinimumValue() const
{
   return 0;
}

double ACCHistogramMidtones::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

ACCSaturationBoost::ACCSaturationBoost( MetaProcess* P ) : MetaFloat( P )
{
   TheACCSaturationBoost = this;
}

IsoString ACCSaturationBoost::Id() const
{
   return "SaturationBoost";
}

int ACCSaturationBoost::Precision() const
{
   return 3;
}

double ACCSaturationBoost::DefaultValue() const
{
   return 1;
}

double ACCSaturationBoost::MinimumValue() const
{
   return 1;
}

double ACCSaturationBoost::MaximumValue() const
{
   return 5;
}

// ----------------------------------------------------------------------------


} // pcl

// ----------------------------------------------------------------------------
// EOF AssistedColorCalibrationParameters.cpp - Released 2015/11/26 16:00:13 UTC
