//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorParameters.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "SubframeSelectorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SSRoutine*                       TheSSRoutineParameter = nullptr;

SSSubframes*                     TheSSSubframesParameter = nullptr;
SSSubframeEnabled*               TheSSSubframeEnabledParameter = nullptr;
SSSubframePath*                  TheSSSubframePathParameter = nullptr;

SSSubframeScale*                 TheSSSubframeScaleParameter = nullptr;
SSCameraGain*                    TheSSCameraGainParameter = nullptr;
SSCameraResolution*              TheSSCameraResolutionParameter = nullptr;
SSSiteLocalMidnight*             TheSSSiteLocalMidnightParameter = nullptr;
SSScaleUnit*                     TheSSScaleUnitParameter = nullptr;
SSDataUnit*                      TheSSDataUnitParameter = nullptr;

SSStructureLayers*               TheSSStructureLayersParameter = nullptr;
SSNoiseLayers*                   TheSSNoiseLayersParameter = nullptr;
SSHotPixelFilterRadius*          TheSSHotPixelFilterRadiusParameter = nullptr;
SSApplyHotPixelFilter*           TheSSApplyHotPixelFilterParameter = nullptr;
SSNoiseReductionFilterRadius*    TheSSNoiseReductionFilterRadiusParameter = nullptr;
SSSensitivity*                   TheSSSensitivityParameter = nullptr;
SSPeakResponse*                  TheSSPeakResponseParameter = nullptr;
SSMaxDistortion*                 TheSSMaxDistortionParameter = nullptr;
SSUpperLimit*                    TheSSUpperLimitParameter = nullptr;
SSBackgroundExpansion*           TheSSBackgroundExpansionParameter = nullptr;
SSXYStretch*                     TheSSXYStretchParameter = nullptr;
SSPSFFit*                        TheSSPSFFitParameter = nullptr;
SSPSFFitCircular*                TheSSPSFFitCircularParameter = nullptr;
SSPedestal*                      TheSSPedestalParameter = nullptr;
SSROIX0*                         TheSSROIX0Parameter = nullptr;
SSROIY0*                         TheSSROIY0Parameter = nullptr;
SSROIX1*                         TheSSROIX1Parameter = nullptr;
SSROIY1*                         TheSSROIY1Parameter = nullptr;

SSApprovalExpression*            TheSSApprovalExpressionParameter = nullptr;
SSWeightingExpression*           TheSSWeightingExpressionParameter = nullptr;

SSSortingProperty*               TheSSSortingPropertyParameter = nullptr;
SSGraphProperty*                 TheSSGraphPropertyParameter = nullptr;

SSMeasurements*                     TheSSMeasurementsParameter = nullptr;
SSMeasurementIndex*                 TheSSMeasurementIndexParameter = nullptr;
SSMeasurementEnabled*               TheSSMeasurementEnabledParameter = nullptr;
SSMeasurementLocked*                TheSSMeasurementLockedParameter = nullptr;
SSMeasurementPath*                  TheSSMeasurementPathParameter = nullptr;
SSMeasurementWeight*                TheSSMeasurementWeightParameter = nullptr;
SSMeasurementFWHM*                  TheSSMeasurementFWHMParameter = nullptr;
SSMeasurementEccentricity*          TheSSMeasurementEccentricityParameter = nullptr;
SSMeasurementSNRWeight*             TheSSMeasurementSNRWeightParameter = nullptr;
SSMeasurementMedian*                TheSSMeasurementMedianParameter = nullptr;
SSMeasurementMedianMeanDev*         TheSSMeasurementMedianMeanDevParameter = nullptr;
SSMeasurementNoise*                 TheSSMeasurementNoiseParameter = nullptr;
SSMeasurementNoiseRatio*            TheSSMeasurementNoiseRatioParameter = nullptr;
SSMeasurementStars*                 TheSSMeasurementStarsParameter = nullptr;
SSMeasurementStarResidual*          TheSSMeasurementStarResidualParameter = nullptr;
SSMeasurementFWHMMeanDev*           TheSSMeasurementFWHMMeanDevParameter = nullptr;
SSMeasurementEccentricityMeanDev*   TheSSMeasurementEccentricityMeanDevParameter = nullptr;
SSMeasurementStarResidualMeanDev*   TheSSMeasurementStarResidualMeanDevParameter = nullptr;

// ----------------------------------------------------------------------------

SSRoutine::SSRoutine( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSRoutineParameter = this;
}

IsoString SSRoutine::Id() const
{
   return "routine";
}

size_type SSRoutine::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSRoutine::ElementId( size_type i ) const
{
   switch ( i )
   {
      default:
      case MeasureSubframes:  return "MeasureSubframes";
      case OutputSubframes: return "OutputSubframes";
      case StarDetectionPreview:  return "StarDetectionPreview";
   }
}

int SSRoutine::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSRoutine::ElementLabel( size_type i ) const
{
   switch ( i )
   {
      default:
      case MeasureSubframes:  return "Measure Subframes";
      case OutputSubframes: return "Output Subframes";
      case StarDetectionPreview:  return "Star Detection Preview";
   }
}

size_type SSRoutine::DefaultValueIndex() const
{
   return Default;
}

IsoString SSRoutine::Tooltip() const
{
   return "<p><strong>Measure Subframes:</strong> Measures subframe parameters "
           "and displays them for Approval and Weighting.</p>"
           "<p><strong>Output Subframes:</strong> Uses Approval and Weighting "
           "information to output subframes.</p>"
           "<p><strong>Star Detection Preview:</strong> Outputs a Structure Map from "
           "the Star Detector for the first subframe. Useful for tweaking Star Detector parameters.</p>";
};

// ----------------------------------------------------------------------------

SSSubframes::SSSubframes( MetaProcess* P ) : MetaTable( P )
{
   TheSSSubframesParameter = this;
}

IsoString SSSubframes::Id() const
{
   return "subframes";
}

// ----------------------------------------------------------------------------

SSSubframeEnabled::SSSubframeEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheSSSubframeEnabledParameter = this;
}

IsoString SSSubframeEnabled::Id() const
{
   return "subframeEnabled";
}

bool SSSubframeEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSSubframePath::SSSubframePath( MetaTable* T ) : MetaString( T )
{
   TheSSSubframePathParameter = this;
}

IsoString SSSubframePath::Id() const
{
   return "subframePath";
}

// ----------------------------------------------------------------------------

SSSubframeScale::SSSubframeScale( MetaProcess* P ) : MetaFloat( P )
{
   TheSSSubframeScaleParameter = this;
}

IsoString SSSubframeScale::Id() const
{
   return "subframeScale";
}

int SSSubframeScale::Precision() const
{
   return 4;
}

double SSSubframeScale::DefaultValue() const
{
   return 1;
}

double SSSubframeScale::MinimumValue() const
{
   return 0.01;
}

double SSSubframeScale::MaximumValue() const
{
   return 100.0;
}

IsoString SSSubframeScale::Tooltip() const
{
   return "<p>Subframe scale in arcseconds per pixel. This affects star-related measurements.</p>"
           "<p>This is only used if the Scale Units are not in Pixels</p>";
}

// ----------------------------------------------------------------------------

SSCameraGain::SSCameraGain( MetaProcess* P ) : MetaFloat( P )
{
   TheSSCameraGainParameter = this;
}

IsoString SSCameraGain::Id() const
{
   return "cameraGain";
}

int SSCameraGain::Precision() const
{
   return 4;
}

double SSCameraGain::DefaultValue() const
{
   return 1;
}

double SSCameraGain::MinimumValue() const
{
   return 0.01;
}

double SSCameraGain::MaximumValue() const
{
   return 100.0;
}

IsoString SSCameraGain::Tooltip() const
{
   return "<p>Subframe camera gain in electrons per Data Number. This affects star-related measurements.</p>"
           "<p>This is only used if the Data Units are not in Data Numbers</p>";
}

// ----------------------------------------------------------------------------

SSCameraResolution::SSCameraResolution( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSCameraResolutionParameter = this;
}

IsoString SSCameraResolution::Id() const
{
   return "cameraResolution";
}

size_type SSCameraResolution::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSCameraResolution::ElementId( size_type i ) const
{
   switch ( i )
   {
      default:
      case Bits8:  return "Bits8";
      case Bits10: return "Bits10";
      case Bits12:  return "Bits12";
      case Bits14:  return "Bits14";
      case Bits16:  return "Bits16";
   }
}

int SSCameraResolution::ElementValue( size_type i ) const
{
   return int( i );
}

int SSCameraResolution::ElementData( size_type i ) const
{
   switch ( i )
   {
      default:
      case Bits8:  return 255;
      case Bits10: return 1023;
      case Bits12:  return 4095;
      case Bits14:  return 16383;
      case Bits16:  return 65535;
   }
}

IsoString SSCameraResolution::ElementLabel( size_type i ) const
{
   switch ( i )
   {
      default:
      case Bits8:  return "8-bit [0, 255]";
      case Bits10: return "10-bit [0, 1023]";
      case Bits12:  return "12-bit [0, 4095]";
      case Bits14:  return "14-bit [0, 16383]";
      case Bits16:  return "16-bit [0, 65535]";
   }
}

size_type SSCameraResolution::DefaultValueIndex() const
{
   return Default;
}

IsoString SSCameraResolution::Tooltip() const
{
   return "<p>Subframe camera resolution in bits per pixel. This affects some subframe-related measurements.</p>";
}

// ----------------------------------------------------------------------------

SSSiteLocalMidnight::SSSiteLocalMidnight( MetaProcess* P ) : MetaInt32( P )
{
   TheSSSiteLocalMidnightParameter = this;
}

IsoString SSSiteLocalMidnight::Id() const
{
   return "siteLocalMidnight";
}

double SSSiteLocalMidnight::DefaultValue() const
{
   return 24;
}

double SSSiteLocalMidnight::MinimumValue() const
{
   return 0;
}

double SSSiteLocalMidnight::MaximumValue() const
{
   return 24;
}

IsoString SSSiteLocalMidnight::Tooltip() const
{
   return "<p>This parameters specifies the Coordinated Universal Time (UTC) of local midnight "
           "at the site of target subframe observation, rounded to the nearest hour from 0 to "
           "23. If this time is unknown or varies by more than six hours, set this parameter "
           "to 24.</p> "
           "<p>This parameter and the value of the FITS keyword DATE-OBS (if available) are used to "
           "identify sequences of subframe observations that occurred during the same night for "
           "data presentation purposes.</p>";
}

// ----------------------------------------------------------------------------

SSScaleUnit::SSScaleUnit( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSScaleUnitParameter = this;
}

IsoString SSScaleUnit::Id() const
{
   return "scaleUnit";
}

size_type SSScaleUnit::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSScaleUnit::ElementId( size_type i ) const
{
   switch ( i )
   {
      default:
      case ArcSeconds:  return "ArcSeconds";
      case Pixel: return "Pixel";
   }
}

int SSScaleUnit::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSScaleUnit::ElementLabel( size_type i ) const
{
   switch ( i )
   {
      default:
      case ArcSeconds:  return "Arcseconds (arcsec)";
      case Pixel: return "Pixels (pixel)";
   }
}

size_type SSScaleUnit::DefaultValueIndex() const
{
   return Default;
}

IsoString SSScaleUnit::Tooltip() const
{
   return "<p>How to present some star-related measurements.</p>";
}

// ----------------------------------------------------------------------------

SSDataUnit::SSDataUnit( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSDataUnitParameter = this;
}

IsoString SSDataUnit::Id() const
{
   return "dataUnit";
}

size_type SSDataUnit::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSDataUnit::ElementId( size_type i ) const
{
   switch ( i )
   {
      default:
      case Electron:  return "Electron";
      case DataNumber: return "DataNumber";
   }
}

int SSDataUnit::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSDataUnit::ElementLabel( size_type i ) const
{
   switch ( i )
   {
      default:
      case Electron:  return "Electrons (e-)";
      case DataNumber: return "Data Numbers (DN)";
   }
}

size_type SSDataUnit::DefaultValueIndex() const
{
   return Default;
}

IsoString SSDataUnit::Tooltip() const
{
   return "<p>How to present some subframe-related measurements.</p>";
}

// ----------------------------------------------------------------------------

SSStructureLayers::SSStructureLayers( MetaProcess* P ) : MetaInt32( P )
{
   TheSSStructureLayersParameter = this;
}

IsoString SSStructureLayers::Id() const
{
   return "structureLayers";
}

double SSStructureLayers::DefaultValue() const
{
   return 5;
}

double SSStructureLayers::MinimumValue() const
{
   return 1;
}

double SSStructureLayers::MaximumValue() const
{
   return 20;
}

IsoString SSStructureLayers::Tooltip() const
{
   return "<p>This parameter specifies the number of wavelet layers used for star detection. "
           "With more wavelet layers larger stars and perhaps also some nonstellar objects will be detected. "
           "Fewer wavelet layers favors detection of smaller, and hence more, stars.</p>";
}

// ----------------------------------------------------------------------------

SSNoiseLayers::SSNoiseLayers( MetaProcess* P ) : MetaInt32( P )
{
   TheSSNoiseLayersParameter = this;
}

IsoString SSNoiseLayers::Id() const
{
   return "noiseLayers";
}

double SSNoiseLayers::DefaultValue() const
{
   return 0;
}

double SSNoiseLayers::MinimumValue() const
{
   return 0;
}

double SSNoiseLayers::MaximumValue() const
{
   return 20;
}

IsoString SSNoiseLayers::Tooltip() const
{
   return "<p>This parameter specifies the number of wavelet layers used for noise reduction. "
           "Noise reduction prevents detection of bright noise structures as false stars, "
           "including hot pixels and cosmic rays.</p> "
           "<p>This parameter can also be used to control the sizes of the smallest detected "
           "stars (increase to exclude more stars).</p>";
}

// ----------------------------------------------------------------------------

SSHotPixelFilterRadius::SSHotPixelFilterRadius( MetaProcess* P ) : MetaInt32( P )
{
   TheSSHotPixelFilterRadiusParameter = this;
}

IsoString SSHotPixelFilterRadius::Id() const
{
   return "hotPixelFilterRadius";
}

double SSHotPixelFilterRadius::DefaultValue() const
{
   return 1;
}

double SSHotPixelFilterRadius::MinimumValue() const
{
   return 0;
}

double SSHotPixelFilterRadius::MaximumValue() const
{
   return 20;
}

IsoString SSHotPixelFilterRadius::Tooltip() const
{
   return "<p>This parameter specifies the radius in pixels of median filter applied before star detection to remove hot pixels.</p>"
           "<p>To disable hot pixel removal, set this parameter to zero.</p>";
}

// ----------------------------------------------------------------------------

SSApplyHotPixelFilter::SSApplyHotPixelFilter( MetaProcess* T ) : MetaBoolean( T )
{
   TheSSApplyHotPixelFilterParameter = this;
}

IsoString SSApplyHotPixelFilter::Id() const
{
   return "applyHotPixelFilter";
}

bool SSApplyHotPixelFilter::DefaultValue() const
{
   return false;
}

IsoString SSApplyHotPixelFilter::Tooltip() const
{
   return "<p>Whether the hot pixel filter removal should be applied to the image used"
           "for star detection, or only to the working image used to build the structure map.</p>"
           "<p>By setting this parameter to true, the detection algorithm is completely"
           "robust to hot pixels (of sizes not larger than hotPixelFilterRadius), but"
           "it is also less sensitive, so less stars will in general be detected."
           "With the default value of false, some hot pixels may be wrongly detected"
           "as stars but the number of true stars detected will generally be larger.</p>";
}

// ----------------------------------------------------------------------------

SSNoiseReductionFilterRadius::SSNoiseReductionFilterRadius( MetaProcess* P ) : MetaInt32( P )
{
   TheSSNoiseReductionFilterRadiusParameter = this;
}

IsoString SSNoiseReductionFilterRadius::Id() const
{
   return "noiseReductionFilterRadius";
}

double SSNoiseReductionFilterRadius::DefaultValue() const
{
   return 0;
}

double SSNoiseReductionFilterRadius::MinimumValue() const
{
   return 0;
}

double SSNoiseReductionFilterRadius::MaximumValue() const
{
   return 20;
}

IsoString SSNoiseReductionFilterRadius::Tooltip() const
{
   return "<p>Half size in pixels of a Gaussian convolution filter applied for noise"
           "reduction. Useful for star detection in low-SNR images.</p>"
           "<p>Setting the value of this parameter > 0 implies "
           "Hot Pixel Filter To Detection Image.</p>";
}

// ----------------------------------------------------------------------------

SSSensitivity::SSSensitivity( MetaProcess* P ) : MetaFloat( P )
{
   TheSSSensitivityParameter = this;
}

IsoString SSSensitivity::Id() const
{
   return "sensitivity";
}

int SSSensitivity::Precision() const
{
   return 4;
}

double SSSensitivity::DefaultValue() const
{
   return 0.1;
}

double SSSensitivity::MinimumValue() const
{
   return 0.0;
}

double SSSensitivity::MaximumValue() const
{
   return 1.0;
}

IsoString SSSensitivity::Tooltip() const
{
   return "<p>The sensitivity of the star detection algorithm is measured with respect to the "
           "local background of each detected star.</p>"
           "<p>Given a star with estimated brightness "
           "s and local background b, sensitivity is the minimum value of (s - b) / b "
           "necessary to trigger star detection.</p>";
}

// ----------------------------------------------------------------------------

SSPeakResponse::SSPeakResponse( MetaProcess* P ) : MetaFloat( P )
{
   TheSSPeakResponseParameter = this;
}

IsoString SSPeakResponse::Id() const
{
   return "peakResponse";
}

int SSPeakResponse::Precision() const
{
   return 4;
}

double SSPeakResponse::DefaultValue() const
{
   return 0.8;
}

double SSPeakResponse::MinimumValue() const
{
   return 0.0;
}

double SSPeakResponse::MaximumValue() const
{
   return 1.0;
}

IsoString SSPeakResponse::Tooltip() const
{
   return "<p>If you decrease this parameter, stars will need to have more prominent "
           "peaks to be detected by the star detection algorithm. By increasing "
           "this parameter, the star detection algorithm will be more "
           "permissive with relatively flat stars.</p>";
}

// ----------------------------------------------------------------------------

SSMaxDistortion::SSMaxDistortion( MetaProcess* P ) : MetaFloat( P )
{
   TheSSMaxDistortionParameter = this;
}

IsoString SSMaxDistortion::Id() const
{
   return "maxDistortion";
}

int SSMaxDistortion::Precision() const
{
   return 4;
}

double SSMaxDistortion::DefaultValue() const
{
   return 0.5;
}

double SSMaxDistortion::MinimumValue() const
{
   return 0.0;
}

double SSMaxDistortion::MaximumValue() const
{
   return 1.0;
}

IsoString SSMaxDistortion::Tooltip() const
{
   return "<p>Star distortion is the fractional area of the star's bounding box "
           "covered by the star. The distortion of a perfectly circular star "
           "is about 0.75 (actually, pi/4). Decrease this parameter to detect "
           "stars with larger elongation.</p>";
}

// ----------------------------------------------------------------------------

SSUpperLimit::SSUpperLimit( MetaProcess* P ) : MetaFloat( P )
{
   TheSSUpperLimitParameter = this;
}

IsoString SSUpperLimit::Id() const
{
   return "upperLimit";
}

int SSUpperLimit::Precision() const
{
   return 4;
}

double SSUpperLimit::DefaultValue() const
{
   return 1.0;
}

double SSUpperLimit::MinimumValue() const
{
   return 0.0;
}

double SSUpperLimit::MaximumValue() const
{
   return 100.0;
}

IsoString SSUpperLimit::Tooltip() const
{
   return "<p>Stars with peak values greater than this value won't be detected.</p>";
}

// ----------------------------------------------------------------------------

SSBackgroundExpansion::SSBackgroundExpansion( MetaProcess* P ) : MetaInt32( P )
{
   TheSSBackgroundExpansionParameter = this;
}

IsoString SSBackgroundExpansion::Id() const
{
   return "backgroundExpansion";
}

double SSBackgroundExpansion::DefaultValue() const
{
   return 3;
}

double SSBackgroundExpansion::MinimumValue() const
{
   return 1;
}

double SSBackgroundExpansion::MaximumValue() const
{
   return 10;
}

IsoString SSBackgroundExpansion::Tooltip() const
{
   return "<p>Local background is evaluated for each star on an inflated rectangular region "
           "around the star detection structure. "
           "Background Expansion is the inflation distance in pixels.</p>";
}

// ----------------------------------------------------------------------------

SSXYStretch::SSXYStretch( MetaProcess* P ) : MetaFloat( P )
{
   TheSSXYStretchParameter = this;
}

IsoString SSXYStretch::Id() const
{
   return "xyStretch";
}

int SSXYStretch::Precision() const
{
   return 4;
}

double SSXYStretch::DefaultValue() const
{
   return 1.5;
}

double SSXYStretch::MinimumValue() const
{
   return 0.0;
}

double SSXYStretch::MaximumValue() const
{
   return 6.0;
}

IsoString SSXYStretch::Tooltip() const
{
   return "<p>Stretch factor for the barycenter search algorithm, in sigma units."
           "Increase it to make the algorithm more robust to nearby structures, such"
           "as multiple/crowded stars and small nebular features. However, too large"
           "of a stretch factor will make the algorithm less accurate.</p>";
}

// ----------------------------------------------------------------------------

SSPSFFit::SSPSFFit( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSPSFFitParameter = this;
}

IsoString SSPSFFit::Id() const
{
   return "psfFit";
}

size_type SSPSFFit::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSPSFFit::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Gaussian:  return "Gaussian";
   case Moffat10:  return "Moffat10";
   case Moffat8:  return "Moffat8";
   case Moffat6:  return "Moffat6";
   case Moffat4:  return "Moffat4";
   case Moffat25:  return "Moffat25";
   case Moffat15:  return "Moffat15";
   case Lorentzian:  return "Lorentzian";
   }
}

int SSPSFFit::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSPSFFit::ElementLabel( size_type i ) const
{
   switch ( i )
   {
   default:
   case Gaussian:  return "Gaussian";
   case Moffat10:  return "Moffat10";
   case Moffat8:  return "Moffat8";
   case Moffat6:  return "Moffat6";
   case Moffat4:  return "Moffat4";
   case Moffat25:  return "Moffat2.5";
   case Moffat15:  return "Moffat1.5";
   case Lorentzian:  return "Lorentzian";
   }
}

size_type SSPSFFit::DefaultValueIndex() const
{
   return Default;
}

IsoString SSPSFFit::Tooltip() const
{
   return "<p>This parameter specifies the <i>point spread function</i> (PSF) used to fit star images.</p>"
           "<p>The function may be either an elliptical <i>Gaussian</i>, an elliptical "
           "<i>Moffat</i> with a selected <i>Beta</i> parameter or an elliptical "
           "<i>Lorentzian</i>.</p>";
}

// ----------------------------------------------------------------------------

SSPSFFitCircular::SSPSFFitCircular( MetaProcess* P ) : MetaBoolean( P )
{
   TheSSPSFFitCircularParameter = this;
}

IsoString SSPSFFitCircular::Id() const
{
   return "psfFitCircular";
}

bool SSPSFFitCircular::DefaultValue() const
{
   return false;
}

IsoString SSPSFFitCircular::Tooltip() const
{
   return "<p>Enable this option to fit circular point spread functions. Disable it to fit elliptical functions.</p>"
           "Circular functions can provide more robust and useful results in cases of strong "
           "undersampling or high noise levels.</p>";
}

// ----------------------------------------------------------------------------

SSPedestal::SSPedestal( MetaProcess* P ) : MetaUInt16( P )
{
   TheSSPedestalParameter = this;
}

IsoString SSPedestal::Id() const
{
   return "pedestal";
}

int SSPedestal::Precision() const
{
   return 0;
}

double SSPedestal::DefaultValue() const
{
   return 0;
}

double SSPedestal::MinimumValue() const
{
   return 0;
}

double SSPedestal::MaximumValue() const
{
   return UINT16_MAX;
}

IsoString SSPedestal::Tooltip() const
{
   return "<p>Pedestal value in the Camera Resolution Data Number range (e.g. 16-bit is from 0 to 65535), "
           "which will be subtracted from each image before processing. This value is "
           "divided by the Camera Resolution.</p>";
}

// ----------------------------------------------------------------------------

SSROIX0::SSROIX0( MetaProcess* P ) : MetaInt32( P )
{
   TheSSROIX0Parameter = this;
}

IsoString SSROIX0::Id() const
{
   return "roiX0";
}

double SSROIX0::DefaultValue() const
{
   return 0;
}

double SSROIX0::MinimumValue() const
{
   return 0;
}

double SSROIX0::MaximumValue() const
{
   return uint16_max;
}

// ----------------------------------------------------------------------------

SSROIY0::SSROIY0( MetaProcess* P ) : MetaInt32( P )
{
   TheSSROIY0Parameter = this;
}

IsoString SSROIY0::Id() const
{
   return "roiY0";
}

double SSROIY0::DefaultValue() const
{
   return 0;
}

double SSROIY0::MinimumValue() const
{
   return 0;
}

double SSROIY0::MaximumValue() const
{
   return uint16_max;
}

// ----------------------------------------------------------------------------

SSROIX1::SSROIX1( MetaProcess* P ) : MetaInt32( P )
{
   TheSSROIX1Parameter = this;
}

IsoString SSROIX1::Id() const
{
   return "roiX1";
}

double SSROIX1::DefaultValue() const
{
   return 0;
}

double SSROIX1::MinimumValue() const
{
   return 0;
}

double SSROIX1::MaximumValue() const
{
   return uint16_max;
}

// ----------------------------------------------------------------------------

SSROIY1::SSROIY1( MetaProcess* P ) : MetaInt32( P )
{
   TheSSROIY1Parameter = this;
}

IsoString SSROIY1::Id() const
{
   return "roiY1";
}

double SSROIY1::DefaultValue() const
{
   return 0;
}

double SSROIY1::MinimumValue() const
{
   return 0;
}

double SSROIY1::MaximumValue() const
{
   return uint16_max;
}

// ----------------------------------------------------------------------------

SSApprovalExpression::SSApprovalExpression( MetaProcess* P ) : MetaString( P )
{
   TheSSApprovalExpressionParameter = this;
}

IsoString SSApprovalExpression::Id() const
{
   return "approvalExpression";
}

IsoString SSApprovalExpression::Tooltip() const
{
   return "<p>Subframe approval expression, a boolean combination of constraints. A blank "
           "expression will approve all subframes. This is technically JavaScript, and although "
           "some characters are limited to prevent misuse, many things are possible such as "
           "<i>Math.abs(FWHMSigma)<i> or using <i>//</i> to comment the end of the line.</p>"
           "<p><i>approval</i> = [ <i>constraint</i> [ [ && | || ] <i>constraint</i> ]... ]</p>"
           "<p><i>constraint</i> = [ <i>weighting</i> [ &lt; | &gt; | &lt;= | &gt;= | == | != ] "
           "<i>weighting</i> | true | false | [ ! ] (<i>approval</i>) ]</p>"
           "<p><i>weighting</i> = <i>term</i> [ [ + | - | * | / ] <i>term</i> ]...</p>"
           "<p><i>term</i> = [ - ] [ <i>number</i> | <i>property</i> | (<i>weighting</i>) ]</p>"
           "<p><i>property</i> = [ Index | Weight | FWHM | Eccentricity | SNRWeight | Median | "
           "MedianMeanDev | Noise | Stars | StarResidual | NoiseRatio | FWHMMeanDev | EccentricityMeanDev | "
           "StarResidualMeanDev]</p>"
           "<p>Each <i>property</i> (excluding <i>Index</i>) also has a Sigma version, "
           "e.g. <i>SNRWeightSigma</i>, where the value is represented in sigma units of the "
           "Mean Absolute Deviation from the Median.</p>";
}

// ----------------------------------------------------------------------------

SSWeightingExpression::SSWeightingExpression( MetaProcess* P ) : MetaString( P )
{
   TheSSWeightingExpressionParameter = this;
}

IsoString SSWeightingExpression::Id() const
{
   return "weightingExpression";
}

IsoString SSWeightingExpression::Tooltip() const
{
   return "<p>Subframe weighting expression, an arithmetic combination of properties. A blank "
           "expression will assign a zero weight to all subframes. This is technically JavaScript, "
           "and although some characters are limited to prevent misuse, many things are possible such as "
           "<i>Math.abs(FWHMSigma)</i> or using <i>//</i> to comment the end of the line.</p>"
           "<p><i>weighting</i> = [ <i>term</i> [ [ + | - | * | / ] <i>term</i> ]... ]</p>"
           "<p><i>term</i> = [ - ] [ <i>number</i> | <i>property</i> | (<i>weighting</i>) ]</p>"
           "<p><i>property</i> = [ Index | FWHM | Eccentricity | SNRWeight | Median | MedianMeanDev | "
           "Noise | Stars | StarResidual | NoiseRatio | FWHMMeanDev | EccentricityMeanDev | "
           "StarResidualMeanDev ]</p>"
           "<p>Each <i>property<i> (excluding <i>Index</i>) also has a Sigma version, "
           "e.g. <i>SNRWeightSigma</i>, where the value is represented in sigma units of the "
           "Mean Absolute Deviation from the Median.</p>";
}

// ----------------------------------------------------------------------------

SSSortingProperty::SSSortingProperty( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSSortingPropertyParameter = this;
}

IsoString SSSortingProperty::Id() const
{
   return "sortProperty";
}

size_type SSSortingProperty::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSSortingProperty::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Index:  return "Index";
   case Weight:  return "Weight";
   case FWHM:  return "FWHM";
   case Eccentricity:  return "Eccentricity";
   case SNRWeight:  return "SNRWeight";
   case Median:  return "Median";
   case MedianMeanDev:  return "MedianMeanDev";
   case Noise:  return "Noise";
   case NoiseRatio:  return "NoiseRatio";
   case Stars:  return "Stars";
   case StarResidual:  return "StarResidual";
   case FWHMMeanDev:  return "FWHMMeanDev";
   case EccentricityMeanDev:  return "EccentricityMeanDev";
   case StarResidualMeanDev:  return "StarResidualMeanDev";
   }
}

int SSSortingProperty::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSSortingProperty::ElementLabel( size_type i ) const
{
   switch ( i )
   {
   default:
   case Index:  return "Index";
   case Weight:  return "Weight";
   case FWHM:  return "FWHM";
   case Eccentricity:  return "Eccentricity";
   case SNRWeight:  return "SNR Weight";
   case Median:  return "Median";
   case MedianMeanDev:  return "Median Mean Dev.";
   case Noise:  return "Noise";
   case NoiseRatio:  return "Noise Ratio";
   case Stars:  return "Stars";
   case StarResidual:  return "Star Residual";
   case FWHMMeanDev:  return "FWHM Mean Dev.";
   case EccentricityMeanDev:  return "Eccentricity Mean Dev.";
   case StarResidualMeanDev:  return "Star Residual Mean Dev.";
   }
}

size_type SSSortingProperty::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

SSGraphProperty::SSGraphProperty( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSSGraphPropertyParameter = this;
}

IsoString SSGraphProperty::Id() const
{
   return "graphProperty";
}

size_type SSGraphProperty::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SSGraphProperty::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Weight:  return "Weight";
   case FWHM:  return "FWHM";
   case Eccentricity:  return "Eccentricity";
   case SNRWeight:  return "SNRWeight";
   case Median:  return "Median";
   case MedianMeanDev:  return "MedianMeanDev";
   case Noise:  return "Noise";
   case NoiseRatio:  return "NoiseRatio";
   case Stars:  return "Stars";
   case StarResidual:  return "StarResidual";
   case FWHMMeanDev:  return "FWHMMeanDev";
   case EccentricityMeanDev:  return "EccentricityMeanDev";
   case StarResidualMeanDev:  return "StarResidualMeanDev";
   }
}

int SSGraphProperty::ElementValue( size_type i ) const
{
   return int( i );
}

IsoString SSGraphProperty::ElementLabel( size_type i ) const
{
   switch ( i )
   {
   default:
   case Weight:  return "Weight";
   case FWHM:  return "FWHM";
   case Eccentricity:  return "Eccentricity";
   case SNRWeight:  return "SNR Weight";
   case Median:  return "Median";
   case MedianMeanDev:  return "Median Mean Dev.";
   case Noise:  return "Noise";
   case NoiseRatio:  return "Noise Ratio";
   case Stars:  return "Stars";
   case StarResidual:  return "Star Residual";
   case FWHMMeanDev:  return "FWHM Mean Dev.";
   case EccentricityMeanDev:  return "Eccentricity Mean Dev.";
   case StarResidualMeanDev:  return "Star Residual Mean Dev.";
   }
}

size_type SSGraphProperty::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

SSMeasurements::SSMeasurements( MetaProcess* P ) : MetaTable( P )
{
   TheSSMeasurementsParameter = this;
}

IsoString SSMeasurements::Id() const
{
   return "measurements";
}

// ----------------------------------------------------------------------------

SSMeasurementIndex::SSMeasurementIndex( MetaTable* T ) : MetaUInt16( T )
{
   TheSSMeasurementIndexParameter = this;
}

IsoString SSMeasurementIndex::Id() const
{
   return "measurementIndex";
}

int SSMeasurementIndex::Precision() const
{
   return 0;
}

double SSMeasurementIndex::DefaultValue() const
{
   return 1;
}

double SSMeasurementIndex::MinimumValue() const
{
   return 1;
}

double SSMeasurementIndex::MaximumValue() const
{
   return UINT16_MAX;
}

// ----------------------------------------------------------------------------

SSMeasurementEnabled::SSMeasurementEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheSSMeasurementEnabledParameter = this;
}

IsoString SSMeasurementEnabled::Id() const
{
   return "measurementEnabled";
}

bool SSMeasurementEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSMeasurementLocked::SSMeasurementLocked( MetaTable* T ) : MetaBoolean( T )
{
   TheSSMeasurementLockedParameter = this;
}

IsoString SSMeasurementLocked::Id() const
{
   return "measurementLocked";
}

bool SSMeasurementLocked::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SSMeasurementPath::SSMeasurementPath( MetaTable* T ) : MetaString( T )
{
   TheSSMeasurementPathParameter = this;
}

IsoString SSMeasurementPath::Id() const
{
   return "measurementPath";
}

// ----------------------------------------------------------------------------

SSMeasurementWeight::SSMeasurementWeight( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementWeightParameter = this;
}

IsoString SSMeasurementWeight::Id() const
{
   return "measurementWeight";
}

int SSMeasurementWeight::Precision() const
{
   return 4;
}

double SSMeasurementWeight::DefaultValue() const
{
   return 0;
}

double SSMeasurementWeight::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementWeight::MaximumValue() const
{
   return 10000.0;
}

// ----------------------------------------------------------------------------

SSMeasurementFWHM::SSMeasurementFWHM( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementFWHMParameter = this;
}

IsoString SSMeasurementFWHM::Id() const
{
   return "measurementFWHM";
}

int SSMeasurementFWHM::Precision() const
{
   return 4;
}

double SSMeasurementFWHM::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementFWHM::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementFWHM::MaximumValue() const
{
   return 100.0;
}

// ----------------------------------------------------------------------------

SSMeasurementEccentricity::SSMeasurementEccentricity( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementEccentricityParameter = this;
}

IsoString SSMeasurementEccentricity::Id() const
{
   return "measurementEccentricity";
}

int SSMeasurementEccentricity::Precision() const
{
   return 4;
}

double SSMeasurementEccentricity::DefaultValue() const
{
   return 0.5;
}

double SSMeasurementEccentricity::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementEccentricity::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

SSMeasurementSNRWeight::SSMeasurementSNRWeight( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementSNRWeightParameter = this;
}

IsoString SSMeasurementSNRWeight::Id() const
{
   return "measurementSNRWeight";
}

int SSMeasurementSNRWeight::Precision() const
{
   return 4;
}

double SSMeasurementSNRWeight::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementSNRWeight::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementSNRWeight::MaximumValue() const
{
   return DBL_MAX;
}

// ----------------------------------------------------------------------------

SSMeasurementMedian::SSMeasurementMedian( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementMedianParameter = this;
}

IsoString SSMeasurementMedian::Id() const
{
   return "measurementMedian";
}

int SSMeasurementMedian::Precision() const
{
   return 12;
}

double SSMeasurementMedian::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementMedian::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementMedian::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

SSMeasurementMedianMeanDev::SSMeasurementMedianMeanDev( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementMedianMeanDevParameter = this;
}

IsoString SSMeasurementMedianMeanDev::Id() const
{
   return "measurementMedianMeanDev";
}

int SSMeasurementMedianMeanDev::Precision() const
{
   return 12;
}

double SSMeasurementMedianMeanDev::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementMedianMeanDev::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementMedianMeanDev::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

SSMeasurementNoise::SSMeasurementNoise( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementNoiseParameter = this;
}

IsoString SSMeasurementNoise::Id() const
{
   return "measurementNoise";
}

int SSMeasurementNoise::Precision() const
{
   return 12;
}

double SSMeasurementNoise::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementNoise::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementNoise::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

SSMeasurementNoiseRatio::SSMeasurementNoiseRatio( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementNoiseRatioParameter = this;
}

IsoString SSMeasurementNoiseRatio::Id() const
{
   return "measurementNoiseRatio";
}

int SSMeasurementNoiseRatio::Precision() const
{
   return 8;
}

double SSMeasurementNoiseRatio::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementNoiseRatio::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementNoiseRatio::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

SSMeasurementStars::SSMeasurementStars( MetaTable* T ) : MetaUInt16( T )
{
   TheSSMeasurementStarsParameter = this;
}

IsoString SSMeasurementStars::Id() const
{
   return "measurementStars";
}

int SSMeasurementStars::Precision() const
{
   return 0;
}

double SSMeasurementStars::DefaultValue() const
{
   return 0;
}

double SSMeasurementStars::MinimumValue() const
{
   return 0;
}

double SSMeasurementStars::MaximumValue() const
{
   return UINT16_MAX;
}

// ----------------------------------------------------------------------------

SSMeasurementStarResidual::SSMeasurementStarResidual( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementStarResidualParameter = this;
}

IsoString SSMeasurementStarResidual::Id() const
{
   return "measurementStarResidual";
}

int SSMeasurementStarResidual::Precision() const
{
   return 4;
}

double SSMeasurementStarResidual::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementStarResidual::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementStarResidual::MaximumValue() const
{
   return DBL_MAX;
}

// ----------------------------------------------------------------------------

SSMeasurementFWHMMeanDev::SSMeasurementFWHMMeanDev( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementFWHMMeanDevParameter = this;
}

IsoString SSMeasurementFWHMMeanDev::Id() const
{
   return "measurementFWHMMeanDev";
}

int SSMeasurementFWHMMeanDev::Precision() const
{
   return 6;
}

double SSMeasurementFWHMMeanDev::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementFWHMMeanDev::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementFWHMMeanDev::MaximumValue() const
{
   return DBL_MAX;
}

// ----------------------------------------------------------------------------

SSMeasurementEccentricityMeanDev::SSMeasurementEccentricityMeanDev( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementEccentricityMeanDevParameter = this;
}

IsoString SSMeasurementEccentricityMeanDev::Id() const
{
   return "measurementEccentricityMeanDev";
}

int SSMeasurementEccentricityMeanDev::Precision() const
{
   return 4;
}

double SSMeasurementEccentricityMeanDev::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementEccentricityMeanDev::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementEccentricityMeanDev::MaximumValue() const
{
   return DBL_MAX;
}

// ----------------------------------------------------------------------------

SSMeasurementStarResidualMeanDev::SSMeasurementStarResidualMeanDev( MetaTable* T ) : MetaFloat( T )
{
   TheSSMeasurementStarResidualMeanDevParameter = this;
}

IsoString SSMeasurementStarResidualMeanDev::Id() const
{
   return "measurementStarResidualMeanDev";
}

int SSMeasurementStarResidualMeanDev::Precision() const
{
   return 4;
}

double SSMeasurementStarResidualMeanDev::DefaultValue() const
{
   return 0.0;
}

double SSMeasurementStarResidualMeanDev::MinimumValue() const
{
   return 0.0;
}

double SSMeasurementStarResidualMeanDev::MaximumValue() const
{
   return DBL_MAX;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorParameters.cpp - Released 2017-08-01T14:26:58Z
