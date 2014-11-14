// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard ImageCalibration Process Module Version 01.03.00.0196
// ****************************************************************************
// ImageCalibrationParameters.h - Released 2014/11/14 17:19:21 UTC
// ****************************************************************************
// This file is part of the standard ImageCalibration PixInsight module.
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

#ifndef __ImageCalibrationParameters_h
#define __ImageCalibrationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ICTargetFrames : public MetaTable
{
public:

   ICTargetFrames( MetaProcess* );

   virtual IsoString Id() const;
};

extern ICTargetFrames* TheICTargetFramesParameter;

// ----------------------------------------------------------------------------

class ICTargetFrameEnabled : public MetaBoolean
{
public:

   ICTargetFrameEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICTargetFrameEnabled* TheICTargetFrameEnabledParameter;

// ----------------------------------------------------------------------------

class ICTargetFramePath : public MetaString
{
public:

   ICTargetFramePath( MetaTable* );

   virtual IsoString Id() const;
};

extern ICTargetFramePath* TheICTargetFramePathParameter;

// ----------------------------------------------------------------------------

class ICInputHints : public MetaString
{
public:

   ICInputHints( MetaProcess* );

   virtual IsoString Id() const;
};

extern ICInputHints* TheICInputHintsParameter;

// ----------------------------------------------------------------------------

class ICOutputHints : public MetaString
{
public:

   ICOutputHints( MetaProcess* );

   virtual IsoString Id() const;
};

extern ICOutputHints* TheICOutputHintsParameter;

// ----------------------------------------------------------------------------

class ICPedestal : public MetaInt32
{
public:

   ICPedestal( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ICPedestal* TheICPedestalParameter;

// ----------------------------------------------------------------------------

class ICPedestalMode : public MetaEnumeration
{
public:

   enum { Literal,
          Keyword,
          CustomKeyword,
          NumberOfPedestalModes,
          Default = Keyword };

   ICPedestalMode( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern ICPedestalMode* TheICPedestalModeParameter;

// ----------------------------------------------------------------------------

class ICPedestalKeyword : public MetaString
{
public:

   ICPedestalKeyword( MetaProcess* );

   virtual IsoString Id() const;
};

extern ICPedestalKeyword* TheICPedestalKeywordParameter;

// ----------------------------------------------------------------------------

class ICOverscanEnabled : public MetaBoolean
{
public:

   ICOverscanEnabled( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICOverscanEnabled* TheICOverscanEnabledParameter;

// ----------------------------------------------------------------------------

class ICOverscanImageX0 : public MetaInt32
{
public:

   ICOverscanImageX0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern ICOverscanImageX0* TheICOverscanImageX0Parameter;

// ----------------------------------------------------------------------------

class ICOverscanImageY0 : public MetaInt32
{
public:

   ICOverscanImageY0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern ICOverscanImageY0* TheICOverscanImageY0Parameter;

// ----------------------------------------------------------------------------

class ICOverscanImageX1 : public MetaInt32
{
public:

   ICOverscanImageX1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern ICOverscanImageX1* TheICOverscanImageX1Parameter;

// ----------------------------------------------------------------------------

class ICOverscanImageY1 : public MetaInt32
{
public:

   ICOverscanImageY1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern ICOverscanImageY1* TheICOverscanImageY1Parameter;

// ----------------------------------------------------------------------------

class ICOverscanRegions : public MetaTable
{
public:

   ICOverscanRegions( MetaProcess* );

   virtual IsoString Id() const;
};

extern ICOverscanRegions* TheICOverscanRegionsParameter;

// ----------------------------------------------------------------------------

class ICOverscanRegionEnabled : public MetaBoolean
{
public:

   ICOverscanRegionEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICOverscanRegionEnabled* TheICOverscanRegionEnabledParameter;

// ----------------------------------------------------------------------------

class ICOverscanSourceX0 : public MetaInt32
{
public:

   ICOverscanSourceX0( MetaTable* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern ICOverscanSourceX0* TheICOverscanSourceX0Parameter;

// ----------------------------------------------------------------------------

class ICOverscanSourceY0 : public MetaInt32
{
public:

   ICOverscanSourceY0( MetaTable* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern ICOverscanSourceY0* TheICOverscanSourceY0Parameter;

// ----------------------------------------------------------------------------

class ICOverscanSourceX1 : public MetaInt32
{
public:

   ICOverscanSourceX1( MetaTable* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern ICOverscanSourceX1* TheICOverscanSourceX1Parameter;

// ----------------------------------------------------------------------------

class ICOverscanSourceY1 : public MetaInt32
{
public:

   ICOverscanSourceY1( MetaTable* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern ICOverscanSourceY1* TheICOverscanSourceY1Parameter;

// ----------------------------------------------------------------------------

class ICOverscanTargetX0 : public MetaInt32
{
public:

   ICOverscanTargetX0( MetaTable* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern ICOverscanTargetX0* TheICOverscanTargetX0Parameter;

// ----------------------------------------------------------------------------

class ICOverscanTargetY0 : public MetaInt32
{
public:

   ICOverscanTargetY0( MetaTable* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern ICOverscanTargetY0* TheICOverscanTargetY0Parameter;

// ----------------------------------------------------------------------------

class ICOverscanTargetX1 : public MetaInt32
{
public:

   ICOverscanTargetX1( MetaTable* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern ICOverscanTargetX1* TheICOverscanTargetX1Parameter;

// ----------------------------------------------------------------------------

class ICOverscanTargetY1 : public MetaInt32
{
public:

   ICOverscanTargetY1( MetaTable* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
};

extern ICOverscanTargetY1* TheICOverscanTargetY1Parameter;

// ----------------------------------------------------------------------------

class ICMasterBiasEnabled : public MetaBoolean
{
public:

   ICMasterBiasEnabled( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICMasterBiasEnabled* TheICMasterBiasEnabledParameter;

// ----------------------------------------------------------------------------

class ICMasterBiasPath : public MetaString
{
public:

   ICMasterBiasPath( MetaProcess* );

   virtual IsoString Id() const;
};

extern ICMasterBiasPath* TheICMasterBiasPathParameter;

// ----------------------------------------------------------------------------

class ICMasterDarkEnabled : public MetaBoolean
{
public:

   ICMasterDarkEnabled( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICMasterDarkEnabled* TheICMasterDarkEnabledParameter;

// ----------------------------------------------------------------------------

class ICMasterDarkPath : public MetaString
{
public:

   ICMasterDarkPath( MetaProcess* );

   virtual IsoString Id() const;
};

extern ICMasterDarkPath* TheICMasterDarkPathParameter;

// ----------------------------------------------------------------------------

class ICMasterFlatEnabled : public MetaBoolean
{
public:

   ICMasterFlatEnabled( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICMasterFlatEnabled* TheICMasterFlatEnabledParameter;

// ----------------------------------------------------------------------------

class ICMasterFlatPath : public MetaString
{
public:

   ICMasterFlatPath( MetaProcess* );

   virtual IsoString Id() const;
};

extern ICMasterFlatPath* TheICMasterFlatPathParameter;

// ----------------------------------------------------------------------------

class ICCalibrateBias : public MetaBoolean
{
public:

   ICCalibrateBias( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICCalibrateBias* TheICCalibrateBiasParameter;

// ----------------------------------------------------------------------------

class ICCalibrateDark : public MetaBoolean
{
public:

   ICCalibrateDark( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICCalibrateDark* TheICCalibrateDarkParameter;

// ----------------------------------------------------------------------------

class ICCalibrateFlat : public MetaBoolean
{
public:

   ICCalibrateFlat( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICCalibrateFlat* TheICCalibrateFlatParameter;

// ----------------------------------------------------------------------------

class ICOptimizeDarks : public MetaBoolean
{
public:

   ICOptimizeDarks( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICOptimizeDarks* TheICOptimizeDarksParameter;

// ----------------------------------------------------------------------------

class ICDarkOptimizationThreshold : public MetaFloat
{
public:

   ICDarkOptimizationThreshold( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ICDarkOptimizationThreshold* TheICDarkOptimizationThresholdParameter;

// ----------------------------------------------------------------------------

class ICDarkOptimizationWindow : public MetaInt32
{
public:

   ICDarkOptimizationWindow( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ICDarkOptimizationWindow* TheICDarkOptimizationWindowParameter;

// ----------------------------------------------------------------------------

class ICDarkCFADetectionMode : public MetaEnumeration
{
public:

   enum { DetectCFA,
          ForceCFA,
          IgnoreCFA,
          NumberOfDarkCFADetectionModes,
          Default = DetectCFA };

   ICDarkCFADetectionMode( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern ICDarkCFADetectionMode* TheICDarkCFADetectionModeParameter;

// ----------------------------------------------------------------------------

class ICEvaluateNoise : public MetaBoolean
{
public:

   ICEvaluateNoise( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICEvaluateNoise* TheICEvaluateNoiseParameter;

// ----------------------------------------------------------------------------

class ICNoiseEvaluationAlgorithm : public MetaEnumeration
{
public:

   enum { KSigma,
          MRS,
          NumberOfNoiseEvaluationAlgorithms,
          Default = MRS };

   ICNoiseEvaluationAlgorithm( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern ICNoiseEvaluationAlgorithm* TheICNoiseEvaluationAlgorithmParameter;

// ----------------------------------------------------------------------------

class ICOutputDirectory : public MetaString
{
public:

   ICOutputDirectory( MetaProcess* );

   virtual IsoString Id() const;
};

extern ICOutputDirectory* TheICOutputDirectoryParameter;

// ----------------------------------------------------------------------------

class ICOutputExtension : public MetaString
{
public:

   ICOutputExtension( MetaProcess* );

   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

extern ICOutputExtension* TheICOutputExtensionParameter;

// ----------------------------------------------------------------------------

class ICOutputPrefix : public MetaString
{
public:

   ICOutputPrefix( MetaProcess* );

   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

extern ICOutputPrefix* TheICOutputPrefixParameter;

// ----------------------------------------------------------------------------

class ICOutputPostfix : public MetaString
{
public:

   ICOutputPostfix( MetaProcess* );

   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

extern ICOutputPostfix* TheICOutputPostfixParameter;

// ----------------------------------------------------------------------------

class ICOutputSampleFormat : public MetaEnumeration
{
public:

   enum { I16,    // unsigned 16-bit integer
          I32,    // unsigned 32-bit integer
          F32,    // IEEE 32-bit floating point (simple precision)
          F64,    // IEEE 64-bit floating point (double precision)
          NumberOfSampleFormats,
          Default = F32 };

   ICOutputSampleFormat( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern ICOutputSampleFormat* TheICOutputSampleFormatParameter;

// ----------------------------------------------------------------------------

class ICOutputPedestal : public MetaInt32
{
public:

   ICOutputPedestal( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern ICOutputPedestal* TheICOutputPedestalParameter;

// ----------------------------------------------------------------------------

class ICOverwriteExistingFiles : public MetaBoolean
{
public:

   ICOverwriteExistingFiles( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICOverwriteExistingFiles* TheICOverwriteExistingFilesParameter;

// ----------------------------------------------------------------------------

class ICOnError : public MetaEnumeration
{
public:

   enum { Continue,
          Abort,
          AskUser,
          NumberOfErrorPolicies,
          Default = Continue };

   ICOnError( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern ICOnError* TheICOnErrorParameter;

// ----------------------------------------------------------------------------

class ICNoGUIMessages : public MetaBoolean
{
public:

   ICNoGUIMessages( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern ICNoGUIMessages* TheICNoGUIMessagesParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Output properties
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class ICOutputData : public MetaTable
{
public:

   ICOutputData( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern ICOutputData* TheICOutputDataParameter;

// ----------------------------------------------------------------------------

class ICOutputFilePath : public MetaString
{
public:

   ICOutputFilePath( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern ICOutputFilePath* TheICOutputFilePathParameter;

// ----------------------------------------------------------------------------

class ICDarkScalingFactorRK : public MetaFloat
{
public:

   ICDarkScalingFactorRK( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern ICDarkScalingFactorRK* TheICDarkScalingFactorRKParameter;

// ----------------------------------------------------------------------------

class ICDarkScalingFactorG : public MetaFloat
{
public:

   ICDarkScalingFactorG( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern ICDarkScalingFactorG* TheICDarkScalingFactorGParameter;

// ----------------------------------------------------------------------------

class ICDarkScalingFactorB : public MetaFloat
{
public:

   ICDarkScalingFactorB( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern ICDarkScalingFactorB* TheICDarkScalingFactorBParameter;

// ----------------------------------------------------------------------------

class ICNoiseEstimateRK : public MetaFloat
{
public:

   ICNoiseEstimateRK( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern ICNoiseEstimateRK* TheICNoiseEstimateRKParameter;

// ----------------------------------------------------------------------------

class ICNoiseEstimateG : public MetaFloat
{
public:

   ICNoiseEstimateG( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern ICNoiseEstimateG* TheICNoiseEstimateGParameter;

// ----------------------------------------------------------------------------

class ICNoiseEstimateB : public MetaFloat
{
public:

   ICNoiseEstimateB( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern ICNoiseEstimateB* TheICNoiseEstimateBParameter;

// ----------------------------------------------------------------------------

class ICNoiseFractionRK : public MetaFloat
{
public:

   ICNoiseFractionRK( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern ICNoiseFractionRK* TheICNoiseFractionRKParameter;

// ----------------------------------------------------------------------------

class ICNoiseFractionG : public MetaFloat
{
public:

   ICNoiseFractionG( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern ICNoiseFractionG* TheICNoiseFractionGParameter;

// ----------------------------------------------------------------------------

class ICNoiseFractionB : public MetaFloat
{
public:

   ICNoiseFractionB( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern ICNoiseFractionB* TheICNoiseFractionBParameter;

// ----------------------------------------------------------------------------

class ICNoiseAlgorithmRK : public MetaString
{
public:

   ICNoiseAlgorithmRK( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern ICNoiseAlgorithmRK* TheICNoiseAlgorithmRKParameter;

// ----------------------------------------------------------------------------

class ICNoiseAlgorithmG : public MetaString
{
public:

   ICNoiseAlgorithmG( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern ICNoiseAlgorithmG* TheICNoiseAlgorithmGParameter;

// ----------------------------------------------------------------------------

class ICNoiseAlgorithmB : public MetaString
{
public:

   ICNoiseAlgorithmB( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern ICNoiseAlgorithmB* TheICNoiseAlgorithmBParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ImageCalibrationParameters_h

// ****************************************************************************
// EOF ImageCalibrationParameters.h - Released 2014/11/14 17:19:21 UTC
