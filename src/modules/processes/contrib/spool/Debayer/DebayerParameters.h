//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 01.07.00.0308
// ----------------------------------------------------------------------------
// DebayerParameters.h - Released 2018-11-23T18:45:59Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
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

#ifndef __DebayerParameters_h
#define __DebayerParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class DebayerBayerPatternParameter : public MetaEnumeration
{
public:

   enum { Auto,
          RGGB,
          BGGR,
          GBRG,
          GRBG,
          GRGB,
          GBGR,
          RGBG,
          BGRG,
          NumberOfItems,
          Default = Auto };

   DebayerBayerPatternParameter( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DebayerBayerPatternParameter* TheDebayerBayerPatternParameter;

// ----------------------------------------------------------------------------

class DebayerMethodParameter : public MetaEnumeration
{
public:

   enum { SuperPixel,
          Bilinear,
          VNG,
          /*AHD,*/ // ### Uncomment when implemented
          NumberOfItems,
          Default = VNG };

   DebayerMethodParameter( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DebayerMethodParameter* TheDebayerMethodParameter;

// ----------------------------------------------------------------------------

class DebayerEvaluateNoise : public MetaBoolean
{
public:

   DebayerEvaluateNoise( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerEvaluateNoise* TheDebayerEvaluateNoiseParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEvaluationAlgorithm : public MetaEnumeration
{
public:

   enum { KSigma,
          MRS,
          NumberOfItems,
          Default = MRS };

   DebayerNoiseEvaluationAlgorithm( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DebayerNoiseEvaluationAlgorithm* TheDebayerNoiseEvaluationAlgorithmParameter;

// ----------------------------------------------------------------------------

class DebayerShowImages : public MetaBoolean
{
public:

   DebayerShowImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerShowImages* TheDebayerShowImagesParameter;

// ----------------------------------------------------------------------------

class DebayerCFASourceFilePath : public MetaString
{
public:

   DebayerCFASourceFilePath( MetaProcess* );

   IsoString Id() const override;
};

extern DebayerCFASourceFilePath* TheDebayerCFASourceFilePathParameter;

// ----------------------------------------------------------------------------

class DebayerTargetItems : public MetaTable
{
public:

   DebayerTargetItems( MetaProcess* );

   IsoString Id() const override;
};

extern DebayerTargetItems* TheDebayerTargetItemsParameter;

// ----------------------------------------------------------------------------

class DebayerTargetEnabled : public MetaBoolean
{
public:

   DebayerTargetEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerTargetEnabled* TheDebayerTargetEnabledParameter;

// ----------------------------------------------------------------------------

class DebayerTargetImage : public MetaString
{
public:

   DebayerTargetImage( MetaTable* );

   IsoString Id() const override;
};

extern DebayerTargetImage* TheDebayerTargetImageParameter;

// ----------------------------------------------------------------------------

class DebayerNoGUIMessages : public MetaBoolean
{
public:

   DebayerNoGUIMessages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerNoGUIMessages* TheDebayerNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

class DebayerInputHints : public MetaString
{
public:

   DebayerInputHints( MetaProcess* );

   IsoString Id() const override;
};

extern DebayerInputHints* TheDebayerInputHintsParameter;

// ----------------------------------------------------------------------------

class DebayerOutputHints : public MetaString
{
public:

   DebayerOutputHints( MetaProcess* );

   IsoString Id() const override;
};

extern DebayerOutputHints* TheDebayerOutputHintsParameter;

// ----------------------------------------------------------------------------

class DebayerOutputDirectory : public MetaString
{
public:

   DebayerOutputDirectory( MetaProcess* );

   IsoString Id() const override;
};

extern DebayerOutputDirectory* TheDebayerOutputDirectoryParameter;

// ----------------------------------------------------------------------------

class DebayerOutputExtension : public MetaString
{
public:

   DebayerOutputExtension( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern DebayerOutputExtension* TheDebayerOutputExtensionParameter;

// ----------------------------------------------------------------------------

class DebayerOutputPrefix : public MetaString
{
public:

   DebayerOutputPrefix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern DebayerOutputPrefix* TheDebayerOutputPrefixParameter;

// ----------------------------------------------------------------------------

class DebayerOutputPostfix : public MetaString
{
public:

   DebayerOutputPostfix( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern DebayerOutputPostfix* TheDebayerOutputPostfixParameter;

// ----------------------------------------------------------------------------

class DebayerOverwriteExistingFiles : public MetaBoolean
{
public:

   DebayerOverwriteExistingFiles( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerOverwriteExistingFiles* TheDebayerOverwriteExistingFilesParameter;

// ----------------------------------------------------------------------------

class DebayerOnError : public MetaEnumeration
{
public:

   enum { Continue,
          Abort,
          AskUser,
          NumberOfErrorPolicies,
          Default = Continue };

   DebayerOnError( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DebayerOnError* TheDebayerOnErrorParameter;

// ----------------------------------------------------------------------------

class DebayerUseFileThreads : public MetaBoolean
{
public:

   DebayerUseFileThreads( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DebayerUseFileThreads* TheDebayerUseFileThreadsParameter;

// ----------------------------------------------------------------------------

class DebayerFileThreadOverload : public MetaFloat
{
public:

   DebayerFileThreadOverload( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerFileThreadOverload* TheDebayerFileThreadOverloadParameter;

// ----------------------------------------------------------------------------

class DebayerMaxFileReadThreads : public MetaInt32
{
public:

   DebayerMaxFileReadThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerMaxFileReadThreads* TheDebayerMaxFileReadThreadsParameter;

// ----------------------------------------------------------------------------

class DebayerMaxFileWriteThreads : public MetaInt32
{
public:

   DebayerMaxFileWriteThreads( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DebayerMaxFileWriteThreads* TheDebayerMaxFileWriteThreadsParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DebayerOutputImage : public MetaString
{
public:

   DebayerOutputImage( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputImage* TheDebayerOutputImageParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEstimateR : public MetaFloat
{
public:

   DebayerNoiseEstimateR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseEstimateR* TheDebayerNoiseEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEstimateG : public MetaFloat
{
public:

   DebayerNoiseEstimateG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseEstimateG* TheDebayerNoiseEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEstimateB : public MetaFloat
{
public:

   DebayerNoiseEstimateB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseEstimateB* TheDebayerNoiseEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseFractionR : public MetaFloat
{
public:

   DebayerNoiseFractionR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseFractionR* TheDebayerNoiseFractionRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseFractionG : public MetaFloat
{
public:

   DebayerNoiseFractionG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseFractionG* TheDebayerNoiseFractionGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseFractionB : public MetaFloat
{
public:

   DebayerNoiseFractionB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseFractionB* TheDebayerNoiseFractionBParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseAlgorithmR : public MetaString
{
public:

   DebayerNoiseAlgorithmR( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseAlgorithmR* TheDebayerNoiseAlgorithmRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseAlgorithmG : public MetaString
{
public:

   DebayerNoiseAlgorithmG( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseAlgorithmG* TheDebayerNoiseAlgorithmGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseAlgorithmB : public MetaString
{
public:

   DebayerNoiseAlgorithmB( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerNoiseAlgorithmB* TheDebayerNoiseAlgorithmBParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DebayerOutputFileData : public MetaTable
{
public:

   DebayerOutputFileData( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileData* TheDebayerOutputFileDataParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePath : public MetaString
{
public:

   DebayerOutputFilePath( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFilePath* TheDebayerOutputFilePathParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseEstimateR : public MetaFloat
{
public:

   DebayerOutputFileNoiseEstimateR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseEstimateR* TheDebayerOutputFileNoiseEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseEstimateG : public MetaFloat
{
public:

   DebayerOutputFileNoiseEstimateG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseEstimateG* TheDebayerOutputFileNoiseEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseEstimateB : public MetaFloat
{
public:

   DebayerOutputFileNoiseEstimateB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseEstimateB* TheDebayerOutputFileNoiseEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseFractionR : public MetaFloat
{
public:

   DebayerOutputFileNoiseFractionR( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseFractionR* TheDebayerOutputFileNoiseFractionRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseFractionG : public MetaFloat
{
public:

   DebayerOutputFileNoiseFractionG( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseFractionG* TheDebayerOutputFileNoiseFractionGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseFractionB : public MetaFloat
{
public:

   DebayerOutputFileNoiseFractionB( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseFractionB* TheDebayerOutputFileNoiseFractionBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseAlgorithmR : public MetaString
{
public:

   DebayerOutputFileNoiseAlgorithmR( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseAlgorithmR* TheDebayerOutputFileNoiseAlgorithmRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseAlgorithmG : public MetaString
{
public:

   DebayerOutputFileNoiseAlgorithmG( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseAlgorithmG* TheDebayerOutputFileNoiseAlgorithmGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseAlgorithmB : public MetaString
{
public:

   DebayerOutputFileNoiseAlgorithmB( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern DebayerOutputFileNoiseAlgorithmB* TheDebayerOutputFileNoiseAlgorithmBParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __DebayerParameters_h

// ----------------------------------------------------------------------------
// EOF DebayerParameters.h - Released 2018-11-23T18:45:59Z
