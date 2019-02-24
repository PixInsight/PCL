//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.03.00.0437
// ----------------------------------------------------------------------------
// DynamicPSFParameters.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "DynamicPSFParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

DPViewTable*             TheDPViewTableParameter = nullptr;
DPViewId*                TheDPViewIdParameter = nullptr;

DPStarTable*             TheDPStarTableParameter = nullptr;
DPStarViewIndex*         TheDPStarViewIndexParameter = nullptr;
DPStarChannel*           TheDPStarChannelParameter = nullptr;
DPStarStatus*            TheDPStarStatusParameter = nullptr;
DPStarRectX0*            TheDPStarRectX0Parameter = nullptr;
DPStarRectY0*            TheDPStarRectY0Parameter = nullptr;
DPStarRectX1*            TheDPStarRectX1Parameter = nullptr;
DPStarRectY1*            TheDPStarRectY1Parameter = nullptr;
DPStarPosX*              TheDPStarPosXParameter = nullptr;
DPStarPosY*              TheDPStarPosYParameter = nullptr;

DPPSFTable*              TheDPPSFTableParameter = nullptr;
DPPSFStarIndex*          TheDPPSFStarIndexParameter = nullptr;
DPPSFFunction*           TheDPPSFFunctionParameter = nullptr;
DPPSFCircular*           TheDPPSFCircularParameter = nullptr;
DPPSFStatus*             TheDPPSFStatusParameter = nullptr;
DPPSFBackground*         TheDPPSFBackgroundParameter = nullptr;
DPPSFAmplitude*          TheDPPSFAmplitudeParameter = nullptr;
DPPSFCentroidX*          TheDPPSFCentroidXParameter = nullptr;
DPPSFCentroidY*          TheDPPSFCentroidYParameter = nullptr;
DPPSFRadiusX*            TheDPPSFRadiusXParameter = nullptr;
DPPSFRadiusY*            TheDPPSFRadiusYParameter = nullptr;
DPPSFRotationAngle*      TheDPPSFRotationAngleParameter = nullptr;
DPPSFBeta*               TheDPPSFBetaParameter = nullptr;
DPPSFMAD*                TheDPPSFMADParameter = nullptr;
DPPSFCelestial*          TheDPPSFCelestialParameter = nullptr;
DPPSFCentroidRA*         TheDPPSFCentroidRAParameter = nullptr;
DPPSFCentroidDec*        TheDPPSFCentroidDecParameter = nullptr;

DPAutoPSF*               TheDPAutoPSFParameter = nullptr;
DPCircularPSF*           TheDPCircularPSFParameter = nullptr;
DPGaussianPSF*           TheDPGaussianPSFParameter = nullptr;
DPMoffatPSF*             TheDPMoffatPSFParameter = nullptr;
DPMoffat10PSF*           TheDPMoffat10PSFParameter = nullptr;
DPMoffat8PSF*            TheDPMoffat8PSFParameter = nullptr;
DPMoffat6PSF*            TheDPMoffat6PSFParameter = nullptr;
DPMoffat4PSF*            TheDPMoffat4PSFParameter = nullptr;
DPMoffat25PSF*           TheDPMoffat25PSFParameter = nullptr;
DPMoffat15PSF*           TheDPMoffat15PSFParameter = nullptr;
DPLorentzianPSF*         TheDPLorentzianPSFParameter = nullptr;

DPSignedAngles*          TheDPSignedAnglesParameter = nullptr;
DPRegenerate*            TheDPRegenerateParameter = nullptr;
DPAstrometry*            TheDPAstrometryParameter = nullptr;

DPSearchRadius*          TheDPSearchRadiusParameter = nullptr;
DPThreshold*             TheDPThresholdParameter = nullptr;
DPAutoAperture*          TheDPAutoApertureParameter = nullptr;
DPScaleMode*             TheDPScaleModeParameter = nullptr;
DPScaleValue*            TheDPScaleValueParameter = nullptr;
DPScaleKeyword*          TheDPScaleKeywordParameter = nullptr;

DPStarColor*             TheDPStarColorParameter = nullptr;
DPSelectedStarColor*     TheDPSelectedStarColorParameter = nullptr;
DPSelectedStarFillColor* TheDPSelectedStarFillColorParameter = nullptr;
DPBadStarColor*          TheDPBadStarColorParameter = nullptr;
DPBadStarFillColor*      TheDPBadStarFillColorParameter = nullptr;

// ----------------------------------------------------------------------------

DPViewTable::DPViewTable( MetaProcess* P ) : MetaTable( P )
{
   TheDPViewTableParameter = this;
}

IsoString DPViewTable::Id() const
{
   return "views";
}

// ----------------------------------------------------------------------------

DPViewId::DPViewId( MetaTable* T ) : MetaString( T )
{
   TheDPViewIdParameter = this;
}

IsoString DPViewId::Id() const
{
   return "id";
}

size_type DPViewId::MinLength() const
{
   return 1;
}

String DPViewId::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DPStarTable::DPStarTable( MetaProcess* P ) : MetaTable( P )
{
   TheDPStarTableParameter = this;
}

IsoString DPStarTable::Id() const
{
   return "stars";
}

// ----------------------------------------------------------------------------

DPStarViewIndex::DPStarViewIndex( MetaTable* T ) : MetaUInt32( T )
{
   TheDPStarViewIndexParameter = this;
}

IsoString DPStarViewIndex::Id() const
{
   return "viewIndex";
}

double DPStarViewIndex::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPStarChannel::DPStarChannel( MetaTable* T ) : MetaInt32( T )
{
   TheDPStarChannelParameter = this;
}

IsoString DPStarChannel::Id() const
{
   return "channel";
}

double DPStarChannel::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPStarStatus::DPStarStatus( MetaTable* T ) : MetaEnumeration( T )
{
   TheDPStarStatusParameter = this;
}

IsoString DPStarStatus::Id() const
{
   return "status";
}

size_type DPStarStatus::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DPStarStatus::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case NotDetected:       return "Star_NotDetected";
   case DetectedOk:        return "Star_DetectedOk";
   case NoSignificantData: return "Star_NoSignificantData";
   case CrossingEdges:     return "Star_CrossingEdges";
   case OutsideImage:      return "Star_OutsideImage";
   case NoConvergence:     return "Star_NoConvergence";
   case UnknownError:      return "Star_UnknownError";
   }
}

int DPStarStatus::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DPStarStatus::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DPStarRectX0::DPStarRectX0( MetaTable* T ) : MetaDouble( T )
{
   TheDPStarRectX0Parameter = this;
}

IsoString DPStarRectX0::Id() const
{
   return "x0";
}

int DPStarRectX0::Precision() const
{
   return 2;
}

double DPStarRectX0::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPStarRectY0::DPStarRectY0( MetaTable* T ) : MetaDouble( T )
{
   TheDPStarRectY0Parameter = this;
}

IsoString DPStarRectY0::Id() const
{
   return "y0";
}

int DPStarRectY0::Precision() const
{
   return 2;
}

double DPStarRectY0::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPStarRectX1::DPStarRectX1( MetaTable* T ) : MetaDouble( T )
{
   TheDPStarRectX1Parameter = this;
}

IsoString DPStarRectX1::Id() const
{
   return "x1";
}

int DPStarRectX1::Precision() const
{
   return 2;
}

double DPStarRectX1::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPStarRectY1::DPStarRectY1( MetaTable* T ) : MetaDouble( T )
{
   TheDPStarRectY1Parameter = this;
}

IsoString DPStarRectY1::Id() const
{
   return "y1";
}

int DPStarRectY1::Precision() const
{
   return 2;
}

double DPStarRectY1::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPStarPosX::DPStarPosX( MetaTable* T ) : MetaDouble( T )
{
   TheDPStarPosXParameter = this;
}

IsoString DPStarPosX::Id() const
{
   return "x";
}

int DPStarPosX::Precision() const
{
   return 2;
}

double DPStarPosX::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPStarPosY::DPStarPosY( MetaTable* T ) : MetaDouble( T )
{
   TheDPStarPosYParameter = this;
}

IsoString DPStarPosY::Id() const
{
   return "y";
}

int DPStarPosY::Precision() const
{
   return 2;
}

double DPStarPosY::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DPPSFTable::DPPSFTable( MetaProcess* P ) : MetaTable( P )
{
   TheDPPSFTableParameter = this;
}

IsoString DPPSFTable::Id() const
{
   return "psf";
}

// ----------------------------------------------------------------------------

DPPSFStarIndex::DPPSFStarIndex( MetaTable* T ) : MetaUInt32( T )
{
   TheDPPSFStarIndexParameter = this;
}

IsoString DPPSFStarIndex::Id() const
{
   return "starIndex";
}

double DPPSFStarIndex::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPPSFFunction::DPPSFFunction( MetaTable* T ) : MetaEnumeration( T )
{
   TheDPPSFFunctionParameter = this;
}

IsoString DPPSFFunction::Id() const
{
   return "function";
}

size_type DPPSFFunction::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DPPSFFunction::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Gaussian:   return "Function_Gaussian";
   case Moffat:     return "Function_Moffat";
   case MoffatA:    return "Function_Moffat10";
   case Moffat8:    return "Function_Moffat8";
   case Moffat6:    return "Function_Moffat6";
   case Moffat4:    return "Function_Moffat4";
   case Moffat25:   return "Function_Moffat25";
   case Moffat15:   return "Function_Moffat15";
   case Lorentzian: return "Function_Lorentzian";
   }
}

int DPPSFFunction::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DPPSFFunction::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DPPSFCircular::DPPSFCircular( MetaTable* T ) : MetaBoolean( T )
{
   TheDPPSFCircularParameter = this;
}

IsoString DPPSFCircular::Id() const
{
   return "circular";
}

bool DPPSFCircular::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DPPSFStatus::DPPSFStatus( MetaTable* T ) : MetaEnumeration( T )
{
   TheDPPSFStatusParameter = this;
}

IsoString DPPSFStatus::Id() const
{
   return "status";
}

size_type DPPSFStatus::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DPPSFStatus::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case NotFitted:          return "PSF_NotFitted";
   case FittedOk:           return "PSF_FittedOk";
   case BadParameters:      return "PSF_BadParameters";
   case NoSolution:         return "PSF_NoSolution";
   case NoConvergence:      return "PSF_NoConvergence";
   case InaccurateSolution: return "PSF_InaccurateSolution";
   case UnknownError:       return "PSF_UnknownError";
   }
}

int DPPSFStatus::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DPPSFStatus::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DPPSFBackground::DPPSFBackground( MetaTable* T ) : MetaDouble( T )
{
   TheDPPSFBackgroundParameter = this;
}

IsoString DPPSFBackground::Id() const
{
   return "B";
}

int DPPSFBackground::Precision() const
{
   return 6;
}

double DPPSFBackground::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPPSFAmplitude::DPPSFAmplitude( MetaTable* T ) : MetaDouble( T )
{
   TheDPPSFAmplitudeParameter = this;
}

IsoString DPPSFAmplitude::Id() const
{
   return "A";
}

int DPPSFAmplitude::Precision() const
{
   return 6;
}

double DPPSFAmplitude::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPPSFCentroidX::DPPSFCentroidX( MetaTable* T ) : MetaDouble( T )
{
   TheDPPSFCentroidXParameter = this;
}

IsoString DPPSFCentroidX::Id() const
{
   return "cx";
}

int DPPSFCentroidX::Precision() const
{
   return 2;
}

double DPPSFCentroidX::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPPSFCentroidY::DPPSFCentroidY( MetaTable* T ) : MetaDouble( T )
{
   TheDPPSFCentroidYParameter = this;
}

IsoString DPPSFCentroidY::Id() const
{
   return "cy";
}

int DPPSFCentroidY::Precision() const
{
   return 2;
}

double DPPSFCentroidY::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPPSFRadiusX::DPPSFRadiusX( MetaTable* T ) : MetaDouble( T )
{
   TheDPPSFRadiusXParameter = this;
}

IsoString DPPSFRadiusX::Id() const
{
   return "sx";
}

int DPPSFRadiusX::Precision() const
{
   return 3;
}

double DPPSFRadiusX::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPPSFRadiusY::DPPSFRadiusY( MetaTable* T ) : MetaDouble( T )
{
   TheDPPSFRadiusYParameter = this;
}

IsoString DPPSFRadiusY::Id() const
{
   return "sy";
}

int DPPSFRadiusY::Precision() const
{
   return 3;
}

double DPPSFRadiusY::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPPSFRotationAngle::DPPSFRotationAngle( MetaTable* T ) : MetaDouble( T )
{
   TheDPPSFRotationAngleParameter = this;
}

IsoString DPPSFRotationAngle::Id() const
{
   return "theta";
}

int DPPSFRotationAngle::Precision() const
{
   return 2;
}

double DPPSFRotationAngle::DefaultValue() const
{
   return 0;
}

double DPPSFRotationAngle::MinimumValue() const
{
   return 0;
}

double DPPSFRotationAngle::MaximumValue() const
{
   return 180;
}

// ----------------------------------------------------------------------------

DPPSFBeta::DPPSFBeta( MetaTable* T ) : MetaDouble( T )
{
   TheDPPSFBetaParameter = this;
}

IsoString DPPSFBeta::Id() const
{
   return "beta";
}

int DPPSFBeta::Precision() const
{
   return 2;
}

double DPPSFBeta::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPPSFMAD::DPPSFMAD( MetaTable* T ) : MetaDouble( T )
{
   TheDPPSFMADParameter = this;
}

IsoString DPPSFMAD::Id() const
{
   return "mad";
}

int DPPSFMAD::Precision() const
{
   return 3;
}

bool DPPSFMAD::ScientificNotation() const
{
   return true;
}

double DPPSFMAD::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPPSFCelestial::DPPSFCelestial( MetaTable* T ) : MetaBoolean( T )
{
   TheDPPSFCelestialParameter = this;
}

IsoString DPPSFCelestial::Id() const
{
   return "celestial";
}

bool DPPSFCelestial::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DPPSFCentroidRA::DPPSFCentroidRA( MetaTable* T ) : MetaDouble( T )
{
   TheDPPSFCentroidRAParameter = this;
}

IsoString DPPSFCentroidRA::Id() const
{
   return "alpha";
}

int DPPSFCentroidRA::Precision() const
{
   return 8;
}

double DPPSFCentroidRA::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPPSFCentroidDec::DPPSFCentroidDec( MetaTable* T ) : MetaDouble( T )
{
   TheDPPSFCentroidDecParameter = this;
}

IsoString DPPSFCentroidDec::Id() const
{
   return "delta";
}

int DPPSFCentroidDec::Precision() const
{
   return 8;
}

double DPPSFCentroidDec::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

DPAutoPSF::DPAutoPSF( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPAutoPSFParameter = this;
}

IsoString DPAutoPSF::Id() const
{
   return "autoPSF";
}

bool DPAutoPSF::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DPCircularPSF::DPCircularPSF( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPCircularPSFParameter = this;
}

IsoString DPCircularPSF::Id() const
{
   return "circularPSF";
}

bool DPCircularPSF::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DPGaussianPSF::DPGaussianPSF( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPGaussianPSFParameter = this;
}

IsoString DPGaussianPSF::Id() const
{
   return "gaussianPSF";
}

bool DPGaussianPSF::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DPMoffatPSF::DPMoffatPSF( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPMoffatPSFParameter = this;
}

IsoString DPMoffatPSF::Id() const
{
   return "moffatPSF";
}

bool DPMoffatPSF::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DPMoffat10PSF::DPMoffat10PSF( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPMoffat10PSFParameter = this;
}

IsoString DPMoffat10PSF::Id() const
{
   return "moffat10PSF";
}

bool DPMoffat10PSF::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DPMoffat8PSF::DPMoffat8PSF( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPMoffat8PSFParameter = this;
}

IsoString DPMoffat8PSF::Id() const
{
   return "moffat8PSF";
}

bool DPMoffat8PSF::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DPMoffat6PSF::DPMoffat6PSF( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPMoffat6PSFParameter = this;
}

IsoString DPMoffat6PSF::Id() const
{
   return "moffat6PSF";
}

bool DPMoffat6PSF::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DPMoffat4PSF::DPMoffat4PSF( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPMoffat4PSFParameter = this;
}

IsoString DPMoffat4PSF::Id() const
{
   return "moffat4PSF";
}

bool DPMoffat4PSF::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DPMoffat25PSF::DPMoffat25PSF( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPMoffat25PSFParameter = this;
}

IsoString DPMoffat25PSF::Id() const
{
   return "moffat25PSF";
}

bool DPMoffat25PSF::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DPMoffat15PSF::DPMoffat15PSF( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPMoffat15PSFParameter = this;
}

IsoString DPMoffat15PSF::Id() const
{
   return "moffat15PSF";
}

bool DPMoffat15PSF::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DPLorentzianPSF::DPLorentzianPSF( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPLorentzianPSFParameter = this;
}

IsoString DPLorentzianPSF::Id() const
{
   return "lorentzianPSF";
}

bool DPLorentzianPSF::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

DPSignedAngles::DPSignedAngles( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPSignedAnglesParameter = this;
}

IsoString DPSignedAngles::Id() const
{
   return "signedAngles";
}

bool DPSignedAngles::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DPRegenerate::DPRegenerate( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPRegenerateParameter = this;
}

IsoString DPRegenerate::Id() const
{
   return "regenerate";
}

bool DPRegenerate::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DPAstrometry::DPAstrometry( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPAstrometryParameter = this;
}

IsoString DPAstrometry::Id() const
{
   return "astrometry";
}

bool DPAstrometry::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DPSearchRadius::DPSearchRadius( MetaProcess* P ) : MetaInt32( P )
{
   TheDPSearchRadiusParameter = this;
}

IsoString DPSearchRadius::Id() const
{
   return "searchRadius";
}

double DPSearchRadius::DefaultValue() const
{
   return 8;
}

double DPSearchRadius::MinimumValue() const
{
   return 1;
}

double DPSearchRadius::MaximumValue() const
{
   return 127;
}

// ----------------------------------------------------------------------------

DPThreshold::DPThreshold( MetaProcess* P ) : MetaFloat( P )
{
   TheDPThresholdParameter = this;
}

IsoString DPThreshold::Id() const
{
   return "threshold";
}

int DPThreshold::Precision() const
{
   return 2;
}

double DPThreshold::DefaultValue() const
{
   return 1;
}

double DPThreshold::MinimumValue() const
{
   return 0.05;
}

double DPThreshold::MaximumValue() const
{
   return 5;
}

// ----------------------------------------------------------------------------

DPAutoAperture::DPAutoAperture( MetaProcess* P ) : MetaBoolean( P )
{
   TheDPAutoApertureParameter = this;
}

IsoString DPAutoAperture::Id() const
{
   return "autoAperture";
}

bool DPAutoAperture::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

DPScaleMode::DPScaleMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheDPScaleModeParameter = this;
}

IsoString DPScaleMode::Id() const
{
   return "scaleMode";
}

size_type DPScaleMode::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString DPScaleMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Pixels:           return "Scale_Pixels";
   case StandardKeywords: return "Scale_StandardKeywords";
   case LiteralValue:     return "Scale_LiteralValue";
   case CustomKeyword:    return "Scale_CustomKeyword";
   }
}

int DPScaleMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type DPScaleMode::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

DPScaleValue::DPScaleValue( MetaProcess* P ) : MetaFloat( P )
{
   TheDPScaleValueParameter = this;
}

IsoString DPScaleValue::Id() const
{
   return "scaleValue";
}

int DPScaleValue::Precision() const
{
   return 2;
}

double DPScaleValue::DefaultValue() const
{
   return 1.0;
}

double DPScaleValue::MinimumValue() const
{
   return 0.01;
}

double DPScaleValue::MaximumValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

DPScaleKeyword::DPScaleKeyword( MetaProcess* P ) : MetaString( P )
{
   TheDPScaleKeywordParameter = this;
}

IsoString DPScaleKeyword::Id() const
{
   return "scaleKeyword";
}

// ----------------------------------------------------------------------------

DPStarColor::DPStarColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheDPStarColorParameter = this;
}

IsoString DPStarColor::Id() const
{
   return "starColor";
}

double DPStarColor::DefaultValue() const
{
   return 0xffe0e0e0;
}

// ----------------------------------------------------------------------------

DPSelectedStarColor::DPSelectedStarColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheDPSelectedStarColorParameter = this;
}

IsoString DPSelectedStarColor::Id() const
{
   return "selectedStarColor";
}

double DPSelectedStarColor::DefaultValue() const
{
   return 0xff00ff00;
}

// ----------------------------------------------------------------------------

DPSelectedStarFillColor::DPSelectedStarFillColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheDPSelectedStarFillColorParameter = this;
}

IsoString DPSelectedStarFillColor::Id() const
{
   return "selectedStarFillColor";
}

double DPSelectedStarFillColor::DefaultValue() const
{
   return 0x00000000;
}

// ----------------------------------------------------------------------------

DPBadStarColor::DPBadStarColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheDPBadStarColorParameter = this;
}

IsoString DPBadStarColor::Id() const
{
   return "badStarColor";
}

double DPBadStarColor::DefaultValue() const
{
   return 0xffff0000;
}

// ----------------------------------------------------------------------------

DPBadStarFillColor::DPBadStarFillColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheDPBadStarFillColorParameter = this;
}

IsoString DPBadStarFillColor::Id() const
{
   return "badStarFillColor";
}

double DPBadStarFillColor::DefaultValue() const
{
   return 0x80ff0000;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicPSFParameters.cpp - Released 2019-01-21T12:06:41Z
