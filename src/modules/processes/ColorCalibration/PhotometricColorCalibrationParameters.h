//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.03.0336
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationParameters.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#ifndef __PhotometricColorCalibrationParameters_h
#define __PhotometricColorCalibrationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class PCCWhiteReferenceId : public MetaString
{
public:

   PCCWhiteReferenceId( MetaProcess* );

   virtual IsoString Id() const;

   virtual String DefaultValue() const;
};

extern PCCWhiteReferenceId* ThePCCWhiteReferenceIdParameter;

// ----------------------------------------------------------------------------

class PCCWhiteReferenceName : public MetaString
{
public:

   PCCWhiteReferenceName( MetaProcess* );

   virtual IsoString Id() const;

   virtual String DefaultValue() const;
};

extern PCCWhiteReferenceName* ThePCCWhiteReferenceNameParameter;

// ----------------------------------------------------------------------------

class PCCWhiteReferenceSr_JV : public MetaFloat
{
public:

   PCCWhiteReferenceSr_JV( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
};

extern PCCWhiteReferenceSr_JV* ThePCCWhiteReferenceSr_JVParameter;

// ----------------------------------------------------------------------------

class PCCWhiteReferenceJB_JV : public MetaFloat
{
public:

   PCCWhiteReferenceJB_JV( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
};

extern PCCWhiteReferenceJB_JV* ThePCCWhiteReferenceJB_JVParameter;

// ----------------------------------------------------------------------------

class PCCZeroPointSr_JV : public MetaFloat
{
public:

   PCCZeroPointSr_JV( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
};

extern PCCZeroPointSr_JV* ThePCCZeroPointSr_JVParameter;

// ----------------------------------------------------------------------------

class PCCZeroPointJB_JV : public MetaFloat
{
public:

   PCCZeroPointJB_JV( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
};

extern PCCZeroPointJB_JV* ThePCCZeroPointJB_JVParameter;

// ----------------------------------------------------------------------------

class PCCFocalLength : public MetaFloat
{
public:

   PCCFocalLength( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCFocalLength* ThePCCFocalLengthParameter;

// ----------------------------------------------------------------------------

class PCCPixelSize : public MetaFloat
{
public:

   PCCPixelSize( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCPixelSize* ThePCCPixelSizeParameter;

// ----------------------------------------------------------------------------

class PCCCenterRA : public MetaDouble
{
public:

   PCCCenterRA( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCCenterRA* ThePCCCenterRAParameter;

// ----------------------------------------------------------------------------

class PCCCenterDec : public MetaDouble
{
public:

   PCCCenterDec( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCCenterDec* ThePCCCenterDecParameter;

// ----------------------------------------------------------------------------

class PCCEpochJD : public MetaDouble
{
public:

   PCCEpochJD( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCEpochJD* ThePCCEpochJDParameter;

// ----------------------------------------------------------------------------

class PCCForcePlateSolve : public MetaBoolean
{
public:

   PCCForcePlateSolve( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCForcePlateSolve* ThePCCForcePlateSolveParameter;

// ----------------------------------------------------------------------------

class PCCIgnoreImagePositionAndScale : public MetaBoolean
{
public:

   PCCIgnoreImagePositionAndScale( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCIgnoreImagePositionAndScale* ThePCCIgnoreImagePositionAndScaleParameter;

// ----------------------------------------------------------------------------

class PCCServerURL : public MetaString
{
public:

   PCCServerURL( MetaProcess* );

   virtual IsoString Id() const;

   virtual String DefaultValue() const;
};

extern PCCServerURL* ThePCCServerURLParameter;

// ----------------------------------------------------------------------------

class PCCSolverCatalogName : public MetaString
{
public:

   PCCSolverCatalogName( MetaProcess* );

   virtual IsoString Id() const;

   virtual String DefaultValue() const;
};

extern PCCSolverCatalogName* ThePCCSolverCatalogNameParameter;

// ----------------------------------------------------------------------------

class PCCSolverAutoCatalog : public MetaBoolean
{
public:

   PCCSolverAutoCatalog( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCSolverAutoCatalog* ThePCCSolverAutoCatalogParameter;

// ----------------------------------------------------------------------------

class PCCSolverLimitMagnitude : public MetaInt32
{
public:

   PCCSolverLimitMagnitude( MetaProcess* );

   virtual IsoString Id() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCSolverLimitMagnitude* ThePCCSolverLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class PCCSolverAutoLimitMagnitude : public MetaBoolean
{
public:

   PCCSolverAutoLimitMagnitude( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCSolverAutoLimitMagnitude* ThePCCSolverAutoLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class PCCSolverAutoLimitMagnitudeFactor : public MetaFloat
{
public:

   PCCSolverAutoLimitMagnitudeFactor( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCSolverAutoLimitMagnitudeFactor* ThePCCSolverAutoLimitMagnitudeFactorParameter;

// ----------------------------------------------------------------------------

class PCCSolverStarSensitivity : public MetaFloat
{
public:

   PCCSolverStarSensitivity( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCSolverStarSensitivity* ThePCCSolverStarSensitivityParameter;

// ----------------------------------------------------------------------------

class PCCSolverNoiseLayers : public MetaInt32
{
public:

   PCCSolverNoiseLayers( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern PCCSolverNoiseLayers* ThePCCSolverNoiseLayersParameter;

// ----------------------------------------------------------------------------

class PCCSolverAlignmentDevice : public MetaEnumeration
{
public:

   // N.B.: Enumerated items must be coherent with the ImageSolver script.
   enum { TriangleSimilarity,
          PolygonMatching,
          NumberOfItems,
          Default = TriangleSimilarity };

   PCCSolverAlignmentDevice( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern PCCSolverAlignmentDevice* ThePCCSolverAlignmentDeviceParameter;

// ----------------------------------------------------------------------------

class PCCSolverDistortionCorrection : public MetaBoolean
{
public:

   PCCSolverDistortionCorrection( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCSolverDistortionCorrection* ThePCCSolverDistortionCorrectionParameter;

// ----------------------------------------------------------------------------

class PCCSolverSplineSmoothing : public MetaFloat
{
public:

   PCCSolverSplineSmoothing( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCSolverSplineSmoothing* ThePCCSolverSplineSmoothingParameter;

// ----------------------------------------------------------------------------

class PCCSolverProjection : public MetaEnumeration
{
public:

   // N.B.: Enumerated items must be coherent with the ImageSolver script.
   enum { Gnomonic,
          Stereographic,
          PlateCarree,
          Mercator,
          HammerAitoff,
          ZenithalEqualArea,
          Orthographic,
          NumberOfItems,
          Default = Gnomonic };

   PCCSolverProjection( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern PCCSolverProjection* ThePCCSolverProjectionParameter;

// ----------------------------------------------------------------------------

class PCCPhotCatalogName : public MetaString
{
public:

   PCCPhotCatalogName( MetaProcess* );

   virtual IsoString Id() const;

   virtual String DefaultValue() const;
};

extern PCCPhotCatalogName* ThePCCPhotCatalogNameParameter;

// ----------------------------------------------------------------------------

class PCCPhotLimitMagnitude : public MetaInt32
{
public:

   PCCPhotLimitMagnitude( MetaProcess* );

   virtual IsoString Id() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCPhotLimitMagnitude* ThePCCPhotLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class PCCPhotAutoLimitMagnitude : public MetaBoolean
{
public:

   PCCPhotAutoLimitMagnitude( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCPhotAutoLimitMagnitude* ThePCCPhotAutoLimitMagnitudeParameter;

// ----------------------------------------------------------------------------

class PCCPhotAutoLimitMagnitudeFactor : public MetaFloat
{
public:

   PCCPhotAutoLimitMagnitudeFactor( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCPhotAutoLimitMagnitudeFactor* ThePCCPhotAutoLimitMagnitudeFactorParameter;

// ----------------------------------------------------------------------------

class PCCPhotAutoAperture : public MetaBoolean
{
public:

   PCCPhotAutoAperture( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCPhotAutoAperture* ThePCCPhotAutoApertureParameter;

// ----------------------------------------------------------------------------

class PCCPhotAperture : public MetaInt32
{
public:

   PCCPhotAperture( MetaProcess* );

   virtual IsoString Id() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCPhotAperture* ThePCCPhotApertureParameter;

// ----------------------------------------------------------------------------

class PCCPhotUsePSF : public MetaBoolean
{
public:

   PCCPhotUsePSF( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCPhotUsePSF* ThePCCPhotUsePSFParameter;

// ----------------------------------------------------------------------------

class PCCPhotSaturationThreshold : public MetaFloat
{
public:

   PCCPhotSaturationThreshold( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCPhotSaturationThreshold* ThePCCPhotSaturationThresholdParameter;

// ----------------------------------------------------------------------------

class PCCPhotShowDetectedStars : public MetaBoolean
{
public:

   PCCPhotShowDetectedStars( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCPhotShowDetectedStars* ThePCCPhotShowDetectedStarsParameter;

// ----------------------------------------------------------------------------

class PCCPhotShowBackgroundModels : public MetaBoolean
{
public:

   PCCPhotShowBackgroundModels( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCPhotShowBackgroundModels* ThePCCPhotShowBackgroundModelsParameter;

// ----------------------------------------------------------------------------

class PCCPhotGenerateGraphs : public MetaBoolean
{
public:

   PCCPhotGenerateGraphs( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCPhotGenerateGraphs* ThePCCPhotGenerateGraphsParameter;

// ----------------------------------------------------------------------------

class PCCApplyCalibration : public MetaBoolean
{
public:

   PCCApplyCalibration( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCApplyCalibration* ThePCCApplyCalibrationParameter;

// ----------------------------------------------------------------------------

class PCCNeutralizeBackground : public MetaBoolean
{
public:

   PCCNeutralizeBackground( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCNeutralizeBackground* ThePCCNeutralizeBackgroundParameter;

// ----------------------------------------------------------------------------

class PCCBackgroundReferenceViewId : public MetaString
{
public:

   PCCBackgroundReferenceViewId( MetaProcess* );

   virtual IsoString Id() const;
};

extern PCCBackgroundReferenceViewId* ThePCCBackgroundReferenceViewIdParameter;

// ----------------------------------------------------------------------------

class PCCBackgroundLow : public MetaFloat
{
public:

   PCCBackgroundLow( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCBackgroundLow* ThePCCBackgroundLowParameter;

// ----------------------------------------------------------------------------

class PCCBackgroundHigh : public MetaFloat
{
public:

   PCCBackgroundHigh( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern PCCBackgroundHigh* ThePCCBackgroundHighParameter;

// ----------------------------------------------------------------------------

class PCCBackgroundUseROI : public MetaBoolean
{
public:

   PCCBackgroundUseROI( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern PCCBackgroundUseROI* ThePCCBackgroundUseROIParameter;

// ----------------------------------------------------------------------------

class PCCBackgroundROIX0 : public MetaInt32
{
public:

   PCCBackgroundROIX0( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern PCCBackgroundROIX0* ThePCCBackgroundROIX0Parameter;

// ----------------------------------------------------------------------------

class PCCBackgroundROIY0 : public MetaInt32
{
public:

   PCCBackgroundROIY0( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern PCCBackgroundROIY0* ThePCCBackgroundROIY0Parameter;

// ----------------------------------------------------------------------------

class PCCBackgroundROIX1 : public MetaInt32
{
public:

   PCCBackgroundROIX1( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern PCCBackgroundROIX1* ThePCCBackgroundROIX1Parameter;

// ----------------------------------------------------------------------------

class PCCBackgroundROIY1 : public MetaInt32
{
public:

   PCCBackgroundROIY1( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern PCCBackgroundROIY1* ThePCCBackgroundROIY1Parameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __PhotometricColorCalibrationParameters_h

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationParameters.h - Released 2019-01-21T12:06:41Z
