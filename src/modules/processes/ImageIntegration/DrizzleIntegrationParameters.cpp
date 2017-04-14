//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.12.01.0368
// ----------------------------------------------------------------------------
// DrizzleIntegrationParameters.cpp - Released 2017-04-14T23:07:12Z
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

#include "DrizzleIntegrationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

DZInputData*                TheDZInputDataParameter = nullptr;
DZItemEnabled*              TheDZItemEnabledParameter = nullptr;
DZItemPath*                 TheDZItemPathParameter = nullptr;
DZInputHints*               TheDZInputHintsParameter = nullptr;
DZInputDirectory*           TheDZInputDirectoryParameter = nullptr;
DZScale*                    TheDZScaleParameter = nullptr;
DZDropShrink*               TheDZDropShrinkParameter = nullptr;
DZKernelFunction*           TheDZKernelFunctionParameter = nullptr;
DZKernelGridSize*           TheDZKernelGridSizeParameter = nullptr;
DZOriginX*                  TheDZOriginXParameter = nullptr;
DZOriginY*                  TheDZOriginYParameter = nullptr;
DZEnableRejection*          TheDZEnableRejectionParameter = nullptr;
DZEnableImageWeighting*     TheDZEnableImageWeightingParameter = nullptr;
DZEnableSurfaceSplines*     TheDZEnableSurfaceSplinesParameter = nullptr;
DZUseROI*                   TheDZUseROIParameter = nullptr;
DZROIX0*                    TheDZROIX0Parameter = nullptr;
DZROIY0*                    TheDZROIY0Parameter = nullptr;
DZROIX1*                    TheDZROIX1Parameter = nullptr;
DZROIY1*                    TheDZROIY1Parameter = nullptr;
DZClosePreviousImages*      TheDZClosePreviousImagesParameter = nullptr;
DZNoGUIMessages*            TheDZNoGUIMessagesParameter = nullptr;
DZOnError*                  TheDZOnErrorParameter = nullptr;

DZIntegrationImageId*       TheDZIntegrationImageIdParameter = nullptr;
DZWeightImageId*            TheDZWeightImageIdParameter = nullptr;
DZNumberOfChannels*         TheDZNumberOfChannelsParameter = nullptr;
DZOutputPixels*             TheDZOutputPixelsParameter = nullptr;
DZIntegratedPixels*         TheDZIntegratedPixelsParameter = nullptr;
DZTotalRejectedLowRK*       TheDZTotalRejectedLowRKParameter = nullptr;
DZTotalRejectedLowG*        TheDZTotalRejectedLowGParameter = nullptr;
DZTotalRejectedLowB*        TheDZTotalRejectedLowBParameter = nullptr;
DZTotalRejectedHighRK*      TheDZTotalRejectedHighRKParameter = nullptr;
DZTotalRejectedHighG*       TheDZTotalRejectedHighGParameter = nullptr;
DZTotalRejectedHighB*       TheDZTotalRejectedHighBParameter = nullptr;
DZOutputData*               TheDZOutputDataParameter = nullptr;
DZImageData*                TheDZImageDataParameter = nullptr;
DZImageFilePath*            TheDZImageFilePathParameter = nullptr;
DZImageWeightRK*            TheDZImageWeightRKParameter = nullptr;
DZImageWeightG*             TheDZImageWeightGParameter = nullptr;
DZImageWeightB*             TheDZImageWeightBParameter = nullptr;
DZImageLocationRK*          TheDZImageLocationRKParameter = nullptr;
DZImageLocationG*           TheDZImageLocationGParameter = nullptr;
DZImageLocationB*           TheDZImageLocationBParameter = nullptr;
DZImageReferenceLocationRK* TheDZImageReferenceLocationRKParameter = nullptr;
DZImageReferenceLocationG*  TheDZImageReferenceLocationGParameter = nullptr;
DZImageReferenceLocationB*  TheDZImageReferenceLocationBParameter = nullptr;
DZImageScaleRK*             TheDZImageScaleRKParameter = nullptr;
DZImageScaleG*              TheDZImageScaleGParameter = nullptr;
DZImageScaleB*              TheDZImageScaleBParameter = nullptr;
DZImageRejectedLowRK*       TheDZImageRejectedLowRKParameter = nullptr;
DZImageRejectedLowG*        TheDZImageRejectedLowGParameter = nullptr;
DZImageRejectedLowB*        TheDZImageRejectedLowBParameter = nullptr;
DZImageRejectedHighRK*      TheDZImageRejectedHighRKParameter = nullptr;
DZImageRejectedHighG*       TheDZImageRejectedHighGParameter = nullptr;
DZImageRejectedHighB*       TheDZImageRejectedHighBParameter = nullptr;
DZImageOutputData*          TheDZImageOutputDataParameter = nullptr;

// ----------------------------------------------------------------------------

DZInputData::DZInputData( MetaProcess* P ) : MetaTable( P )
{
   TheDZInputDataParameter = this;
}

IsoString DZInputData::Id() const
{
   return "inputData";
}

// ----------------------------------------------------------------------------

DZItemEnabled::DZItemEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheDZItemEnabledParameter = this;
}

IsoString DZItemEnabled::Id() const
{
   return "enabled";
}

bool DZItemEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZItemPath::DZItemPath( MetaTable* T ) : MetaString( T )
{
   TheDZItemPathParameter = this;
}

IsoString DZItemPath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------

DZInputHints::DZInputHints( MetaProcess* P ) : MetaString( P )
{
   TheDZInputHintsParameter = this;
}

IsoString DZInputHints::Id() const
{
   return "inputHints";
}

// ----------------------------------------------------------------------------

DZInputDirectory::DZInputDirectory( MetaProcess* P ) : MetaString( P )
{
   TheDZInputDirectoryParameter = this;
}

IsoString DZInputDirectory::Id() const
{
   return "inputDirectory";
}

// ----------------------------------------------------------------------------

DZScale::DZScale( MetaProcess* P ) : MetaFloat( P )
{
   TheDZScaleParameter = this;
}

IsoString DZScale::Id() const
{
   return "scale";
}

int DZScale::Precision() const
{
   return 2;
}

double DZScale::DefaultValue() const
{
   return 2;
}

double DZScale::MinimumValue() const
{
   return 1;
}

double DZScale::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

DZDropShrink::DZDropShrink( MetaProcess* P ) : MetaFloat( P )
{
   TheDZDropShrinkParameter = this;
}

IsoString DZDropShrink::Id() const
{
   return "dropShrink";
}

int DZDropShrink::Precision() const
{
   return 2;
}

double DZDropShrink::DefaultValue() const
{
   return 0.90;
}

double DZDropShrink::MinimumValue() const
{
   return 0.1;
}

double DZDropShrink::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

DZKernelFunction::DZKernelFunction( MetaProcess* p ) : MetaEnumeration( p )
{
   TheDZKernelFunctionParameter = this;
}

IsoString DZKernelFunction::Id() const
{
   return "kernelFunction";
}

size_type DZKernelFunction::NumberOfElements() const
{
   return Count;
}

IsoString DZKernelFunction::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Square:     return "Kernel_Square";
   case Circular:   return "Kernel_Circular";
   case Gaussian:   return "Kernel_Gaussian";
   case Variable10: return "Kernel_Variable10";
   case Variable15: return "Kernel_Variable15";
   case Variable30: return "Kernel_Variable30";
   case Variable40: return "Kernel_Variable40";
   case Variable50: return "Kernel_Variable50";
   case Variable60: return "Kernel_Variable60";
   }
}

int DZKernelFunction::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DZKernelFunction::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DZKernelGridSize::DZKernelGridSize( MetaProcess* P ) : MetaInt32( P )
{
   TheDZKernelGridSizeParameter = this;
}

IsoString DZKernelGridSize::Id() const
{
   return "kernelGridSize";
}

double DZKernelGridSize::DefaultValue() const
{
   return 16;
}

double DZKernelGridSize::MinimumValue() const
{
   return 4;
}

double DZKernelGridSize::MaximumValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

DZOriginX::DZOriginX( MetaProcess* P ) : MetaFloat( P )
{
   TheDZOriginXParameter = this;
}

IsoString DZOriginX::Id() const
{
   return "originX";
}

int DZOriginX::Precision() const
{
   return 2;
}

double DZOriginX::DefaultValue() const
{
   return 0.50;
}

double DZOriginX::MinimumValue() const
{
   return 0.00;
}

double DZOriginX::MaximumValue() const
{
   return 1.00;
}

// ----------------------------------------------------------------------------

DZOriginY::DZOriginY( MetaProcess* P ) : MetaFloat( P )
{
   TheDZOriginYParameter = this;
}

IsoString DZOriginY::Id() const
{
   return "originY";
}

int DZOriginY::Precision() const
{
   return 2;
}

double DZOriginY::DefaultValue() const
{
   return 0.50;
}

double DZOriginY::MinimumValue() const
{
   return 0.00;
}

double DZOriginY::MaximumValue() const
{
   return 1.00;
}

// ----------------------------------------------------------------------------

DZEnableRejection::DZEnableRejection( MetaProcess* P ) : MetaBoolean( P )
{
   TheDZEnableRejectionParameter = this;
}

IsoString DZEnableRejection::Id() const
{
   return "enableRejection";
}

bool DZEnableRejection::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZEnableImageWeighting::DZEnableImageWeighting( MetaProcess* P ) : MetaBoolean( P )
{
   TheDZEnableImageWeightingParameter = this;
}

IsoString DZEnableImageWeighting::Id() const
{
   return "enableImageWeighting";
}

bool DZEnableImageWeighting::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZEnableSurfaceSplines::DZEnableSurfaceSplines( MetaProcess* P ) : MetaBoolean( P )
{
   TheDZEnableSurfaceSplinesParameter = this;
}

IsoString DZEnableSurfaceSplines::Id() const
{
   return "enableSurfaceSplines";
}

bool DZEnableSurfaceSplines::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZUseROI::DZUseROI( MetaProcess* P ) : MetaBoolean( P )
{
   TheDZUseROIParameter = this;
}

IsoString DZUseROI::Id() const
{
   return "useROI";
}

bool DZUseROI::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DZROIX0::DZROIX0( MetaProcess* P ) : MetaInt32( P )
{
   TheDZROIX0Parameter = this;
}

IsoString DZROIX0::Id() const
{
   return "roiX0";
}

double DZROIX0::DefaultValue() const
{
   return 0;
}

double DZROIX0::MinimumValue() const
{
   return 0;
}

double DZROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

DZROIY0::DZROIY0( MetaProcess* P ) : MetaInt32( P )
{
   TheDZROIY0Parameter = this;
}

IsoString DZROIY0::Id() const
{
   return "roiY0";
}

double DZROIY0::DefaultValue() const
{
   return 0;
}

double DZROIY0::MinimumValue() const
{
   return 0;
}

double DZROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

DZROIX1::DZROIX1( MetaProcess* P ) : MetaInt32( P )
{
   TheDZROIX1Parameter = this;
}

IsoString DZROIX1::Id() const
{
   return "roiX1";
}

double DZROIX1::DefaultValue() const
{
   return 0;
}

double DZROIX1::MinimumValue() const
{
   return 0;
}

double DZROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

DZROIY1::DZROIY1( MetaProcess* P ) : MetaInt32( P )
{
   TheDZROIY1Parameter = this;
}

IsoString DZROIY1::Id() const
{
   return "roiY1";
}

double DZROIY1::DefaultValue() const
{
   return 0;
}

double DZROIY1::MinimumValue() const
{
   return 0;
}

double DZROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

DZClosePreviousImages::DZClosePreviousImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheDZClosePreviousImagesParameter = this;
}

IsoString DZClosePreviousImages::Id() const
{
   return "closePreviousImages";
}

bool DZClosePreviousImages::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DZNoGUIMessages::DZNoGUIMessages( MetaProcess* p ) : MetaBoolean( p )
{
   TheDZNoGUIMessagesParameter = this;
}

IsoString DZNoGUIMessages::Id() const
{
   return "noGUIMessages";
}

bool DZNoGUIMessages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZOnError::DZOnError( MetaProcess* p ) : MetaEnumeration( p )
{
   TheDZOnErrorParameter = this;
}

IsoString DZOnError::Id() const
{
   return "onError";
}

size_type DZOnError::NumberOfElements() const
{
   return Count;
}

IsoString DZOnError::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Continue: return "Continue";
   case Abort:    return "Abort";
   case AskUser:  return "AskUser";
   }
}

int DZOnError::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DZOnError::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Output properties
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DZIntegrationImageId::DZIntegrationImageId( MetaProcess* P ) : MetaString( P )
{
   TheDZIntegrationImageIdParameter = this;
}

IsoString DZIntegrationImageId::Id() const
{
   return "integrationImageId";
}

bool DZIntegrationImageId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZWeightImageId::DZWeightImageId( MetaProcess* P ) : MetaString( P )
{
   TheDZWeightImageIdParameter = this;
}

IsoString DZWeightImageId::Id() const
{
   return "weightImageId";
}

bool DZWeightImageId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZNumberOfChannels::DZNumberOfChannels( MetaProcess* P ) : MetaInt32( P )
{
   TheDZNumberOfChannelsParameter = this;
}

IsoString DZNumberOfChannels::Id() const
{
   return "numberOfChannels";
}

bool DZNumberOfChannels::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZOutputPixels::DZOutputPixels( MetaProcess* P ) : MetaUInt64( P )
{
   TheDZOutputPixelsParameter = this;
}

IsoString DZOutputPixels::Id() const
{
   return "outputPixels";
}

bool DZOutputPixels::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZIntegratedPixels::DZIntegratedPixels( MetaProcess* P ) : MetaUInt64( P )
{
   TheDZIntegratedPixelsParameter = this;
}

IsoString DZIntegratedPixels::Id() const
{
   return "integratedPixels";
}

bool DZIntegratedPixels::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZTotalRejectedLowRK::DZTotalRejectedLowRK( MetaProcess* P ) : MetaUInt64( P )
{
   TheDZTotalRejectedLowRKParameter = this;
}

IsoString DZTotalRejectedLowRK::Id() const
{
   return "totalRejectedLowRK";
}

bool DZTotalRejectedLowRK::IsReadOnly() const
{
   return true;
}

DZTotalRejectedLowG::DZTotalRejectedLowG( MetaProcess* P ) : MetaUInt64( P )
{
   TheDZTotalRejectedLowGParameter = this;
}

IsoString DZTotalRejectedLowG::Id() const
{
   return "totalRejectedLowG";
}

bool DZTotalRejectedLowG::IsReadOnly() const
{
   return true;
}

DZTotalRejectedLowB::DZTotalRejectedLowB( MetaProcess* P ) : MetaUInt64( P )
{
   TheDZTotalRejectedLowBParameter = this;
}

IsoString DZTotalRejectedLowB::Id() const
{
   return "totalRejectedLowB";
}

bool DZTotalRejectedLowB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZTotalRejectedHighRK::DZTotalRejectedHighRK( MetaProcess* P ) : MetaUInt64( P )
{
   TheDZTotalRejectedHighRKParameter = this;
}

IsoString DZTotalRejectedHighRK::Id() const
{
   return "totalRejectedHighRK";
}

bool DZTotalRejectedHighRK::IsReadOnly() const
{
   return true;
}

DZTotalRejectedHighG::DZTotalRejectedHighG( MetaProcess* P ) : MetaUInt64( P )
{
   TheDZTotalRejectedHighGParameter = this;
}

IsoString DZTotalRejectedHighG::Id() const
{
   return "totalRejectedHighG";
}

bool DZTotalRejectedHighG::IsReadOnly() const
{
   return true;
}

DZTotalRejectedHighB::DZTotalRejectedHighB( MetaProcess* P ) : MetaUInt64( P )
{
   TheDZTotalRejectedHighBParameter = this;
}

IsoString DZTotalRejectedHighB::Id() const
{
   return "totalRejectedHighB";
}

bool DZTotalRejectedHighB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZOutputData::DZOutputData( MetaProcess* P ) : MetaFloat( P )
{
   TheDZOutputDataParameter = this;
}

IsoString DZOutputData::Id() const
{
   return "totalData";
}

int DZOutputData::Precision() const
{
   return 3;
}

bool DZOutputData::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZImageData::DZImageData( MetaProcess* P ) : MetaTable( P )
{
   TheDZImageDataParameter = this;
}

IsoString DZImageData::Id() const
{
   return "imageData";
}

bool DZImageData::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZImageFilePath::DZImageFilePath( MetaTable* T ) : MetaString( T )
{
   TheDZImageFilePathParameter = this;
}

IsoString DZImageFilePath::Id() const
{
   return "filePath";
}

bool DZImageFilePath::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZImageWeightRK::DZImageWeightRK( MetaTable* T ) : MetaFloat( T )
{
   TheDZImageWeightRKParameter = this;
}

IsoString DZImageWeightRK::Id() const
{
   return "weightRK";
}

int DZImageWeightRK::Precision() const
{
   return 5;
}

bool DZImageWeightRK::IsReadOnly() const
{
   return true;
}

DZImageWeightG::DZImageWeightG( MetaTable* T ) : MetaFloat( T )
{
   TheDZImageWeightGParameter = this;
}

IsoString DZImageWeightG::Id() const
{
   return "weightG";
}

int DZImageWeightG::Precision() const
{
   return 5;
}

bool DZImageWeightG::IsReadOnly() const
{
   return true;
}

DZImageWeightB::DZImageWeightB( MetaTable* T ) : MetaFloat( T )
{
   TheDZImageWeightBParameter = this;
}

IsoString DZImageWeightB::Id() const
{
   return "weightB";
}

int DZImageWeightB::Precision() const
{
   return 5;
}

bool DZImageWeightB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZImageLocationRK::DZImageLocationRK( MetaTable* T ) : MetaDouble( T )
{
   TheDZImageLocationRKParameter = this;
}

IsoString DZImageLocationRK::Id() const
{
   return "locationRK";
}

int DZImageLocationRK::Precision() const
{
   return 3;
}

bool DZImageLocationRK::ScientificNotation() const
{
   return true;
}

bool DZImageLocationRK::IsReadOnly() const
{
   return true;
}

DZImageLocationG::DZImageLocationG( MetaTable* T ) : MetaDouble( T )
{
   TheDZImageLocationGParameter = this;
}

IsoString DZImageLocationG::Id() const
{
   return "locationG";
}

int DZImageLocationG::Precision() const
{
   return 3;
}

bool DZImageLocationG::ScientificNotation() const
{
   return true;
}

bool DZImageLocationG::IsReadOnly() const
{
   return true;
}

DZImageLocationB::DZImageLocationB( MetaTable* T ) : MetaDouble( T )
{
   TheDZImageLocationBParameter = this;
}

IsoString DZImageLocationB::Id() const
{
   return "locationB";
}

int DZImageLocationB::Precision() const
{
   return 3;
}

bool DZImageLocationB::ScientificNotation() const
{
   return true;
}

bool DZImageLocationB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZImageReferenceLocationRK::DZImageReferenceLocationRK( MetaTable* T ) : MetaDouble( T )
{
   TheDZImageReferenceLocationRKParameter = this;
}

IsoString DZImageReferenceLocationRK::Id() const
{
   return "referenceLocationRK";
}

int DZImageReferenceLocationRK::Precision() const
{
   return 3;
}

bool DZImageReferenceLocationRK::ScientificNotation() const
{
   return true;
}

bool DZImageReferenceLocationRK::IsReadOnly() const
{
   return true;
}

DZImageReferenceLocationG::DZImageReferenceLocationG( MetaTable* T ) : MetaDouble( T )
{
   TheDZImageReferenceLocationGParameter = this;
}

IsoString DZImageReferenceLocationG::Id() const
{
   return "referenceLocationG";
}

int DZImageReferenceLocationG::Precision() const
{
   return 3;
}

bool DZImageReferenceLocationG::ScientificNotation() const
{
   return true;
}

bool DZImageReferenceLocationG::IsReadOnly() const
{
   return true;
}

DZImageReferenceLocationB::DZImageReferenceLocationB( MetaTable* T ) : MetaDouble( T )
{
   TheDZImageReferenceLocationBParameter = this;
}

IsoString DZImageReferenceLocationB::Id() const
{
   return "referenceLocationB";
}

int DZImageReferenceLocationB::Precision() const
{
   return 3;
}

bool DZImageReferenceLocationB::ScientificNotation() const
{
   return true;
}

bool DZImageReferenceLocationB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZImageScaleRK::DZImageScaleRK( MetaTable* T ) : MetaDouble( T )
{
   TheDZImageScaleRKParameter = this;
}

IsoString DZImageScaleRK::Id() const
{
   return "scalingFactorRK";
}

int DZImageScaleRK::Precision() const
{
   return 3;
}

bool DZImageScaleRK::ScientificNotation() const
{
   return true;
}

bool DZImageScaleRK::IsReadOnly() const
{
   return true;
}

DZImageScaleG::DZImageScaleG( MetaTable* T ) : MetaDouble( T )
{
   TheDZImageScaleGParameter = this;
}

IsoString DZImageScaleG::Id() const
{
   return "scalingFactorG";
}

int DZImageScaleG::Precision() const
{
   return 3;
}

bool DZImageScaleG::ScientificNotation() const
{
   return true;
}

bool DZImageScaleG::IsReadOnly() const
{
   return true;
}

DZImageScaleB::DZImageScaleB( MetaTable* T ) : MetaDouble( T )
{
   TheDZImageScaleBParameter = this;
}

IsoString DZImageScaleB::Id() const
{
   return "scalingFactorB";
}

int DZImageScaleB::Precision() const
{
   return 3;
}

bool DZImageScaleB::ScientificNotation() const
{
   return true;
}

bool DZImageScaleB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZImageRejectedLowRK::DZImageRejectedLowRK( MetaTable* T ) : MetaUInt64( T )
{
   TheDZImageRejectedLowRKParameter = this;
}

IsoString DZImageRejectedLowRK::Id() const
{
   return "rejectedLowRK";
}

bool DZImageRejectedLowRK::IsReadOnly() const
{
   return true;
}

DZImageRejectedLowG::DZImageRejectedLowG( MetaTable* T ) : MetaUInt64( T )
{
   TheDZImageRejectedLowGParameter = this;
}

IsoString DZImageRejectedLowG::Id() const
{
   return "rejectedLowG";
}

bool DZImageRejectedLowG::IsReadOnly() const
{
   return true;
}

DZImageRejectedLowB::DZImageRejectedLowB( MetaTable* T ) : MetaUInt64( T )
{
   TheDZImageRejectedLowBParameter = this;
}

IsoString DZImageRejectedLowB::Id() const
{
   return "rejectedLowB";
}

bool DZImageRejectedLowB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZImageRejectedHighRK::DZImageRejectedHighRK( MetaTable* T ) : MetaUInt64( T )
{
   TheDZImageRejectedHighRKParameter = this;
}

IsoString DZImageRejectedHighRK::Id() const
{
   return "rejectedHighRK";
}

bool DZImageRejectedHighRK::IsReadOnly() const
{
   return true;
}

DZImageRejectedHighG::DZImageRejectedHighG( MetaTable* T ) : MetaUInt64( T )
{
   TheDZImageRejectedHighGParameter = this;
}

IsoString DZImageRejectedHighG::Id() const
{
   return "rejectedHighG";
}

bool DZImageRejectedHighG::IsReadOnly() const
{
   return true;
}

DZImageRejectedHighB::DZImageRejectedHighB( MetaTable* T ) : MetaUInt64( T )
{
   TheDZImageRejectedHighBParameter = this;
}

IsoString DZImageRejectedHighB::Id() const
{
   return "rejectedHighB";
}

bool DZImageRejectedHighB::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

DZImageOutputData::DZImageOutputData( MetaTable* T ) : MetaFloat( T )
{
   TheDZImageOutputDataParameter = this;
}

IsoString DZImageOutputData::Id() const
{
   return "totalData";
}

int DZImageOutputData::Precision() const
{
   return 3;
}

bool DZImageOutputData::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DrizzleIntegrationParameters.cpp - Released 2017-04-14T23:07:12Z
