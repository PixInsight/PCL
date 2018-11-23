//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.03.0326
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationParameters.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "PhotometricColorCalibrationParameters.h"
#include "PhotometricColorCalibrationProcess.h"

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PCCWhiteReferenceId*               ThePCCWhiteReferenceIdParameter = nullptr;
PCCWhiteReferenceName*             ThePCCWhiteReferenceNameParameter = nullptr;
PCCWhiteReferenceSr_JV*            ThePCCWhiteReferenceSr_JVParameter = nullptr;
PCCWhiteReferenceJB_JV*            ThePCCWhiteReferenceJB_JVParameter = nullptr;
PCCZeroPointSr_JV*                 ThePCCZeroPointSr_JVParameter = nullptr;
PCCZeroPointJB_JV*                 ThePCCZeroPointJB_JVParameter = nullptr;

PCCFocalLength*                    ThePCCFocalLengthParameter = nullptr;
PCCPixelSize*                      ThePCCPixelSizeParameter = nullptr;
PCCCenterRA*                       ThePCCCenterRAParameter = nullptr;
PCCCenterDec*                      ThePCCCenterDecParameter = nullptr;
PCCEpochJD*                        ThePCCEpochJDParameter = nullptr;
PCCForcePlateSolve*                ThePCCForcePlateSolveParameter = nullptr;
PCCIgnoreImagePositionAndScale*    ThePCCIgnoreImagePositionAndScaleParameter = nullptr;
PCCServerURL*                      ThePCCServerURLParameter = nullptr;
PCCSolverCatalogName*              ThePCCSolverCatalogNameParameter = nullptr;
PCCSolverAutoCatalog*              ThePCCSolverAutoCatalogParameter = nullptr;
PCCSolverLimitMagnitude*           ThePCCSolverLimitMagnitudeParameter = nullptr;
PCCSolverAutoLimitMagnitude*       ThePCCSolverAutoLimitMagnitudeParameter = nullptr;
PCCSolverAutoLimitMagnitudeFactor* ThePCCSolverAutoLimitMagnitudeFactorParameter= nullptr;
PCCSolverStarSensitivity*          ThePCCSolverStarSensitivityParameter = nullptr;
PCCSolverNoiseLayers*              ThePCCSolverNoiseLayersParameter = nullptr;
PCCSolverAlignmentDevice*          ThePCCSolverAlignmentDeviceParameter = nullptr;
PCCSolverDistortionCorrection*     ThePCCSolverDistortionCorrectionParameter = nullptr;
PCCSolverSplineSmoothing*          ThePCCSolverSplineSmoothingParameter = nullptr;
PCCSolverProjection*               ThePCCSolverProjectionParameter = nullptr;

PCCPhotCatalogName*                ThePCCPhotCatalogNameParameter = nullptr;
PCCPhotLimitMagnitude*             ThePCCPhotLimitMagnitudeParameter = nullptr;
PCCPhotAutoLimitMagnitude*         ThePCCPhotAutoLimitMagnitudeParameter = nullptr;
PCCPhotAutoLimitMagnitudeFactor*   ThePCCPhotAutoLimitMagnitudeFactorParameter= nullptr;
PCCPhotAutoAperture*               ThePCCPhotAutoApertureParameter = nullptr;
PCCPhotAperture*                   ThePCCPhotApertureParameter = nullptr;
PCCPhotUsePSF*                     ThePCCPhotUsePSFParameter = nullptr;
PCCPhotSaturationThreshold*        ThePCCPhotSaturationThresholdParameter = nullptr;
PCCPhotShowDetectedStars*          ThePCCPhotShowDetectedStarsParameter = nullptr;
PCCPhotShowBackgroundModels*       ThePCCPhotShowBackgroundModelsParameter = nullptr;
PCCPhotGenerateGraphs*             ThePCCPhotGenerateGraphsParameter = nullptr;

PCCApplyCalibration*               ThePCCApplyCalibrationParameter = nullptr;

PCCNeutralizeBackground*           ThePCCNeutralizeBackgroundParameter = nullptr;
PCCBackgroundReferenceViewId*      ThePCCBackgroundReferenceViewIdParameter = nullptr;
PCCBackgroundLow*                  ThePCCBackgroundLowParameter = nullptr;
PCCBackgroundHigh*                 ThePCCBackgroundHighParameter = nullptr;
PCCBackgroundUseROI*               ThePCCBackgroundUseROIParameter = nullptr;
PCCBackgroundROIX0*                ThePCCBackgroundROIX0Parameter = nullptr;
PCCBackgroundROIY0*                ThePCCBackgroundROIY0Parameter = nullptr;
PCCBackgroundROIX1*                ThePCCBackgroundROIX1Parameter = nullptr;
PCCBackgroundROIY1*                ThePCCBackgroundROIY1Parameter = nullptr;

// ----------------------------------------------------------------------------

PCCWhiteReferenceId::PCCWhiteReferenceId( MetaProcess* P ) : MetaString( P )
{
   ThePCCWhiteReferenceIdParameter = this;
}

IsoString PCCWhiteReferenceId::Id() const
{
   return "whiteReferenceId";
}

String PCCWhiteReferenceId::DefaultValue() const
{
   if ( API )
   {
      PhotometricColorCalibrationProcess::InitializeWhiteReferences( false/*interactive*/ );
      if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
         return PhotometricColorCalibrationProcess::DefaultWhiteReference().id;
   }
   return "S";
}

// ----------------------------------------------------------------------------

PCCWhiteReferenceName::PCCWhiteReferenceName( MetaProcess* P ) : MetaString( P )
{
   ThePCCWhiteReferenceNameParameter = this;
}

IsoString PCCWhiteReferenceName::Id() const
{
   return "whiteReferenceName";
}

String PCCWhiteReferenceName::DefaultValue() const
{
   if ( API )
   {
      PhotometricColorCalibrationProcess::InitializeWhiteReferences( false/*interactive*/ );
      if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
         return PhotometricColorCalibrationProcess::DefaultWhiteReference().name;
   }
   return "Average Spiral Galaxy";
}

// ----------------------------------------------------------------------------

PCCWhiteReferenceSr_JV::PCCWhiteReferenceSr_JV( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCWhiteReferenceSr_JVParameter = this;
}

IsoString PCCWhiteReferenceSr_JV::Id() const
{
   return "whiteReferenceSr_JV";
}

int PCCWhiteReferenceSr_JV::Precision() const
{
   return 5;
}

double PCCWhiteReferenceSr_JV::DefaultValue() const
{
   if ( API )
   {
      PhotometricColorCalibrationProcess::InitializeWhiteReferences( false/*interactive*/ );
      if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
         return PhotometricColorCalibrationProcess::DefaultWhiteReference().Sr_JV;
   }
   return -0.37433; // average spiral
}

// ----------------------------------------------------------------------------

PCCWhiteReferenceJB_JV::PCCWhiteReferenceJB_JV( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCWhiteReferenceJB_JVParameter = this;
}

IsoString PCCWhiteReferenceJB_JV::Id() const
{
   return "whiteReferenceJB_JV";
}

int PCCWhiteReferenceJB_JV::Precision() const
{
   return 5;
}

double PCCWhiteReferenceJB_JV::DefaultValue() const
{
   if ( API )
   {
      PhotometricColorCalibrationProcess::InitializeWhiteReferences( false/*interactive*/ );
      if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
         return PhotometricColorCalibrationProcess::DefaultWhiteReference().JB_JV;
   }
   return +0.28594; // average spiral
}

// ----------------------------------------------------------------------------

PCCZeroPointSr_JV::PCCZeroPointSr_JV( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCZeroPointSr_JVParameter = this;
}

IsoString PCCZeroPointSr_JV::Id() const
{
   return "zeroPointSr_JV";
}

int PCCZeroPointSr_JV::Precision() const
{
   return 5;
}

double PCCZeroPointSr_JV::DefaultValue() const
{
   if ( API )
   {
      PhotometricColorCalibrationProcess::InitializeWhiteReferences( false/*interactive*/ );
      if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
         return PhotometricColorCalibrationProcess::ZeroPoint().Sr_JV;
   }
   return -0.14656;
}

// ----------------------------------------------------------------------------

PCCZeroPointJB_JV::PCCZeroPointJB_JV( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCZeroPointJB_JVParameter = this;
}

IsoString PCCZeroPointJB_JV::Id() const
{
   return "zeroPointJB_JV";
}

int PCCZeroPointJB_JV::Precision() const
{
   return 5;
}

double PCCZeroPointJB_JV::DefaultValue() const
{
   if ( API )
   {
      PhotometricColorCalibrationProcess::InitializeWhiteReferences( false/*interactive*/ );
      if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
         return PhotometricColorCalibrationProcess::ZeroPoint().JB_JV;
   }
   return -0.42432;
}

// ----------------------------------------------------------------------------

PCCFocalLength::PCCFocalLength( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCFocalLengthParameter = this;
}

IsoString PCCFocalLength::Id() const
{
   return "focalLength";
}

int PCCFocalLength::Precision() const
{
   return 2;
}

double PCCFocalLength::MinimumValue() const
{
   return 0.10;
}

double PCCFocalLength::MaximumValue() const
{
   return int_max;
}

double PCCFocalLength::DefaultValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

PCCPixelSize::PCCPixelSize( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCPixelSizeParameter = this;
}

IsoString PCCPixelSize::Id() const
{
   return "pixelSize";
}

int PCCPixelSize::Precision() const
{
   return 2;
}

double PCCPixelSize::MinimumValue() const
{
   return 0.10;
}

double PCCPixelSize::MaximumValue() const
{
   return 1000;
}

double PCCPixelSize::DefaultValue() const
{
   return 9;
}

// ----------------------------------------------------------------------------

PCCCenterRA::PCCCenterRA( MetaProcess* P ) : MetaDouble( P )
{
   ThePCCCenterRAParameter = this;
}

IsoString PCCCenterRA::Id() const
{
   return "centerRA";
}

int PCCCenterRA::Precision() const
{
   return 7; // 1 mas = 2.7...e-07 deg
}

double PCCCenterRA::MinimumValue() const
{
   return 0;
}

double PCCCenterRA::MaximumValue() const
{
   return 360;
}

double PCCCenterRA::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

PCCCenterDec::PCCCenterDec( MetaProcess* P ) : MetaDouble( P )
{
   ThePCCCenterDecParameter = this;
}

IsoString PCCCenterDec::Id() const
{
   return "centerDec";
}

int PCCCenterDec::Precision() const
{
   return 7; // 1 mas = 2.7...e-07 deg
}

double PCCCenterDec::MinimumValue() const
{
   return -90;
}

double PCCCenterDec::MaximumValue() const
{
   return +90;
}

double PCCCenterDec::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

PCCEpochJD::PCCEpochJD( MetaProcess* P ) : MetaDouble( P )
{
   ThePCCEpochJDParameter = this;
}

IsoString PCCEpochJD::Id() const
{
   return "epochJD";
}

int PCCEpochJD::Precision() const
{
   return 7; // 1 mas = 2.7...e-07 deg
}

double PCCEpochJD::MinimumValue() const
{
   return 0;
}

double PCCEpochJD::MaximumValue() const
{
   return int_max;
}

double PCCEpochJD::DefaultValue() const
{
   return 2451545.0; // J2000.0
}

// ----------------------------------------------------------------------------

PCCForcePlateSolve::PCCForcePlateSolve( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCForcePlateSolveParameter = this;
}

IsoString PCCForcePlateSolve::Id() const
{
   return "forcePlateSolve";
}

bool PCCForcePlateSolve::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PCCIgnoreImagePositionAndScale::PCCIgnoreImagePositionAndScale( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCIgnoreImagePositionAndScaleParameter = this;
}

IsoString PCCIgnoreImagePositionAndScale::Id() const
{
   return "ignoreImagePositionAndScale";
}

bool PCCIgnoreImagePositionAndScale::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PCCServerURL::PCCServerURL( MetaProcess* P ) : MetaString( P )
{
   ThePCCServerURLParameter = this;
}

IsoString PCCServerURL::Id() const
{
   return "serverURL";
}

String PCCServerURL::DefaultValue() const
{
   return "http://cdsarc.u-strasbg.fr/";
}

// ----------------------------------------------------------------------------

PCCSolverCatalogName::PCCSolverCatalogName( MetaProcess* P ) : MetaString( P )
{
   ThePCCSolverCatalogNameParameter = this;
}

IsoString PCCSolverCatalogName::Id() const
{
   return "solverCatalogName";
}

String PCCSolverCatalogName::DefaultValue() const
{
   return "PPMXL";
}

// ----------------------------------------------------------------------------

PCCSolverAutoCatalog::PCCSolverAutoCatalog( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCSolverAutoCatalogParameter = this;
}

IsoString PCCSolverAutoCatalog::Id() const
{
   return "solverAutoCatalog";
}

bool PCCSolverAutoCatalog::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCSolverLimitMagnitude::PCCSolverLimitMagnitude( MetaProcess* P ) : MetaInt32( P )
{
   ThePCCSolverLimitMagnitudeParameter = this;
}

IsoString PCCSolverLimitMagnitude::Id() const
{
   return "solverLimitMagnitude";
}

double PCCSolverLimitMagnitude::MinimumValue() const
{
   return 0;
}

double PCCSolverLimitMagnitude::MaximumValue() const
{
   return 30;
}

double PCCSolverLimitMagnitude::DefaultValue() const
{
   return 12;
}

// ----------------------------------------------------------------------------

PCCSolverAutoLimitMagnitude::PCCSolverAutoLimitMagnitude( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCSolverAutoLimitMagnitudeParameter = this;
}

IsoString PCCSolverAutoLimitMagnitude::Id() const
{
   return "solverAutoLimitMagnitude";
}

bool PCCSolverAutoLimitMagnitude::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCSolverAutoLimitMagnitudeFactor::PCCSolverAutoLimitMagnitudeFactor( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCSolverAutoLimitMagnitudeFactorParameter = this;
}

IsoString PCCSolverAutoLimitMagnitudeFactor::Id() const
{
   return "solverAutoLimitMagnitudeFactor";
}

int PCCSolverAutoLimitMagnitudeFactor::Precision() const
{
   return 2;
}

double PCCSolverAutoLimitMagnitudeFactor::MinimumValue() const
{
   return 10;
}

double PCCSolverAutoLimitMagnitudeFactor::MaximumValue() const
{
   return 25;
}

double PCCSolverAutoLimitMagnitudeFactor::DefaultValue() const
{
   return 16.5;
}

// ----------------------------------------------------------------------------

PCCSolverStarSensitivity::PCCSolverStarSensitivity( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCSolverStarSensitivityParameter = this;
}

IsoString PCCSolverStarSensitivity::Id() const
{
   return "solverStarSensitivity";
}

int PCCSolverStarSensitivity::Precision() const
{
   return 2;
}

double PCCSolverStarSensitivity::MinimumValue() const
{
   return -3;
}

double PCCSolverStarSensitivity::MaximumValue() const
{
   return +3;
}

double PCCSolverStarSensitivity::DefaultValue() const
{
   return -1; // J2000.0
}

// ----------------------------------------------------------------------------

PCCSolverNoiseLayers::PCCSolverNoiseLayers( MetaProcess* P ) : MetaInt32( P )
{
   ThePCCSolverNoiseLayersParameter = this;
}

IsoString PCCSolverNoiseLayers::Id() const
{
   return "solverNoiseLayers";
}

double PCCSolverNoiseLayers::MinimumValue() const
{
   return 0;
}

double PCCSolverNoiseLayers::MaximumValue() const
{
   return 5;
}

double PCCSolverNoiseLayers::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

PCCSolverAlignmentDevice::PCCSolverAlignmentDevice( MetaProcess* P ) : MetaEnumeration( P )
{
   ThePCCSolverAlignmentDeviceParameter = this;
}

IsoString PCCSolverAlignmentDevice::Id() const
{
   return "solverAlignmentDevice";
}

size_type PCCSolverAlignmentDevice::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString PCCSolverAlignmentDevice::ElementId( size_type index ) const
{
   switch ( index )
   {
   default:
   case TriangleSimilarity: return "AlignmentDevice_TriangleSimilarity";
   case PolygonMatching:    return "AlignmentDevice_PolygonMatching";
   }
}

int PCCSolverAlignmentDevice::ElementValue( size_type index ) const
{
   return int( index );
}

size_type PCCSolverAlignmentDevice::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

PCCSolverDistortionCorrection::PCCSolverDistortionCorrection( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCSolverDistortionCorrectionParameter = this;
}

IsoString PCCSolverDistortionCorrection::Id() const
{
   return "solverDistortionCorrection";
}

bool PCCSolverDistortionCorrection::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PCCSolverSplineSmoothing::PCCSolverSplineSmoothing( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCSolverSplineSmoothingParameter = this;
}

IsoString PCCSolverSplineSmoothing::Id() const
{
   return "solverSplineSmoothing";
}

int PCCSolverSplineSmoothing::Precision() const
{
   return 2;
}

double PCCSolverSplineSmoothing::MinimumValue() const
{
   return 0;
}

double PCCSolverSplineSmoothing::MaximumValue() const
{
   return 0.5;
}

double PCCSolverSplineSmoothing::DefaultValue() const
{
   return 0.05;
}

// ----------------------------------------------------------------------------

PCCSolverProjection::PCCSolverProjection( MetaProcess* P ) : MetaEnumeration( P )
{
   ThePCCSolverProjectionParameter = this;
}

IsoString PCCSolverProjection::Id() const
{
   return "solverProjection";
}

size_type PCCSolverProjection::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString PCCSolverProjection::ElementId( size_type index ) const
{
   switch ( index )
   {
   default:
   case Gnomonic:          return "Projection_Gnomonic";
   case Stereographic:     return "Projection_Stereographic";
   case PlateCarree:       return "Projection_PlateCarree";
   case Mercator:          return "Projection_Mercator";
   case HammerAitoff:      return "Projection_HammerAitoff";
   case ZenithalEqualArea: return "Projection_ZenithalEqualArea";
   case Orthographic:      return "Projection_Orthographic";
   }
}

int PCCSolverProjection::ElementValue( size_type index ) const
{
   return int( index );
}

size_type PCCSolverProjection::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

PCCPhotCatalogName::PCCPhotCatalogName( MetaProcess* P ) : MetaString( P )
{
   ThePCCPhotCatalogNameParameter = this;
}

IsoString PCCPhotCatalogName::Id() const
{
   return "photCatalogName";
}

String PCCPhotCatalogName::DefaultValue() const
{
   return "APASS";
}

// ----------------------------------------------------------------------------

PCCPhotLimitMagnitude::PCCPhotLimitMagnitude( MetaProcess* P ) : MetaInt32( P )
{
   ThePCCPhotLimitMagnitudeParameter = this;
}

IsoString PCCPhotLimitMagnitude::Id() const
{
   return "photLimitMagnitude";
}

double PCCPhotLimitMagnitude::MinimumValue() const
{
   return 9;
}

double PCCPhotLimitMagnitude::MaximumValue() const
{
   return 30;
}

double PCCPhotLimitMagnitude::DefaultValue() const
{
   return 12;
}

// ----------------------------------------------------------------------------

PCCPhotAutoLimitMagnitude::PCCPhotAutoLimitMagnitude( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCPhotAutoLimitMagnitudeParameter = this;
}

IsoString PCCPhotAutoLimitMagnitude::Id() const
{
   return "photAutoLimitMagnitude";
}

bool PCCPhotAutoLimitMagnitude::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCPhotAutoLimitMagnitudeFactor::PCCPhotAutoLimitMagnitudeFactor( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCPhotAutoLimitMagnitudeFactorParameter = this;
}

IsoString PCCPhotAutoLimitMagnitudeFactor::Id() const
{
   return "photAutoLimitMagnitudeFactor";
}

int PCCPhotAutoLimitMagnitudeFactor::Precision() const
{
   return 2;
}

double PCCPhotAutoLimitMagnitudeFactor::MinimumValue() const
{
   return 10;
}

double PCCPhotAutoLimitMagnitudeFactor::MaximumValue() const
{
   return 25;
}

double PCCPhotAutoLimitMagnitudeFactor::DefaultValue() const
{
   return 15;
}

// ----------------------------------------------------------------------------

PCCPhotAutoAperture::PCCPhotAutoAperture( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCPhotAutoApertureParameter = this;
}

IsoString PCCPhotAutoAperture::Id() const
{
   return "photAutoAperture";
}

bool PCCPhotAutoAperture::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCPhotAperture::PCCPhotAperture( MetaProcess* P ) : MetaInt32( P )
{
   ThePCCPhotApertureParameter = this;
}

IsoString PCCPhotAperture::Id() const
{
   return "photAperture";
}

double PCCPhotAperture::MinimumValue() const
{
   return 5;
}

double PCCPhotAperture::MaximumValue() const
{
   return 30;
}

double PCCPhotAperture::DefaultValue() const
{
   return 8;
}

// ----------------------------------------------------------------------------

PCCPhotUsePSF::PCCPhotUsePSF( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCPhotUsePSFParameter = this;
}

IsoString PCCPhotUsePSF::Id() const
{
   return "photUsePSF";
}

bool PCCPhotUsePSF::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PCCPhotSaturationThreshold::PCCPhotSaturationThreshold( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCPhotSaturationThresholdParameter = this;
}

IsoString PCCPhotSaturationThreshold::Id() const
{
   return "photSaturationThreshold";
}

int PCCPhotSaturationThreshold::Precision() const
{
   return 2;
}

double PCCPhotSaturationThreshold::MinimumValue() const
{
   return 0.25;
}

double PCCPhotSaturationThreshold::MaximumValue() const
{
   return 1.00;
}

double PCCPhotSaturationThreshold::DefaultValue() const
{
   return 0.95;
}

// ----------------------------------------------------------------------------

PCCPhotShowDetectedStars::PCCPhotShowDetectedStars( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCPhotShowDetectedStarsParameter = this;
}

IsoString PCCPhotShowDetectedStars::Id() const
{
   return "photShowDetectedStars";
}

bool PCCPhotShowDetectedStars::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PCCPhotShowBackgroundModels::PCCPhotShowBackgroundModels( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCPhotShowBackgroundModelsParameter = this;
}

IsoString PCCPhotShowBackgroundModels::Id() const
{
   return "photShowBackgroundModels";
}

bool PCCPhotShowBackgroundModels::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PCCPhotGenerateGraphs::PCCPhotGenerateGraphs( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCPhotGenerateGraphsParameter = this;
}

IsoString PCCPhotGenerateGraphs::Id() const
{
   return "photGenerateGraphs";
}

bool PCCPhotGenerateGraphs::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCApplyCalibration::PCCApplyCalibration( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCApplyCalibrationParameter = this;
}

IsoString PCCApplyCalibration::Id() const
{
   return "applyCalibration";
}

bool PCCApplyCalibration::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCNeutralizeBackground::PCCNeutralizeBackground( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCNeutralizeBackgroundParameter = this;
}

IsoString PCCNeutralizeBackground::Id() const
{
   return "neutralizeBackground";
}

bool PCCNeutralizeBackground::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

PCCBackgroundReferenceViewId::PCCBackgroundReferenceViewId( MetaProcess* P ) : MetaString( P )
{
   ThePCCBackgroundReferenceViewIdParameter = this;
}

IsoString PCCBackgroundReferenceViewId::Id() const
{
   return "backgroundReferenceViewId";
}

// ----------------------------------------------------------------------------

PCCBackgroundLow::PCCBackgroundLow( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCBackgroundLowParameter = this;
}

IsoString PCCBackgroundLow::Id() const
{
   return "backgroundLow";
}

int PCCBackgroundLow::Precision() const
{
   return 7;
}

double PCCBackgroundLow::MinimumValue() const
{
   return 0;
}

double PCCBackgroundLow::MaximumValue() const
{
   return 1;
}

double PCCBackgroundLow::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

PCCBackgroundHigh::PCCBackgroundHigh( MetaProcess* P ) : MetaFloat( P )
{
   ThePCCBackgroundHighParameter = this;
}

IsoString PCCBackgroundHigh::Id() const
{
   return "backgroundHigh";
}

int PCCBackgroundHigh::Precision() const
{
   return 7;
}

double PCCBackgroundHigh::MinimumValue() const
{
   return 0;
}

double PCCBackgroundHigh::MaximumValue() const
{
   return 1;
}

double PCCBackgroundHigh::DefaultValue() const
{
   return 0.1;
}

// ----------------------------------------------------------------------------

PCCBackgroundUseROI::PCCBackgroundUseROI( MetaProcess* P ) : MetaBoolean( P )
{
   ThePCCBackgroundUseROIParameter = this;
}

IsoString PCCBackgroundUseROI::Id() const
{
   return "backgroundUseROI";
}

bool PCCBackgroundUseROI::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

PCCBackgroundROIX0::PCCBackgroundROIX0( MetaProcess* P ) : MetaInt32( P )
{
   ThePCCBackgroundROIX0Parameter = this;
}

IsoString PCCBackgroundROIX0::Id() const
{
   return "backgroundROIX0";
}

double PCCBackgroundROIX0::DefaultValue() const
{
   return 0;
}

double PCCBackgroundROIX0::MinimumValue() const
{
   return 0;
}

double PCCBackgroundROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

PCCBackgroundROIY0::PCCBackgroundROIY0( MetaProcess* P ) : MetaInt32( P )
{
   ThePCCBackgroundROIY0Parameter = this;
}

IsoString PCCBackgroundROIY0::Id() const
{
   return "backgroundROIY0";
}

double PCCBackgroundROIY0::DefaultValue() const
{
   return 0;
}

double PCCBackgroundROIY0::MinimumValue() const
{
   return 0;
}

double PCCBackgroundROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

PCCBackgroundROIX1::PCCBackgroundROIX1( MetaProcess* P ) : MetaInt32( P )
{
   ThePCCBackgroundROIX1Parameter = this;
}

IsoString PCCBackgroundROIX1::Id() const
{
   return "backgroundROIX1";
}

double PCCBackgroundROIX1::DefaultValue() const
{
   return 0;
}

double PCCBackgroundROIX1::MinimumValue() const
{
   return 0;
}

double PCCBackgroundROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

PCCBackgroundROIY1::PCCBackgroundROIY1( MetaProcess* P ) : MetaInt32( P )
{
   ThePCCBackgroundROIY1Parameter = this;
}

IsoString PCCBackgroundROIY1::Id() const
{
   return "backgroundROIY1";
}

double PCCBackgroundROIY1::DefaultValue() const
{
   return 0;
}

double PCCBackgroundROIY1::MinimumValue() const
{
   return 0;
}

double PCCBackgroundROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationParameters.cpp - Released 2018-11-23T18:45:58Z
