//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0257
// ----------------------------------------------------------------------------
// ConvolutionParameters.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
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

#include "ConvolutionParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ConMode*            TheConModeParameter = 0;
ConSigma*           TheConSigmaParameter = 0;
ConShape*           TheConShapeParameter = 0;
ConAspectRatio*     TheConAspectRatioParameter = 0;
ConRotationAngle*   TheConRotationAngleParameter = 0;
ConSourceCode*      TheConSourceCodeParameter = 0;
ConRescaleHighPass* TheConRescaleHighPassParameter;
ConViewId*          TheConViewIdParameter = 0;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ConMode::ConMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheConModeParameter = this;
}

IsoString ConMode::Id() const
{
   return "mode";
}

size_type ConMode::NumberOfElements() const
{
    return NumberOfModes;
}

IsoString ConMode::ElementId( size_type i) const
{
   switch ( i )
   {
   default:
   case Parametric: return "Parametric";
   case Library:    return "Library";
   case Image:      return "Image";
   }
}

int ConMode::ElementValue( size_type i ) const
{
    return int( i );
}

size_type ConMode::DefaultValueIndex() const
{
   return size_type( Default );
}

IsoString ConMode::ElementAliases() const
{
   return "Gaussian=Parametric";
}

// ----------------------------------------------------------------------------

ConSigma::ConSigma( MetaProcess* P ) : MetaFloat( P )
{
   TheConSigmaParameter = this;
}

IsoString ConSigma::Id() const
{
   return "sigma";
}

int ConSigma::Precision() const
{
   return 2;
}

double ConSigma::MinimumValue() const
{
   return 0.1;
}

double ConSigma::MaximumValue() const
{
   return 250;
}

double ConSigma::DefaultValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

ConShape::ConShape( MetaProcess* P ) : MetaFloat( P )
{
   TheConShapeParameter = this;
}

IsoString ConShape::Id() const
{
   return "shape";
}

int ConShape::Precision() const
{
   return 2;
}

double ConShape::MinimumValue() const
{
   return 0.25;
}

double ConShape::MaximumValue() const
{
   return 6;
}

double ConShape::DefaultValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

ConAspectRatio::ConAspectRatio( MetaProcess* P ) : MetaFloat( P )
{
   TheConAspectRatioParameter = this;
}

IsoString ConAspectRatio::Id() const
{
   return "aspectRatio";
}

int ConAspectRatio::Precision() const
{
   return 2;
}

double ConAspectRatio::MinimumValue() const
{
   return 0;
}

double ConAspectRatio::MaximumValue() const
{
   return 1;
}

double ConAspectRatio::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

ConRotationAngle::ConRotationAngle( MetaProcess* P ) : MetaFloat( P )
{
   TheConRotationAngleParameter = this;
}

IsoString ConRotationAngle::Id() const
{
   return "rotationAngle";
}

int ConRotationAngle::Precision() const
{
   return 2;
}

double ConRotationAngle::MinimumValue() const
{
   return 0;
}

double ConRotationAngle::MaximumValue() const
{
   return 180;
}

double ConRotationAngle::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ConSourceCode::ConSourceCode( MetaProcess* P ) : MetaString( P )
{
   TheConSourceCodeParameter = this;
}

IsoString ConSourceCode::Id() const
{
   return "filterSource";
}

// ----------------------------------------------------------------------------

ConRescaleHighPass::ConRescaleHighPass( MetaProcess* P ) : MetaBoolean( P )
{
   TheConRescaleHighPassParameter = this;
}

IsoString ConRescaleHighPass::Id() const
{
   return "rescaleHighPass";
}

bool ConRescaleHighPass::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ConViewId::ConViewId( MetaProcess* P ) : MetaString( P )
{
   TheConViewIdParameter = this;
}

IsoString ConViewId::Id() const
{
   return "viewId";
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ConvolutionParameters.cpp - Released 2017-08-01T14:26:58Z
