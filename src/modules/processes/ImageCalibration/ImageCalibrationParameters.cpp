//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0345
// ----------------------------------------------------------------------------
// ImageCalibrationParameters.cpp - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "ImageCalibrationParameters.h"

#include <pcl/BicubicInterpolation.h>  // for __PCL_BICUBIC_SPLINE_CLAMPING_THRESHOLD

namespace pcl
{

// ----------------------------------------------------------------------------

ICTargetFrames*              TheICTargetFramesParameter = 0;
ICTargetFrameEnabled*        TheICTargetFrameEnabledParameter = 0;
ICTargetFramePath*           TheICTargetFramePathParameter = 0;

ICInputHints*                TheICInputHintsParameter = 0;
ICOutputHints*               TheICOutputHintsParameter = 0;

ICPedestal*                  TheICPedestalParameter = 0;
ICPedestalMode*              TheICPedestalModeParameter = 0;
ICPedestalKeyword*           TheICPedestalKeywordParameter = 0;

ICOverscanEnabled*           TheICOverscanEnabledParameter = 0;

ICOverscanImageX0*           TheICOverscanImageX0Parameter = 0;
ICOverscanImageY0*           TheICOverscanImageY0Parameter = 0;
ICOverscanImageX1*           TheICOverscanImageX1Parameter = 0;
ICOverscanImageY1*           TheICOverscanImageY1Parameter = 0;

ICOverscanRegions*           TheICOverscanRegionsParameter = 0;

ICOverscanRegionEnabled*     TheICOverscanRegionEnabledParameter = 0;

ICOverscanSourceX0*          TheICOverscanSourceX0Parameter = 0;
ICOverscanSourceY0*          TheICOverscanSourceY0Parameter = 0;
ICOverscanSourceX1*          TheICOverscanSourceX1Parameter = 0;
ICOverscanSourceY1*          TheICOverscanSourceY1Parameter = 0;

ICOverscanTargetX0*          TheICOverscanTargetX0Parameter = 0;
ICOverscanTargetY0*          TheICOverscanTargetY0Parameter = 0;
ICOverscanTargetX1*          TheICOverscanTargetX1Parameter = 0;
ICOverscanTargetY1*          TheICOverscanTargetY1Parameter = 0;

ICMasterBiasEnabled*         TheICMasterBiasEnabledParameter = 0;
ICMasterBiasPath*            TheICMasterBiasPathParameter = 0;

ICMasterDarkEnabled*         TheICMasterDarkEnabledParameter = 0;
ICMasterDarkPath*            TheICMasterDarkPathParameter = 0;

ICMasterFlatEnabled*         TheICMasterFlatEnabledParameter = 0;
ICMasterFlatPath*            TheICMasterFlatPathParameter = 0;

ICCalibrateBias*             TheICCalibrateBiasParameter = 0;
ICCalibrateDark*             TheICCalibrateDarkParameter = 0;
ICCalibrateFlat*             TheICCalibrateFlatParameter = 0;

ICOptimizeDarks*             TheICOptimizeDarksParameter = 0;
ICDarkOptimizationThreshold* TheICDarkOptimizationThresholdParameter = 0;
ICDarkOptimizationLow*       TheICDarkOptimizationLowParameter = 0;
ICDarkOptimizationWindow*    TheICDarkOptimizationWindowParameter = 0;
ICDarkCFADetectionMode*      TheICDarkCFADetectionModeParameter = 0;

ICEvaluateNoise*             TheICEvaluateNoiseParameter = 0;
ICNoiseEvaluationAlgorithm*  TheICNoiseEvaluationAlgorithmParameter = 0;

ICOutputDirectory*           TheICOutputDirectoryParameter = 0;
ICOutputExtension*           TheICOutputExtensionParameter = 0;
ICOutputPrefix*              TheICOutputPrefixParameter = 0;
ICOutputPostfix*             TheICOutputPostfixParameter = 0;
ICOutputSampleFormat*        TheICOutputSampleFormatParameter = 0;
ICOutputPedestal*            TheICOutputPedestalParameter = 0;
ICOverwriteExistingFiles*    TheICOverwriteExistingFilesParameter = 0;
ICOnError*                   TheICOnErrorParameter = 0;
ICNoGUIMessages*             TheICNoGUIMessagesParameter = 0;

ICOutputData*                TheICOutputDataParameter = 0;
ICOutputFilePath*            TheICOutputFilePathParameter = 0;
ICDarkScalingFactorRK*       TheICDarkScalingFactorRKParameter = 0;
ICDarkScalingFactorG*        TheICDarkScalingFactorGParameter = 0;
ICDarkScalingFactorB*        TheICDarkScalingFactorBParameter = 0;
ICNoiseEstimateRK*           TheICNoiseEstimateRKParameter = 0;
ICNoiseEstimateG*            TheICNoiseEstimateGParameter = 0;
ICNoiseEstimateB*            TheICNoiseEstimateBParameter = 0;
ICNoiseFractionRK*           TheICNoiseFractionRKParameter = 0;
ICNoiseFractionG*            TheICNoiseFractionGParameter = 0;
ICNoiseFractionB*            TheICNoiseFractionBParameter = 0;
ICNoiseAlgorithmRK*          TheICNoiseAlgorithmRKParameter = 0;
ICNoiseAlgorithmG*           TheICNoiseAlgorithmGParameter = 0;
ICNoiseAlgorithmB*           TheICNoiseAlgorithmBParameter = 0;

// ----------------------------------------------------------------------------

ICTargetFrames::ICTargetFrames( MetaProcess* P ) : MetaTable( P )
{
   TheICTargetFramesParameter = this;
}

IsoString ICTargetFrames::Id() const
{
   return "targetFrames";
}

// ----------------------------------------------------------------------------

ICTargetFrameEnabled::ICTargetFrameEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheICTargetFrameEnabledParameter = this;
}

IsoString ICTargetFrameEnabled::Id() const
{
   return "enabled";
}

bool ICTargetFrameEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICTargetFramePath::ICTargetFramePath( MetaTable* T ) : MetaString( T )
{
   TheICTargetFramePathParameter = this;
}

IsoString ICTargetFramePath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------

ICInputHints::ICInputHints( MetaProcess* P ) : MetaString( P )
{
   TheICInputHintsParameter = this;
}

IsoString ICInputHints::Id() const
{
   return "inputHints";
}

// ----------------------------------------------------------------------------

ICOutputHints::ICOutputHints( MetaProcess* P ) : MetaString( P )
{
   TheICOutputHintsParameter = this;
}

IsoString ICOutputHints::Id() const
{
   return "outputHints";
}

// ----------------------------------------------------------------------------

ICPedestal::ICPedestal( MetaProcess* P ) : MetaInt32( P )
{
   TheICPedestalParameter = this;
}

IsoString ICPedestal::Id() const
{
   return "pedestal";
}

double ICPedestal::DefaultValue() const
{
   return 0;
}

double ICPedestal::MinimumValue() const
{
   return 0;
}

double ICPedestal::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

ICPedestalMode::ICPedestalMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICPedestalModeParameter = this;
}

IsoString ICPedestalMode::Id() const
{
   return "pedestalMode";
}

size_type ICPedestalMode::NumberOfElements() const
{
   return NumberOfPedestalModes;
}

IsoString ICPedestalMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Literal:       return "Literal";
   default:
   case Keyword:       return "Keyword";
   case CustomKeyword: return "CustomKeyword";
   }
}

int ICPedestalMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICPedestalMode::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICPedestalKeyword::ICPedestalKeyword( MetaProcess* P ) : MetaString( P )
{
   TheICPedestalKeywordParameter = this;
}

IsoString ICPedestalKeyword::Id() const
{
   return "pedestalKeyword";
}

// ----------------------------------------------------------------------------

ICOverscanEnabled::ICOverscanEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheICOverscanEnabledParameter = this;
}

IsoString ICOverscanEnabled::Id() const
{
   return "overscanEnabled";
}

bool ICOverscanEnabled::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICOverscanImageX0::ICOverscanImageX0( MetaProcess* P ) : MetaInt32( P )
{
   TheICOverscanImageX0Parameter = this;
}

IsoString ICOverscanImageX0::Id() const
{
   return "overscanImageX0";
}

double ICOverscanImageX0::DefaultValue() const
{
   return 0;
}

double ICOverscanImageX0::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanImageY0::ICOverscanImageY0( MetaProcess* P ) : MetaInt32( P )
{
   TheICOverscanImageY0Parameter = this;
}

IsoString ICOverscanImageY0::Id() const
{
   return "overscanImageY0";
}

double ICOverscanImageY0::DefaultValue() const
{
   return 0;
}

double ICOverscanImageY0::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanImageX1::ICOverscanImageX1( MetaProcess* P ) : MetaInt32( P )
{
   TheICOverscanImageX1Parameter = this;
}

IsoString ICOverscanImageX1::Id() const
{
   return "overscanImageX1";
}

double ICOverscanImageX1::DefaultValue() const
{
   return 0;
}

double ICOverscanImageX1::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanImageY1::ICOverscanImageY1( MetaProcess* P ) : MetaInt32( P )
{
   TheICOverscanImageY1Parameter = this;
}

IsoString ICOverscanImageY1::Id() const
{
   return "overscanImageY1";
}

double ICOverscanImageY1::DefaultValue() const
{
   return 0;
}

double ICOverscanImageY1::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanRegions::ICOverscanRegions( MetaProcess* P ) : MetaTable( P )
{
   TheICOverscanRegionsParameter = this;
}

IsoString ICOverscanRegions::Id() const
{
   return "overscanRegions";
}

// ----------------------------------------------------------------------------

ICOverscanRegionEnabled::ICOverscanRegionEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheICOverscanRegionEnabledParameter = this;
}

IsoString ICOverscanRegionEnabled::Id() const
{
   return "enabled";
}

bool ICOverscanRegionEnabled::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICOverscanSourceX0::ICOverscanSourceX0( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanSourceX0Parameter = this;
}

IsoString ICOverscanSourceX0::Id() const
{
   return "sourceX0";
}

double ICOverscanSourceX0::DefaultValue() const
{
   return 0;
}

double ICOverscanSourceX0::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanSourceY0::ICOverscanSourceY0( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanSourceY0Parameter = this;
}

IsoString ICOverscanSourceY0::Id() const
{
   return "sourceY0";
}

double ICOverscanSourceY0::DefaultValue() const
{
   return 0;
}

double ICOverscanSourceY0::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanSourceX1::ICOverscanSourceX1( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanSourceX1Parameter = this;
}

IsoString ICOverscanSourceX1::Id() const
{
   return "sourceX1";
}

double ICOverscanSourceX1::DefaultValue() const
{
   return 0;
}

double ICOverscanSourceX1::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanSourceY1::ICOverscanSourceY1( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanSourceY1Parameter = this;
}

IsoString ICOverscanSourceY1::Id() const
{
   return "sourceY1";
}

double ICOverscanSourceY1::DefaultValue() const
{
   return 0;
}

double ICOverscanSourceY1::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanTargetX0::ICOverscanTargetX0( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanTargetX0Parameter = this;
}

IsoString ICOverscanTargetX0::Id() const
{
   return "targetX0";
}

double ICOverscanTargetX0::DefaultValue() const
{
   return 0;
}

double ICOverscanTargetX0::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanTargetY0::ICOverscanTargetY0( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanTargetY0Parameter = this;
}

IsoString ICOverscanTargetY0::Id() const
{
   return "targetY0";
}

double ICOverscanTargetY0::DefaultValue() const
{
   return 0;
}

double ICOverscanTargetY0::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanTargetX1::ICOverscanTargetX1( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanTargetX1Parameter = this;
}

IsoString ICOverscanTargetX1::Id() const
{
   return "targetX1";
}

double ICOverscanTargetX1::DefaultValue() const
{
   return 0;
}

double ICOverscanTargetX1::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICOverscanTargetY1::ICOverscanTargetY1( MetaTable* T ) : MetaInt32( T )
{
   TheICOverscanTargetY1Parameter = this;
}

IsoString ICOverscanTargetY1::Id() const
{
   return "targetY1";
}

double ICOverscanTargetY1::DefaultValue() const
{
   return 0;
}

double ICOverscanTargetY1::MinimumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ICMasterBiasEnabled::ICMasterBiasEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheICMasterBiasEnabledParameter = this;
}

IsoString ICMasterBiasEnabled::Id() const
{
   return "masterBiasEnabled";
}

bool ICMasterBiasEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICMasterBiasPath::ICMasterBiasPath( MetaProcess* P ) : MetaString( P )
{
   TheICMasterBiasPathParameter = this;
}

IsoString ICMasterBiasPath::Id() const
{
   return "masterBiasPath";
}

// ----------------------------------------------------------------------------

ICMasterDarkEnabled::ICMasterDarkEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheICMasterDarkEnabledParameter = this;
}

IsoString ICMasterDarkEnabled::Id() const
{
   return "masterDarkEnabled";
}

bool ICMasterDarkEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICMasterDarkPath::ICMasterDarkPath( MetaProcess* P ) : MetaString( P )
{
   TheICMasterDarkPathParameter = this;
}

IsoString ICMasterDarkPath::Id() const
{
   return "masterDarkPath";
}

// ----------------------------------------------------------------------------

ICMasterFlatEnabled::ICMasterFlatEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheICMasterFlatEnabledParameter = this;
}

IsoString ICMasterFlatEnabled::Id() const
{
   return "masterFlatEnabled";
}

bool ICMasterFlatEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICMasterFlatPath::ICMasterFlatPath( MetaProcess* P ) : MetaString( P )
{
   TheICMasterFlatPathParameter = this;
}

IsoString ICMasterFlatPath::Id() const
{
   return "masterFlatPath";
}

// ----------------------------------------------------------------------------

ICCalibrateBias::ICCalibrateBias( MetaProcess* P ) : MetaBoolean( P )
{
   TheICCalibrateBiasParameter = this;
}

IsoString ICCalibrateBias::Id() const
{
   return "calibrateBias";
}

bool ICCalibrateBias::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICCalibrateDark::ICCalibrateDark( MetaProcess* P ) : MetaBoolean( P )
{
   TheICCalibrateDarkParameter = this;
}

IsoString ICCalibrateDark::Id() const
{
   return "calibrateDark";
}

bool ICCalibrateDark::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICCalibrateFlat::ICCalibrateFlat( MetaProcess* P ) : MetaBoolean( P )
{
   TheICCalibrateFlatParameter = this;
}

IsoString ICCalibrateFlat::Id() const
{
   return "calibrateFlat";
}

bool ICCalibrateFlat::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICOptimizeDarks::ICOptimizeDarks( MetaProcess* P ) : MetaBoolean( P )
{
   TheICOptimizeDarksParameter = this;
}

IsoString ICOptimizeDarks::Id() const
{
   return "optimizeDarks";
}

bool ICOptimizeDarks::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICDarkOptimizationThreshold::ICDarkOptimizationThreshold( MetaProcess* P ) : MetaFloat( P )
{
   TheICDarkOptimizationThresholdParameter = this;
}

IsoString ICDarkOptimizationThreshold::Id() const
{
   return "darkOptimizationThreshold";
}

int ICDarkOptimizationThreshold::Precision() const
{
   return 5;
}

double ICDarkOptimizationThreshold::DefaultValue() const
{
   return 0.0;
}

double ICDarkOptimizationThreshold::MinimumValue() const
{
   return 0.0;
}

double ICDarkOptimizationThreshold::MaximumValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

ICDarkOptimizationLow::ICDarkOptimizationLow( MetaProcess* P ) : MetaFloat( P )
{
   TheICDarkOptimizationLowParameter = this;
}

IsoString ICDarkOptimizationLow::Id() const
{
   return "darkOptimizationLow";
}

int ICDarkOptimizationLow::Precision() const
{
   return 4;
}

double ICDarkOptimizationLow::DefaultValue() const
{
   return 3.0;
}

double ICDarkOptimizationLow::MinimumValue() const
{
   return 0.0;
}

double ICDarkOptimizationLow::MaximumValue() const
{
   return 10.0;
}

// ----------------------------------------------------------------------------

ICDarkOptimizationWindow::ICDarkOptimizationWindow( MetaProcess* P ) : MetaInt32( P )
{
   TheICDarkOptimizationWindowParameter = this;
}

IsoString ICDarkOptimizationWindow::Id() const
{
   return "darkOptimizationWindow";
}

double ICDarkOptimizationWindow::DefaultValue() const
{
   return 1024;
}

double ICDarkOptimizationWindow::MinimumValue() const
{
   return 0; // zero means window disabled
}

double ICDarkOptimizationWindow::MaximumValue() const
{
   return 65536;
}

// ----------------------------------------------------------------------------

ICDarkCFADetectionMode::ICDarkCFADetectionMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICDarkCFADetectionModeParameter = this;
}

IsoString ICDarkCFADetectionMode::Id() const
{
   return "darkCFADetectionMode";
}

size_type ICDarkCFADetectionMode::NumberOfElements() const
{
   return NumberOfDarkCFADetectionModes;
}

IsoString ICDarkCFADetectionMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case DetectCFA: return "DetectCFA";
   case ForceCFA:  return "ForceCFA";
   case IgnoreCFA: return "IgnoreCFA";
   }
}

int ICDarkCFADetectionMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICDarkCFADetectionMode::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICEvaluateNoise::ICEvaluateNoise( MetaProcess* P ) : MetaBoolean( P )
{
   TheICEvaluateNoiseParameter = this;
}

IsoString ICEvaluateNoise::Id() const
{
   return "evaluateNoise";
}

bool ICEvaluateNoise::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseEvaluationAlgorithm::ICNoiseEvaluationAlgorithm( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICNoiseEvaluationAlgorithmParameter = this;
}

IsoString ICNoiseEvaluationAlgorithm::Id() const
{
   return "noiseEvaluationAlgorithm";
}

size_type ICNoiseEvaluationAlgorithm::NumberOfElements() const
{
   return NumberOfNoiseEvaluationAlgorithms;
}

IsoString ICNoiseEvaluationAlgorithm::ElementId( size_type i ) const
{
   switch ( i )
   {
   case KSigma: return "NoiseEvaluation_KSigma";
   default:
   case MRS:    return "NoiseEvaluation_MRS";
   }
}

int ICNoiseEvaluationAlgorithm::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICNoiseEvaluationAlgorithm::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICOutputDirectory::ICOutputDirectory( MetaProcess* P ) : MetaString( P )
{
   TheICOutputDirectoryParameter = this;
}

IsoString ICOutputDirectory::Id() const
{
   return "outputDirectory";
}

// ----------------------------------------------------------------------------

ICOutputExtension::ICOutputExtension( MetaProcess* P ) : MetaString( P )
{
   TheICOutputExtensionParameter = this;
}

IsoString ICOutputExtension::Id() const
{
   return "outputExtension";
}

String ICOutputExtension::DefaultValue() const
{
   return ".xisf";
}

// ----------------------------------------------------------------------------

ICOutputPrefix::ICOutputPrefix( MetaProcess* P ) : MetaString( P )
{
   TheICOutputPrefixParameter = this;
}

IsoString ICOutputPrefix::Id() const
{
   return "outputPrefix";
}

String ICOutputPrefix::DefaultValue() const
{
   return String(); // no prefix
}

// ----------------------------------------------------------------------------

ICOutputPostfix::ICOutputPostfix( MetaProcess* P ) : MetaString( P )
{
   TheICOutputPostfixParameter = this;
}

IsoString ICOutputPostfix::Id() const
{
   return "outputPostfix";
}

String ICOutputPostfix::DefaultValue() const
{
   return "_c";
}

// ----------------------------------------------------------------------------

ICOutputSampleFormat::ICOutputSampleFormat( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICOutputSampleFormatParameter = this;
}

IsoString ICOutputSampleFormat::Id() const
{
   return "outputSampleFormat";
}

size_type ICOutputSampleFormat::NumberOfElements() const
{
   return NumberOfSampleFormats;
}

IsoString ICOutputSampleFormat::ElementId( size_type i ) const
{
   switch ( i )
   {
   case I16:          return "i16";
   case I32:          return "i32";
   default:
   case F32:          return "f32";
   case F64:          return "f64";
   }
}

int ICOutputSampleFormat::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICOutputSampleFormat::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICOutputPedestal::ICOutputPedestal( MetaProcess* P ) : MetaInt32( P )
{
   TheICOutputPedestalParameter = this;
}

IsoString ICOutputPedestal::Id() const
{
   return "outputPedestal";
}

double ICOutputPedestal::DefaultValue() const
{
   return 0;
}

double ICOutputPedestal::MinimumValue() const
{
   return 0;
}

double ICOutputPedestal::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

ICOverwriteExistingFiles::ICOverwriteExistingFiles( MetaProcess* P ) : MetaBoolean( P )
{
   TheICOverwriteExistingFilesParameter = this;
}

IsoString ICOverwriteExistingFiles::Id() const
{
   return "overwriteExistingFiles";
}

bool ICOverwriteExistingFiles::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICOnError::ICOnError( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICOnErrorParameter = this;
}

IsoString ICOnError::Id() const
{
   return "onError";
}

size_type ICOnError::NumberOfElements() const
{
   return NumberOfErrorPolicies;
}

IsoString ICOnError::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Continue: return "Continue";
   case Abort:    return "Abort";
   case AskUser:  return "AskUser";
   }
}

int ICOnError::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICOnError::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

ICNoGUIMessages::ICNoGUIMessages( MetaProcess* p ) : MetaBoolean( p )
{
   TheICNoGUIMessagesParameter = this;
}

IsoString ICNoGUIMessages::Id() const
{
   return "noGUIMessages";
}

bool ICNoGUIMessages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ICOutputData::ICOutputData( MetaProcess* P ) : MetaTable( P )
{
   TheICOutputDataParameter = this;
}

IsoString ICOutputData::Id() const
{
   return "outputData";
}

bool ICOutputData::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICOutputFilePath::ICOutputFilePath( MetaTable* T ) : MetaString( T )
{
   TheICOutputFilePathParameter = this;
}

IsoString ICOutputFilePath::Id() const
{
   return "outputFilePath";
}

bool ICOutputFilePath::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICDarkScalingFactorRK::ICDarkScalingFactorRK( MetaTable* T ) : MetaFloat( T )
{
   TheICDarkScalingFactorRKParameter = this;
}

IsoString ICDarkScalingFactorRK::Id() const
{
   return "darkScalingFactorRK";
}

int ICDarkScalingFactorRK::Precision() const
{
   return 3;
}

bool ICDarkScalingFactorRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICDarkScalingFactorG::ICDarkScalingFactorG( MetaTable* T ) : MetaFloat( T )
{
   TheICDarkScalingFactorGParameter = this;
}

IsoString ICDarkScalingFactorG::Id() const
{
   return "darkScalingFactorG";
}

int ICDarkScalingFactorG::Precision() const
{
   return 3;
}

bool ICDarkScalingFactorG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICDarkScalingFactorB::ICDarkScalingFactorB( MetaTable* T ) : MetaFloat( T )
{
   TheICDarkScalingFactorBParameter = this;
}

IsoString ICDarkScalingFactorB::Id() const
{
   return "darkScalingFactorB";
}

int ICDarkScalingFactorB::Precision() const
{
   return 3;
}

bool ICDarkScalingFactorB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseEstimateRK::ICNoiseEstimateRK( MetaTable* T ) : MetaFloat( T )
{
   TheICNoiseEstimateRKParameter = this;
}

IsoString ICNoiseEstimateRK::Id() const
{
   return "noiseEstimateRK";
}

int ICNoiseEstimateRK::Precision() const
{
   return 3;
}

bool ICNoiseEstimateRK::ScientificNotation() const
{
   return true;
}

bool ICNoiseEstimateRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseEstimateG::ICNoiseEstimateG( MetaTable* T ) : MetaFloat( T )
{
   TheICNoiseEstimateGParameter = this;
}

IsoString ICNoiseEstimateG::Id() const
{
   return "noiseEstimateG";
}

int ICNoiseEstimateG::Precision() const
{
   return 3;
}

bool ICNoiseEstimateG::ScientificNotation() const
{
   return true;
}

bool ICNoiseEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseEstimateB::ICNoiseEstimateB( MetaTable* T ) : MetaFloat( T )
{
   TheICNoiseEstimateBParameter = this;
}

IsoString ICNoiseEstimateB::Id() const
{
   return "noiseEstimateB";
}

int ICNoiseEstimateB::Precision() const
{
   return 3;
}

bool ICNoiseEstimateB::ScientificNotation() const
{
   return true;
}

bool ICNoiseEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseFractionRK::ICNoiseFractionRK( MetaTable* T ) : MetaFloat( T )
{
   TheICNoiseFractionRKParameter = this;
}

IsoString ICNoiseFractionRK::Id() const
{
   return "noiseFractionRK";
}

int ICNoiseFractionRK::Precision() const
{
   return 3;
}

bool ICNoiseFractionRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseFractionG::ICNoiseFractionG( MetaTable* T ) : MetaFloat( T )
{
   TheICNoiseFractionGParameter = this;
}

IsoString ICNoiseFractionG::Id() const
{
   return "noiseFractionG";
}

int ICNoiseFractionG::Precision() const
{
   return 3;
}

bool ICNoiseFractionG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseFractionB::ICNoiseFractionB( MetaTable* T ) : MetaFloat( T )
{
   TheICNoiseFractionBParameter = this;
}

IsoString ICNoiseFractionB::Id() const
{
   return "noiseFractionB";
}

int ICNoiseFractionB::Precision() const
{
   return 3;
}

bool ICNoiseFractionB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseAlgorithmRK::ICNoiseAlgorithmRK( MetaTable* T ) : MetaString( T )
{
   TheICNoiseAlgorithmRKParameter = this;
}

IsoString ICNoiseAlgorithmRK::Id() const
{
   return "noiseAlgorithmRK";
}

bool ICNoiseAlgorithmRK::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseAlgorithmG::ICNoiseAlgorithmG( MetaTable* T ) : MetaString( T )
{
   TheICNoiseAlgorithmGParameter = this;
}

IsoString ICNoiseAlgorithmG::Id() const
{
   return "noiseAlgorithmG";
}

bool ICNoiseAlgorithmG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICNoiseAlgorithmB::ICNoiseAlgorithmB( MetaTable* T ) : MetaString( T )
{
   TheICNoiseAlgorithmBParameter = this;
}

IsoString ICNoiseAlgorithmB::Id() const
{
   return "noiseAlgorithmB";
}

bool ICNoiseAlgorithmB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageCalibrationParameters.cpp - Released 2018-11-01T11:07:21Z
