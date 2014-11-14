// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard Geometry Process Module Version 01.01.00.0247
// ****************************************************************************
// CommonParameters.cpp - Released 2014/11/14 17:18:46 UTC
// ****************************************************************************
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "CommonParameters.h"

#include <pcl/Constants.h>
#include <pcl/ImageVariant.h>

namespace pcl
{

// ----------------------------------------------------------------------------

RotationAngle::RotationAngle( MetaProcess* p ) : MetaDouble( p )
{
}

IsoString RotationAngle::Id() const
{
   return "angle";
}

int RotationAngle::Precision() const
{
   return 4;
}

double RotationAngle::DefaultValue() const
{
   return 0;
}

double RotationAngle::MinimumValue() const
{
   return -Const<double>::pi();
}

double RotationAngle::MaximumValue() const
{
   return +Const<double>::pi();
}

// ----------------------------------------------------------------------------

OptimizeFastRotations::OptimizeFastRotations( MetaProcess* p ) : MetaBoolean( p )
{
}

IsoString OptimizeFastRotations::Id() const
{
   return "optimizeFast";
}

bool OptimizeFastRotations::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

InterpolationAlgorithm::InterpolationAlgorithm( MetaProcess* p ) : MetaEnumeration( p )
{
}

IsoString InterpolationAlgorithm::Id() const
{
   return "interpolation";
}

size_type InterpolationAlgorithm::NumberOfElements() const
{
   return NumberOfInterpolationAlgorithms;
}

IsoString InterpolationAlgorithm::ElementId( size_type i ) const
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
   case MitchellNetravaliFilter : return "MitchellNetravaliFilter";
   case CatmullRomSplineFilter  : return "CatmullRomSplineFilter";
   case CubicBSplineFilter      : return "CubicBSplineFilter";
   case Auto                    : return "Auto";
   }
}

int InterpolationAlgorithm::ElementValue( size_type i ) const
{
   return int( i );
}

size_type InterpolationAlgorithm::DefaultValueIndex() const
{
   return Default;
}

PixelInterpolation* NewInterpolation( int interpolation, int w, int h, int w0, int h0,
                                      bool rotate, float clampingThreshold, float smoothness,
                                      ImageVariant& image )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: return NewInterpolation( interpolation, w, h, w0, h0,
                                        rotate, clampingThreshold, smoothness,
                                        reinterpret_cast<FloatPixelTraits*>( 0 ) );
      case 64: return NewInterpolation( interpolation, w, h, w0, h0,
                                        rotate, clampingThreshold, smoothness,
                                        reinterpret_cast<DoublePixelTraits*>( 0 ) );
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: return NewInterpolation( interpolation, w, h, w0, h0,
                                        rotate, clampingThreshold, smoothness,
                                        reinterpret_cast<UInt8PixelTraits*>( 0 ) );
      case 16: return NewInterpolation( interpolation, w, h, w0, h0,
                                        rotate, clampingThreshold, smoothness,
                                        reinterpret_cast<UInt16PixelTraits*>( 0 ) );
      case 32: return NewInterpolation( interpolation, w, h, w0, h0,
                                        rotate, clampingThreshold, smoothness,
                                        reinterpret_cast<UInt32PixelTraits*>( 0 ) );
      }
   return 0;
}

// ----------------------------------------------------------------------------

ClampingThreshold::ClampingThreshold( MetaProcess* p ) : MetaFloat( p )
{
}

IsoString ClampingThreshold::Id() const
{
   return "clampingThreshold";
}

int ClampingThreshold::Precision() const
{
   return 2;
}

double ClampingThreshold::DefaultValue() const
{
   return 0.30;
}

double ClampingThreshold::MinimumValue() const
{
   return 0;
}

double ClampingThreshold::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

Smoothness::Smoothness( MetaProcess* P ) : MetaFloat( P )
{
}

IsoString Smoothness::Id() const
{
   return "smoothness";
}

int Smoothness::Precision() const
{
   return 2;
}

double Smoothness::DefaultValue() const
{
   return 1.5;
}

double Smoothness::MinimumValue() const
{
   return 1;
}

double Smoothness::MaximumValue() const
{
   return 5;
}

// ----------------------------------------------------------------------------

XResolution::XResolution( MetaProcess* P ) : MetaDouble( P )
{
}

IsoString XResolution::Id() const
{
   return "xResolution";
}

int XResolution::Precision() const
{
   return 3;
}

double XResolution::DefaultValue() const
{
   return 72;
}

double XResolution::MinimumValue() const
{
   return 1;
}

double XResolution::MaximumValue() const
{
   return 10000;
}

// ----------------------------------------------------------------------------

YResolution::YResolution( MetaProcess* P ) : MetaDouble( P )
{
}

IsoString YResolution::Id() const
{
   return "yResolution";
}

int YResolution::Precision() const
{
   return 3;
}

double YResolution::DefaultValue() const
{
   return 72;
}

double YResolution::MinimumValue() const
{
   return 1;
}

double YResolution::MaximumValue() const
{
   return 10000;
}

// ----------------------------------------------------------------------------

MetricResolution::MetricResolution( MetaProcess* P ) : MetaBoolean( P )
{
}

IsoString MetricResolution::Id() const
{
   return "metric";
}

bool MetricResolution::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ForceResolution::ForceResolution( MetaProcess* P ) : MetaBoolean( P )
{
}

IsoString ForceResolution::Id() const
{
   return "forceResolution";
}

bool ForceResolution::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

FillRed::FillRed( MetaProcess* p ) : MetaDouble( p )
{
}

IsoString FillRed::Id() const
{
   return "red";
}

int FillRed::Precision() const
{
   return 6;
}

double FillRed::DefaultValue() const
{
   return 0;
}

double FillRed::MinimumValue() const
{
   return 0;
}

double FillRed::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

FillGreen::FillGreen( MetaProcess* p ) : MetaDouble( p )
{
}

IsoString FillGreen::Id() const
{
   return "green";
}

int FillGreen::Precision() const
{
   return 6;
}

double FillGreen::DefaultValue() const
{
   return 0;
}

double FillGreen::MinimumValue() const
{
   return 0;
}

double FillGreen::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

FillBlue::FillBlue( MetaProcess* p ) : MetaDouble( p )
{
}

IsoString FillBlue::Id() const
{
   return "blue";
}

int FillBlue::Precision() const
{
   return 6;
}

double FillBlue::DefaultValue() const
{
   return 0;
}

double FillBlue::MinimumValue() const
{
   return 0;
}

double FillBlue::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

FillAlpha::FillAlpha( MetaProcess* p ) : MetaDouble( p )
{
}

IsoString FillAlpha::Id() const
{
   return "alpha";
}

int FillAlpha::Precision() const
{
   return 6;
}

double FillAlpha::DefaultValue() const
{
   return 1; // default alpha = 1
}

double FillAlpha::MinimumValue() const
{
   return 0;
}

double FillAlpha::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF CommonParameters.cpp - Released 2014/11/14 17:18:46 UTC
