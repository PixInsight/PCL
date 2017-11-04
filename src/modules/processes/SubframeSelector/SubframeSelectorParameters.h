//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorParameters.h - Released 2017-08-01T14:26:58Z
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
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR STargetFERVICES; AND LOSS OF USE,
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

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
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

   virtual IsoString Id() const;
};

extern SSSubframes* TheSSSubframesParameter;

// ----------------------------------------------------------------------------

class SSSubframeEnabled : public MetaBoolean
{
public:

   SSSubframeEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern SSSubframeEnabled* TheSSSubframeEnabledParameter;

// ----------------------------------------------------------------------------

class SSSubframePath : public MetaString
{
public:

   SSSubframePath( MetaTable* );

   virtual IsoString Id() const;
};

extern SSSubframePath* TheSSSubframePathParameter;

// ----------------------------------------------------------------------------

class SSSubframeScale : public MetaFloat
{
public:

   SSSubframeScale( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   IsoString Tooltip() const;
};

extern SSSubframeScale* TheSSSubframeScaleParameter;

// ----------------------------------------------------------------------------

class SSCameraGain : public MetaFloat
{
public:

   SSCameraGain( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

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

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
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

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

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

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
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

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
   IsoString ElementLabel( size_type ) const;

   IsoString Tooltip() const;
};

extern SSDataUnit* TheSSDataUnitParameter;

// ----------------------------------------------------------------------------

class SSStructureLayers : public MetaInt32
{
   public:

   SSStructureLayers( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   IsoString Tooltip() const;
};

extern SSStructureLayers* TheSSStructureLayersParameter;

// ----------------------------------------------------------------------------

class SSNoiseLayers : public MetaInt32
{
   public:

   SSNoiseLayers( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   IsoString Tooltip() const;
};

extern SSNoiseLayers* TheSSNoiseLayersParameter;

// ----------------------------------------------------------------------------

class SSHotPixelFilterRadius : public MetaInt32
{
   public:

   SSHotPixelFilterRadius( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   IsoString Tooltip() const;
};

extern SSHotPixelFilterRadius* TheSSHotPixelFilterRadiusParameter;

// ----------------------------------------------------------------------------

class SSApplyHotPixelFilter : public MetaBoolean
{
   public:

   SSApplyHotPixelFilter( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;

   IsoString Tooltip() const;
};

extern SSApplyHotPixelFilter* TheSSApplyHotPixelFilterParameter;

// ----------------------------------------------------------------------------

class SSNoiseReductionFilterRadius : public MetaInt32
{
   public:

   SSNoiseReductionFilterRadius( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   IsoString Tooltip() const;
};

extern SSNoiseReductionFilterRadius* TheSSNoiseReductionFilterRadiusParameter;

// ----------------------------------------------------------------------------

class SSSensitivity : public MetaFloat
{
   public:

   SSSensitivity( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   IsoString Tooltip() const;
};

extern SSSensitivity* TheSSSensitivityParameter;

// ----------------------------------------------------------------------------

class SSPeakResponse : public MetaFloat
{
   public:

   SSPeakResponse( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   IsoString Tooltip() const;
};

extern SSPeakResponse* TheSSPeakResponseParameter;

// ----------------------------------------------------------------------------

class SSMaxDistortion : public MetaFloat
{
   public:

   SSMaxDistortion( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   IsoString Tooltip() const;
};

extern SSMaxDistortion* TheSSMaxDistortionParameter;

// ----------------------------------------------------------------------------

class SSUpperLimit : public MetaFloat
{
   public:

   SSUpperLimit( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   IsoString Tooltip() const;
};

extern SSUpperLimit* TheSSUpperLimitParameter;

// ----------------------------------------------------------------------------

class SSBackgroundExpansion : public MetaInt32
{
   public:

   SSBackgroundExpansion( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   IsoString Tooltip() const;
};

extern SSBackgroundExpansion* TheSSBackgroundExpansionParameter;

// ----------------------------------------------------------------------------

class SSXYStretch : public MetaFloat
{
   public:

   SSXYStretch( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

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

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
   IsoString ElementLabel( size_type ) const;

   IsoString Tooltip() const;
};

extern SSPSFFit* TheSSPSFFitParameter;

// ----------------------------------------------------------------------------

class SSPSFFitCircular : public MetaBoolean
{
public:

   SSPSFFitCircular( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;

   IsoString Tooltip() const;
};

extern SSPSFFitCircular* TheSSPSFFitCircularParameter;

// ----------------------------------------------------------------------------

class SSPedestal : public MetaUInt16
{
public:

   SSPedestal( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;

   IsoString Tooltip() const;
};

extern SSPedestal* TheSSPedestalParameter;

// ----------------------------------------------------------------------------

class SSROIX0 : public MetaInt32
{
   public:

   SSROIX0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSROIX0* TheSSROIX0Parameter;


// ----------------------------------------------------------------------------

class SSROIY0 : public MetaInt32
{
   public:

   SSROIY0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSROIY0* TheSSROIY0Parameter;

// ----------------------------------------------------------------------------

class SSROIX1 : public MetaInt32
{
   public:

   SSROIX1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSROIX1* TheSSROIX1Parameter;

// ----------------------------------------------------------------------------

class SSROIY1 : public MetaInt32
{
   public:

   SSROIY1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSROIY1* TheSSROIY1Parameter;

// ----------------------------------------------------------------------------

class SSOutputDirectory : public MetaString
{
public:

   SSOutputDirectory( MetaProcess* );

   virtual IsoString Id() const;

   IsoString Tooltip() const;
};

extern SSOutputDirectory* TheSSOutputDirectoryParameter;

// ----------------------------------------------------------------------------

class SSOutputPrefix : public MetaString
{
public:

   SSOutputPrefix( MetaProcess* );

   virtual IsoString Id() const;
   virtual String DefaultValue() const;

   IsoString Tooltip() const;
};

extern SSOutputPrefix* TheSSOutputPrefixParameter;

// ----------------------------------------------------------------------------

class SSOutputPostfix : public MetaString
{
public:

   SSOutputPostfix( MetaProcess* );

   virtual IsoString Id() const;
   virtual String DefaultValue() const;

   IsoString Tooltip() const;
};

extern SSOutputPostfix* TheSSOutputPostfixParameter;

// ----------------------------------------------------------------------------

class SSOutputKeyword : public MetaString
{
public:

   SSOutputKeyword( MetaProcess* );

   virtual IsoString Id() const;
   virtual String DefaultValue() const;

   IsoString Tooltip() const;
};

extern SSOutputKeyword* TheSSOutputKeywordParameter;

// ----------------------------------------------------------------------------

class SSOverwriteExistingFiles : public MetaBoolean
{
public:

   SSOverwriteExistingFiles( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;

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

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   IsoString ElementLabel( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;

   IsoString Tooltip() const;
};

extern SSOnError* TheSSOnErrorParameter;

// ----------------------------------------------------------------------------

class SSApprovalExpression : public MetaString
{
   public:

   SSApprovalExpression( MetaProcess* );

   virtual IsoString Id() const;

   IsoString Tooltip() const;
};

extern SSApprovalExpression* TheSSApprovalExpressionParameter;

// ----------------------------------------------------------------------------

class SSWeightingExpression : public MetaString
{
   public:

   SSWeightingExpression( MetaProcess* );

   virtual IsoString Id() const;

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

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
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

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
   IsoString ElementLabel( size_type ) const;
};

extern SSGraphProperty* TheSSGraphPropertyParameter;

// ----------------------------------------------------------------------------

class SSMeasurements : public MetaTable
{
   public:

   SSMeasurements( MetaProcess* );

   virtual IsoString Id() const;
};

extern SSMeasurements* TheSSMeasurementsParameter;

// ----------------------------------------------------------------------------

class SSMeasurementIndex : public MetaUInt16
{
   public:

   SSMeasurementIndex( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementIndex* TheSSMeasurementIndexParameter;

// ----------------------------------------------------------------------------

class SSMeasurementEnabled : public MetaBoolean
{
   public:

   SSMeasurementEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern SSMeasurementEnabled* TheSSMeasurementEnabledParameter;

// ----------------------------------------------------------------------------

class SSMeasurementLocked : public MetaBoolean
{
   public:

   SSMeasurementLocked( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern SSMeasurementLocked* TheSSMeasurementLockedParameter;

// ----------------------------------------------------------------------------

class SSMeasurementPath : public MetaString
{
   public:

   SSMeasurementPath( MetaTable* );

   virtual IsoString Id() const;
};

extern SSMeasurementPath* TheSSMeasurementPathParameter;

// ----------------------------------------------------------------------------

class SSMeasurementWeight : public MetaFloat
{
   public:

   SSMeasurementWeight( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementWeight* TheSSMeasurementWeightParameter;

// ----------------------------------------------------------------------------

class SSMeasurementFWHM : public MetaFloat
{
   public:

   SSMeasurementFWHM( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementFWHM* TheSSMeasurementFWHMParameter;

// ----------------------------------------------------------------------------

class SSMeasurementEccentricity : public MetaFloat
{
public:

   SSMeasurementEccentricity( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementEccentricity* TheSSMeasurementEccentricityParameter;

// ----------------------------------------------------------------------------

class SSMeasurementSNRWeight : public MetaFloat
{
public:

   SSMeasurementSNRWeight( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementSNRWeight* TheSSMeasurementSNRWeightParameter;

// ----------------------------------------------------------------------------

class SSMeasurementMedian : public MetaFloat
{
public:

   SSMeasurementMedian( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementMedian* TheSSMeasurementMedianParameter;

// ----------------------------------------------------------------------------

class SSMeasurementMedianMeanDev : public MetaFloat
{
public:

   SSMeasurementMedianMeanDev( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementMedianMeanDev* TheSSMeasurementMedianMeanDevParameter;

// ----------------------------------------------------------------------------

class SSMeasurementNoise : public MetaFloat
{
public:

   SSMeasurementNoise( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementNoise* TheSSMeasurementNoiseParameter;

// ----------------------------------------------------------------------------

class SSMeasurementNoiseRatio : public MetaFloat
{
public:

   SSMeasurementNoiseRatio( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementNoiseRatio* TheSSMeasurementNoiseRatioParameter;

// ----------------------------------------------------------------------------

class SSMeasurementStars : public MetaUInt16
{
public:

   SSMeasurementStars( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementStars* TheSSMeasurementStarsParameter;

// ----------------------------------------------------------------------------

class SSMeasurementStarResidual : public MetaFloat
{
public:

   SSMeasurementStarResidual( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementStarResidual* TheSSMeasurementStarResidualParameter;

// ----------------------------------------------------------------------------

class SSMeasurementFWHMMeanDev : public MetaFloat
{
public:

   SSMeasurementFWHMMeanDev( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementFWHMMeanDev* TheSSMeasurementFWHMMeanDevParameter;

// ----------------------------------------------------------------------------

class SSMeasurementEccentricityMeanDev : public MetaFloat
{
public:

   SSMeasurementEccentricityMeanDev( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementEccentricityMeanDev* TheSSMeasurementEccentricityMeanDevParameter;

// ----------------------------------------------------------------------------

class SSMeasurementStarResidualMeanDev : public MetaFloat
{
public:

   SSMeasurementStarResidualMeanDev( MetaTable* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SSMeasurementStarResidualMeanDev* TheSSMeasurementStarResidualMeanDevParameter;

// ----------------------------------------------------------------------------


   PCL_END_LOCAL

} // pcl

#endif   // __SubframeSelectorParameters_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorParameters.h - Released 2017-08-01T14:26:58Z
