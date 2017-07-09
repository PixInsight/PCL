//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 01.00.00.0319
// ----------------------------------------------------------------------------
// MorphologicalTransformationParameters.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
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

#include "MorphologicalTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

MorphologicalOp*     TheMorphologicalOpParameter = 0;
InterlacingDistance* TheInterlacingDistanceParameter = 0;
LowThreshold*        TheLowThresholdParameter = 0;
HighThreshold*       TheHighThresholdParameter = 0;
NumberOfIterations*  TheNumberOfIterationsParameter = 0;
Amount*              TheAmountParameter = 0;
SelectionPoint*      TheSelectionPointParameter = 0;
StructureName*       TheStructureNameParameter = 0;
StructureSize*       TheStructureSizeParameter = 0;
StructureWayTable*   TheStructureWayTableParameter = 0;
StructureWayMask*    TheStructureWayMaskParameter = 0;

// ----------------------------------------------------------------------------

MorphologicalOp::MorphologicalOp( MetaProcess* P ) : MetaEnumeration( P )
{
   TheMorphologicalOpParameter = this;
}

IsoString MorphologicalOp::Id() const
{
   return "operator";
}

size_type MorphologicalOp::NumberOfElements() const
{
   return NumberOfMorphologicalOps;
}

IsoString MorphologicalOp::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Erosion:     return "Erosion";
   case Dilation:    return "Dilation";
   case Opening:     return "Opening";
   case Closing:     return "Closing";
   case Median:      return "Median";
   case Selection:   return "Selection";
   case Midpoint:    return "Midpoint";
   }
}

int MorphologicalOp::ElementValue( size_type i ) const
{
   return int( i );
}

size_type MorphologicalOp::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

InterlacingDistance::InterlacingDistance( MetaProcess* P ) : MetaUInt32( P )
{
   TheInterlacingDistanceParameter = this;
}

IsoString InterlacingDistance::Id() const
{
   return "interlacingDistance";
}

double InterlacingDistance::DefaultValue() const
{
   return 1;
}

double InterlacingDistance::MinimumValue() const
{
   return 1;
}

double InterlacingDistance::MaximumValue() const
{
   return 4096;
}

// ----------------------------------------------------------------------------

LowThreshold::LowThreshold( MetaProcess* P ) : MetaDouble( P )
{
   TheLowThresholdParameter = this;
}

IsoString LowThreshold::Id() const
{
   return "lowThreshold";
}

int LowThreshold::Precision() const
{
   return 6;
}

double LowThreshold::DefaultValue() const
{
   return 0;
}

double LowThreshold::MinimumValue() const
{
   return 0;
}

double LowThreshold::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

HighThreshold::HighThreshold( MetaProcess* P ) : MetaDouble( P )
{
   TheHighThresholdParameter = this;
}

IsoString HighThreshold::Id() const
{
   return "highThreshold";
}

int HighThreshold::Precision() const
{
   return 6;
}

double HighThreshold::DefaultValue() const
{
   return 0;
}

double HighThreshold::MinimumValue() const
{
   return 0;
}

double HighThreshold::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

NumberOfIterations::NumberOfIterations( MetaProcess* P ) : MetaUInt32( P )
{
   TheNumberOfIterationsParameter = this;
}

IsoString NumberOfIterations::Id() const
{
   return "numberOfIterations";
}

double NumberOfIterations::DefaultValue() const
{
   return 1;
}

double NumberOfIterations::MinimumValue() const
{
   return 1;
}

double NumberOfIterations::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

Amount::Amount( MetaProcess* P ) : MetaFloat( P )
{
   TheAmountParameter = this;
}

IsoString Amount::Id() const
{
   return "amount";
}

int Amount::Precision() const
{
   return 2;
}

double Amount::DefaultValue() const
{
   return 1;
}

double Amount::MinimumValue() const
{
   return 0;
}

double Amount::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

SelectionPoint::SelectionPoint( MetaProcess* P ) : MetaFloat( P )
{
   TheSelectionPointParameter = this;
}

IsoString SelectionPoint::Id() const
{
   return "selectionPoint";
}

int SelectionPoint::Precision() const
{
   return 2;
}

double SelectionPoint::DefaultValue() const
{
   return 0.5;
}

double SelectionPoint::MinimumValue() const
{
   return 0;
}

double SelectionPoint::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

StructureName::StructureName( MetaProcess* P ) : MetaString( P )
{
   TheStructureNameParameter = this;
}

IsoString StructureName::Id() const
{
   return "structureName";
}

// ----------------------------------------------------------------------------

StructureSize::StructureSize( MetaProcess* P ) : MetaUInt32( P )
{
   TheStructureSizeParameter = this;
}

IsoString StructureSize::Id() const
{
   return "structureSize";
}

double StructureSize::DefaultValue() const
{
   return 3;
}

double StructureSize::MinimumValue() const
{
   return 3;
}

double StructureSize::MaximumValue() const
{
   return 25;
}

// ----------------------------------------------------------------------------

StructureWayTable::StructureWayTable( MetaProcess* P ) : MetaTable( P )
{
   TheStructureWayTableParameter = this;
}

IsoString StructureWayTable::Id() const
{
   return "structureWayTable";
}

size_type StructureWayTable::MinLength() const
{
   return 1;
}

// ----------------------------------------------------------------------------

StructureWayMask::StructureWayMask( MetaTable* T ) : MetaBlock( T )
{
   TheStructureWayMaskParameter = this;
}

IsoString StructureWayMask::Id() const
{
   return "mask";
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MorphologicalTransformationParameters.cpp - Released 2017-07-09T18:07:33Z
