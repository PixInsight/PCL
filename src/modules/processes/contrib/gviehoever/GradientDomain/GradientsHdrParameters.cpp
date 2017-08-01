//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0215
// ----------------------------------------------------------------------------
// GradientsHdrParameters.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2015. Licensed under LGPL 2.1
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#include "GradientsHdrParameters.h"
#include <cfloat>

namespace pcl
{

// ----------------------------------------------------------------------------

GradientsHdrParameterExpGradient*    TheGradientsHdrParameterExpGradient = 0;
GradientsHdrParameterLogMaxGradient*    TheGradientsHdrParameterLogMaxGradient = 0;
GradientsHdrParameterLogMinGradient*    TheGradientsHdrParameterLogMinGradient = 0;
GradientsHdrParameterRescale01*  TheGradientsHdrParameterRescale01 = 0;
GradientsHdrParameterPreserveColor*  TheGradientsHdrParameterPreserveColor = 0;

// ----------------------------------------------------------------------------

GradientsHdrParameterExpGradient::GradientsHdrParameterExpGradient( MetaProcess* P ) : MetaDouble( P )
{
   TheGradientsHdrParameterExpGradient = this;
}

IsoString GradientsHdrParameterExpGradient::Id() const
{
   return "expGradient";
}

int GradientsHdrParameterExpGradient::Precision() const
{
   return 3;
}

double GradientsHdrParameterExpGradient::DefaultValue() const
{
   return 1.0;
}

double GradientsHdrParameterExpGradient::MinimumValue() const
{
   return 0.001;
}

double GradientsHdrParameterExpGradient::MaximumValue() const
{
   return 1.1;
}

// ----------------------------------------------------------------------------

GradientsHdrParameterLogMaxGradient::GradientsHdrParameterLogMaxGradient( MetaProcess* P ) : MetaDouble( P )
{
   TheGradientsHdrParameterLogMaxGradient = this;
}

IsoString GradientsHdrParameterLogMaxGradient::Id() const
{
   return "logMaxGradient";
}

int GradientsHdrParameterLogMaxGradient::Precision() const
{
   return 2;
}

double GradientsHdrParameterLogMaxGradient::DefaultValue() const
{
   return MaximumValue();
}

double GradientsHdrParameterLogMaxGradient::MinimumValue() const
{
   return -DBL_DIG/2;
}

double GradientsHdrParameterLogMaxGradient::MaximumValue() const
{
   return 0.0;
}

// ----------------------------------------------------------------------------

GradientsHdrParameterLogMinGradient::GradientsHdrParameterLogMinGradient( MetaProcess* P ) : MetaDouble( P )
{
   TheGradientsHdrParameterLogMinGradient = this;
}

IsoString GradientsHdrParameterLogMinGradient::Id() const
{
   return "logMinGradient";
}

int GradientsHdrParameterLogMinGradient::Precision() const
{
   return 2;
}

double GradientsHdrParameterLogMinGradient::DefaultValue() const
{
  return MinimumValue();
}

double GradientsHdrParameterLogMinGradient::MinimumValue() const
{
  return -(DBL_DIG-1)/2;
}

double GradientsHdrParameterLogMinGradient::MaximumValue() const
{
   return 0.0;
}


// ----------------------------------------------------------------------------


GradientsHdrParameterRescale01::GradientsHdrParameterRescale01( MetaProcess* P ) : MetaBoolean( P )
{
   TheGradientsHdrParameterRescale01 = this;
}

IsoString GradientsHdrParameterRescale01::Id() const
{
   return "bRescale01";
}

bool GradientsHdrParameterRescale01::DefaultValue() const
{
   return true;
}


GradientsHdrParameterPreserveColor::GradientsHdrParameterPreserveColor( MetaProcess* P ) : MetaBoolean( P )
{
   TheGradientsHdrParameterPreserveColor = this;
}

IsoString GradientsHdrParameterPreserveColor::Id() const
{
   return "bPreserveColor";
}

bool GradientsHdrParameterPreserveColor::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------


} // pcl

// ----------------------------------------------------------------------------
// EOF GradientsHdrParameters.cpp - Released 2017-08-01T14:26:58Z
