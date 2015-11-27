//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0155
// ----------------------------------------------------------------------------
// GradientsHdrParameters.h - Released 2015/11/26 16:00:13 UTC
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

#ifndef __GradientsHdrParameters_h
#define __GradientsHdrParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class GradientsHdrParameterLogMaxGradient : public MetaDouble
{
public:

   GradientsHdrParameterLogMaxGradient( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GradientsHdrParameterLogMaxGradient* TheGradientsHdrParameterLogMaxGradient;

// ----------------------------------------------------------------------------

class GradientsHdrParameterLogMinGradient : public MetaDouble
{
public:

   GradientsHdrParameterLogMinGradient( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GradientsHdrParameterLogMinGradient* TheGradientsHdrParameterLogMinGradient;

// ----------------------------------------------------------------------------

class GradientsHdrParameterExpGradient : public MetaDouble
{
public:

   GradientsHdrParameterExpGradient( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GradientsHdrParameterExpGradient* TheGradientsHdrParameterExpGradient;

// ----------------------------------------------------------------------------


class GradientsHdrParameterRescale01 : public MetaBoolean
{
public:

   GradientsHdrParameterRescale01( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern GradientsHdrParameterRescale01* TheGradientsHdrParameterRescale01;

// ----------------------------------------------------------------------------


class GradientsHdrParameterPreserveColor : public MetaBoolean
{
public:

   GradientsHdrParameterPreserveColor( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern GradientsHdrParameterPreserveColor* TheGradientsHdrParameterPreserveColor;

PCL_END_LOCAL

} // pcl

#endif   // __GradientsHdrParameters_h

// ----------------------------------------------------------------------------
// EOF GradientsHdrParameters.h - Released 2015/11/26 16:00:13 UTC
