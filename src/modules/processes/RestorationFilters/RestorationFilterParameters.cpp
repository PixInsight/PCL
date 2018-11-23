//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 01.00.05.0336
// ----------------------------------------------------------------------------
// RestorationFilterParameters.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard RestorationFilters PixInsight module.
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

#include "RestorationFilterParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RFWienerK*                    TheRFWienerKParameter = nullptr;
RFLeastSquaresGamma*          TheRFLeastSquaresGammaParameter = nullptr;
RFAlgorithm*                  TheRFAlgorithmParameter = nullptr;
RFPSFMode*                    TheRFPSFModeParameter = nullptr;
RFPSFParametricSigma*         TheRFPSFParametricSigmaParameter = nullptr;
RFPSFParametricShape*         TheRFPSFParametricShapeParameter = nullptr;
RFPSFParametricAspectRatio*   TheRFPSFParametricAspectRatioParameter = nullptr;
RFPSFParametricRotationAngle* TheRFPSFParametricRotationAngleParameter = nullptr;
RFPSFMotionLength*            TheRFPSFMotionLengthParameter = nullptr;
RFPSFMotionRotationAngle*     TheRFPSFMotionRotationAngleParameter = nullptr;
RFPSFViewId*                  TheRFPSFViewIdParameter = nullptr;
RFAmount*                     TheRFAmountParameter = nullptr;
RFToLuminance*                TheRFToLuminanceParameter = nullptr;
RFLinear*                     TheRFLinearParameter = nullptr;
RFDeringing*                  TheRFDeringingParameter = nullptr;
RFDeringingDark*              TheRFDeringingDarkParameter = nullptr;
RFDeringingBright*            TheRFDeringingBrightParameter = nullptr;
RFOutputDeringingMaps*        TheRFOutputDeringingMapsParameter = nullptr;
RFRangeLow*                   TheRFRangeLowParameter = nullptr;
RFRangeHigh*                  TheRFRangeHighParameter = nullptr;

// ----------------------------------------------------------------------------

RFWienerK::RFWienerK( MetaProcess* P ) : MetaDouble( P )
{
   TheRFWienerKParameter = this;
}

IsoString RFWienerK::Id() const
{
   return "K";
}

int RFWienerK::Precision() const
{
   return 15;
}

double RFWienerK::MinimumValue() const
{
   return 1.0e-15;
}

double RFWienerK::MaximumValue() const
{
   return 1;
}

double RFWienerK::DefaultValue() const
{
   return 0.0001;
}

// ----------------------------------------------------------------------------

RFLeastSquaresGamma::RFLeastSquaresGamma( MetaProcess* P ) : MetaDouble( P )
{
   TheRFLeastSquaresGammaParameter = this;
}

IsoString RFLeastSquaresGamma::Id() const
{
   return "gamma";
}

int RFLeastSquaresGamma::Precision() const
{
   return 15;
}

double RFLeastSquaresGamma::MinimumValue() const
{
   return 1.0e-15;
}

double RFLeastSquaresGamma::MaximumValue() const
{
   return 1;
}

double RFLeastSquaresGamma::DefaultValue() const
{
   return 0.001;
}

// ----------------------------------------------------------------------------

RFAlgorithm::RFAlgorithm( MetaProcess* P ) : MetaEnumeration( P )
{
   TheRFAlgorithmParameter = this;
}

IsoString RFAlgorithm::Id() const
{
   return "algorithm";
}

size_type RFAlgorithm::NumberOfElements() const
{
    return NumberOfAlgorithms;
}

IsoString RFAlgorithm::ElementId( size_type i) const
{
   switch ( i )
   {
   case Wiener:                  return "Wiener";
   default:
   case ConstrainedLeastSquares: return "ConstrainedLeastSquares";
   }
}

int RFAlgorithm::ElementValue( size_type i ) const
{
    return int( i );
}

size_type RFAlgorithm::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

RFPSFMode::RFPSFMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheRFPSFModeParameter = this;
}

IsoString RFPSFMode::Id() const
{
   return "psfMode";
}

size_type RFPSFMode::NumberOfElements() const
{
    return NumberOfModes;
}

IsoString RFPSFMode::ElementId( size_type i) const
{
   switch ( i )
   {
   default:
   case Parametric: return "Parametric";
   case MotionBlur: return "MotionBlur";
   case External:   return "External";
   }
}

int RFPSFMode::ElementValue( size_type i ) const
{
    return int( i );
}

size_type RFPSFMode::DefaultValueIndex() const
{
   return size_type( Default );
}

IsoString RFPSFMode::ElementAliases() const
{
   return "Gaussian=Parametric";
}

// ----------------------------------------------------------------------------

RFPSFParametricSigma::RFPSFParametricSigma( MetaProcess* P ) : MetaFloat( P )
{
   TheRFPSFParametricSigmaParameter = this;
}

IsoString RFPSFParametricSigma::Id() const
{
   return "psfSigma";
}

IsoString RFPSFParametricSigma::Aliases() const
{
   return "psfGaussianSigma";
}

int RFPSFParametricSigma::Precision() const
{
   return 2;
}

double RFPSFParametricSigma::MinimumValue() const
{
   return 0.1;
}

double RFPSFParametricSigma::MaximumValue() const
{
   return 25;
}

double RFPSFParametricSigma::DefaultValue() const
{
   return 2.0;
}

// ----------------------------------------------------------------------------

RFPSFParametricShape::RFPSFParametricShape( MetaProcess* P ) : MetaFloat( P )
{
   TheRFPSFParametricShapeParameter = this;
}

IsoString RFPSFParametricShape::Id() const
{
   return "psfShape";
}

IsoString RFPSFParametricShape::Aliases() const
{
   return "psfGaussianShape";
}

int RFPSFParametricShape::Precision() const
{
   return 2;
}

double RFPSFParametricShape::MinimumValue() const
{
   return 0.25;
}

double RFPSFParametricShape::MaximumValue() const
{
   return 6;
}

double RFPSFParametricShape::DefaultValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

RFPSFParametricAspectRatio::RFPSFParametricAspectRatio( MetaProcess* P ) : MetaFloat( P )
{
   TheRFPSFParametricAspectRatioParameter = this;
}

IsoString RFPSFParametricAspectRatio::Id() const
{
   return "psfAspectRatio";
}

IsoString RFPSFParametricAspectRatio::Aliases() const
{
   return "psfGaussianAspectRatio";
}

int RFPSFParametricAspectRatio::Precision() const
{
   return 2;
}

double RFPSFParametricAspectRatio::MinimumValue() const
{
   return 0;
}

double RFPSFParametricAspectRatio::MaximumValue() const
{
   return 1;
}

double RFPSFParametricAspectRatio::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

RFPSFParametricRotationAngle::RFPSFParametricRotationAngle( MetaProcess* P ) : MetaFloat( P )
{
   TheRFPSFParametricRotationAngleParameter = this;
}

IsoString RFPSFParametricRotationAngle::Id() const
{
   return "psfRotationAngle";
}

IsoString RFPSFParametricRotationAngle::Aliases() const
{
   return "psfGaussianRotationAngle";
}

int RFPSFParametricRotationAngle::Precision() const
{
   return 2;
}

double RFPSFParametricRotationAngle::MinimumValue() const
{
   return 0;
}

double RFPSFParametricRotationAngle::MaximumValue() const
{
   return 180;
}

double RFPSFParametricRotationAngle::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

RFPSFMotionLength::RFPSFMotionLength( MetaProcess* P ) : MetaFloat( P )
{
   TheRFPSFMotionLengthParameter = this;
}

IsoString RFPSFMotionLength::Id() const
{
   return "psfMotionLength";
}

int RFPSFMotionLength::Precision() const
{
   return 2;
}

double RFPSFMotionLength::MinimumValue() const
{
   return 0.01;
}

double RFPSFMotionLength::MaximumValue() const
{
   return 100;
}

double RFPSFMotionLength::DefaultValue() const
{
   return 5;
}

// ----------------------------------------------------------------------------

RFPSFMotionRotationAngle::RFPSFMotionRotationAngle( MetaProcess* P ) : MetaFloat( P )
{
   TheRFPSFMotionRotationAngleParameter = this;
}

IsoString RFPSFMotionRotationAngle::Id() const
{
   return "psfMotionRotationAngle";
}

int RFPSFMotionRotationAngle::Precision() const
{
   return 2;
}

double RFPSFMotionRotationAngle::MinimumValue() const
{
   return 0;
}

double RFPSFMotionRotationAngle::MaximumValue() const
{
   return 180;
}

double RFPSFMotionRotationAngle::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

RFPSFViewId::RFPSFViewId( MetaProcess* P ) : MetaString( P )
{
   TheRFPSFViewIdParameter = this;
}

IsoString RFPSFViewId::Id() const
{
   return "psfViewId";
}

// ----------------------------------------------------------------------------

RFAmount::RFAmount( MetaProcess* P ) : MetaFloat( P )
{
   TheRFAmountParameter = this;
}

IsoString RFAmount::Id() const
{
   return "amount";
}

int RFAmount::Precision() const
{
   return 2;
}

double RFAmount::MinimumValue() const
{
   return 0;
}

double RFAmount::MaximumValue() const
{
   return 1;
}

double RFAmount::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

RFToLuminance::RFToLuminance( MetaProcess* P ) : MetaBoolean( P )
{
   TheRFToLuminanceParameter = this;
}

IsoString RFToLuminance::Id() const
{
   return "toLuminance";
}

IsoString RFToLuminance::Aliases() const
{
   return "useLuminance";
}

bool RFToLuminance::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

RFLinear::RFLinear( MetaProcess* P ) : MetaBoolean( P )
{
   TheRFLinearParameter = this;
}

IsoString RFLinear::Id() const
{
   return "linear";
}

bool RFLinear::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

RFDeringing::RFDeringing( MetaProcess* P ) : MetaBoolean( P )
{
   TheRFDeringingParameter = this;
}

IsoString RFDeringing::Id() const
{
   return "deringing";
}

bool RFDeringing::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

RFDeringingDark::RFDeringingDark( MetaProcess* P ) : MetaFloat( P )
{
   TheRFDeringingDarkParameter = this;
}

IsoString RFDeringingDark::Id() const
{
   return "deringingDark";
}

int RFDeringingDark::Precision() const
{
   return 4;
}

double RFDeringingDark::DefaultValue() const
{
   return 0.1000;
}

double RFDeringingDark::MinimumValue() const
{
   return 0;
}

double RFDeringingDark::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

RFDeringingBright::RFDeringingBright( MetaProcess* P ) : MetaFloat( P )
{
   TheRFDeringingBrightParameter = this;
}

IsoString RFDeringingBright::Id() const
{
   return "deringingBright";
}

int RFDeringingBright::Precision() const
{
   return 4;
}

double RFDeringingBright::DefaultValue() const
{
   return 0;
}

double RFDeringingBright::MinimumValue() const
{
   return 0;
}

double RFDeringingBright::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

RFOutputDeringingMaps::RFOutputDeringingMaps( MetaProcess* P ) : MetaBoolean( P )
{
   TheRFOutputDeringingMapsParameter = this;
}

IsoString RFOutputDeringingMaps::Id() const
{
   return "outputDeringingMaps";
}

bool RFOutputDeringingMaps::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

RFRangeLow::RFRangeLow( MetaProcess* P ) : MetaFloat( P )
{
   TheRFRangeLowParameter = this;
}

IsoString RFRangeLow::Id() const
{
   return "rangeLow";
}

int RFRangeLow::Precision() const
{
   return 7;
}

double RFRangeLow::MinimumValue() const
{
   return 0;
}

double RFRangeLow::MaximumValue() const
{
   return 1;
}

double RFRangeLow::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

RFRangeHigh::RFRangeHigh( MetaProcess* P ) : MetaFloat( P )
{
   TheRFRangeHighParameter = this;
}

IsoString RFRangeHigh::Id() const
{
   return "rangeHigh";
}

int RFRangeHigh::Precision() const
{
   return 7;
}

double RFRangeHigh::MinimumValue() const
{
   return 0;
}

double RFRangeHigh::MaximumValue() const
{
   return 1;
}

double RFRangeHigh::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RestorationFilterParameters.cpp - Released 2018-11-23T18:45:58Z
