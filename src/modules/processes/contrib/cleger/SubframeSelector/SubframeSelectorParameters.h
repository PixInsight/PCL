//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.01.0012
// ----------------------------------------------------------------------------
// SubframeSelectorParameters.h - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2018 Cameron Leger
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

#ifndef __SubframeSelectorParameters_h
#define __SubframeSelectorParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SSRoutine : public MetaEnumeration
{
public:

   enum { MeasureSubframes,
          OutputSubframes,
          StarDetectionPreview,
          NumberOfItems,
          Default = MeasureSubframes };

   SSRoutine( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
   int ElementData( size_type ) const;
   IsoString Tooltip() const;
};

extern SSRoutine* TheSSRoutineParameter;

// ----------------------------------------------------------------------------

class SSSubframes : public MetaTable
{
public:

   SSSubframes( MetaProcess* );

   IsoString Id() const override;
};

extern SSSubframes* TheSSSubframesParameter;

// ----------------------------------------------------------------------------

class SSSubframeEnabled : public MetaBoolean
{
public:

   SSSubframeEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SSSubframeEnabled* TheSSSubframeEnabledParameter;

// ----------------------------------------------------------------------------

class SSSubframePath : public MetaString
{
public:

   SSSubframePath( MetaTable* );

   IsoString Id() const override;
};

extern SSSubframePath* TheSSSubframePathParameter;

// ----------------------------------------------------------------------------

class SSFileCache : public MetaBoolean
{
public:

   SSFileCache( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSFileCache* TheSSFileCacheParameter;

// ----------------------------------------------------------------------------

class SSSubframeScale : public MetaFloat
{
public:

   SSSubframeScale( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSSubframeScale* TheSSSubframeScaleParameter;

// ----------------------------------------------------------------------------

class SSCameraGain : public MetaFloat
{
public:

   SSCameraGain( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSCameraGain* TheSSCameraGainParameter;

// ----------------------------------------------------------------------------

class SSCameraResolution : public MetaEnumeration
{
public:

   enum { Bits8,
          Bits10,
          Bits12,
          Bits14,
          Bits16,
          NumberOfItems,
          Default = Bits8 };

   SSCameraResolution( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
   int ElementData( size_type ) const;
   IsoString Tooltip() const;
};

extern SSCameraResolution* TheSSCameraResolutionParameter;

// ----------------------------------------------------------------------------

class SSSiteLocalMidnight : public MetaInt32
{
public:

   SSSiteLocalMidnight( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSSiteLocalMidnight* TheSSSiteLocalMidnightParameter;

// ----------------------------------------------------------------------------

class SSScaleUnit : public MetaEnumeration
{
public:

   enum { ArcSeconds,
          Pixel,
          NumberOfItems,
          Default = ArcSeconds };

   SSScaleUnit( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
   IsoString Tooltip() const;
};

extern SSScaleUnit* TheSSScaleUnitParameter;

// ----------------------------------------------------------------------------

class SSDataUnit : public MetaEnumeration
{
public:

   enum { Electron,
          DataNumber,
          NumberOfItems,
          Default = Electron };

   SSDataUnit( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
   IsoString Tooltip() const;
};

extern SSDataUnit* TheSSDataUnitParameter;

// ----------------------------------------------------------------------------

class SSStructureLayers : public MetaInt32
{
public:

   SSStructureLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSStructureLayers* TheSSStructureLayersParameter;

// ----------------------------------------------------------------------------

class SSNoiseLayers : public MetaInt32
{
public:

   SSNoiseLayers( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSNoiseLayers* TheSSNoiseLayersParameter;

// ----------------------------------------------------------------------------

class SSHotPixelFilterRadius : public MetaInt32
{
public:

   SSHotPixelFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSHotPixelFilterRadius* TheSSHotPixelFilterRadiusParameter;

// ----------------------------------------------------------------------------

class SSApplyHotPixelFilter : public MetaBoolean
{
public:

   SSApplyHotPixelFilter( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSApplyHotPixelFilter* TheSSApplyHotPixelFilterParameter;

// ----------------------------------------------------------------------------

class SSNoiseReductionFilterRadius : public MetaInt32
{
public:

   SSNoiseReductionFilterRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSNoiseReductionFilterRadius* TheSSNoiseReductionFilterRadiusParameter;

// ----------------------------------------------------------------------------

class SSSensitivity : public MetaFloat
{
public:

   SSSensitivity( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSSensitivity* TheSSSensitivityParameter;

// ----------------------------------------------------------------------------

class SSPeakResponse : public MetaFloat
{
public:

   SSPeakResponse( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSPeakResponse* TheSSPeakResponseParameter;

// ----------------------------------------------------------------------------

class SSMaxDistortion : public MetaFloat
{
public:

   SSMaxDistortion( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSMaxDistortion* TheSSMaxDistortionParameter;

// ----------------------------------------------------------------------------

class SSUpperLimit : public MetaFloat
{
public:

   SSUpperLimit( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSUpperLimit* TheSSUpperLimitParameter;

// ----------------------------------------------------------------------------

class SSBackgroundExpansion : public MetaInt32
{
public:

   SSBackgroundExpansion( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSBackgroundExpansion* TheSSBackgroundExpansionParameter;

// ----------------------------------------------------------------------------

class SSXYStretch : public MetaFloat
{
public:

   SSXYStretch( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSXYStretch* TheSSXYStretchParameter;

// ----------------------------------------------------------------------------

class SSPSFFit : public MetaEnumeration
{
public:

   enum { Gaussian,
          Moffat10,
          Moffat8,
          Moffat6,
          Moffat4,
          Moffat25,
          Moffat15,
          Lorentzian,
          NumberOfItems,
          Default = Gaussian };

   SSPSFFit( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
   IsoString Tooltip() const;
};

extern SSPSFFit* TheSSPSFFitParameter;

// ----------------------------------------------------------------------------

class SSPSFFitCircular : public MetaBoolean
{
public:

   SSPSFFitCircular( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSPSFFitCircular* TheSSPSFFitCircularParameter;

// ----------------------------------------------------------------------------

class SSPedestal : public MetaUInt16
{
public:

   SSPedestal( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   IsoString Tooltip() const;
};

extern SSPedestal* TheSSPedestalParameter;

// ----------------------------------------------------------------------------

class SSROIX0 : public MetaInt32
{
public:

   SSROIX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSROIX0* TheSSROIX0Parameter;


// ----------------------------------------------------------------------------

class SSROIY0 : public MetaInt32
{
public:

   SSROIY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSROIY0* TheSSROIY0Parameter;

// ----------------------------------------------------------------------------

class SSROIX1 : public MetaInt32
{
public:

   SSROIX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSROIX1* TheSSROIX1Parameter;

// ----------------------------------------------------------------------------

class SSROIY1 : public MetaInt32
{
public:

   SSROIY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSROIY1* TheSSROIY1Parameter;

// ----------------------------------------------------------------------------

class SSInputHints : public MetaString
{
public:

   SSInputHints( MetaProcess* );

   IsoString Id() const override;

   IsoString Tooltip() const;
};

extern SSInputHints* TheSSInputHintsParameter;

// ----------------------------------------------------------------------------

class SSOutputHints : public MetaString
{
public:

   SSOutputHints( MetaProcess* );

   IsoString Id() const override;
};

extern SSOutputHints* TheSSOutputHintsParameter;

// ----------------------------------------------------------------------------

class SSOutputDirectory : public MetaString
{
public:

   SSOutputDirectory( MetaProcess* );

   IsoString Id() const override;

   IsoString Tooltip() const;
};

extern SSOutputDirectory* TheSSOutputDirectoryParameter;

// ----------------------------------------------------------------------------

class SSOutputExtension : public MetaString
{
public:

   SSOutputExtension( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern SSOutputExtension* TheSSOutputExtensionParameter;

// ----------------------------------------------------------------------------

class SSOutputPrefix : public MetaString
{
public:

   SSOutputPrefix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSOutputPrefix* TheSSOutputPrefixParameter;

// ----------------------------------------------------------------------------

class SSOutputPostfix : public MetaString
{
public:

   SSOutputPostfix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSOutputPostfix* TheSSOutputPostfixParameter;

// ----------------------------------------------------------------------------

class SSOutputKeyword : public MetaString
{
public:

   SSOutputKeyword( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSOutputKeyword* TheSSOutputKeywordParameter;

// ----------------------------------------------------------------------------

class SSOverwriteExistingFiles : public MetaBoolean
{
public:

   SSOverwriteExistingFiles( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;

   IsoString Tooltip() const;
};

extern SSOverwriteExistingFiles* TheSSOverwriteExistingFilesParameter;

// ----------------------------------------------------------------------------

class SSOnError : public MetaEnumeration
{
public:

   enum { Continue,
          Abort,
          AskUser,
          NumberOfItems,
          Default = Continue };

   SSOnError( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
   IsoString Tooltip() const;
};

extern SSOnError* TheSSOnErrorParameter;

// ----------------------------------------------------------------------------

class SSApprovalExpression : public MetaString
{
public:

   SSApprovalExpression( MetaProcess* );

   IsoString Id() const override;

   IsoString Tooltip() const;
};

extern SSApprovalExpression* TheSSApprovalExpressionParameter;

// ----------------------------------------------------------------------------

class SSWeightingExpression : public MetaString
{
public:

   SSWeightingExpression( MetaProcess* );

   IsoString Id() const override;

   IsoString Tooltip() const;
};

extern SSWeightingExpression* TheSSWeightingExpressionParameter;

// ----------------------------------------------------------------------------

class SSSortingProperty : public MetaEnumeration
{
public:

   enum { Index,
          Weight,
          FWHM,
          Eccentricity,
          SNRWeight,
          Median,
          MedianMeanDev,
          Noise,
          NoiseRatio,
          Stars,
          StarResidual,
          FWHMMeanDev,
          EccentricityMeanDev,
          StarResidualMeanDev,
          NumberOfItems,
          Default = Index };

   SSSortingProperty( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
};

extern SSSortingProperty* TheSSSortingPropertyParameter;

// ----------------------------------------------------------------------------

class SSGraphProperty : public MetaEnumeration
{
public:

   enum { Weight,
          FWHM,
          Eccentricity,
          SNRWeight,
          Median,
          MedianMeanDev,
          Noise,
          NoiseRatio,
          Stars,
          StarResidual,
          FWHMMeanDev,
          EccentricityMeanDev,
          StarResidualMeanDev,
          NumberOfItems,
          Default = FWHM };

   SSGraphProperty( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   IsoString ElementLabel( size_type ) const;
};

extern SSGraphProperty* TheSSGraphPropertyParameter;

// ----------------------------------------------------------------------------

class SSMeasurements : public MetaTable
{
public:

   SSMeasurements( MetaProcess* );

   IsoString Id() const override;
};

extern SSMeasurements* TheSSMeasurementsParameter;

// ----------------------------------------------------------------------------

class SSMeasurementIndex : public MetaUInt16
{
public:

   SSMeasurementIndex( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementIndex* TheSSMeasurementIndexParameter;

// ----------------------------------------------------------------------------

class SSMeasurementEnabled : public MetaBoolean
{
public:

   SSMeasurementEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SSMeasurementEnabled* TheSSMeasurementEnabledParameter;

// ----------------------------------------------------------------------------

class SSMeasurementLocked : public MetaBoolean
{
public:

   SSMeasurementLocked( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SSMeasurementLocked* TheSSMeasurementLockedParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPath : public MetaString
{
public:

   SSMeasurementPath( MetaTable* );

   IsoString Id() const override;
};

extern SSMeasurementPath* TheSSMeasurementPathParameter;

// ----------------------------------------------------------------------------

class SSMeasurementWeight : public MetaFloat
{
public:

   SSMeasurementWeight( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementWeight* TheSSMeasurementWeightParameter;

// ----------------------------------------------------------------------------

class SSMeasurementFWHM : public MetaFloat
{
public:

   SSMeasurementFWHM( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementFWHM* TheSSMeasurementFWHMParameter;

// ----------------------------------------------------------------------------

class SSMeasurementEccentricity : public MetaFloat
{
public:

   SSMeasurementEccentricity( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementEccentricity* TheSSMeasurementEccentricityParameter;

// ----------------------------------------------------------------------------

class SSMeasurementSNRWeight : public MetaFloat
{
public:

   SSMeasurementSNRWeight( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementSNRWeight* TheSSMeasurementSNRWeightParameter;

// ----------------------------------------------------------------------------

class SSMeasurementMedian : public MetaFloat
{
public:

   SSMeasurementMedian( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementMedian* TheSSMeasurementMedianParameter;

// ----------------------------------------------------------------------------

class SSMeasurementMedianMeanDev : public MetaFloat
{
public:

   SSMeasurementMedianMeanDev( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementMedianMeanDev* TheSSMeasurementMedianMeanDevParameter;

// ----------------------------------------------------------------------------

class SSMeasurementNoise : public MetaFloat
{
public:

   SSMeasurementNoise( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementNoise* TheSSMeasurementNoiseParameter;

// ----------------------------------------------------------------------------

class SSMeasurementNoiseRatio : public MetaFloat
{
public:

   SSMeasurementNoiseRatio( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementNoiseRatio* TheSSMeasurementNoiseRatioParameter;

// ----------------------------------------------------------------------------

class SSMeasurementStars : public MetaUInt16
{
public:

   SSMeasurementStars( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementStars* TheSSMeasurementStarsParameter;

// ----------------------------------------------------------------------------

class SSMeasurementStarResidual : public MetaFloat
{
public:

   SSMeasurementStarResidual( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementStarResidual* TheSSMeasurementStarResidualParameter;

// ----------------------------------------------------------------------------

class SSMeasurementFWHMMeanDev : public MetaFloat
{
public:

   SSMeasurementFWHMMeanDev( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementFWHMMeanDev* TheSSMeasurementFWHMMeanDevParameter;

// ----------------------------------------------------------------------------

class SSMeasurementEccentricityMeanDev : public MetaFloat
{
public:

   SSMeasurementEccentricityMeanDev( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementEccentricityMeanDev* TheSSMeasurementEccentricityMeanDevParameter;

// ----------------------------------------------------------------------------

class SSMeasurementStarResidualMeanDev : public MetaFloat
{
public:

   SSMeasurementStarResidualMeanDev( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SSMeasurementStarResidualMeanDev* TheSSMeasurementStarResidualMeanDevParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __SubframeSelectorParameters_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorParameters.h - Released 2018-11-23T18:45:58Z
