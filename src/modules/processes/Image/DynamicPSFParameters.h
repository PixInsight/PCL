//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.03.00.0437
// ----------------------------------------------------------------------------
// DynamicPSFParameters.h - Released 2019-01-21T12:06:41Z
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

#ifndef __DynamicPSFParameters_h
#define __DynamicPSFParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class DPViewTable : public MetaTable
{
public:

   DPViewTable( MetaProcess* );

   IsoString Id() const override;
};

extern DPViewTable* TheDPViewTableParameter;

// ----------------------------------------------------------------------------

class DPViewId : public MetaString
{
public:

   DPViewId( MetaTable* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern DPViewId* TheDPViewIdParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DPStarTable : public MetaTable
{
public:

   DPStarTable( MetaProcess* );

   IsoString Id() const override;
};

extern DPStarTable* TheDPStarTableParameter;

// ----------------------------------------------------------------------------

class DPStarViewIndex : public MetaUInt32
{
public:

   DPStarViewIndex( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern DPStarViewIndex* TheDPStarViewIndexParameter;

// ----------------------------------------------------------------------------

class DPStarChannel : public MetaInt32
{
public:

   DPStarChannel( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern DPStarChannel* TheDPStarChannelParameter;

// ----------------------------------------------------------------------------

class DPStarStatus : public MetaEnumeration
{
public:

   /*
    * NB: Must be compatible with StarDetector::Status
    */
   enum { NotDetected,
          DetectedOk,
          NoSignificantData,
          CrossingEdges,
          OutsideImage,
          NoConvergence,
          UnknownError,
          NumberOfItems,
          Default = NotDetected };

   DPStarStatus( MetaTable* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DPStarStatus* TheDPStarStatusParameter;

// ----------------------------------------------------------------------------

class DPStarRectX0 : public MetaDouble
{
public:

   DPStarRectX0( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPStarRectX0* TheDPStarRectX0Parameter;

// ----------------------------------------------------------------------------

class DPStarRectY0 : public MetaDouble
{
public:

   DPStarRectY0( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPStarRectY0* TheDPStarRectY0Parameter;

// ----------------------------------------------------------------------------

class DPStarRectX1 : public MetaDouble
{
public:

   DPStarRectX1( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPStarRectX1* TheDPStarRectX1Parameter;

// ----------------------------------------------------------------------------

class DPStarRectY1 : public MetaDouble
{
public:

   DPStarRectY1( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPStarRectY1* TheDPStarRectY1Parameter;

// ----------------------------------------------------------------------------

class DPStarPosX : public MetaDouble
{
public:

   DPStarPosX( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPStarPosX* TheDPStarPosXParameter;

// ----------------------------------------------------------------------------

class DPStarPosY : public MetaDouble
{
public:

   DPStarPosY( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPStarPosY* TheDPStarPosYParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DPPSFTable : public MetaTable
{
public:

   DPPSFTable( MetaProcess* );

   IsoString Id() const override;
};

extern DPPSFTable* TheDPPSFTableParameter;

// ----------------------------------------------------------------------------

class DPPSFStarIndex : public MetaUInt32
{
public:

   DPPSFStarIndex( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern DPPSFStarIndex* TheDPPSFStarIndexParameter;

// ----------------------------------------------------------------------------

class DPPSFFunction : public MetaEnumeration
{
public:

   /*
    * NB: Must be compatible with PSFFit::Function
    */
   enum { Gaussian,   // Elliptic Gaussian function
          Moffat,     // Elliptic Moffat function, variable beta
          MoffatA,    // Elliptic Moffat function, beta=10
          Moffat8,    // Elliptic Moffat function, beta=8
          Moffat6,    // Elliptic Moffat function, beta=6
          Moffat4,    // Elliptic Moffat function, beta=4
          Moffat25,   // Elliptic Moffat function, beta=2.5
          Moffat15,   // Elliptic Moffat function, beta=1.5
          Lorentzian, // Elliptic Lorentzian function (beta=1)
          NumberOfItems,
          Default = Gaussian };

   DPPSFFunction( MetaTable* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DPPSFFunction* TheDPPSFFunctionParameter;

// ----------------------------------------------------------------------------

class DPPSFCircular : public MetaBoolean
{
public:

   DPPSFCircular( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPPSFCircular* TheDPPSFCircularParameter;

// ----------------------------------------------------------------------------

class DPPSFStatus : public MetaEnumeration
{
public:

   /*
    * NB: Must be compatible with PSFFit::Status
    */
   enum { NotFitted,
          FittedOk,
          BadParameters,
          NoSolution,
          NoConvergence,
          InaccurateSolution,
          UnknownError,
          NumberOfItems,
          Default = NotFitted };

   DPPSFStatus( MetaTable* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DPPSFStatus* TheDPPSFStatusParameter;

// ----------------------------------------------------------------------------

class DPPSFBackground : public MetaDouble
{
public:

   DPPSFBackground( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPPSFBackground* TheDPPSFBackgroundParameter;

// ----------------------------------------------------------------------------

class DPPSFAmplitude : public MetaDouble
{
public:

   DPPSFAmplitude( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPPSFAmplitude* TheDPPSFAmplitudeParameter;

// ----------------------------------------------------------------------------

class DPPSFCentroidX : public MetaDouble
{
public:

   DPPSFCentroidX( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPPSFCentroidX* TheDPPSFCentroidXParameter;

// ----------------------------------------------------------------------------

class DPPSFCentroidY : public MetaDouble
{
public:

   DPPSFCentroidY( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPPSFCentroidY* TheDPPSFCentroidYParameter;

// ----------------------------------------------------------------------------

class DPPSFRadiusX : public MetaDouble
{
public:

   DPPSFRadiusX( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPPSFRadiusX* TheDPPSFRadiusXParameter;

// ----------------------------------------------------------------------------

class DPPSFRadiusY : public MetaDouble
{
public:

   DPPSFRadiusY( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPPSFRadiusY* TheDPPSFRadiusYParameter;

// ----------------------------------------------------------------------------

class DPPSFRotationAngle : public MetaDouble
{
public:

   DPPSFRotationAngle( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DPPSFRotationAngle* TheDPPSFRotationAngleParameter;

// ----------------------------------------------------------------------------

class DPPSFBeta : public MetaDouble
{
public:

   DPPSFBeta( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPPSFBeta* TheDPPSFBetaParameter;

// ----------------------------------------------------------------------------

class DPPSFMAD : public MetaDouble
{
public:

   DPPSFMAD( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool ScientificNotation() const override;
   double DefaultValue() const override;
};

extern DPPSFMAD* TheDPPSFMADParameter;

// ----------------------------------------------------------------------------

class DPPSFCelestial : public MetaBoolean
{
public:

   DPPSFCelestial( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPPSFCelestial* TheDPPSFCelestialParameter;

// ----------------------------------------------------------------------------

class DPPSFCentroidRA : public MetaDouble
{
public:

   DPPSFCentroidRA( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPPSFCentroidRA* TheDPPSFCentroidRAParameter;

// ----------------------------------------------------------------------------

class DPPSFCentroidDec : public MetaDouble
{
public:

   DPPSFCentroidDec( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DPPSFCentroidDec* TheDPPSFCentroidDecParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DPAutoPSF : public MetaBoolean
{
public:

   DPAutoPSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPAutoPSF* TheDPAutoPSFParameter;

// ----------------------------------------------------------------------------

class DPCircularPSF : public MetaBoolean
{
public:

   DPCircularPSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPCircularPSF* TheDPCircularPSFParameter;

// ----------------------------------------------------------------------------

class DPGaussianPSF : public MetaBoolean
{
public:

   DPGaussianPSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPGaussianPSF* TheDPGaussianPSFParameter;

// ----------------------------------------------------------------------------

class DPMoffatPSF : public MetaBoolean
{
public:

   DPMoffatPSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPMoffatPSF* TheDPMoffatPSFParameter;

// ----------------------------------------------------------------------------

class DPMoffat10PSF : public MetaBoolean
{
public:

   DPMoffat10PSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPMoffat10PSF* TheDPMoffat10PSFParameter;

// ----------------------------------------------------------------------------

class DPMoffat8PSF : public MetaBoolean
{
public:

   DPMoffat8PSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPMoffat8PSF* TheDPMoffat8PSFParameter;

// ----------------------------------------------------------------------------

class DPMoffat6PSF : public MetaBoolean
{
public:

   DPMoffat6PSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPMoffat6PSF* TheDPMoffat6PSFParameter;

// ----------------------------------------------------------------------------

class DPMoffat4PSF : public MetaBoolean
{
public:

   DPMoffat4PSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPMoffat4PSF* TheDPMoffat4PSFParameter;

// ----------------------------------------------------------------------------

class DPMoffat25PSF : public MetaBoolean
{
public:

   DPMoffat25PSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPMoffat25PSF* TheDPMoffat25PSFParameter;

// ----------------------------------------------------------------------------

class DPMoffat15PSF : public MetaBoolean
{
public:

   DPMoffat15PSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPMoffat15PSF* TheDPMoffat15PSFParameter;

// ----------------------------------------------------------------------------

class DPLorentzianPSF : public MetaBoolean
{
public:

   DPLorentzianPSF( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPLorentzianPSF* TheDPLorentzianPSFParameter;

// ----------------------------------------------------------------------------

class DPSignedAngles : public MetaBoolean
{
public:

   DPSignedAngles( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPSignedAngles* TheDPSignedAnglesParameter;

// ----------------------------------------------------------------------------

class DPRegenerate : public MetaBoolean
{
public:

   DPRegenerate( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPRegenerate* TheDPRegenerateParameter;

// ----------------------------------------------------------------------------

class DPAstrometry : public MetaBoolean
{
public:

   DPAstrometry( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPAstrometry* TheDPAstrometryParameter;

// ----------------------------------------------------------------------------

class DPSearchRadius : public MetaInt32
{
public:

   DPSearchRadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DPSearchRadius* TheDPSearchRadiusParameter;

// ----------------------------------------------------------------------------

class DPThreshold : public MetaFloat
{
public:

   DPThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DPThreshold* TheDPThresholdParameter;

// ----------------------------------------------------------------------------

class DPAutoAperture : public MetaBoolean
{
public:

   DPAutoAperture( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern DPAutoAperture* TheDPAutoApertureParameter;

// ----------------------------------------------------------------------------

class DPScaleMode : public MetaEnumeration
{
public:

   enum { Pixels,
          StandardKeywords,
          LiteralValue,
          CustomKeyword,
          NumberOfItems,
          Default = Pixels };

   DPScaleMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern DPScaleMode* TheDPScaleModeParameter;

// ----------------------------------------------------------------------------

class DPScaleValue : public MetaFloat
{
public:

   DPScaleValue( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern DPScaleValue* TheDPScaleValueParameter;

// ----------------------------------------------------------------------------

class DPScaleKeyword : public MetaString
{
public:

   DPScaleKeyword( MetaProcess* );

   IsoString Id() const override;
};

extern DPScaleKeyword* TheDPScaleKeywordParameter;

// ----------------------------------------------------------------------------

class DPStarColor : public MetaUInt32
{
public:

   DPStarColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern DPStarColor* TheDPStarColorParameter;

// ----------------------------------------------------------------------------

class DPSelectedStarColor : public MetaUInt32
{
public:

   DPSelectedStarColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern DPSelectedStarColor* TheDPSelectedStarColorParameter;

// ----------------------------------------------------------------------------

class DPSelectedStarFillColor : public MetaUInt32
{
public:

   DPSelectedStarFillColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern DPSelectedStarFillColor* TheDPSelectedStarFillColorParameter;

// ----------------------------------------------------------------------------

class DPBadStarColor : public MetaUInt32
{
public:

   DPBadStarColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern DPBadStarColor* TheDPBadStarColorParameter;

// ----------------------------------------------------------------------------

class DPBadStarFillColor : public MetaUInt32
{
public:

   DPBadStarFillColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern DPBadStarFillColor* TheDPBadStarFillColorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __DynamicPSFParameters_h

// ----------------------------------------------------------------------------
// EOF DynamicPSFParameters.h - Released 2019-01-21T12:06:41Z
