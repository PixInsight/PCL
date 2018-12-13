//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard GREYCstoration Process Module Version 01.00.02.0341
// ----------------------------------------------------------------------------
// GREYCstorationParameters.cpp - Released 2018-12-12T09:25:24Z
// ----------------------------------------------------------------------------
// This file is part of the standard GREYCstoration PixInsight module.
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

/******************************************************************************
 * CImg Library and GREYCstoration Algorithm:
 *   Copyright David Tschumperlé - http://www.greyc.ensicaen.fr/~dtschump/
 *
 * See:
 *   http://cimg.sourceforge.net
 *   http://www.greyc.ensicaen.fr/~dtschump/greycstoration/
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL:
 * "http://www.cecill.info".
 *****************************************************************************/

#include "GREYCstorationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GREYCsAmplitude*         TheGREYCsAmplitudeParameter = 0;
GREYCsIterations*        TheGREYCsIterationsParameter = 0;
GREYCsSharpness*         TheGREYCsSharpnessParameter = 0;
GREYCsAnisotropy*        TheGREYCsAnisotropyParameter = 0;
GREYCsAlpha*             TheGREYCsAlphaParameter = 0;
GREYCsSigma*             TheGREYCsSigmaParameter = 0;
GREYCsFastApproximation* TheGREYCsFastApproximationParameter = 0;
GREYCsPrecision*         TheGREYCsPrecisionParameter = 0;
GREYCsSpatialStepSize*   TheGREYCsSpatialStepSizeParameter = 0;
GREYCsAngularStepSize*   TheGREYCsAngularStepSizeParameter = 0;
GREYCsInterpolation*     TheGREYCsInterpolationParameter = 0;
GREYCsCoupledChannels*   TheGREYCsCoupledChannelsParameter = 0;

// ----------------------------------------------------------------------------

GREYCsAmplitude::GREYCsAmplitude( MetaProcess* P ) : MetaFloat( P )
{
   TheGREYCsAmplitudeParameter = this;
}

IsoString GREYCsAmplitude::Id() const
{
   return "amplitude";
}

int GREYCsAmplitude::Precision() const
{
   return 1;
}

double GREYCsAmplitude::DefaultValue() const
{
   return 60;
}

double GREYCsAmplitude::MinimumValue() const
{
   return 0;
}

double GREYCsAmplitude::MaximumValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

GREYCsIterations::GREYCsIterations( MetaProcess* P ) : MetaInt32( P )
{
   TheGREYCsIterationsParameter = this;
}

IsoString GREYCsIterations::Id() const
{
   return "numberOfIterations";
}

double GREYCsIterations::DefaultValue() const
{
   return 1;
}

double GREYCsIterations::MinimumValue() const
{
   return 1;
}

double GREYCsIterations::MaximumValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

GREYCsSharpness::GREYCsSharpness( MetaProcess* P ) : MetaFloat( P )
{
   TheGREYCsSharpnessParameter = this;
}

IsoString GREYCsSharpness::Id() const
{
   return "contourPreservation";
}

int GREYCsSharpness::Precision() const
{
   return 2;
}

double GREYCsSharpness::DefaultValue() const
{
   return 0.7;
}

double GREYCsSharpness::MinimumValue() const
{
   return 0;
}

double GREYCsSharpness::MaximumValue() const
{
   return 2.5;
}

// ----------------------------------------------------------------------------

GREYCsAnisotropy::GREYCsAnisotropy( MetaProcess* P ) : MetaFloat( P )
{
   TheGREYCsAnisotropyParameter = this;
}

IsoString GREYCsAnisotropy::Id() const
{
   return "anisotropy";
}

int GREYCsAnisotropy::Precision() const
{
   return 2;
}

double GREYCsAnisotropy::DefaultValue() const
{
   return 0.3;
}

double GREYCsAnisotropy::MinimumValue() const
{
   return 0;
}

double GREYCsAnisotropy::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

GREYCsAlpha::GREYCsAlpha( MetaProcess* P ) : MetaFloat( P )
{
   TheGREYCsAlphaParameter = this;
}

IsoString GREYCsAlpha::Id() const
{
   return "noiseScale";
}

int GREYCsAlpha::Precision() const
{
   return 2;
}

double GREYCsAlpha::DefaultValue() const
{
   return 0.6;
}

double GREYCsAlpha::MinimumValue() const
{
   return 0;
}

double GREYCsAlpha::MaximumValue() const
{
   return 3;
}

// ----------------------------------------------------------------------------

GREYCsSigma::GREYCsSigma( MetaProcess* P ) : MetaFloat( P )
{
   TheGREYCsSigmaParameter = this;
}

IsoString GREYCsSigma::Id() const
{
   return "geometryRegularity";
}

int GREYCsSigma::Precision() const
{
   return 2;
}

double GREYCsSigma::DefaultValue() const
{
   return 1.1;
}

double GREYCsSigma::MinimumValue() const
{
   return 0;
}

double GREYCsSigma::MaximumValue() const
{
   return 3;
}

// ----------------------------------------------------------------------------

GREYCsFastApproximation::GREYCsFastApproximation( MetaProcess* P ) : MetaBoolean( P )
{
   TheGREYCsFastApproximationParameter = this;
}

IsoString GREYCsFastApproximation::Id() const
{
   return "fastApproximation";
}

bool GREYCsFastApproximation::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

GREYCsPrecision::GREYCsPrecision( MetaProcess* P ) : MetaFloat( P )
{
   TheGREYCsPrecisionParameter = this;
}

IsoString GREYCsPrecision::Id() const
{
   return "precision";
}

int GREYCsPrecision::Precision() const
{
   return 1;
}

double GREYCsPrecision::DefaultValue() const
{
   return 2;
}

double GREYCsPrecision::MinimumValue() const
{
   return 0.1;
}

double GREYCsPrecision::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

GREYCsSpatialStepSize::GREYCsSpatialStepSize( MetaProcess* P ) : MetaFloat( P )
{
   TheGREYCsSpatialStepSizeParameter = this;
}

IsoString GREYCsSpatialStepSize::Id() const
{
   return "spatialIntegrationStep";
}

int GREYCsSpatialStepSize::Precision() const
{
   return 2;
}

double GREYCsSpatialStepSize::DefaultValue() const
{
   return 0.8;
}

double GREYCsSpatialStepSize::MinimumValue() const
{
   return 0.1;
}

double GREYCsSpatialStepSize::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

GREYCsAngularStepSize::GREYCsAngularStepSize( MetaProcess* P ) : MetaFloat( P )
{
   TheGREYCsAngularStepSizeParameter = this;
}

IsoString GREYCsAngularStepSize::Id() const
{
   return "angularIntegrationStep";
}

int GREYCsAngularStepSize::Precision() const
{
   return 0;
}

double GREYCsAngularStepSize::DefaultValue() const
{
   return 30;
}

double GREYCsAngularStepSize::MinimumValue() const
{
   return 1;
}

double GREYCsAngularStepSize::MaximumValue() const
{
   return 90;
}

// ----------------------------------------------------------------------------

GREYCsInterpolation::GREYCsInterpolation( MetaProcess* p ) : MetaEnumeration( p )
{
   TheGREYCsInterpolationParameter = this;
}

IsoString GREYCsInterpolation::Id() const
{
   return "interpolation";
}

size_type GREYCsInterpolation::NumberOfElements() const
{
   return 3;
}

IsoString GREYCsInterpolation::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Nearest:    return "Nearest";
   case Bilinear:   return "Bilinear";
   case RungeKutta: return "RungeKutta";
   }
}

int GREYCsInterpolation::ElementValue( size_type i ) const
{
   return int( i );
}

size_type GREYCsInterpolation::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

GREYCsCoupledChannels::GREYCsCoupledChannels( MetaProcess* P ) : MetaBoolean( P )
{
   TheGREYCsCoupledChannelsParameter = this;
}

IsoString GREYCsCoupledChannels::Id() const
{
   return "coupledChannels";
}

bool GREYCsCoupledChannels::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GREYCstorationParameters.cpp - Released 2018-12-12T09:25:24Z
