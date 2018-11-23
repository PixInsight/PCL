//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 01.07.00.0308
// ----------------------------------------------------------------------------
// DebayerParameters.cpp - Released 2018-11-23T18:45:59Z
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

#include "DebayerParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

DebayerBayerPatternParameter*	    TheDebayerBayerPatternParameter = nullptr;
DebayerMethodParameter*           TheDebayerMethodParameter = nullptr;
DebayerEvaluateNoise*             TheDebayerEvaluateNoiseParameter = nullptr;
DebayerNoiseEvaluationAlgorithm*  TheDebayerNoiseEvaluationAlgorithmParameter = nullptr;
DebayerShowImages*                TheDebayerShowImagesParameter = nullptr;
DebayerCFASourceFilePath*         TheDebayerCFASourceFilePathParameter = nullptr;
DebayerTargetItems*               TheDebayerTargetItemsParameter = nullptr;
DebayerTargetEnabled*             TheDebayerTargetEnabledParameter = nullptr;
DebayerTargetImage*               TheDebayerTargetImageParameter = nullptr;
DebayerNoGUIMessages*             TheDebayerNoGUIMessagesParameter = nullptr;
DebayerInputHints*                TheDebayerInputHintsParameter = nullptr;
DebayerOutputHints*               TheDebayerOutputHintsParameter = nullptr;
DebayerOutputDirectory*           TheDebayerOutputDirectoryParameter = nullptr;
DebayerOutputExtension*           TheDebayerOutputExtensionParameter = nullptr;
DebayerOutputPrefix*              TheDebayerOutputPrefixParameter = nullptr;
DebayerOutputPostfix*             TheDebayerOutputPostfixParameter = nullptr;
DebayerOverwriteExistingFiles*    TheDebayerOverwriteExistingFilesParameter = nullptr;
DebayerOnError*                   TheDebayerOnErrorParameter = nullptr;
DebayerUseFileThreads*            TheDebayerUseFileThreadsParameter = nullptr;
DebayerFileThreadOverload*        TheDebayerFileThreadOverloadParameter = nullptr;
DebayerMaxFileReadThreads*        TheDebayerMaxFileReadThreadsParameter = nullptr;
DebayerMaxFileWriteThreads*       TheDebayerMaxFileWriteThreadsParameter = nullptr;

DebayerOutputImage*               TheDebayerOutputImageParameter = nullptr;
DebayerNoiseEstimateR*            TheDebayerNoiseEstimateRParameter = nullptr;
DebayerNoiseEstimateG*            TheDebayerNoiseEstimateGParameter = nullptr;
DebayerNoiseEstimateB*            TheDebayerNoiseEstimateBParameter = nullptr;
DebayerNoiseFractionR*            TheDebayerNoiseFractionRParameter = nullptr;
DebayerNoiseFractionG*            TheDebayerNoiseFractionGParameter = nullptr;
DebayerNoiseFractionB*            TheDebayerNoiseFractionBParameter = nullptr;
DebayerNoiseAlgorithmR*           TheDebayerNoiseAlgorithmRParameter = nullptr;
DebayerNoiseAlgorithmG*           TheDebayerNoiseAlgorithmGParameter = nullptr;
DebayerNoiseAlgorithmB*           TheDebayerNoiseAlgorithmBParameter = nullptr;

DebayerOutputFileData*            TheDebayerOutputFileDataParameter = nullptr;
DebayerOutputFilePath*            TheDebayerOutputFilePathParameter = nullptr;
DebayerOutputFileNoiseEstimateR*  TheDebayerOutputFileNoiseEstimateRParameter = nullptr;
DebayerOutputFileNoiseEstimateG*  TheDebayerOutputFileNoiseEstimateGParameter = nullptr;
DebayerOutputFileNoiseEstimateB*  TheDebayerOutputFileNoiseEstimateBParameter = nullptr;
DebayerOutputFileNoiseFractionR*  TheDebayerOutputFileNoiseFractionRParameter = nullptr;
DebayerOutputFileNoiseFractionG*  TheDebayerOutputFileNoiseFractionGParameter = nullptr;
DebayerOutputFileNoiseFractionB*  TheDebayerOutputFileNoiseFractionBParameter = nullptr;
DebayerOutputFileNoiseAlgorithmR* TheDebayerOutputFileNoiseAlgorithmRParameter = nullptr;
DebayerOutputFileNoiseAlgorithmG* TheDebayerOutputFileNoiseAlgorithmGParameter = nullptr;
DebayerOutputFileNoiseAlgorithmB* TheDebayerOutputFileNoiseAlgorithmBParameter = nullptr;

// ----------------------------------------------------------------------------

DebayerBayerPatternParameter::DebayerBayerPatternParameter( MetaProcess* P ) : MetaEnumeration( P )
{
   TheDebayerBayerPatternParameter = this;
}

IsoString DebayerBayerPatternParameter::Id() const
{
   return "cfaPattern";
}

IsoString DebayerBayerPatternParameter::Aliases() const
{
   return "BayerPattern,bayerPattern";
}

size_type DebayerBayerPatternParameter::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DebayerBayerPatternParameter::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Auto: return "Auto";
   case RGGB: return "RGGB";
   case BGGR: return "BGGR";
   case GBRG: return "GBRG";
   case GRBG: return "GRBG";
   case GRGB: return "GRGB";
   case GBGR: return "GBGR";
   case RGBG: return "RGBG";
   case BGRG: return "BGRG";
   }
}

int DebayerBayerPatternParameter::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DebayerBayerPatternParameter::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

DebayerMethodParameter::DebayerMethodParameter( MetaProcess* P ) : MetaEnumeration( P )
{
   TheDebayerMethodParameter = this;
}

IsoString DebayerMethodParameter::Id() const
{
   return "debayerMethod";
}

IsoString DebayerMethodParameter::Aliases() const
{
   return "DebayerMethod";
}

size_type DebayerMethodParameter::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DebayerMethodParameter::ElementId( size_type i ) const
{
   switch ( i )
   {
   case SuperPixel: return "SuperPixel";
   case Bilinear:   return "Bilinear";
   default:
   case VNG:        return "VNG";
   // case AHD:        return "AHD"; ### Uncomment when implemented
   }
}

int DebayerMethodParameter::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DebayerMethodParameter::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

DebayerEvaluateNoise::DebayerEvaluateNoise( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerEvaluateNoiseParameter = this;
}

IsoString DebayerEvaluateNoise::Id() const
{
   return "evaluateNoise";
}

bool DebayerEvaluateNoise::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseEvaluationAlgorithm::DebayerNoiseEvaluationAlgorithm( MetaProcess* P ) : MetaEnumeration( P )
{
   TheDebayerNoiseEvaluationAlgorithmParameter = this;
}

IsoString DebayerNoiseEvaluationAlgorithm::Id() const
{
   return "noiseEvaluationAlgorithm";
}

size_type DebayerNoiseEvaluationAlgorithm::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DebayerNoiseEvaluationAlgorithm::ElementId( size_type i ) const
{
   switch ( i )
   {
   case KSigma: return "NoiseEvaluation_KSigma";
   default:
   case MRS:    return "NoiseEvaluation_MRS";
   }
}

int DebayerNoiseEvaluationAlgorithm::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DebayerNoiseEvaluationAlgorithm::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DebayerShowImages::DebayerShowImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerShowImagesParameter = this;
}

IsoString DebayerShowImages::Id() const
{
   return "showImages";
}

bool DebayerShowImages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerCFASourceFilePath::DebayerCFASourceFilePath( MetaProcess* P ) : MetaString( P )
{
   TheDebayerCFASourceFilePathParameter = this;
}

IsoString DebayerCFASourceFilePath::Id() const
{
   return "cfaSourceFilePath";
}

// ----------------------------------------------------------------------------

DebayerTargetItems::DebayerTargetItems( MetaProcess* P ) : MetaTable( P )
{
   TheDebayerTargetItemsParameter = this;
}

IsoString DebayerTargetItems::Id() const
{
   return "targetItems";
}

// ----------------------------------------------------------------------------

DebayerTargetEnabled::DebayerTargetEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheDebayerTargetEnabledParameter = this;
}

IsoString DebayerTargetEnabled::Id() const
{
   return "enabled";
}

bool DebayerTargetEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerTargetImage::DebayerTargetImage( MetaTable* T ) : MetaString( T )
{
   TheDebayerTargetImageParameter = this;
}

IsoString DebayerTargetImage::Id() const
{
   return "image";
}

// ----------------------------------------------------------------------------

DebayerNoGUIMessages::DebayerNoGUIMessages( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerNoGUIMessagesParameter = this;
}

IsoString DebayerNoGUIMessages::Id() const
{
   return "noGUIMessages";
}

bool DebayerNoGUIMessages::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DebayerInputHints::DebayerInputHints( MetaProcess* P ) : MetaString( P )
{
   TheDebayerInputHintsParameter = this;
}

IsoString DebayerInputHints::Id() const
{
   return "inputHints";
}

// ----------------------------------------------------------------------------

DebayerOutputHints::DebayerOutputHints( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputHintsParameter = this;
}

IsoString DebayerOutputHints::Id() const
{
   return "outputHints";
}

// ----------------------------------------------------------------------------

DebayerOutputDirectory::DebayerOutputDirectory( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputDirectoryParameter = this;
}

IsoString DebayerOutputDirectory::Id() const
{
   return "outputDirectory";
}

// ----------------------------------------------------------------------------

DebayerOutputExtension::DebayerOutputExtension( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputExtensionParameter = this;
}

IsoString DebayerOutputExtension::Id() const
{
   return "outputExtension";
}

String DebayerOutputExtension::DefaultValue() const
{
   return ".xisf";
}

// ----------------------------------------------------------------------------

DebayerOutputPrefix::DebayerOutputPrefix( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputPrefixParameter = this;
}

IsoString DebayerOutputPrefix::Id() const
{
   return "outputPrefix";
}

String DebayerOutputPrefix::DefaultValue() const
{
   return String();
}

// ----------------------------------------------------------------------------

DebayerOutputPostfix::DebayerOutputPostfix( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputPostfixParameter = this;
}

IsoString DebayerOutputPostfix::Id() const
{
   return "outputPostfix";
}

String DebayerOutputPostfix::DefaultValue() const
{
   return "_d";
}

// ----------------------------------------------------------------------------

DebayerOverwriteExistingFiles::DebayerOverwriteExistingFiles( MetaProcess* P ) : MetaBoolean( P )
{
   TheDebayerOverwriteExistingFilesParameter = this;
}

IsoString DebayerOverwriteExistingFiles::Id() const
{
   return "overwriteExistingFiles";
}

bool DebayerOverwriteExistingFiles::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DebayerOnError::DebayerOnError( MetaProcess* p ) : MetaEnumeration( p )
{
   TheDebayerOnErrorParameter = this;
}

IsoString DebayerOnError::Id() const
{
   return "onError";
}

size_type DebayerOnError::NumberOfElements() const
{
   return NumberOfErrorPolicies;
}

IsoString DebayerOnError::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Continue: return "OnError_Continue";
   case Abort:    return "OnError_Abort";
   case AskUser:  return "OnError_AskUser";
   }
}

int DebayerOnError::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DebayerOnError::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DebayerUseFileThreads::DebayerUseFileThreads( MetaProcess* p ) : MetaBoolean( p )
{
   TheDebayerUseFileThreadsParameter = this;
}

IsoString DebayerUseFileThreads::Id() const
{
   return "useFileThreads";
}

bool DebayerUseFileThreads::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerFileThreadOverload::DebayerFileThreadOverload( MetaProcess* p ) : MetaFloat( p )
{
   TheDebayerFileThreadOverloadParameter = this;
}

IsoString DebayerFileThreadOverload::Id() const
{
   return "fileThreadOverload";
}

int DebayerFileThreadOverload::Precision() const
{
   return 2;
}

double DebayerFileThreadOverload::DefaultValue() const
{
   return 1.0;
}

double DebayerFileThreadOverload::MinimumValue() const
{
   return 1;
}

double DebayerFileThreadOverload::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

DebayerMaxFileReadThreads::DebayerMaxFileReadThreads( MetaProcess* p ) : MetaInt32( p )
{
   TheDebayerMaxFileReadThreadsParameter = this;
}

IsoString DebayerMaxFileReadThreads::Id() const
{
   return "maxFileReadThreads";
}

double DebayerMaxFileReadThreads::DefaultValue() const
{
   return 1;
}

double DebayerMaxFileReadThreads::MinimumValue() const
{
   return 1;
}

double DebayerMaxFileReadThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

DebayerMaxFileWriteThreads::DebayerMaxFileWriteThreads( MetaProcess* p ) : MetaInt32( p )
{
   TheDebayerMaxFileWriteThreadsParameter = this;
}

IsoString DebayerMaxFileWriteThreads::Id() const
{
   return "maxFileWriteThreads";
}

double DebayerMaxFileWriteThreads::DefaultValue() const
{
   return 1;
}

double DebayerMaxFileWriteThreads::MinimumValue() const
{
   return 1;
}

double DebayerMaxFileWriteThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

DebayerOutputImage::DebayerOutputImage( MetaProcess* P ) : MetaString( P )
{
   TheDebayerOutputImageParameter = this;
}

IsoString DebayerOutputImage::Id() const
{
   return "outputImage";
}

bool DebayerOutputImage::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseEstimateR::DebayerNoiseEstimateR( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerNoiseEstimateRParameter = this;
}

IsoString DebayerNoiseEstimateR::Id() const
{
   return "noiseEstimateR";
}

int DebayerNoiseEstimateR::Precision() const
{
   return 3;
}

bool DebayerNoiseEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseEstimateG::DebayerNoiseEstimateG( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerNoiseEstimateGParameter = this;
}

IsoString DebayerNoiseEstimateG::Id() const
{
   return "noiseEstimateG";
}

int DebayerNoiseEstimateG::Precision() const
{
   return 3;
}

bool DebayerNoiseEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseEstimateB::DebayerNoiseEstimateB( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerNoiseEstimateBParameter = this;
}

IsoString DebayerNoiseEstimateB::Id() const
{
   return "noiseEstimateB";
}

int DebayerNoiseEstimateB::Precision() const
{
   return 3;
}

bool DebayerNoiseEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerNoiseEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseFractionR::DebayerNoiseFractionR( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerNoiseFractionRParameter = this;
}

IsoString DebayerNoiseFractionR::Id() const
{
   return "noiseFractionR";
}

int DebayerNoiseFractionR::Precision() const
{
   return 3;
}

bool DebayerNoiseFractionR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseFractionG::DebayerNoiseFractionG( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerNoiseFractionGParameter = this;
}

IsoString DebayerNoiseFractionG::Id() const
{
   return "noiseFractionG";
}

int DebayerNoiseFractionG::Precision() const
{
   return 3;
}

bool DebayerNoiseFractionG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseFractionB::DebayerNoiseFractionB( MetaProcess* P ) : MetaFloat( P )
{
   TheDebayerNoiseFractionBParameter = this;
}

IsoString DebayerNoiseFractionB::Id() const
{
   return "noiseFractionB";
}

int DebayerNoiseFractionB::Precision() const
{
   return 3;
}

bool DebayerNoiseFractionB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseAlgorithmR::DebayerNoiseAlgorithmR( MetaProcess* P ) : MetaString( P )
{
   TheDebayerNoiseAlgorithmRParameter = this;
}

IsoString DebayerNoiseAlgorithmR::Id() const
{
   return "noiseAlgorithmR";
}

bool DebayerNoiseAlgorithmR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseAlgorithmG::DebayerNoiseAlgorithmG( MetaProcess* P ) : MetaString( P )
{
   TheDebayerNoiseAlgorithmGParameter = this;
}

IsoString DebayerNoiseAlgorithmG::Id() const
{
   return "noiseAlgorithmG";
}

bool DebayerNoiseAlgorithmG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerNoiseAlgorithmB::DebayerNoiseAlgorithmB( MetaProcess* P ) : MetaString( P )
{
   TheDebayerNoiseAlgorithmBParameter = this;
}

IsoString DebayerNoiseAlgorithmB::Id() const
{
   return "noiseAlgorithmB";
}

bool DebayerNoiseAlgorithmB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileData::DebayerOutputFileData( MetaProcess* P ) : MetaTable( P )
{
   TheDebayerOutputFileDataParameter = this;
}

IsoString DebayerOutputFileData::Id() const
{
   return "outputFileData";
}

bool DebayerOutputFileData::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFilePath::DebayerOutputFilePath( MetaTable* T ) : MetaString( T )
{
   TheDebayerOutputFilePathParameter = this;
}

IsoString DebayerOutputFilePath::Id() const
{
   return "filePath";
}

bool DebayerOutputFilePath::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseEstimateR::DebayerOutputFileNoiseEstimateR( MetaTable* T ) : MetaFloat( T )
{
   TheDebayerOutputFileNoiseEstimateRParameter = this;
}

IsoString DebayerOutputFileNoiseEstimateR::Id() const
{
   return "noiseEstimateR";
}

int DebayerOutputFileNoiseEstimateR::Precision() const
{
   return 3;
}

bool DebayerOutputFileNoiseEstimateR::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFileNoiseEstimateR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseEstimateG::DebayerOutputFileNoiseEstimateG( MetaTable* T ) : MetaFloat( T )
{
   TheDebayerOutputFileNoiseEstimateGParameter = this;
}

IsoString DebayerOutputFileNoiseEstimateG::Id() const
{
   return "noiseEstimateG";
}

int DebayerOutputFileNoiseEstimateG::Precision() const
{
   return 3;
}

bool DebayerOutputFileNoiseEstimateG::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFileNoiseEstimateG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseEstimateB::DebayerOutputFileNoiseEstimateB( MetaTable* T ) : MetaFloat( T )
{
   TheDebayerOutputFileNoiseEstimateBParameter = this;
}

IsoString DebayerOutputFileNoiseEstimateB::Id() const
{
   return "noiseEstimateB";
}

int DebayerOutputFileNoiseEstimateB::Precision() const
{
   return 3;
}

bool DebayerOutputFileNoiseEstimateB::ScientificNotation() const
{
   return true;
}

bool DebayerOutputFileNoiseEstimateB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseFractionR::DebayerOutputFileNoiseFractionR( MetaTable* T ) : MetaFloat( T )
{
   TheDebayerOutputFileNoiseFractionRParameter = this;
}

IsoString DebayerOutputFileNoiseFractionR::Id() const
{
   return "noiseFractionR";
}

int DebayerOutputFileNoiseFractionR::Precision() const
{
   return 3;
}

bool DebayerOutputFileNoiseFractionR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseFractionG::DebayerOutputFileNoiseFractionG( MetaTable* T ) : MetaFloat( T )
{
   TheDebayerOutputFileNoiseFractionGParameter = this;
}

IsoString DebayerOutputFileNoiseFractionG::Id() const
{
   return "noiseFractionG";
}

int DebayerOutputFileNoiseFractionG::Precision() const
{
   return 3;
}

bool DebayerOutputFileNoiseFractionG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseFractionB::DebayerOutputFileNoiseFractionB( MetaTable* T ) : MetaFloat( T )
{
   TheDebayerOutputFileNoiseFractionBParameter = this;
}

IsoString DebayerOutputFileNoiseFractionB::Id() const
{
   return "noiseFractionB";
}

int DebayerOutputFileNoiseFractionB::Precision() const
{
   return 3;
}

bool DebayerOutputFileNoiseFractionB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseAlgorithmR::DebayerOutputFileNoiseAlgorithmR( MetaTable* T ) : MetaString( T )
{
   TheDebayerOutputFileNoiseAlgorithmRParameter = this;
}

IsoString DebayerOutputFileNoiseAlgorithmR::Id() const
{
   return "noiseAlgorithmR";
}

bool DebayerOutputFileNoiseAlgorithmR::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseAlgorithmG::DebayerOutputFileNoiseAlgorithmG( MetaTable* T ) : MetaString( T )
{
   TheDebayerOutputFileNoiseAlgorithmGParameter = this;
}

IsoString DebayerOutputFileNoiseAlgorithmG::Id() const
{
   return "noiseAlgorithmG";
}

bool DebayerOutputFileNoiseAlgorithmG::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DebayerOutputFileNoiseAlgorithmB::DebayerOutputFileNoiseAlgorithmB( MetaTable* T ) : MetaString( T )
{
   TheDebayerOutputFileNoiseAlgorithmBParameter = this;
}

IsoString DebayerOutputFileNoiseAlgorithmB::Id() const
{
   return "noiseAlgorithmB";
}

bool DebayerOutputFileNoiseAlgorithmB::IsReadOnly() const
{
   return true;
}















// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DebayerParameters.cpp - Released 2018-11-23T18:45:59Z
