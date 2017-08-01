//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.01.0377
// ----------------------------------------------------------------------------
// ResampleParameters.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#include "ResampleParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RSXSize*                TheRSXSizeParameter = nullptr;
RSYSize*                TheRSYSizeParameter = nullptr;
RSMode*                 TheRSModeParameter = nullptr;
RSAbsoluteMode*         TheRSAbsoluteModeParameter = nullptr;
XResolution*            TheRSXResolutionParameter = nullptr;
YResolution*            TheRSYResolutionParameter = nullptr;
MetricResolution*       TheRSMetricResolutionParameter = nullptr;
ForceResolution*        TheRSForceResolutionParameter = nullptr;
InterpolationAlgorithm* TheRSInterpolationAlgorithmParameter = nullptr;
ClampingThreshold*      TheRSClampingThresholdParameter = nullptr;
Smoothness*             TheRSSmoothnessParameter = nullptr;
NoGUIMessages*          TheRSNoGUIMessagesParameter = nullptr;

// ----------------------------------------------------------------------------

RSXSize::RSXSize( MetaProcess* P ) : MetaDouble( P )
{
   TheRSXSizeParameter = this;
}

IsoString RSXSize::Id() const
{
   return "xSize";
}

int RSXSize::Precision() const
{
   return 4;
}

double RSXSize::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

RSYSize::RSYSize( MetaProcess* P ) : MetaDouble( P )
{
   TheRSYSizeParameter = this;
}

IsoString RSYSize::Id() const
{
   return "ySize";
}

int RSYSize::Precision() const
{
   return 4;
}

double RSYSize::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

RSMode::RSMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheRSModeParameter = this;
}

IsoString RSMode::Id() const
{
   return "mode";
}

size_type RSMode::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString RSMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case RelativeDimensions:   return "RelativeDimensions";
   case AbsolutePixels:       return "AbsolutePixels";
   case AbsoluteCentimeters:  return "AbsoluteCentimeters";
   case AbsoluteInches:       return "AbsoluteInches";
   case ForceArea:            return "ForceArea";
   }
}

int RSMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type RSMode::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

RSAbsoluteMode::RSAbsoluteMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheRSAbsoluteModeParameter = this;
}

IsoString RSAbsoluteMode::Id() const
{
   return "absoluteMode";
}

size_type RSAbsoluteMode::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString RSAbsoluteMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case ForceWidthAndHeight:  return "ForceWidthAndHeight";
   case ForceWidth:           return "ForceWidth";
   case ForceHeight:          return "ForceHeight";
   }
}

int RSAbsoluteMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type RSAbsoluteMode::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ResampleParameters.cpp - Released 2017-08-01T14:26:58Z
