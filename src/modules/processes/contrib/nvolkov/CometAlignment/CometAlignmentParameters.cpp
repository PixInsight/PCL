// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard CometAlignment Process Module Version 01.00.06.0057
// ****************************************************************************
// CometAlignmentParameters.cpp - Released 2014/10/29 07:35:26 UTC
// ****************************************************************************
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2014 Nikolay Volkov
// Copyright (c) 2003-2014 Pleiades Astrophoto S.L.
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
// ****************************************************************************

#include "CometAlignmentParameters.h"


namespace pcl
{

// ----------------------------------------------------------------------------

CATargetFrames*            TheTargetFrames = 0;
CATargetFrameEnabled*      TheTargetFrameEnabled = 0;
CATargetFramePath*         TheTargetFramePath = 0;

CATargetFrameDate*         TheTargetFrameDate = 0;
CATargetFrameJDate*        TheTargetFrameJDate = 0;
CATargetFrameX*            TheTargetFrameX = 0;
CATargetFrameY*            TheTargetFrameY = 0;

CAOutputDir*               TheOutputDir = 0;
CAPrefix*                  ThePrefix = 0;
CAPostfix*                 ThePostfix = 0;
CAOverwrite*               TheOverwrite = 0;

CAReference*               TheReference = 0;
CAPixelInterpolation*      ThePixelInterpolationParameter = 0;
CALinearClampingThreshold* TheLinearClampingThresholdParameter = 0;

// ----------------------------------------------------------------------------

CATargetFrames::CATargetFrames( MetaProcess* P ) : MetaTable( P )
{
   TheTargetFrames = this;
}

IsoString CATargetFrames::Id() const
{
   return "targetFrames";
}

// ----------------------------------------------------------------------------

CATargetFrameEnabled::CATargetFrameEnabled( MetaTable* T ) : MetaBoolean( T ) { TheTargetFrameEnabled = this; }
IsoString CATargetFrameEnabled::Id() const { return "enabled"; }
bool CATargetFrameEnabled::DefaultValue() const { return true; }

// ----------------------------------------------------------------------------

CATargetFramePath::CATargetFramePath( MetaTable* T ) : MetaString( T ) { TheTargetFramePath = this; }
IsoString CATargetFramePath::Id() const { return "path"; }

// ----------------------------------------------------------------------------

CATargetFrameDate::CATargetFrameDate( MetaTable* T ) : MetaString( T ) { TheTargetFrameDate = this; }
IsoString CATargetFrameDate::Id() const { return "date"; }

// ----------------------------------------------------------------------------

CATargetFrameJDate::CATargetFrameJDate( MetaTable* T ) : MetaDouble( T ) { TheTargetFrameJDate = this; }
IsoString CATargetFrameJDate::Id() const { return "JDate"; }

// ----------------------------------------------------------------------------
CATargetFrameX::CATargetFrameX( MetaTable* T ) : MetaDouble( T ) { TheTargetFrameX = this; }
IsoString CATargetFrameX::Id() const { return "x"; }
int CATargetFrameX::Precision() const { return 8; }
double CATargetFrameX::MinimumValue() const { return 0; }
double CATargetFrameX::MaximumValue() const { return 100000; }

// ----------------------------------------------------------------------------
CATargetFrameY::CATargetFrameY( MetaTable* T ) : MetaDouble( T ) { TheTargetFrameY = this; }
IsoString CATargetFrameY::Id() const { return "y"; }
int CATargetFrameY::Precision() const { return 8; }
double CATargetFrameY::MinimumValue() const { return 0; }
double CATargetFrameY::MaximumValue() const { return 100000; }

// ----------------------------------------------------------------------------

CAOutputDir::CAOutputDir( MetaProcess* P ) : MetaString( P ) { TheOutputDir = this; }
IsoString CAOutputDir::Id() const { return "outputDir"; }
String CAOutputDir::DefaultValue() const { return ""; }

// ----------------------------------------------------------------------------

CAPrefix::CAPrefix( MetaProcess* P ) : MetaString( P ) { ThePrefix = this; }
IsoString CAPrefix::Id() const { return "prefix"; }
String CAPrefix::DefaultValue() const { return ""; }

// ----------------------------------------------------------------------------

CAPostfix::CAPostfix( MetaProcess* P ) : MetaString( P ) { ThePostfix = this; }
IsoString CAPostfix::Id() const { return "postfix"; }
String CAPostfix::DefaultValue() const { return "_ca"; }

// ----------------------------------------------------------------------------

CAOverwrite::CAOverwrite( MetaProcess* P ) : MetaBoolean( P ) { TheOverwrite = this; }
IsoString CAOverwrite::Id() const { return "overwrite"; }
bool CAOverwrite::DefaultValue() const { return true; }

// ----------------------------------------------------------------------------
CAReference::CAReference( MetaProcess* P ) : MetaUInt16( P ) { TheReference = this; }
IsoString CAReference::Id() const { return "reference"; }

// ----------------------------------------------------------------------------

CAPixelInterpolation::CAPixelInterpolation( MetaProcess* p ) : MetaEnumeration( p ) { ThePixelInterpolationParameter = this; }
IsoString CAPixelInterpolation::Id() const { return "pixelInterpolation"; }
size_type CAPixelInterpolation::NumberOfElements() const { return NumberOfInterpolationAlgorithms; }
IsoString CAPixelInterpolation::ElementId( size_type i ) const
{
   switch ( i )
   {
   case NearestNeighbor         : return "NearestNeighbor";
   case Bilinear                : return "Bilinear";
   default:
   case BicubicSpline           : return "BicubicSpline";
   case BicubicBSpline          : return "BicubicBSpline";
   case Lanczos3                : return "Lanczos3";
   case Lanczos4                : return "Lanczos4";
   case Lanczos5                : return "Lanczos5";
   case MitchellNetravaliFilter : return "MitchellNetravaliFilter";
   case CatmullRomSplineFilter  : return "CatmullRomSplineFilter";
   case CubicBSplineFilter      : return "CubicBSplineFilter";
   case Auto                    : return "Auto";
   }
}
int CAPixelInterpolation::ElementValue( size_type i ) const { return int( i ); }
size_type CAPixelInterpolation::DefaultValueIndex() const { return size_type( Default ); }

// ----------------------------------------------------------------------------

CALinearClampingThreshold::CALinearClampingThreshold( MetaProcess* p ) : MetaFloat( p ) { TheLinearClampingThresholdParameter = this; }
IsoString CALinearClampingThreshold::Id() const { return "linearClampingThreshold"; }
int CALinearClampingThreshold::Precision() const { return 2; }
double CALinearClampingThreshold::DefaultValue() const { return 0.3; }  // __PCL_BICUBIC_SPLINE_CLAMPING_THRESHOLD;
double CALinearClampingThreshold::MinimumValue() const { return 0; }
double CALinearClampingThreshold::MaximumValue() const { return 1; }

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF CometAlignmentParameters.cpp - Released 2014/10/29 07:35:26 UTC
