//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0223
// ----------------------------------------------------------------------------
// GradientsMergeMosaicParameters.cpp - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2018. Licensed under LGPL 2.1
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#include "GradientsMergeMosaicParameters.h"
#include "Assert.h"
//#include <cassert>

namespace pcl
{

// ----------------------------------------------------------------------------

GradientsMergeMosaicTargetFrames*           TheGradientsMergeMosaicTargetFramesParameter = 0;
GradientsMergeMosaicTargetFrameEnabled*     TheGradientsMergeMosaicTargetFrameEnabledParameter = 0;
GradientsMergeMosaicTargetFramePath*        TheGradientsMergeMosaicTargetFramePathParameter = 0;
GradientsMergeMosaicType*                   TheGradientsMergeMosaicTypeParameter = 0;
GradientsMergeMosaicShrinkCount*            TheGradientsMergeMosaicShrinkCountParameter = 0;
GradientsMergeMosaicFeatherRadius*          TheGradientsMergeMosaicFeatherRadiusParameter = 0;
GradientsMergeMosaicBlackPoint*             TheGradientsMergeMosaicBlackPointParameter = 0;
GradientsMergeMosaicGenerateMask*           TheGradientsMergeMosaicGenerateMaskParameter = 0;
// ----------------------------------------------------------------------------

GradientsMergeMosaicTargetFrames::GradientsMergeMosaicTargetFrames( MetaProcess* P ) : MetaTable( P )
{
   TheGradientsMergeMosaicTargetFramesParameter = this;
}

IsoString GradientsMergeMosaicTargetFrames::Id() const
{
   return "targetFrames";
}

// ----------------------------------------------------------------------------

GradientsMergeMosaicTargetFrameEnabled::GradientsMergeMosaicTargetFrameEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheGradientsMergeMosaicTargetFrameEnabledParameter = this;
}

IsoString GradientsMergeMosaicTargetFrameEnabled::Id() const
{
   return "enabled";
}

bool GradientsMergeMosaicTargetFrameEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

GradientsMergeMosaicTargetFramePath::GradientsMergeMosaicTargetFramePath( MetaTable* T ) : MetaString( T )
{
   TheGradientsMergeMosaicTargetFramePathParameter = this;
}

IsoString GradientsMergeMosaicTargetFramePath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------


GradientsMergeMosaicType::GradientsMergeMosaicType( MetaProcess* P ) : MetaEnumeration( P )
{
   TheGradientsMergeMosaicTypeParameter = this;
}

IsoString GradientsMergeMosaicType::Id() const
{
   return "type";
}

size_type GradientsMergeMosaicType::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString GradientsMergeMosaicType::ElementId( size_type i ) const
{
   switch ( i )
   {
   default: Assert(false);
   case Overlay:  return "Overlay";
   case Average: return "Average";
   }
}

int GradientsMergeMosaicType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type GradientsMergeMosaicType::DefaultValueIndex() const
{
   return Default;
}


// ----------------------------------------------------------------------------

GradientsMergeMosaicShrinkCount::GradientsMergeMosaicShrinkCount( MetaProcess* P ) : MetaInt32( P )
{
   TheGradientsMergeMosaicShrinkCountParameter = this;
}

IsoString GradientsMergeMosaicShrinkCount::Id() const
{
   return "nShrinkCount";
}

double GradientsMergeMosaicShrinkCount::DefaultValue() const
{
   return 1;
}

double GradientsMergeMosaicShrinkCount::MinimumValue() const
{
   return 0;
}

double GradientsMergeMosaicShrinkCount::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

GradientsMergeMosaicFeatherRadius::GradientsMergeMosaicFeatherRadius( MetaProcess* P ) : MetaInt32( P )
{
   TheGradientsMergeMosaicFeatherRadiusParameter = this;
}

IsoString GradientsMergeMosaicFeatherRadius::Id() const
{
   return "nFeatherRadius";
}

double GradientsMergeMosaicFeatherRadius::DefaultValue() const
{
   return 10;
}

double GradientsMergeMosaicFeatherRadius::MinimumValue() const
{
   return 0;
}

double GradientsMergeMosaicFeatherRadius::MaximumValue() const
{
   return 250;
}


// ----------------------------------------------------------------------------

GradientsMergeMosaicBlackPoint::GradientsMergeMosaicBlackPoint( MetaProcess* P ) : MetaDouble( P )
{
   TheGradientsMergeMosaicBlackPointParameter = this;
}

IsoString GradientsMergeMosaicBlackPoint::Id() const
{
   return "blackPoint";
}

int GradientsMergeMosaicBlackPoint::Precision() const
{
   return 3;
}

double GradientsMergeMosaicBlackPoint::DefaultValue() const
{
   return 0.0;
}

double GradientsMergeMosaicBlackPoint::MinimumValue() const
{
   return 0.0;
}

double GradientsMergeMosaicBlackPoint::MaximumValue() const
{
   return 0.1;
}

// ----------------------------------------------------------------------------

GradientsMergeMosaicGenerateMask::GradientsMergeMosaicGenerateMask( MetaProcess* P ) : MetaBoolean( P )
{
   TheGradientsMergeMosaicGenerateMaskParameter = this;
}

IsoString GradientsMergeMosaicGenerateMask::Id() const
{
   return "generateMask";
}

bool GradientsMergeMosaicGenerateMask::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GradientsMergeMosaicParameters.cpp - Released 2018-11-01T11:07:21Z
