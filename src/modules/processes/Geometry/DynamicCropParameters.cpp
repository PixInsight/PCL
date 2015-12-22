//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.01.00.0310
// ----------------------------------------------------------------------------
// DynamicCropParameters.cpp - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "DynamicCropParameters.h"

#include <pcl/Constants.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CropCenterX*            TheCropCenterXParameter = 0;
CropCenterY*            TheCropCenterYParameter = 0;
CropWidth*              TheCropWidthParameter = 0;
CropHeight*             TheCropHeightParameter = 0;
RotationAngle*          TheRotationAngleDynamicCropParameter = 0;
ScaleX*                 TheScaleXParameter = 0;
ScaleY*                 TheScaleYParameter = 0;
OptimizeFastRotations*  TheOptimizeFastRotationsDynamicCropParameter = 0;
InterpolationAlgorithm* TheInterpolationAlgorithmDynamicCropParameter = 0;
ClampingThreshold*      TheClampingThresholdDynamicCropParameter = 0;
Smoothness*             TheSmoothnessDynamicCropParameter = 0;
FillRed*                TheFillRedDynamicCropParameter = 0;
FillGreen*              TheFillGreenDynamicCropParameter = 0;
FillBlue*               TheFillBlueDynamicCropParameter = 0;
FillAlpha*              TheFillAlphaDynamicCropParameter = 0;

// ----------------------------------------------------------------------------

CropCenterX::CropCenterX( MetaProcess* p ) : MetaDouble( p )
{
   TheCropCenterXParameter = this;
}

IsoString CropCenterX::Id() const
{
   return "centerX";
}

int CropCenterX::Precision() const
{
   return 5;
}

double CropCenterX::DefaultValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

CropCenterY::CropCenterY( MetaProcess* p ) : MetaDouble( p )
{
   TheCropCenterYParameter = this;
}

IsoString CropCenterY::Id() const
{
   return "centerY";
}

int CropCenterY::Precision() const
{
   return 5;
}

double CropCenterY::DefaultValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

CropWidth::CropWidth( MetaProcess* p ) : MetaDouble( p )
{
   TheCropWidthParameter = this;
}

IsoString CropWidth::Id() const
{
   return "width";
}

int CropWidth::Precision() const
{
   return 5;
}

double CropWidth::DefaultValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

CropHeight::CropHeight( MetaProcess* p ) : MetaDouble( p )
{
   TheCropHeightParameter = this;
}

IsoString CropHeight::Id() const
{
   return "height";
}

int CropHeight::Precision() const
{
   return 5;
}

double CropHeight::DefaultValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

ScaleX::ScaleX( MetaProcess* p ) : MetaDouble( p )
{
   TheScaleXParameter = this;
}

IsoString ScaleX::Id() const
{
   return "scaleX";
}

int ScaleX::Precision() const
{
   return 5;
}

double ScaleX::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

ScaleY::ScaleY( MetaProcess* p ) : MetaDouble( p )
{
   TheScaleYParameter = this;
}

IsoString ScaleY::Id() const
{
   return "scaleY";
}

int ScaleY::Precision() const
{
   return 5;
}

double ScaleY::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicCropParameters.cpp - Released 2015/12/18 08:55:08 UTC
