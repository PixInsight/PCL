//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.15.00.0398
// ----------------------------------------------------------------------------
// ImageIntegrationParameters.h - Released 2017-05-05T08:37:32Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#ifndef __ImageIntegrationParameters_h
#define __ImageIntegrationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class IIImages : public MetaTable
{
public:

   IIImages( MetaProcess* );

   virtual IsoString Id() const;
};

extern IIImages* TheIIImagesParameter;

// ----------------------------------------------------------------------------

class IIImageEnabled : public MetaBoolean
{
public:

   IIImageEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIImageEnabled* TheIIImageEnabledParameter;

// ----------------------------------------------------------------------------

class IIImagePath : public MetaString
{
public:

   IIImagePath( MetaTable* );

   virtual IsoString Id() const;
};

extern IIImagePath* TheIIImagePathParameter;

// ----------------------------------------------------------------------------

class IIDrizzlePath : public MetaString
{
public:

   IIDrizzlePath( MetaTable* );

   virtual IsoString Id() const;
};

extern IIDrizzlePath* TheIIDrizzlePathParameter;

// ----------------------------------------------------------------------------

class IIInputHints : public MetaString
{
public:

   IIInputHints( MetaProcess* );

   virtual IsoString Id() const;
};

extern IIInputHints* TheIIInputHintsParameter;

// ----------------------------------------------------------------------------

class IICombination : public MetaEnumeration
{
public:

   enum { Average,
          Median,
          Minimum,
          Maximum,
          NumberOfCombinationOperations,
          Default = Average };

   IICombination( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern IICombination* TheIICombinationParameter;

// ----------------------------------------------------------------------------

class IIWeightMode : public MetaEnumeration
{
public:

   enum { DontCare,              // weight = 1
          ExposureTimeWeight,    // EXPTIME keyword or ImageOptions::exposure
          NoiseEvaluationWeight, // MRS Gaussian noise estimates
          SignalWeight,          // mean - median distance
          MedianWeight,          // median
          AverageWeight,         // mean
          KeywordWeight,         // user-specified keyword
          NumberOfWeightModes,
          Default = NoiseEvaluationWeight };

   IIWeightMode( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern IIWeightMode* TheIIWeightModeParameter;

// ----------------------------------------------------------------------------

class IIWeightKeyword : public MetaString
{
public:

   IIWeightKeyword( MetaProcess* );

   virtual IsoString Id() const;
};

extern IIWeightKeyword* TheIIWeightKeywordParameter;

// ----------------------------------------------------------------------------

class IIWeightScale : public MetaEnumeration
{
public:

   enum { AvgDev,    // average deviation from the median
          MAD,       // median deviation from the median (MAD)
          BWMV,      // biweight midvariance
          PBMV,      // percentage bend midvariance
          Sn,        // Rousseeuw/Croux Sn estimator of scale
          Qn,        // Rousseeuw/Croux Qn estimator of scale
          IKSS,      // iterative k-sigma estimator of scale
          NumberOfWeightScales,
          Default = IKSS };

   IIWeightScale( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern IIWeightScale* TheIIWeightScaleParameter;

// ----------------------------------------------------------------------------

class IIIgnoreNoiseKeywords : public MetaBoolean
{
public:

   IIIgnoreNoiseKeywords( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIIgnoreNoiseKeywords* TheIIIgnoreNoiseKeywordsParameter;

// ----------------------------------------------------------------------------

class IINormalization : public MetaEnumeration
{
public:

   enum { NoNormalization,
          Additive,
          Multiplicative,
          AdditiveWithScaling,
          MultiplicativeWithScaling,
          NumberOfNormalizationAlgorithms,
          Default = AdditiveWithScaling };

   IINormalization( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern IINormalization* TheIINormalizationParameter;

// ----------------------------------------------------------------------------

class IIRejection : public MetaEnumeration
{
public:

   enum { NoRejection,
          MinMax,
          PercentileClip,
          SigmaClip,
          WinsorizedSigmaClip,
          AveragedSigmaClip,
          LinearFit,
          CCDClip,
          NumberOfRejectionAlgorithms,
          Default = NoRejection };

   IIRejection( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern IIRejection* TheIIRejectionParameter;

// ----------------------------------------------------------------------------

class IIRejectionNormalization : public MetaEnumeration
{
public:

   enum { NoRejectionNormalization,
          Scale,
          EqualizeFluxes,
          NumberOfRejectionNormalizationAlgorithms,
          Default = Scale };

   IIRejectionNormalization( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern IIRejectionNormalization* TheIIRejectionNormalizationParameter;

// ----------------------------------------------------------------------------

class IIMinMaxLow : public MetaInt32
{
public:

   IIMinMaxLow( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern IIMinMaxLow* TheIIMinMaxLowParameter;

// ----------------------------------------------------------------------------

class IIMinMaxHigh : public MetaInt32
{
public:

   IIMinMaxHigh( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern IIMinMaxHigh* TheIIMinMaxHighParameter;

// ----------------------------------------------------------------------------

class IIPercentileLow : public MetaFloat
{
public:

   IIPercentileLow( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IIPercentileLow* TheIIPercentileLowParameter;

// ----------------------------------------------------------------------------

class IIPercentileHigh : public MetaFloat
{
public:

   IIPercentileHigh( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IIPercentileHigh* TheIIPercentileHighParameter;

// ----------------------------------------------------------------------------

class IISigmaLow : public MetaFloat
{
public:

   IISigmaLow( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IISigmaLow* TheIISigmaLowParameter;

// ----------------------------------------------------------------------------

class IISigmaHigh : public MetaFloat
{
public:

   IISigmaHigh( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IISigmaHigh* TheIISigmaHighParameter;

// ----------------------------------------------------------------------------

class IILinearFitLow : public MetaFloat
{
public:

   IILinearFitLow( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IILinearFitLow* TheIILinearFitLowParameter;

// ----------------------------------------------------------------------------

class IILinearFitHigh : public MetaFloat
{
public:

   IILinearFitHigh( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IILinearFitHigh* TheIILinearFitHighParameter;

// ----------------------------------------------------------------------------

class IICCDGain : public MetaFloat
{
public:

   IICCDGain( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IICCDGain* TheIICCDGainParameter;

// ----------------------------------------------------------------------------

class IICCDReadNoise : public MetaFloat
{
public:

   IICCDReadNoise( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IICCDReadNoise* TheIICCDReadNoiseParameter;

// ----------------------------------------------------------------------------

class IICCDScaleNoise : public MetaFloat
{
public:

   IICCDScaleNoise( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IICCDScaleNoise* TheIICCDScaleNoiseParameter;

// ----------------------------------------------------------------------------

class IIClipLow : public MetaBoolean
{
public:

   IIClipLow( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIClipLow* TheIIClipLowParameter;

// ----------------------------------------------------------------------------

class IIClipHigh : public MetaBoolean
{
public:

   IIClipHigh( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIClipHigh* TheIIClipHighParameter;

// ----------------------------------------------------------------------------

class IIRangeClipLow : public MetaBoolean
{
public:

   IIRangeClipLow( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIRangeClipLow* TheIIRangeClipLowParameter;

// ----------------------------------------------------------------------------

class IIRangeLow : public MetaFloat
{
public:

   IIRangeLow( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IIRangeLow* TheIIRangeLowParameter;

// ----------------------------------------------------------------------------

class IIRangeClipHigh : public MetaBoolean
{
public:

   IIRangeClipHigh( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIRangeClipHigh* TheIIRangeClipHighParameter;

// ----------------------------------------------------------------------------

class IIRangeHigh : public MetaFloat
{
public:

   IIRangeHigh( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IIRangeHigh* TheIIRangeHighParameter;

// ----------------------------------------------------------------------------

class IIMapRangeRejection : public MetaBoolean
{
public:

   IIMapRangeRejection( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIMapRangeRejection* TheIIMapRangeRejectionParameter;

// ----------------------------------------------------------------------------

class IIReportRangeRejection : public MetaBoolean
{
public:

   IIReportRangeRejection( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIReportRangeRejection* TheIIReportRangeRejectionParameter;

// ----------------------------------------------------------------------------

class IILargeScaleClipLow : public MetaBoolean
{
public:

   IILargeScaleClipLow( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IILargeScaleClipLow* TheIILargeScaleClipLowParameter;

// ----------------------------------------------------------------------------

class IILargeScaleClipLowProtectedLayers : public MetaInt32
{
public:

   IILargeScaleClipLowProtectedLayers( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IILargeScaleClipLowProtectedLayers* TheIILargeScaleClipLowProtectedLayersParameter;

// ----------------------------------------------------------------------------

class IILargeScaleClipLowGrowth : public MetaInt32
{
public:

   IILargeScaleClipLowGrowth( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IILargeScaleClipLowGrowth* TheIILargeScaleClipLowGrowthParameter;

// ----------------------------------------------------------------------------

class IILargeScaleClipHigh : public MetaBoolean
{
public:

   IILargeScaleClipHigh( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IILargeScaleClipHigh* TheIILargeScaleClipHighParameter;

// ----------------------------------------------------------------------------

class IILargeScaleClipHighProtectedLayers : public MetaInt32
{
public:

   IILargeScaleClipHighProtectedLayers( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IILargeScaleClipHighProtectedLayers* TheIILargeScaleClipHighProtectedLayersParameter;

// ----------------------------------------------------------------------------

class IILargeScaleClipHighGrowth : public MetaInt32
{
public:

   IILargeScaleClipHighGrowth( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IILargeScaleClipHighGrowth* TheIILargeScaleClipHighGrowthParameter;

// ----------------------------------------------------------------------------

class IIGenerate64BitResult : public MetaBoolean
{
public:

   IIGenerate64BitResult( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIGenerate64BitResult* TheIIGenerate64BitResultParameter;

// ----------------------------------------------------------------------------

class IIGenerateRejectionMaps : public MetaBoolean
{
public:

   IIGenerateRejectionMaps( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIGenerateRejectionMaps* TheIIGenerateRejectionMapsParameter;

// ----------------------------------------------------------------------------

class IIGenerateIntegratedImage : public MetaBoolean
{
public:

   IIGenerateIntegratedImage( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIGenerateIntegratedImage* TheIIGenerateIntegratedImageParameter;

// ----------------------------------------------------------------------------

class IIGenerateDrizzleData : public MetaBoolean
{
public:

   IIGenerateDrizzleData( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIGenerateDrizzleData* TheIIGenerateDrizzleDataParameter;

// ----------------------------------------------------------------------------

class IIClosePreviousImages : public MetaBoolean
{
public:

   IIClosePreviousImages( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIClosePreviousImages* TheIIClosePreviousImagesParameter;

// ----------------------------------------------------------------------------

class IIBufferSize : public MetaInt32
{
public:

   IIBufferSize( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IIBufferSize* TheIIBufferSizeParameter;

// ----------------------------------------------------------------------------

class IIStackSize : public MetaInt32
{
public:

   IIStackSize( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IIStackSize* TheIIStackSizeParameter;

// ----------------------------------------------------------------------------

class IIUseROI : public MetaBoolean
{
public:

   IIUseROI( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIUseROI* TheIIUseROIParameter;

// ----------------------------------------------------------------------------

class IIROIX0 : public MetaInt32
{
public:

   IIROIX0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IIROIX0* TheIIROIX0Parameter;


// ----------------------------------------------------------------------------

class IIROIY0 : public MetaInt32
{
public:

   IIROIY0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IIROIY0* TheIIROIY0Parameter;

// ----------------------------------------------------------------------------

class IIROIX1 : public MetaInt32
{
public:

   IIROIX1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IIROIX1* TheIIROIX1Parameter;

// ----------------------------------------------------------------------------

class IIROIY1 : public MetaInt32
{
public:

   IIROIY1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IIROIY1* TheIIROIY1Parameter;

// ----------------------------------------------------------------------------

class IIUseCache : public MetaBoolean
{
public:

   IIUseCache( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIUseCache* TheIIUseCacheParameter;

// ----------------------------------------------------------------------------

class IIEvaluateNoise : public MetaBoolean
{
public:

   IIEvaluateNoise( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIEvaluateNoise* TheIIEvaluateNoiseParameter;

// ----------------------------------------------------------------------------

class IIMRSMinDataFraction : public MetaFloat
{
public:

   IIMRSMinDataFraction( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IIMRSMinDataFraction* TheIIMRSMinDataFractionParameter;

// ----------------------------------------------------------------------------

class IINoGUIMessages : public MetaBoolean
{
public:

   IINoGUIMessages( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IINoGUIMessages* TheIINoGUIMessagesParameter;

// ----------------------------------------------------------------------------

class IIUseFileThreads : public MetaBoolean
{
public:

   IIUseFileThreads( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern IIUseFileThreads* TheIIUseFileThreadsParameter;

// ----------------------------------------------------------------------------

class IIFileThreadOverload : public MetaFloat
{
public:

   IIFileThreadOverload( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern IIFileThreadOverload* TheIIFileThreadOverloadParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Output properties
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class IIIntegrationImageId : public MetaString
{
public:

   IIIntegrationImageId( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IIIntegrationImageId* TheIIIntegrationImageIdParameter;

// ----------------------------------------------------------------------------

class IILowRejectionMapImageId : public MetaString
{
public:

   IILowRejectionMapImageId( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IILowRejectionMapImageId* TheIILowRejectionMapImageIdParameter;

// ----------------------------------------------------------------------------

class IIHighRejectionMapImageId : public MetaString
{
public:

   IIHighRejectionMapImageId( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IIHighRejectionMapImageId* TheIIHighRejectionMapImageIdParameter;

// ----------------------------------------------------------------------------

class IISlopeMapImageId : public MetaString
{
public:

   IISlopeMapImageId( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IISlopeMapImageId* TheIISlopeMapImageIdParameter;

// ----------------------------------------------------------------------------

class IINumberOfChannels : public MetaInt32
{
public:

   IINumberOfChannels( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IINumberOfChannels* TheIINumberOfChannelsParameter;

// ----------------------------------------------------------------------------

class IINumberOfPixels : public MetaUInt64
{
public:

   IINumberOfPixels( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IINumberOfPixels* TheIINumberOfPixelsParameter;

// ----------------------------------------------------------------------------

class IITotalPixels : public MetaUInt64
{
public:

   IITotalPixels( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IITotalPixels* TheIITotalPixelsParameter;

// ----------------------------------------------------------------------------

class IITotalRejectedLowRK : public MetaUInt64
{
public:

   IITotalRejectedLowRK( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IITotalRejectedLowRK* TheIITotalRejectedLowRKParameter;

// ----------------------------------------------------------------------------

class IITotalRejectedLowG : public MetaUInt64
{
public:

   IITotalRejectedLowG( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IITotalRejectedLowG* TheIITotalRejectedLowGParameter;

// ----------------------------------------------------------------------------

class IITotalRejectedLowB : public MetaUInt64
{
public:

   IITotalRejectedLowB( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IITotalRejectedLowB* TheIITotalRejectedLowBParameter;

// ----------------------------------------------------------------------------

class IITotalRejectedHighRK : public MetaUInt64
{
public:

   IITotalRejectedHighRK( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IITotalRejectedHighRK* TheIITotalRejectedHighRKParameter;

// ----------------------------------------------------------------------------

class IITotalRejectedHighG : public MetaUInt64
{
public:

   IITotalRejectedHighG( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IITotalRejectedHighG* TheIITotalRejectedHighGParameter;

// ----------------------------------------------------------------------------

class IITotalRejectedHighB : public MetaUInt64
{
public:

   IITotalRejectedHighB( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IITotalRejectedHighB* TheIITotalRejectedHighBParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseEstimateRK : public MetaDouble
{
public:

   IIFinalNoiseEstimateRK( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern IIFinalNoiseEstimateRK* TheIIFinalNoiseEstimateRKParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseEstimateG : public MetaDouble
{
public:

   IIFinalNoiseEstimateG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern IIFinalNoiseEstimateG* TheIIFinalNoiseEstimateGParameter;

// ----------------------------------------------------------------------------

class IIFinalNoiseEstimateB : public MetaDouble
{
public:

   IIFinalNoiseEstimateB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern IIFinalNoiseEstimateB* TheIIFinalNoiseEstimateBParameter;

// ----------------------------------------------------------------------------

class IIFinalScaleEstimateRK : public MetaDouble
{
public:

   IIFinalScaleEstimateRK( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern IIFinalScaleEstimateRK* TheIIFinalScaleEstimateRKParameter;

// ----------------------------------------------------------------------------

class IIFinalScaleEstimateG : public MetaDouble
{
public:

   IIFinalScaleEstimateG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern IIFinalScaleEstimateG* TheIIFinalScaleEstimateGParameter;

// ----------------------------------------------------------------------------

class IIFinalScaleEstimateB : public MetaDouble
{
public:

   IIFinalScaleEstimateB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern IIFinalScaleEstimateB* TheIIFinalScaleEstimateBParameter;

// ----------------------------------------------------------------------------

class IIFinalLocationEstimateRK : public MetaDouble
{
public:

   IIFinalLocationEstimateRK( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern IIFinalLocationEstimateRK* TheIIFinalLocationEstimateRKParameter;

// ----------------------------------------------------------------------------

class IIFinalLocationEstimateG : public MetaDouble
{
public:

   IIFinalLocationEstimateG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern IIFinalLocationEstimateG* TheIIFinalLocationEstimateGParameter;

// ----------------------------------------------------------------------------

class IIFinalLocationEstimateB : public MetaDouble
{
public:

   IIFinalLocationEstimateB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern IIFinalLocationEstimateB* TheIIFinalLocationEstimateBParameter;

// ----------------------------------------------------------------------------

class IIReferenceNoiseReductionRK : public MetaFloat
{
public:

   IIReferenceNoiseReductionRK( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIReferenceNoiseReductionRK* TheIIReferenceNoiseReductionRKParameter;

// ----------------------------------------------------------------------------

class IIReferenceNoiseReductionG : public MetaFloat
{
public:

   IIReferenceNoiseReductionG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIReferenceNoiseReductionG* TheIIReferenceNoiseReductionGParameter;

// ----------------------------------------------------------------------------

class IIReferenceNoiseReductionB : public MetaFloat
{
public:

   IIReferenceNoiseReductionB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIReferenceNoiseReductionB* TheIIReferenceNoiseReductionBParameter;

// ----------------------------------------------------------------------------

class IIMedianNoiseReductionRK : public MetaFloat
{
public:

   IIMedianNoiseReductionRK( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIMedianNoiseReductionRK* TheIIMedianNoiseReductionRKParameter;

// ----------------------------------------------------------------------------

class IIMedianNoiseReductionG : public MetaFloat
{
public:

   IIMedianNoiseReductionG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIMedianNoiseReductionG* TheIIMedianNoiseReductionGParameter;

// ----------------------------------------------------------------------------

class IIMedianNoiseReductionB : public MetaFloat
{
public:

   IIMedianNoiseReductionB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIMedianNoiseReductionB* TheIIMedianNoiseReductionBParameter;

// ----------------------------------------------------------------------------

class IIReferenceSNRIncrementRK : public MetaFloat
{
public:

   IIReferenceSNRIncrementRK( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIReferenceSNRIncrementRK* TheIIReferenceSNRIncrementRKParameter;

// ----------------------------------------------------------------------------

class IIReferenceSNRIncrementG : public MetaFloat
{
public:

   IIReferenceSNRIncrementG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIReferenceSNRIncrementG* TheIIReferenceSNRIncrementGParameter;

// ----------------------------------------------------------------------------

class IIReferenceSNRIncrementB : public MetaFloat
{
public:

   IIReferenceSNRIncrementB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIReferenceSNRIncrementB* TheIIReferenceSNRIncrementBParameter;

// ----------------------------------------------------------------------------

class IIAverageSNRIncrementRK : public MetaFloat
{
public:

   IIAverageSNRIncrementRK( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIAverageSNRIncrementRK* TheIIAverageSNRIncrementRKParameter;

// ----------------------------------------------------------------------------

class IIAverageSNRIncrementG : public MetaFloat
{
public:

   IIAverageSNRIncrementG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIAverageSNRIncrementG* TheIIAverageSNRIncrementGParameter;

// ----------------------------------------------------------------------------

class IIAverageSNRIncrementB : public MetaFloat
{
public:

   IIAverageSNRIncrementB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIAverageSNRIncrementB* TheIIAverageSNRIncrementBParameter;

// ----------------------------------------------------------------------------

class IIImageData : public MetaTable
{
public:

   IIImageData( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IIImageData* TheIIImageDataParameter;

// ----------------------------------------------------------------------------

class IIImageWeightRK : public MetaFloat
{
public:

   IIImageWeightRK( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIImageWeightRK* TheIIImageWeightRKParameter;

// ----------------------------------------------------------------------------

class IIImageWeightG : public MetaFloat
{
public:

   IIImageWeightG( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIImageWeightG* TheIIImageWeightGParameter;

// ----------------------------------------------------------------------------

class IIImageWeightB : public MetaFloat
{
public:

   IIImageWeightB( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern IIImageWeightB* TheIIImageWeightBParameter;

// ----------------------------------------------------------------------------

class IIImageRejectedLowRK : public MetaUInt64
{
public:

   IIImageRejectedLowRK( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IIImageRejectedLowRK* TheIIImageRejectedLowRKParameter;

// ----------------------------------------------------------------------------

class IIImageRejectedLowG : public MetaUInt64
{
public:

   IIImageRejectedLowG( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IIImageRejectedLowG* TheIIImageRejectedLowGParameter;

// ----------------------------------------------------------------------------

class IIImageRejectedLowB : public MetaUInt64
{
public:

   IIImageRejectedLowB( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IIImageRejectedLowB* TheIIImageRejectedLowBParameter;

// ----------------------------------------------------------------------------

class IIImageRejectedHighRK : public MetaUInt64
{
public:

   IIImageRejectedHighRK( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IIImageRejectedHighRK* TheIIImageRejectedHighRKParameter;

// ----------------------------------------------------------------------------

class IIImageRejectedHighG : public MetaUInt64
{
public:

   IIImageRejectedHighG( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IIImageRejectedHighG* TheIIImageRejectedHighGParameter;

// ----------------------------------------------------------------------------

class IIImageRejectedHighB : public MetaUInt64
{
public:

   IIImageRejectedHighB( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern IIImageRejectedHighB* TheIIImageRejectedHighBParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ImageIntegrationParameters_h

// ----------------------------------------------------------------------------
// EOF ImageIntegrationParameters.h - Released 2017-05-05T08:37:32Z
