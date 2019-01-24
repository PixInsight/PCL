//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0362
// ----------------------------------------------------------------------------
// LocalNormalizationParameters.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "LocalNormalizationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

LNScale*                       TheLNScaleParameter = nullptr;
LNNoScale*                     TheLNNoScaleParameter = nullptr;
LNRejection*                   TheLNRejectionParameter = nullptr;
LNBackgroundRejectionLimit*    TheLNBackgroundRejectionLimitParameter = nullptr;
LNReferenceRejectionThreshold* TheLNReferenceRejectionThresholdParameter = nullptr;
LNTargetRejectionThreshold*    TheLNTargetRejectionThresholdParameter = nullptr;
LNHotPixelFilterRadius*        TheLNHotPixelFilterRadiusParameter = nullptr;
LNNoiseReductionFilterRadius*  TheLNNoiseReductionFilterRadiusParameter = nullptr;
LNReferencePathOrViewId*       TheLNReferencePathOrViewIdParameter = nullptr;
LNReferenceIsView*             TheLNReferenceIsViewParameter = nullptr;
LNTargetItems*                 TheLNTargetItemsParameter = nullptr;
LNTargetEnabled*               TheLNTargetEnabledParameter = nullptr;
LNTargetImage*                 TheLNTargetImageParameter = nullptr;
LNInputHints*                  TheLNInputHintsParameter = nullptr;
LNOutputHints*                 TheLNOutputHintsParameter = nullptr;
LNGenerateNormalizedImages*    TheLNGenerateNormalizedImagesParameter = nullptr;
LNGenerateNormalizationData*   TheLNGenerateNormalizationDataParameter = nullptr;
LNShowBackgroundModels*        TheLNShowBackgroundModelsParameter = nullptr;
LNShowRejectionMaps*           TheLNShowRejectionMapsParameter = nullptr;
LNPlotNormalizationFunctions*  TheLNPlotNormalizationFunctionsParameter = nullptr;
LNNoGUIMessages*               TheLNNoGUIMessagesParameter = nullptr;
LNOutputDirectory*             TheLNOutputDirectoryParameter = nullptr;
LNOutputExtension*             TheLNOutputExtensionParameter = nullptr;
LNOutputPrefix*                TheLNOutputPrefixParameter = nullptr;
LNOutputPostfix*               TheLNOutputPostfixParameter = nullptr;
LNOverwriteExistingFiles*      TheLNOverwriteExistingFilesParameter = nullptr;
LNOnError*                     TheLNOnErrorParameter = nullptr;
LNUseFileThreads*              TheLNUseFileThreadsParameter = nullptr;
LNFileThreadOverload*          TheLNFileThreadOverloadParameter = nullptr;
LNMaxFileReadThreads*          TheLNMaxFileReadThreadsParameter = nullptr;
LNMaxFileWriteThreads*         TheLNMaxFileWriteThreadsParameter = nullptr;
LNGraphSize*                   TheLNGraphSizeParameter = nullptr;
LNGraphTextSize*               TheLNGraphTextSizeParameter = nullptr;
LNGraphTitleSize*              TheLNGraphTitleSizeParameter = nullptr;
LNGraphTransparent*            TheLNGraphTransparentParameter = nullptr;
LNGraphOutputDirectory*        TheLNGraphOutputDirectoryParameter = nullptr;

// ----------------------------------------------------------------------------

LNScale::LNScale( MetaProcess* P ) : MetaInt32( P )
{
   TheLNScaleParameter = this;
}

IsoString LNScale::Id() const
{
   return "scale";
}

double LNScale::MinimumValue() const
{
   return 32;
}

double LNScale::MaximumValue() const
{
   return 65536;
}

double LNScale::DefaultValue() const
{
   return 128;
}

// ----------------------------------------------------------------------------

LNNoScale::LNNoScale( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNNoScaleParameter = this;
}

IsoString LNNoScale::Id() const
{
   return "noScale";
}

bool LNNoScale::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNRejection::LNRejection( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNRejectionParameter = this;
}

IsoString LNRejection::Id() const
{
   return "rejection";
}

bool LNRejection::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNBackgroundRejectionLimit::LNBackgroundRejectionLimit( MetaProcess* P ) : MetaFloat( P )
{
   TheLNBackgroundRejectionLimitParameter = this;
}

IsoString LNBackgroundRejectionLimit::Id() const
{
   return "backgroundRejectionLimit";
}

int LNBackgroundRejectionLimit::Precision() const
{
   return 3;
}

double LNBackgroundRejectionLimit::DefaultValue() const
{
   return 0.050;
}

double LNBackgroundRejectionLimit::MinimumValue() const
{
   return 0.001;
}

double LNBackgroundRejectionLimit::MaximumValue() const
{
   return 0.500;
}

// ----------------------------------------------------------------------------

LNReferenceRejectionThreshold::LNReferenceRejectionThreshold( MetaProcess* P ) : MetaFloat( P )
{
   TheLNReferenceRejectionThresholdParameter = this;
}

IsoString LNReferenceRejectionThreshold::Id() const
{
   return "referenceRejectionThreshold";
}

int LNReferenceRejectionThreshold::Precision() const
{
   return 3;
}

double LNReferenceRejectionThreshold::DefaultValue() const
{
   return 0.500;
}

double LNReferenceRejectionThreshold::MinimumValue() const
{
   return 0;
}

double LNReferenceRejectionThreshold::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

LNTargetRejectionThreshold::LNTargetRejectionThreshold( MetaProcess* P ) : MetaFloat( P )
{
   TheLNTargetRejectionThresholdParameter = this;
}

IsoString LNTargetRejectionThreshold::Id() const
{
   return "targetRejectionThreshold";
}

int LNTargetRejectionThreshold::Precision() const
{
   return 3;
}

double LNTargetRejectionThreshold::DefaultValue() const
{
   return 0.500;
}

double LNTargetRejectionThreshold::MinimumValue() const
{
   return 0;
}

double LNTargetRejectionThreshold::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

LNHotPixelFilterRadius::LNHotPixelFilterRadius( MetaProcess* p ) : MetaInt32( p )
{
   TheLNHotPixelFilterRadiusParameter = this;
}

IsoString LNHotPixelFilterRadius::Id() const
{
   return "hotPixelFilterRadius";
}

double LNHotPixelFilterRadius::DefaultValue() const
{
   return 2;
}

double LNHotPixelFilterRadius::MinimumValue() const
{
   return 0;
}

double LNHotPixelFilterRadius::MaximumValue() const
{
   return 4;
}

// ----------------------------------------------------------------------------

LNNoiseReductionFilterRadius::LNNoiseReductionFilterRadius( MetaProcess* p ) : MetaInt32( p )
{
   TheLNNoiseReductionFilterRadiusParameter = this;
}

IsoString LNNoiseReductionFilterRadius::Id() const
{
   return "noiseReductionFilterRadius";
}

double LNNoiseReductionFilterRadius::DefaultValue() const
{
   return 0;
}

double LNNoiseReductionFilterRadius::MinimumValue() const
{
   return 0;
}

double LNNoiseReductionFilterRadius::MaximumValue() const
{
   return 24;
}

// ----------------------------------------------------------------------------

LNReferencePathOrViewId::LNReferencePathOrViewId( MetaProcess* P ) : MetaString( P )
{
   TheLNReferencePathOrViewIdParameter = this;
}

IsoString LNReferencePathOrViewId::Id() const
{
   return "referencePathOrViewId";
}

// ----------------------------------------------------------------------------

LNReferenceIsView::LNReferenceIsView( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNReferenceIsViewParameter = this;
}

IsoString LNReferenceIsView::Id() const
{
   return "referenceIsView";
}

bool LNReferenceIsView::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNTargetItems::LNTargetItems( MetaProcess* P ) : MetaTable( P )
{
   TheLNTargetItemsParameter = this;
}

IsoString LNTargetItems::Id() const
{
   return "targetItems";
}

// ----------------------------------------------------------------------------

LNTargetEnabled::LNTargetEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheLNTargetEnabledParameter = this;
}

IsoString LNTargetEnabled::Id() const
{
   return "enabled";
}

bool LNTargetEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNTargetImage::LNTargetImage( MetaTable* T ) : MetaString( T )
{
   TheLNTargetImageParameter = this;
}

IsoString LNTargetImage::Id() const
{
   return "image";
}

// ----------------------------------------------------------------------------

LNInputHints::LNInputHints( MetaProcess* P ) : MetaString( P )
{
   TheLNInputHintsParameter = this;
}

IsoString LNInputHints::Id() const
{
   return "inputHints";
}

// ----------------------------------------------------------------------------

LNOutputHints::LNOutputHints( MetaProcess* P ) : MetaString( P )
{
   TheLNOutputHintsParameter = this;
}

IsoString LNOutputHints::Id() const
{
   return "outputHints";
}

// ----------------------------------------------------------------------------

LNGenerateNormalizedImages::LNGenerateNormalizedImages( MetaProcess* p ) : MetaEnumeration( p )
{
   TheLNGenerateNormalizedImagesParameter = this;
}

IsoString LNGenerateNormalizedImages::Id() const
{
   return "generateNormalizedImages";
}

size_type LNGenerateNormalizedImages::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString LNGenerateNormalizedImages::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Never:               return "GenerateNormalizedImages_Never";
   case Always:              return "GenerateNormalizedImages_Always";
   default:
   case ViewExecutionOnly:   return "GenerateNormalizedImages_ViewExecutionOnly";
   case GlobalExecutionOnly: return "GenerateNormalizedImages_GlobalExecutionOnly";
   }
}

int LNGenerateNormalizedImages::ElementValue( size_type i ) const
{
   return int( i );
}

size_type LNGenerateNormalizedImages::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

LNGenerateNormalizationData::LNGenerateNormalizationData( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNGenerateNormalizationDataParameter = this;
}

IsoString LNGenerateNormalizationData::Id() const
{
   return "generateNormalizationData";
}

bool LNGenerateNormalizationData::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNShowBackgroundModels::LNShowBackgroundModels( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNShowBackgroundModelsParameter = this;
}

IsoString LNShowBackgroundModels::Id() const
{
   return "showBackgroundModels";
}

bool LNShowBackgroundModels::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNShowRejectionMaps::LNShowRejectionMaps( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNShowRejectionMapsParameter = this;
}

IsoString LNShowRejectionMaps::Id() const
{
   return "showRejectionMaps";
}

bool LNShowRejectionMaps::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNPlotNormalizationFunctions::LNPlotNormalizationFunctions( MetaProcess* p ) : MetaEnumeration( p )
{
   TheLNPlotNormalizationFunctionsParameter = this;
}

IsoString LNPlotNormalizationFunctions::Id() const
{
   return "plotNormalizationFunctions";
}

size_type LNPlotNormalizationFunctions::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString LNPlotNormalizationFunctions::ElementId( size_type i ) const
{
   switch ( i )
   {
   case DontPlot:  return "PlotNormalizationFunctions_DontPlot";
   case Line3D:    return "PlotNormalizationFunctions_Line3D";
   default:
   case Palette3D: return "PlotNormalizationFunctions_Palette3D";
   case Map3D:     return "PlotNormalizationFunctions_Map3D";
   }
}

int LNPlotNormalizationFunctions::ElementValue( size_type i ) const
{
   return int( i );
}

size_type LNPlotNormalizationFunctions::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

LNNoGUIMessages::LNNoGUIMessages( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNNoGUIMessagesParameter = this;
}

IsoString LNNoGUIMessages::Id() const
{
   return "noGUIMessages";
}

bool LNNoGUIMessages::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNOutputDirectory::LNOutputDirectory( MetaProcess* P ) : MetaString( P )
{
   TheLNOutputDirectoryParameter = this;
}

IsoString LNOutputDirectory::Id() const
{
   return "outputDirectory";
}

// ----------------------------------------------------------------------------

LNOutputExtension::LNOutputExtension( MetaProcess* P ) : MetaString( P )
{
   TheLNOutputExtensionParameter = this;
}

IsoString LNOutputExtension::Id() const
{
   return "outputExtension";
}

String LNOutputExtension::DefaultValue() const
{
   return ".xisf";
}

// ----------------------------------------------------------------------------

LNOutputPrefix::LNOutputPrefix( MetaProcess* P ) : MetaString( P )
{
   TheLNOutputPrefixParameter = this;
}

IsoString LNOutputPrefix::Id() const
{
   return "outputPrefix";
}

String LNOutputPrefix::DefaultValue() const
{
   return String();
}

// ----------------------------------------------------------------------------

LNOutputPostfix::LNOutputPostfix( MetaProcess* P ) : MetaString( P )
{
   TheLNOutputPostfixParameter = this;
}

IsoString LNOutputPostfix::Id() const
{
   return "outputPostfix";
}

String LNOutputPostfix::DefaultValue() const
{
   return "_n";
}

// ----------------------------------------------------------------------------

LNOverwriteExistingFiles::LNOverwriteExistingFiles( MetaProcess* P ) : MetaBoolean( P )
{
   TheLNOverwriteExistingFilesParameter = this;
}

IsoString LNOverwriteExistingFiles::Id() const
{
   return "overwriteExistingFiles";
}

bool LNOverwriteExistingFiles::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNOnError::LNOnError( MetaProcess* p ) : MetaEnumeration( p )
{
   TheLNOnErrorParameter = this;
}

IsoString LNOnError::Id() const
{
   return "onError";
}

size_type LNOnError::NumberOfElements() const
{
   return NumberOfErrorPolicies;
}

IsoString LNOnError::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Continue: return "OnError_Continue";
   case Abort:    return "OnError_Abort";
   case AskUser:  return "OnError_AskUser";
   }
}

int LNOnError::ElementValue( size_type i ) const
{
   return int( i );
}

size_type LNOnError::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

LNUseFileThreads::LNUseFileThreads( MetaProcess* p ) : MetaBoolean( p )
{
   TheLNUseFileThreadsParameter = this;
}

IsoString LNUseFileThreads::Id() const
{
   return "useFileThreads";
}

bool LNUseFileThreads::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LNFileThreadOverload::LNFileThreadOverload( MetaProcess* p ) : MetaFloat( p )
{
   TheLNFileThreadOverloadParameter = this;
}

IsoString LNFileThreadOverload::Id() const
{
   return "fileThreadOverload";
}

int LNFileThreadOverload::Precision() const
{
   return 2;
}

double LNFileThreadOverload::DefaultValue() const
{
   return 1.2;
}

double LNFileThreadOverload::MinimumValue() const
{
   return 1;
}

double LNFileThreadOverload::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

LNMaxFileReadThreads::LNMaxFileReadThreads( MetaProcess* p ) : MetaInt32( p )
{
   TheLNMaxFileReadThreadsParameter = this;
}

IsoString LNMaxFileReadThreads::Id() const
{
   return "maxFileReadThreads";
}

double LNMaxFileReadThreads::DefaultValue() const
{
   return 1;
}

double LNMaxFileReadThreads::MinimumValue() const
{
   return 1;
}

double LNMaxFileReadThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

LNMaxFileWriteThreads::LNMaxFileWriteThreads( MetaProcess* p ) : MetaInt32( p )
{
   TheLNMaxFileWriteThreadsParameter = this;
}

IsoString LNMaxFileWriteThreads::Id() const
{
   return "maxFileWriteThreads";
}

double LNMaxFileWriteThreads::DefaultValue() const
{
   return 1;
}

double LNMaxFileWriteThreads::MinimumValue() const
{
   return 1;
}

double LNMaxFileWriteThreads::MaximumValue() const
{
   return 1024;
}

// ----------------------------------------------------------------------------

LNGraphSize::LNGraphSize( MetaProcess* p ) : MetaInt32( p )
{
   TheLNGraphSizeParameter = this;
}

IsoString LNGraphSize::Id() const
{
   return "graphSize";
}

double LNGraphSize::DefaultValue() const
{
   return 800;
}

double LNGraphSize::MinimumValue() const
{
   return 256;
}

double LNGraphSize::MaximumValue() const
{
   return 8192;
}

// ----------------------------------------------------------------------------

LNGraphTextSize::LNGraphTextSize( MetaProcess* p ) : MetaInt32( p )
{
   TheLNGraphTextSizeParameter = this;
}

IsoString LNGraphTextSize::Id() const
{
   return "graphTextSize";
}

double LNGraphTextSize::DefaultValue() const
{
   return 12;
}

double LNGraphTextSize::MinimumValue() const
{
   return 8;
}

double LNGraphTextSize::MaximumValue() const
{
   return 64;
}

// ----------------------------------------------------------------------------

LNGraphTitleSize::LNGraphTitleSize( MetaProcess* p ) : MetaInt32( p )
{
   TheLNGraphTitleSizeParameter = this;
}

IsoString LNGraphTitleSize::Id() const
{
   return "graphTitleSize";
}

double LNGraphTitleSize::DefaultValue() const
{
   return 18;
}

double LNGraphTitleSize::MinimumValue() const
{
   return 8;
}

double LNGraphTitleSize::MaximumValue() const
{
   return 64;
}

// ----------------------------------------------------------------------------

LNGraphTransparent::LNGraphTransparent( MetaProcess* p ) : MetaBoolean( p )
{
   TheLNGraphTransparentParameter = this;
}

IsoString LNGraphTransparent::Id() const
{
   return "graphTransparent";
}

bool LNGraphTransparent::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LNGraphOutputDirectory::LNGraphOutputDirectory( MetaProcess* P ) : MetaString( P )
{
   TheLNGraphOutputDirectoryParameter = this;
}

IsoString LNGraphOutputDirectory::Id() const
{
   return "graphOutputDirectory";
}

String LNGraphOutputDirectory::DefaultValue() const
{
   return String(); // use TMPDIR by default
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LocalNormalizationParameters.cpp - Released 2019-01-21T12:06:41Z
