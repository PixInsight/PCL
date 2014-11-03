// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Flux Process Module Version 01.00.00.0064
// ****************************************************************************
// FluxCalibrationParameters.cpp - Released 2014/10/29 07:34:55 UTC
// ****************************************************************************
// This file is part of the standard Flux PixInsight module.
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

#include "FluxCalibrationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FCWavelengthValue*               TheFCWavelengthValueParameter = 0;
FCWavelengthMode*                TheFCWavelengthModeParameter = 0;
FCWavelengthKeyword*             TheFCWavelengthKeywordParameter = 0;

FCTransmissivityValue*           TheFCTransmissivityValueParameter = 0;
FCTransmissivityMode*            TheFCTransmissivityModeParameter = 0;
FCTransmissivityKeyword*         TheFCTransmissivityKeywordParameter = 0;

FCFilterWidthValue*              TheFCFilterWidthValueParameter = 0;
FCFilterWidthMode*               TheFCFilterWidthModeParameter = 0;
FCFilterWidthKeyword*            TheFCFilterWidthKeywordParameter = 0;

FCApertureValue*                 TheFCApertureValueParameter = 0;
FCApertureMode*                  TheFCApertureModeParameter = 0;
FCApertureKeyword*               TheFCApertureKeywordParameter = 0;

FCCentralObstructionValue*       TheFCCentralObstructionValueParameter = 0;
FCCentralObstructionMode*        TheFCCentralObstructionModeParameter = 0;
FCCentralObstructionKeyword*     TheFCCentralObstructionKeywordParameter = 0;

FCExposureTimeValue*             TheFCExposureTimeValueParameter = 0;
FCExposureTimeMode*              TheFCExposureTimeModeParameter = 0;
FCExposureTimeKeyword*           TheFCExposureTimeKeywordParameter = 0;

FCAtmosphericExtinctionValue*    TheFCAtmosphericExtinctionValueParameter = 0;
FCAtmosphericExtinctionMode*     TheFCAtmosphericExtinctionModeParameter = 0;
FCAtmosphericExtinctionKeyword*  TheFCAtmosphericExtinctionKeywordParameter = 0;

FCSensorGainValue*               TheFCSensorGainValueParameter = 0;
FCSensorGainMode*                TheFCSensorGainModeParameter = 0;
FCSensorGainKeyword*             TheFCSensorGainKeywordParameter = 0;

FCQuantumEfficiencyValue*        TheFCQuantumEfficiencyValueParameter = 0;
FCQuantumEfficiencyMode*         TheFCQuantumEfficiencyModeParameter = 0;
FCQuantumEfficiencyKeyword*      TheFCQuantumEfficiencyKeywordParameter = 0;

// ----------------------------------------------------------------------------

FCWavelengthValue::FCWavelengthValue( MetaProcess* P ) : MetaFloat( P )
{
   TheFCWavelengthValueParameter = this;
}

IsoString FCWavelengthValue::Id() const
{
   return "wavelength";
}

int FCWavelengthValue::Precision() const
{
   return 4;
}

double FCWavelengthValue::MinimumValue() const
{
   return 0;
}

double FCWavelengthValue::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

FCWavelengthMode::FCWavelengthMode( MetaProcess* P ) : FCParameterMode( P )
{
   TheFCWavelengthModeParameter = this;
}

IsoString FCWavelengthMode::Id() const
{
   return "wavelengthMode";
}

// ----------------------------------------------------------------------------

FCWavelengthKeyword::FCWavelengthKeyword( MetaProcess* P ) : MetaString( P )
{
   TheFCWavelengthKeywordParameter = this;
}

IsoString FCWavelengthKeyword::Id() const
{
   return "wavelengthKeyword";
}

size_type FCWavelengthKeyword::MinLength() const
{
   return 2;
}

String FCWavelengthKeyword::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_--";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FCTransmissivityValue::FCTransmissivityValue( MetaProcess* P ) : MetaFloat( P )
{
   TheFCTransmissivityValueParameter = this;
}

IsoString FCTransmissivityValue::Id() const
{
   return "transmissivity";
}

int FCTransmissivityValue::Precision() const
{
   return 4;
}

double FCTransmissivityValue::MinimumValue() const
{
   return 0;
}

double FCTransmissivityValue::MaximumValue() const
{
   return 1;
}

double FCTransmissivityValue::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

FCTransmissivityMode::FCTransmissivityMode( MetaProcess* P ) : FCParameterMode( P )
{
   TheFCTransmissivityModeParameter = this;
}

IsoString FCTransmissivityMode::Id() const
{
   return "transmissivityMode";
}

// ----------------------------------------------------------------------------

FCTransmissivityKeyword::FCTransmissivityKeyword( MetaProcess* P ) : MetaString( P )
{
   TheFCTransmissivityKeywordParameter = this;
}

IsoString FCTransmissivityKeyword::Id() const
{
   return "transmissivityKeyword";
}

size_type FCTransmissivityKeyword::MinLength() const
{
   return 2;
}

String FCTransmissivityKeyword::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FCFilterWidthValue::FCFilterWidthValue( MetaProcess* P ) : MetaFloat( P )
{
   TheFCFilterWidthValueParameter = this;
}

IsoString FCFilterWidthValue::Id() const
{
   return "filterWidth";
}

int FCFilterWidthValue::Precision() const
{
   return 4;
}

double FCFilterWidthValue::MinimumValue() const
{
   return 0;
}

double FCFilterWidthValue::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

FCFilterWidthMode::FCFilterWidthMode( MetaProcess* P ) : FCParameterMode( P )
{
   TheFCFilterWidthModeParameter = this;
}

IsoString FCFilterWidthMode::Id() const
{
   return "filterWidthMode";
}

// ----------------------------------------------------------------------------

FCFilterWidthKeyword::FCFilterWidthKeyword( MetaProcess* P ) : MetaString( P )
{
   TheFCFilterWidthKeywordParameter = this;
}

IsoString FCFilterWidthKeyword::Id() const
{
   return "filterWidthKeyword";
}

size_type FCFilterWidthKeyword::MinLength() const
{
   return 2;
}

String FCFilterWidthKeyword::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FCApertureValue::FCApertureValue( MetaProcess* P ) : MetaFloat( P )
{
   TheFCApertureValueParameter = this;
}

IsoString FCApertureValue::Id() const
{
   return "aperture";
}

int FCApertureValue::Precision() const
{
   return 2;
}

double FCApertureValue::MinimumValue() const
{
   return 0;
}

double FCApertureValue::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

FCApertureMode::FCApertureMode( MetaProcess* P ) : FCParameterMode( P )
{
   TheFCApertureModeParameter = this;
}

IsoString FCApertureMode::Id() const
{
   return "apertureMode";
}

// ----------------------------------------------------------------------------

FCApertureKeyword::FCApertureKeyword( MetaProcess* P ) : MetaString( P )
{
   TheFCApertureKeywordParameter = this;
}

IsoString FCApertureKeyword::Id() const
{
   return "apertureKeyword";
}

size_type FCApertureKeyword::MinLength() const
{
   return 2;
}

String FCApertureKeyword::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FCCentralObstructionValue::FCCentralObstructionValue( MetaProcess* P ) : MetaFloat( P )
{
   TheFCCentralObstructionValueParameter = this;
}

IsoString FCCentralObstructionValue::Id() const
{
   return "centralObstruction";
}

int FCCentralObstructionValue::Precision() const
{
   return 2;
}

double FCCentralObstructionValue::MinimumValue() const
{
   return 0;
}

double FCCentralObstructionValue::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

FCCentralObstructionMode::FCCentralObstructionMode( MetaProcess* P ) : FCParameterMode( P )
{
   TheFCCentralObstructionModeParameter = this;
}

IsoString FCCentralObstructionMode::Id() const
{
   return "centralObstructionMode";
}

// ----------------------------------------------------------------------------

FCCentralObstructionKeyword::FCCentralObstructionKeyword( MetaProcess* P ) : MetaString( P )
{
   TheFCCentralObstructionKeywordParameter = this;
}

IsoString FCCentralObstructionKeyword::Id() const
{
   return "centralObstructionKeyword";
}

size_type FCCentralObstructionKeyword::MinLength() const
{
   return 1;
}

String FCCentralObstructionKeyword::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FCExposureTimeValue::FCExposureTimeValue( MetaProcess* P ) : MetaFloat( P )
{
   TheFCExposureTimeValueParameter = this;
}

IsoString FCExposureTimeValue::Id() const
{
   return "exposureTime";
}

int FCExposureTimeValue::Precision() const
{
   return 3;
}

double FCExposureTimeValue::MinimumValue() const
{
   return 0;
}

double FCExposureTimeValue::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

FCExposureTimeMode::FCExposureTimeMode( MetaProcess* P ) : FCParameterMode( P )
{
   TheFCExposureTimeModeParameter = this;
}

IsoString FCExposureTimeMode::Id() const
{
   return "exposureTimeMode";
}

// ----------------------------------------------------------------------------

FCExposureTimeKeyword::FCExposureTimeKeyword( MetaProcess* P ) : MetaString( P )
{
   TheFCExposureTimeKeywordParameter = this;
}

IsoString FCExposureTimeKeyword::Id() const
{
   return "exposureTimeKeyword";
}

size_type FCExposureTimeKeyword::MinLength() const
{
   return 1;
}

String FCExposureTimeKeyword::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FCAtmosphericExtinctionValue::FCAtmosphericExtinctionValue( MetaProcess* P ) : MetaFloat( P )
{
   TheFCAtmosphericExtinctionValueParameter = this;
}

IsoString FCAtmosphericExtinctionValue::Id() const
{
   return "atmosphericExtinction";
}

int FCAtmosphericExtinctionValue::Precision() const
{
   return 4;
}

double FCAtmosphericExtinctionValue::MinimumValue() const
{
   return 0;
}

double FCAtmosphericExtinctionValue::MaximumValue() const
{
   return 1;
}

double FCAtmosphericExtinctionValue::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

FCAtmosphericExtinctionMode::FCAtmosphericExtinctionMode( MetaProcess* P ) : FCParameterMode( P )
{
   TheFCAtmosphericExtinctionModeParameter = this;
}

IsoString FCAtmosphericExtinctionMode::Id() const
{
   return "atmosphericExtinctionMode";
}

// ----------------------------------------------------------------------------

FCAtmosphericExtinctionKeyword::FCAtmosphericExtinctionKeyword( MetaProcess* P ) : MetaString( P )
{
   TheFCAtmosphericExtinctionKeywordParameter = this;
}

IsoString FCAtmosphericExtinctionKeyword::Id() const
{
   return "atmosphericExtinctionKeyword";
}

size_type FCAtmosphericExtinctionKeyword::MinLength() const
{
   return 0;
}

String FCAtmosphericExtinctionKeyword::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FCSensorGainValue::FCSensorGainValue( MetaProcess* P ) : MetaFloat( P )
{
   TheFCSensorGainValueParameter = this;
}

IsoString FCSensorGainValue::Id() const
{
   return "sensorGain";
}

int FCSensorGainValue::Precision() const
{
   return 2;
}

double FCSensorGainValue::MinimumValue() const
{
   return 0;
}

double FCSensorGainValue::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

FCSensorGainMode::FCSensorGainMode( MetaProcess* P ) : FCParameterMode( P )
{
   TheFCSensorGainModeParameter = this;
}

IsoString FCSensorGainMode::Id() const
{
   return "sensorGainMode";
}

// ----------------------------------------------------------------------------

FCSensorGainKeyword::FCSensorGainKeyword( MetaProcess* P ) : MetaString( P )
{
   TheFCSensorGainKeywordParameter = this;
}

IsoString FCSensorGainKeyword::Id() const
{
   return "sensorGainKeyword";
}

size_type FCSensorGainKeyword::MinLength() const
{
   return 1;
}

String FCSensorGainKeyword::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";
}

// ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------

FCQuantumEfficiencyValue::FCQuantumEfficiencyValue( MetaProcess* P ) : MetaFloat( P )
{
   TheFCQuantumEfficiencyValueParameter = this;
}

IsoString FCQuantumEfficiencyValue::Id() const
{
   return "quantumEfficiency";
}

int FCQuantumEfficiencyValue::Precision() const
{
   return 4;
}

double FCQuantumEfficiencyValue::MinimumValue() const
{
   return 0;
}

double FCQuantumEfficiencyValue::MaximumValue() const
{
   return 1;
}

double FCQuantumEfficiencyValue::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

FCQuantumEfficiencyMode::FCQuantumEfficiencyMode( MetaProcess* P ) : FCParameterMode( P )
{
   TheFCQuantumEfficiencyModeParameter = this;
}

IsoString FCQuantumEfficiencyMode::Id() const
{
   return "quantumEfficiencyMode";
}

// ----------------------------------------------------------------------------

FCQuantumEfficiencyKeyword::FCQuantumEfficiencyKeyword( MetaProcess* P ) : MetaString( P )
{
   TheFCQuantumEfficiencyKeywordParameter = this;
}

IsoString FCQuantumEfficiencyKeyword::Id() const
{
   return "quantumEfficiencyKeyword";
}

size_type FCQuantumEfficiencyKeyword::MinLength() const
{
   return 1;
}

String FCQuantumEfficiencyKeyword::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF FluxCalibrationParameters.cpp - Released 2014/10/29 07:34:55 UTC
