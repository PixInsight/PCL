// ****************************************************************************
// PixInsight Class Library - PCL 02.01.00.0779
// Standard StarMonitor Process Module Version 01.00.05.0050
// ****************************************************************************
// StarMonitorParameters.cpp - Released 2014/01/30 00:00:00 UTC
// ****************************************************************************
// This file is part of the standard StarMonitor PixInsight module.
//
// Copyright (c) 2003-2016, Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "StarMonitorParameters.h"

#include <pcl/BicubicInterpolation.h>  // for __PCL_BICUBIC_SPLINE_CLAMPING_THRESHOLD

namespace pcl{

// ----------------------------------------------------------------------------

SMStructureLayers* TheSMStructureLayersParameter = 0;
SMNoiseLayers* TheSMNoiseLayersParameter = 0;
SMHotPixelFilterRadius* TheSMHotPixelFilterRadiusParameter = 0;
SMSensitivity* TheSMSensitivityParameter = 0;
SMPeakResponse* TheSMPeakResponseParameter = 0;
SMMaxStarDistortion* TheSMMaxStarDistortionParameter = 0;
SMInvert* TheSMInvertParameter = 0;

SMMonitoredFolder* TheSMMonitoredFolderParameter = 0;
SMTargetItems* TheSMTargetItemsParameter = 0;
SMTargetImage* TheSMTargetImageParameter = 0;
SMStarQuantity* TheSMStarQuantityParameter = 0;
SMBackground* TheSMBackgroundParameter = 0;
SMAmplitude* TheSMAmplitudeParameter = 0;
SMRadiusX* TheSMRadiusXParameter = 0;
SMRadiusY* TheSMRadiusYParameter = 0;
SMFWHMx* TheSMFWHMxParameter = 0;
SMFWHMy* TheSMFWHMyParameter = 0;
SMRotationAngle* TheSMRotationAngleParameter = 0;
SMMAD* TheSMMADParameter = 0;
SMxScale* TheSMxScaleParameter = 0;
SMyScale* TheSMyScaleParameter = 0;

SMInputHints* TheSMInputHintsParameter = 0;
SMScaleMode* TheSMScaleModeParameter = 0;
SMScaleValue* TheSMScaleValueParameter = 0;

SMAlertCheck* TheSMAlertCheckParameter = 0;
SMAlertFWHMxCheck* TheSMAlertFWHMxCheckParameter = 0;
SMAlertRoundnessCheck* TheSMAlertRoundnessCheckParameter = 0;
SMAlertBackgroundCheck* TheSMAlertBackgroundCheckParameter = 0;
SMAlertStarQuantityCheck* TheSMAlertStarQuantityCheckParameter = 0;

SMAlertExecute* TheSMAlertExecuteParameter = 0;
SMAlertArguments* TheSMAlertArgumentsParameter = 0;
SMAlertFWHMx* TheSMAlertFWHMxParameter = 0;
SMAlertRoundness* TheSMAlertRoundnessParameter = 0;
SMAlertBackground* TheSMAlertBackgroundParameter = 0;
SMAlertStarQuantity* TheSMAlertStarQuantityParameter = 0;

SMStructureLayers::SMStructureLayers(MetaProcess* p) : MetaInt32(p)
{
   TheSMStructureLayersParameter = this;
}

IsoString SMStructureLayers::Id() const
{
   return "structureLayers";
}

double SMStructureLayers::DefaultValue() const
{
   return 5;
}

double SMStructureLayers::MinimumValue() const
{
   return 1;
}

double SMStructureLayers::MaximumValue() const
{
   return 8;
}

// ----------------------------------------------------------------------------

SMNoiseLayers::SMNoiseLayers(MetaProcess* p) : MetaInt32(p)
{
   TheSMNoiseLayersParameter = this;
}

IsoString SMNoiseLayers::Id() const
{
   return "noiseLayers";
}

double SMNoiseLayers::DefaultValue() const
{
   return 1;
}

double SMNoiseLayers::MinimumValue() const
{
   return 0;
}

double SMNoiseLayers::MaximumValue() const
{
   return 4;
}

// ----------------------------------------------------------------------------

SMHotPixelFilterRadius::SMHotPixelFilterRadius(MetaProcess* p) : MetaInt32(p)
{
   TheSMHotPixelFilterRadiusParameter = this;
}

IsoString SMHotPixelFilterRadius::Id() const
{
   return "hotPixelFilterRadius";
}

double SMHotPixelFilterRadius::DefaultValue() const
{
   return 1;
}

double SMHotPixelFilterRadius::MinimumValue() const
{
   return 0;
}

double SMHotPixelFilterRadius::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

SMSensitivity::SMSensitivity(MetaProcess* p) : MetaFloat(p)
{
   TheSMSensitivityParameter = this;
}

IsoString SMSensitivity::Id() const
{
   return "sensitivity";
}

int SMSensitivity::Precision() const
{
   return 3;
}

double SMSensitivity::DefaultValue() const
{
   return 10.0;
}

double SMSensitivity::MinimumValue() const
{
   return 0.001;
}

double SMSensitivity::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

SMPeakResponse::SMPeakResponse(MetaProcess* p) : MetaFloat(p)
{
   TheSMPeakResponseParameter = this;
}

IsoString SMPeakResponse::Id() const
{
   return "peakResponse";
}

int SMPeakResponse::Precision() const
{
   return 2;
}

double SMPeakResponse::DefaultValue() const
{
   return 0.80;
}

double SMPeakResponse::MinimumValue() const
{
   return 0;
}

double SMPeakResponse::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

SMMaxStarDistortion::SMMaxStarDistortion(MetaProcess* p) : MetaFloat(p)
{
   TheSMMaxStarDistortionParameter = this;
}

IsoString SMMaxStarDistortion::Id() const
{
   return "maxStarDistortion";
}

int SMMaxStarDistortion::Precision() const
{
   return 3;
}

double SMMaxStarDistortion::DefaultValue() const
{
   return 0.5;
}

double SMMaxStarDistortion::MinimumValue() const
{
   return 0;
}

double SMMaxStarDistortion::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

SMInvert::SMInvert(MetaProcess* p) : MetaBoolean(p)
{
   TheSMInvertParameter = this;
}

IsoString SMInvert::Id() const
{
   return "invert";
}

bool SMInvert::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SMMonitoredFolder::SMMonitoredFolder(MetaProcess* p) : MetaString(p)
{
   TheSMMonitoredFolderParameter = this;
}

IsoString SMMonitoredFolder::Id() const
{
   return "monitoredFolder";
}

// ----------------------------------------------------------------------------

SMTargetItems::SMTargetItems(MetaProcess* p) : MetaTable(p)
{
   TheSMTargetItemsParameter = this;
}

IsoString SMTargetItems::Id() const
{
   return "targets";
}

// ----------------------------------------------------------------------------

SMTargetImage::SMTargetImage(MetaTable* t) : MetaString(t)
{
   TheSMTargetImageParameter = this;
}

IsoString SMTargetImage::Id() const
{
   return "image";
}

// ----------------------------------------------------------------------------

SMStarQuantity::SMStarQuantity(MetaTable* T) : MetaUInt32(T)
{
   TheSMStarQuantityParameter = this;
}

IsoString SMStarQuantity::Id() const
{
   return "starQTY";
}

// ----------------------------------------------------------------------------

SMBackground::SMBackground(MetaTable* T) : MetaDouble(T)
{
   TheSMBackgroundParameter = this;
}

IsoString SMBackground::Id() const
{
   return "B";
}

int SMBackground::Precision() const
{
   return 6;
}

// ----------------------------------------------------------------------------

SMAmplitude::SMAmplitude(MetaTable* T) : MetaDouble(T)
{
   TheSMAmplitudeParameter = this;
}

IsoString SMAmplitude::Id() const
{
   return "A";
}

int SMAmplitude::Precision() const
{
   return 6;
}

// ----------------------------------------------------------------------------

SMRadiusX::SMRadiusX(MetaTable* T) : MetaDouble(T)
{
   TheSMRadiusXParameter = this;
}

IsoString SMRadiusX::Id() const
{
   return "sx";
}

int SMRadiusX::Precision() const
{
   return 6;
}

// ----------------------------------------------------------------------------

SMRadiusY::SMRadiusY(MetaTable* T) : MetaDouble(T)
{
   TheSMRadiusYParameter = this;
}

IsoString SMRadiusY::Id() const
{
   return "sy";
}

int SMRadiusY::Precision() const
{
   return 6;
}

// ----------------------------------------------------------------------------

SMFWHMx::SMFWHMx(MetaTable* T) : MetaDouble(T)
{
   TheSMFWHMxParameter = this;
}

IsoString SMFWHMx::Id() const
{
   return "FWHMx";
}

int SMFWHMx::Precision() const
{
   return 6;
}

// ----------------------------------------------------------------------------

SMFWHMy::SMFWHMy(MetaTable* T) : MetaDouble(T)
{
   TheSMFWHMyParameter = this;
}

IsoString SMFWHMy::Id() const
{
   return "FWHMy";
}

int SMFWHMy::Precision() const
{
   return 6;
}

// ----------------------------------------------------------------------------

SMRotationAngle::SMRotationAngle(MetaTable* T) : MetaDouble(T)
{
   TheSMRotationAngleParameter = this;
}

IsoString SMRotationAngle::Id() const
{
   return "theta";
}

int SMRotationAngle::Precision() const
{
   return 6;
}

// ----------------------------------------------------------------------------

SMMAD::SMMAD(MetaTable* T) : MetaDouble(T)
{
   TheSMMADParameter = this;
}

IsoString SMMAD::Id() const
{
   return "mad";
}

int SMMAD::Precision() const
{
   return 6;
}

// ----------------------------------------------------------------------------

SMxScale::SMxScale(MetaTable* T) : MetaFloat(T)
{
   TheSMxScaleParameter = this;
}

IsoString SMxScale::Id() const
{
   return "xScale";
}

int SMxScale::Precision() const
{
   return 6;
}

// ----------------------------------------------------------------------------

SMyScale::SMyScale(MetaTable* T) : MetaFloat(T)
{
   TheSMyScaleParameter = this;
}

IsoString SMyScale::Id() const
{
   return "yScale";
}

int SMyScale::Precision() const
{
   return 6;
}

// ----------------------------------------------------------------------------

SMInputHints::SMInputHints(MetaProcess* P) : MetaString(P)
{
   TheSMInputHintsParameter = this;
}

IsoString SMInputHints::Id() const
{
   return "inputHints";
}

// ----------------------------------------------------------------------------

SMScaleMode::SMScaleMode(MetaProcess* P) : MetaEnumeration(P)
{
   TheSMScaleModeParameter = this;
}

IsoString SMScaleMode::Id() const
{
   return "scaleMode";
}

size_type SMScaleMode::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SMScaleMode::ElementId(size_type i) const
{
   switch (i)
   {
      default:
      case Pixels: return "Scale_Pixels";
      case StandardKeywords: return "Scale_StandardKeywords";
      case LiteralValue: return "Scale_LiteralValue";
   }
}

int SMScaleMode::ElementValue(size_type i) const
{
   return int( i);
}

size_type SMScaleMode::DefaultValueIndex() const
{
   return size_type(Default);
}

// ----------------------------------------------------------------------------

SMScaleValue::SMScaleValue(MetaProcess* P) : MetaFloat(P)
{
   TheSMScaleValueParameter = this;
}

IsoString SMScaleValue::Id() const
{
   return "scaleValue";
}

int SMScaleValue::Precision() const
{
   return 2;
}

double SMScaleValue::DefaultValue() const
{
   return 1.0;
}

double SMScaleValue::MinimumValue() const
{
   return 0.01;
}

double SMScaleValue::MaximumValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

SMAlertCheck::SMAlertCheck(MetaProcess* p) : MetaBoolean(p)
{
   TheSMAlertCheckParameter = this;
}

IsoString SMAlertCheck::Id() const
{
   return "alertCheck";
}

bool SMAlertCheck::DefaultValue() const
{
   return true;
}

//

SMAlertExecute::SMAlertExecute(MetaProcess* P) : MetaString(P)
{
   TheSMAlertExecuteParameter = this;
}

IsoString SMAlertExecute::Id() const
{
   return "alertExecute";
}

String SMAlertExecute::DefaultValue() const
{
#ifdef __PCL_LINUX
   return "aplay";
#endif
#ifdef __PCL_FREEBSD
   return "";
#endif
#ifdef __PCL_MACOSX
   return "";
#endif
#ifdef __PCL_WINDOWS
   return "calc.exe";
#endif
}

// ----------------------------------------------------------------------------

SMAlertArguments::SMAlertArguments(MetaProcess* P) : MetaString(P)
{
   TheSMAlertArgumentsParameter = this;
}

IsoString SMAlertArguments::Id() const
{
   return "alertArguments";
}

String SMAlertArguments::DefaultValue() const
{
#ifdef __PCL_LINUX
   return "/home/user/curve.wav";
#endif
#ifdef __PCL_FREEBSD
   return "";
#endif
#ifdef __PCL_MACOSX
   return "";
#endif
#ifdef __PCL_WINDOWS
   return "";
#endif
}

// ----------------------------------------------------------------------------

SMAlertFWHMxCheck::SMAlertFWHMxCheck(MetaProcess* p) : MetaBoolean(p)
{
   TheSMAlertFWHMxCheckParameter = this;
}

IsoString SMAlertFWHMxCheck::Id() const
{
   return "alertFWHMxCheck";
}

bool SMAlertFWHMxCheck::DefaultValue() const
{
   return true;
}

SMAlertFWHMx::SMAlertFWHMx(MetaProcess* P) : MetaFloat(P)
{
   TheSMAlertFWHMxParameter = this;
}

IsoString SMAlertFWHMx::Id() const
{
   return "alertFWHMx";
}

int SMAlertFWHMx::Precision() const
{
   return 2;
}

double SMAlertFWHMx::DefaultValue() const
{
   return 5.0;
}

double SMAlertFWHMx::MinimumValue() const
{
   return 0.0;
}

double SMAlertFWHMx::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

SMAlertRoundnessCheck::SMAlertRoundnessCheck(MetaProcess* p) : MetaBoolean(p)
{
   TheSMAlertRoundnessCheckParameter = this;
}

IsoString SMAlertRoundnessCheck::Id() const
{
   return "alertRoundnessCheck";
}

bool SMAlertRoundnessCheck::DefaultValue() const
{
   return false;
}

SMAlertRoundness::SMAlertRoundness(MetaProcess* P) : MetaFloat(P)
{
   TheSMAlertRoundnessParameter = this;
}

IsoString SMAlertRoundness::Id() const
{
   return "alertRoundness";
}

int SMAlertRoundness::Precision() const
{
   return 2;
}

double SMAlertRoundness::DefaultValue() const
{
   return 0.9;
}

double SMAlertRoundness::MinimumValue() const
{
   return 0.0;
}

double SMAlertRoundness::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

SMAlertBackgroundCheck::SMAlertBackgroundCheck(MetaProcess* p) : MetaBoolean(p)
{
   TheSMAlertBackgroundCheckParameter = this;
}

IsoString SMAlertBackgroundCheck::Id() const
{
   return "alertBackgroundCheck";
}

bool SMAlertBackgroundCheck::DefaultValue() const
{
   return false;
}

SMAlertBackground::SMAlertBackground(MetaProcess* P) : MetaDouble(P)
{
   TheSMAlertBackgroundParameter = this;
}

IsoString SMAlertBackground::Id() const
{
   return "alertBackground";
}

int SMAlertBackground::Precision() const
{
   return 6;
}

double SMAlertBackground::DefaultValue() const
{
   return 0.1;
}

double SMAlertBackground::MinimumValue() const
{
   return 0.0;
}

double SMAlertBackground::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

SMAlertStarQuantityCheck::SMAlertStarQuantityCheck(MetaProcess* p) : MetaBoolean(p)
{
   TheSMAlertStarQuantityCheckParameter = this;
}

IsoString SMAlertStarQuantityCheck::Id() const
{
   return "alertStarQuantityCheck";
}

bool SMAlertStarQuantityCheck::DefaultValue() const
{
   return false;
}

SMAlertStarQuantity::SMAlertStarQuantity(MetaProcess* P) : MetaUInt32(P)
{
   TheSMAlertStarQuantityParameter = this;
}

IsoString SMAlertStarQuantity::Id() const
{
   return "alertStarQuantity";
}
//int SMAlertFWHMx::Precision() const { return 2; }

double SMAlertStarQuantity::DefaultValue() const
{
   return 100.0;
}

double SMAlertStarQuantity::MinimumValue() const
{
   return 0.0;
}

double SMAlertStarQuantity::MaximumValue() const
{
   return 10000;
}

// ----------------------------------------------------------------------------


} // pcl

// ****************************************************************************
// EOF StarMonitorParameters.cpp - Released 2014/01/30 00:00:00 UTC
