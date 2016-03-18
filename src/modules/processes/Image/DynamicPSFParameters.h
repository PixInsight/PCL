//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0352
// ----------------------------------------------------------------------------
// DynamicPSFParameters.h - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

   virtual IsoString Id() const;
};

extern DPViewTable* TheDPViewTableParameter;

// ----------------------------------------------------------------------------

class DPViewId : public MetaString
{
public:

   DPViewId( MetaTable* );

   virtual IsoString Id() const;
   virtual size_type MinLength() const;
   virtual String AllowedCharacters() const;
};

extern DPViewId* TheDPViewIdParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DPStarTable : public MetaTable
{
public:

   DPStarTable( MetaProcess* );

   virtual IsoString Id() const;
};

extern DPStarTable* TheDPStarTableParameter;

// ----------------------------------------------------------------------------

class DPStarViewIndex : public MetaUInt32
{
public:

   DPStarViewIndex( MetaTable* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
};

extern DPStarViewIndex* TheDPStarViewIndexParameter;

// ----------------------------------------------------------------------------

class DPStarChannel : public MetaInt32
{
public:

   DPStarChannel( MetaTable* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
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

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern DPStarStatus* TheDPStarStatusParameter;

// ----------------------------------------------------------------------------

class DPStarRectX0 : public MetaDouble
{
public:

   DPStarRectX0( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPStarRectX0* TheDPStarRectX0Parameter;

// ----------------------------------------------------------------------------

class DPStarRectY0 : public MetaDouble
{
public:

   DPStarRectY0( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPStarRectY0* TheDPStarRectY0Parameter;

// ----------------------------------------------------------------------------

class DPStarRectX1 : public MetaDouble
{
public:

   DPStarRectX1( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPStarRectX1* TheDPStarRectX1Parameter;

// ----------------------------------------------------------------------------

class DPStarRectY1 : public MetaDouble
{
public:

   DPStarRectY1( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPStarRectY1* TheDPStarRectY1Parameter;

// ----------------------------------------------------------------------------

class DPStarPosX : public MetaDouble
{
public:

   DPStarPosX( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPStarPosX* TheDPStarPosXParameter;

// ----------------------------------------------------------------------------

class DPStarPosY : public MetaDouble
{
public:

   DPStarPosY( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPStarPosY* TheDPStarPosYParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DPPSFTable : public MetaTable
{
public:

   DPPSFTable( MetaProcess* );

   virtual IsoString Id() const;
};

extern DPPSFTable* TheDPPSFTableParameter;

// ----------------------------------------------------------------------------

class DPPSFStarIndex : public MetaUInt32
{
public:

   DPPSFStarIndex( MetaTable* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
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

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern DPPSFFunction* TheDPPSFFunctionParameter;

// ----------------------------------------------------------------------------

class DPPSFCircular : public MetaBoolean
{
public:

   DPPSFCircular( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
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

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern DPPSFStatus* TheDPPSFStatusParameter;

// ----------------------------------------------------------------------------

class DPPSFBackground : public MetaDouble
{
public:

   DPPSFBackground( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPPSFBackground* TheDPPSFBackgroundParameter;

// ----------------------------------------------------------------------------

class DPPSFAmplitude : public MetaDouble
{
public:

   DPPSFAmplitude( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPPSFAmplitude* TheDPPSFAmplitudeParameter;

// ----------------------------------------------------------------------------

class DPPSFCentroidX : public MetaDouble
{
public:

   DPPSFCentroidX( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPPSFCentroidX* TheDPPSFCentroidXParameter;

// ----------------------------------------------------------------------------

class DPPSFCentroidY : public MetaDouble
{
public:

   DPPSFCentroidY( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPPSFCentroidY* TheDPPSFCentroidYParameter;

// ----------------------------------------------------------------------------

class DPPSFRadiusX : public MetaDouble
{
public:

   DPPSFRadiusX( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPPSFRadiusX* TheDPPSFRadiusXParameter;

// ----------------------------------------------------------------------------

class DPPSFRadiusY : public MetaDouble
{
public:

   DPPSFRadiusY( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPPSFRadiusY* TheDPPSFRadiusYParameter;

// ----------------------------------------------------------------------------

class DPPSFRotationAngle : public MetaDouble
{
public:

   DPPSFRotationAngle( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DPPSFRotationAngle* TheDPPSFRotationAngleParameter;

// ----------------------------------------------------------------------------

class DPPSFBeta : public MetaDouble
{
public:

   DPPSFBeta( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern DPPSFBeta* TheDPPSFBetaParameter;

// ----------------------------------------------------------------------------

class DPPSFMAD : public MetaDouble
{
public:

   DPPSFMAD( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual double DefaultValue() const;
};

extern DPPSFMAD* TheDPPSFMADParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DPAutoPSF : public MetaBoolean
{
public:

   DPAutoPSF( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPAutoPSF* TheDPAutoPSFParameter;

// ----------------------------------------------------------------------------

class DPCircularPSF : public MetaBoolean
{
public:

   DPCircularPSF( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPCircularPSF* TheDPCircularPSFParameter;

// ----------------------------------------------------------------------------

class DPGaussianPSF : public MetaBoolean
{
public:

   DPGaussianPSF( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPGaussianPSF* TheDPGaussianPSFParameter;

// ----------------------------------------------------------------------------

class DPMoffatPSF : public MetaBoolean
{
public:

   DPMoffatPSF( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPMoffatPSF* TheDPMoffatPSFParameter;

// ----------------------------------------------------------------------------

class DPMoffat10PSF : public MetaBoolean
{
public:

   DPMoffat10PSF( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPMoffat10PSF* TheDPMoffat10PSFParameter;

// ----------------------------------------------------------------------------

class DPMoffat8PSF : public MetaBoolean
{
public:

   DPMoffat8PSF( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPMoffat8PSF* TheDPMoffat8PSFParameter;

// ----------------------------------------------------------------------------

class DPMoffat6PSF : public MetaBoolean
{
public:

   DPMoffat6PSF( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPMoffat6PSF* TheDPMoffat6PSFParameter;

// ----------------------------------------------------------------------------

class DPMoffat4PSF : public MetaBoolean
{
public:

   DPMoffat4PSF( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPMoffat4PSF* TheDPMoffat4PSFParameter;

// ----------------------------------------------------------------------------

class DPMoffat25PSF : public MetaBoolean
{
public:

   DPMoffat25PSF( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPMoffat25PSF* TheDPMoffat25PSFParameter;

// ----------------------------------------------------------------------------

class DPMoffat15PSF : public MetaBoolean
{
public:

   DPMoffat15PSF( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPMoffat15PSF* TheDPMoffat15PSFParameter;

// ----------------------------------------------------------------------------

class DPLorentzianPSF : public MetaBoolean
{
public:

   DPLorentzianPSF( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPLorentzianPSF* TheDPLorentzianPSFParameter;

// ----------------------------------------------------------------------------

class DPSignedAngles : public MetaBoolean
{
public:

   DPSignedAngles( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPSignedAngles* TheDPSignedAnglesParameter;

// ----------------------------------------------------------------------------

class DPRegenerate : public MetaBoolean
{
public:

   DPRegenerate( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DPRegenerate* TheDPRegenerateParameter;

// ----------------------------------------------------------------------------

class DPSearchRadius : public MetaInt32
{
public:

   DPSearchRadius( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DPSearchRadius* TheDPSearchRadiusParameter;

// ----------------------------------------------------------------------------

class DPThreshold : public MetaFloat
{
public:

   DPThreshold( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DPThreshold* TheDPThresholdParameter;

// ----------------------------------------------------------------------------

class DPAutoAperture : public MetaBoolean
{
public:

   DPAutoAperture( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
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

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern DPScaleMode* TheDPScaleModeParameter;

// ----------------------------------------------------------------------------

class DPScaleValue : public MetaFloat
{
public:

   DPScaleValue( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DPScaleValue* TheDPScaleValueParameter;

// ----------------------------------------------------------------------------

class DPScaleKeyword : public MetaString
{
public:

   DPScaleKeyword( MetaProcess* );

   virtual IsoString Id() const;
};

extern DPScaleKeyword* TheDPScaleKeywordParameter;

// ----------------------------------------------------------------------------

class DPStarColor : public MetaUInt32
{
public:

   DPStarColor( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
};

extern DPStarColor* TheDPStarColorParameter;

// ----------------------------------------------------------------------------

class DPSelectedStarColor : public MetaUInt32
{
public:

   DPSelectedStarColor( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
};

extern DPSelectedStarColor* TheDPSelectedStarColorParameter;

// ----------------------------------------------------------------------------

class DPSelectedStarFillColor : public MetaUInt32
{
public:

   DPSelectedStarFillColor( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
};

extern DPSelectedStarFillColor* TheDPSelectedStarFillColorParameter;

// ----------------------------------------------------------------------------

class DPBadStarColor : public MetaUInt32
{
public:

   DPBadStarColor( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
};

extern DPBadStarColor* TheDPBadStarColorParameter;

// ----------------------------------------------------------------------------

class DPBadStarFillColor : public MetaUInt32
{
public:

   DPBadStarFillColor( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
};

extern DPBadStarFillColor* TheDPBadStarFillColorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __DynamicPSFParameters_h

// ----------------------------------------------------------------------------
// EOF DynamicPSFParameters.h - Released 2016/02/21 20:22:43 UTC
