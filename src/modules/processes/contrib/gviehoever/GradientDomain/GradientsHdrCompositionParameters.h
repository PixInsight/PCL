//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0155
// ----------------------------------------------------------------------------
// GradientsHdrCompositionParameters.h - Released 2015/11/26 16:00:13 UTC
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

#ifndef __GradientsHdrCompositionParameters_h
#define __GradientsHdrCompositionParameters_h


#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class GradientsHdrCompositionTargetFrames : public MetaTable
{
public:

   GradientsHdrCompositionTargetFrames( MetaProcess* );

   virtual IsoString Id() const;
};

extern GradientsHdrCompositionTargetFrames* TheGradientsHdrCompositionTargetFramesParameter;

// ----------------------------------------------------------------------------

class GradientsHdrCompositionTargetFrameEnabled : public MetaBoolean
{
public:

   GradientsHdrCompositionTargetFrameEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern GradientsHdrCompositionTargetFrameEnabled* TheGradientsHdrCompositionTargetFrameEnabledParameter;

// ----------------------------------------------------------------------------

class GradientsHdrCompositionTargetFramePath : public MetaString
{
public:

   GradientsHdrCompositionTargetFramePath( MetaTable* );

   virtual IsoString Id() const;
};

extern GradientsHdrCompositionTargetFramePath* TheGradientsHdrCompositionTargetFramePathParameter;

// ----------------------------------------------------------------------------

class GradientsHdrCompositionLogBias : public MetaDouble
{
public:

   GradientsHdrCompositionLogBias( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GradientsHdrCompositionLogBias* TheGradientsHdrCompositionLogBiasParameter;

// ----------------------------------------------------------------------------

class GradientsHdrCompositionKeepLog : public MetaBoolean
{
public:

   GradientsHdrCompositionKeepLog( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern GradientsHdrCompositionKeepLog* TheGradientsHdrCompositionKeepLogParameter;

// ----------------------------------------------------------------------------

class GradientsHdrCompositionNegativeBias : public MetaBoolean
{
public:

   GradientsHdrCompositionNegativeBias( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern GradientsHdrCompositionNegativeBias* TheGradientsHdrCompositionNegativeBiasParameter;

// ----------------------------------------------------------------------------

class GradientsHdrCompositionGenerateMask : public MetaBoolean
{
public:

   GradientsHdrCompositionGenerateMask( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern GradientsHdrCompositionGenerateMask* TheGradientsHdrCompositionGenerateMaskParameter;

// ----------------------------------------------------------------------------


PCL_END_LOCAL

} // pcl

#endif

// ----------------------------------------------------------------------------
// EOF GradientsHdrCompositionParameters.h - Released 2015/11/26 16:00:13 UTC
