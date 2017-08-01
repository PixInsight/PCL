//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 01.06.00.0281
// ----------------------------------------------------------------------------
// DebayerParameters.h - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
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

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
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

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern DebayerMethodParameter* TheDebayerMethodParameter;

// ----------------------------------------------------------------------------

class DebayerEvaluateNoise : public MetaBoolean
{
public:

   DebayerEvaluateNoise( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
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

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern DebayerNoiseEvaluationAlgorithm* TheDebayerNoiseEvaluationAlgorithmParameter;

// ----------------------------------------------------------------------------

class DebayerShowImages : public MetaBoolean
{
public:

   DebayerShowImages( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DebayerShowImages* TheDebayerShowImagesParameter;

// ----------------------------------------------------------------------------

class DebayerCFASourceFilePath : public MetaString
{
public:

   DebayerCFASourceFilePath( MetaProcess* );

   virtual IsoString Id() const;
};

extern DebayerCFASourceFilePath* TheDebayerCFASourceFilePathParameter;

// ----------------------------------------------------------------------------

class DebayerTargetItems : public MetaTable
{
public:

   DebayerTargetItems( MetaProcess* );

   virtual IsoString Id() const;
};

extern DebayerTargetItems* TheDebayerTargetItemsParameter;

// ----------------------------------------------------------------------------

class DebayerTargetEnabled : public MetaBoolean
{
public:

   DebayerTargetEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DebayerTargetEnabled* TheDebayerTargetEnabledParameter;

// ----------------------------------------------------------------------------

class DebayerTargetImage : public MetaString
{
public:

   DebayerTargetImage( MetaTable* );

   virtual IsoString Id() const;
};

extern DebayerTargetImage* TheDebayerTargetImageParameter;

// ----------------------------------------------------------------------------

class DebayerNoGUIMessages : public MetaBoolean
{
public:

   DebayerNoGUIMessages( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DebayerNoGUIMessages* TheDebayerNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

class DebayerInputHints : public MetaString
{
public:

   DebayerInputHints( MetaProcess* );

   virtual IsoString Id() const;
};

extern DebayerInputHints* TheDebayerInputHintsParameter;

// ----------------------------------------------------------------------------

class DebayerOutputHints : public MetaString
{
public:

   DebayerOutputHints( MetaProcess* );

   virtual IsoString Id() const;
};

extern DebayerOutputHints* TheDebayerOutputHintsParameter;

// ----------------------------------------------------------------------------

class DebayerOutputDirectory : public MetaString
{
public:

   DebayerOutputDirectory( MetaProcess* );

   virtual IsoString Id() const;
};

extern DebayerOutputDirectory* TheDebayerOutputDirectoryParameter;

// ----------------------------------------------------------------------------

class DebayerOutputExtension : public MetaString
{
public:

   DebayerOutputExtension( MetaProcess* );

   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

extern DebayerOutputExtension* TheDebayerOutputExtensionParameter;

// ----------------------------------------------------------------------------

class DebayerOutputPrefix : public MetaString
{
public:

   DebayerOutputPrefix( MetaProcess* );

   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

extern DebayerOutputPrefix* TheDebayerOutputPrefixParameter;

// ----------------------------------------------------------------------------

class DebayerOutputPostfix : public MetaString
{
public:

   DebayerOutputPostfix( MetaProcess* );

   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

extern DebayerOutputPostfix* TheDebayerOutputPostfixParameter;

// ----------------------------------------------------------------------------

class DebayerOverwriteExistingFiles : public MetaBoolean
{
public:

   DebayerOverwriteExistingFiles( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
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

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern DebayerOnError* TheDebayerOnErrorParameter;

// ----------------------------------------------------------------------------

class DebayerUseFileThreads : public MetaBoolean
{
public:

   DebayerUseFileThreads( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DebayerUseFileThreads* TheDebayerUseFileThreadsParameter;

// ----------------------------------------------------------------------------

class DebayerFileThreadOverload : public MetaFloat
{
public:

   DebayerFileThreadOverload( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DebayerFileThreadOverload* TheDebayerFileThreadOverloadParameter;

// ----------------------------------------------------------------------------

class DebayerMaxFileReadThreads : public MetaInt32
{
public:

   DebayerMaxFileReadThreads( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DebayerMaxFileReadThreads* TheDebayerMaxFileReadThreadsParameter;

// ----------------------------------------------------------------------------

class DebayerMaxFileWriteThreads : public MetaInt32
{
public:

   DebayerMaxFileWriteThreads( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DebayerMaxFileWriteThreads* TheDebayerMaxFileWriteThreadsParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DebayerOutputImage : public MetaString
{
public:

   DebayerOutputImage( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputImage* TheDebayerOutputImageParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEstimateR : public MetaFloat
{
public:

   DebayerNoiseEstimateR( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseEstimateR* TheDebayerNoiseEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEstimateG : public MetaFloat
{
public:

   DebayerNoiseEstimateG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseEstimateG* TheDebayerNoiseEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseEstimateB : public MetaFloat
{
public:

   DebayerNoiseEstimateB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseEstimateB* TheDebayerNoiseEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseFractionR : public MetaFloat
{
public:

   DebayerNoiseFractionR( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseFractionR* TheDebayerNoiseFractionRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseFractionG : public MetaFloat
{
public:

   DebayerNoiseFractionG( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseFractionG* TheDebayerNoiseFractionGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseFractionB : public MetaFloat
{
public:

   DebayerNoiseFractionB( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseFractionB* TheDebayerNoiseFractionBParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseAlgorithmR : public MetaString
{
public:

   DebayerNoiseAlgorithmR( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseAlgorithmR* TheDebayerNoiseAlgorithmRParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseAlgorithmG : public MetaString
{
public:

   DebayerNoiseAlgorithmG( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseAlgorithmG* TheDebayerNoiseAlgorithmGParameter;

// ----------------------------------------------------------------------------

class DebayerNoiseAlgorithmB : public MetaString
{
public:

   DebayerNoiseAlgorithmB( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerNoiseAlgorithmB* TheDebayerNoiseAlgorithmBParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DebayerOutputFileData : public MetaTable
{
public:

   DebayerOutputFileData( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputFileData* TheDebayerOutputFileDataParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFilePath : public MetaString
{
public:

   DebayerOutputFilePath( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputFilePath* TheDebayerOutputFilePathParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseEstimateR : public MetaFloat
{
public:

   DebayerOutputFileNoiseEstimateR( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputFileNoiseEstimateR* TheDebayerOutputFileNoiseEstimateRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseEstimateG : public MetaFloat
{
public:

   DebayerOutputFileNoiseEstimateG( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputFileNoiseEstimateG* TheDebayerOutputFileNoiseEstimateGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseEstimateB : public MetaFloat
{
public:

   DebayerOutputFileNoiseEstimateB( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputFileNoiseEstimateB* TheDebayerOutputFileNoiseEstimateBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseFractionR : public MetaFloat
{
public:

   DebayerOutputFileNoiseFractionR( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputFileNoiseFractionR* TheDebayerOutputFileNoiseFractionRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseFractionG : public MetaFloat
{
public:

   DebayerOutputFileNoiseFractionG( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputFileNoiseFractionG* TheDebayerOutputFileNoiseFractionGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseFractionB : public MetaFloat
{
public:

   DebayerOutputFileNoiseFractionB( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputFileNoiseFractionB* TheDebayerOutputFileNoiseFractionBParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseAlgorithmR : public MetaString
{
public:

   DebayerOutputFileNoiseAlgorithmR( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputFileNoiseAlgorithmR* TheDebayerOutputFileNoiseAlgorithmRParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseAlgorithmG : public MetaString
{
public:

   DebayerOutputFileNoiseAlgorithmG( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputFileNoiseAlgorithmG* TheDebayerOutputFileNoiseAlgorithmGParameter;

// ----------------------------------------------------------------------------

class DebayerOutputFileNoiseAlgorithmB : public MetaString
{
public:

   DebayerOutputFileNoiseAlgorithmB( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DebayerOutputFileNoiseAlgorithmB* TheDebayerOutputFileNoiseAlgorithmBParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __DebayerParameters_h

// ----------------------------------------------------------------------------
// EOF DebayerParameters.h - Released 2017-08-01T14:26:58Z
