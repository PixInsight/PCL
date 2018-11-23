//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0230
// ----------------------------------------------------------------------------
// GradientsHdrCompositionParameters.cpp - Released 2018-11-23T18:45:58Z
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

#include "GradientsHdrCompositionParameters.h"
#include "Assert.h"
//#include <cassert>

namespace pcl
{

// ----------------------------------------------------------------------------

GradientsHdrCompositionTargetFrames*           TheGradientsHdrCompositionTargetFramesParameter = 0;
GradientsHdrCompositionTargetFrameEnabled*     TheGradientsHdrCompositionTargetFrameEnabledParameter = 0;
GradientsHdrCompositionTargetFramePath*        TheGradientsHdrCompositionTargetFramePathParameter = 0;
GradientsHdrCompositionLogBias*                TheGradientsHdrCompositionLogBiasParameter = 0;
GradientsHdrCompositionKeepLog*                TheGradientsHdrCompositionKeepLogParameter = 0;
GradientsHdrCompositionNegativeBias*           TheGradientsHdrCompositionNegativeBiasParameter = 0;
GradientsHdrCompositionGenerateMask*           TheGradientsHdrCompositionGenerateMaskParameter = 0;
// ----------------------------------------------------------------------------

GradientsHdrCompositionTargetFrames::GradientsHdrCompositionTargetFrames( MetaProcess* P ) : MetaTable( P )
{
   TheGradientsHdrCompositionTargetFramesParameter = this;
}

IsoString GradientsHdrCompositionTargetFrames::Id() const
{
   return "targetFrames";
}

// ----------------------------------------------------------------------------

GradientsHdrCompositionTargetFrameEnabled::GradientsHdrCompositionTargetFrameEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheGradientsHdrCompositionTargetFrameEnabledParameter = this;
}

IsoString GradientsHdrCompositionTargetFrameEnabled::Id() const
{
   return "enabled";
}

bool GradientsHdrCompositionTargetFrameEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

GradientsHdrCompositionTargetFramePath::GradientsHdrCompositionTargetFramePath( MetaTable* T ) : MetaString( T )
{
   TheGradientsHdrCompositionTargetFramePathParameter = this;
}

IsoString GradientsHdrCompositionTargetFramePath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------

GradientsHdrCompositionLogBias::GradientsHdrCompositionLogBias( MetaProcess* P ) : MetaDouble( P )
{
   TheGradientsHdrCompositionLogBiasParameter = this;
}

IsoString GradientsHdrCompositionLogBias::Id() const
{
   return "dLogBias";
}

int GradientsHdrCompositionLogBias::Precision() const
{
   return 2;
}

double GradientsHdrCompositionLogBias::DefaultValue() const
{
   return MinimumValue();
}

double GradientsHdrCompositionLogBias::MinimumValue() const
{
   return -DBL_DIG/2;
}

double GradientsHdrCompositionLogBias::MaximumValue() const
{
   return 0.0;
}

// ----------------------------------------------------------------------------

GradientsHdrCompositionKeepLog::GradientsHdrCompositionKeepLog( MetaProcess* P ) : MetaBoolean( P )
{
   TheGradientsHdrCompositionKeepLogParameter = this;
}

IsoString GradientsHdrCompositionKeepLog::Id() const
{
   return "bKeepLog";
}

bool GradientsHdrCompositionKeepLog::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

GradientsHdrCompositionNegativeBias::GradientsHdrCompositionNegativeBias( MetaProcess* P ) : MetaBoolean( P )
{
   TheGradientsHdrCompositionNegativeBiasParameter = this;
}

IsoString GradientsHdrCompositionNegativeBias::Id() const
{
   return "bNegativeBias";
}

bool GradientsHdrCompositionNegativeBias::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

GradientsHdrCompositionGenerateMask::GradientsHdrCompositionGenerateMask( MetaProcess* P ) : MetaBoolean( P )
{
   TheGradientsHdrCompositionGenerateMaskParameter = this;
}

IsoString GradientsHdrCompositionGenerateMask::Id() const
{
   return "generateMask";
}

bool GradientsHdrCompositionGenerateMask::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GradientsHdrCompositionParameters.cpp - Released 2018-11-23T18:45:58Z
