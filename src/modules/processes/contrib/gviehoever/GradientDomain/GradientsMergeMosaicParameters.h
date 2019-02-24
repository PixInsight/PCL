//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0240
// ----------------------------------------------------------------------------
// GradientsMergeMosaicParameters.h - Released 2019-01-21T12:06:42Z
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

#ifndef __GradientsMergeMosaicParameters_h
#define __GradientsMergeMosaicParameters_h


#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class GradientsMergeMosaicTargetFrames : public MetaTable
{
public:

   GradientsMergeMosaicTargetFrames( MetaProcess* );

   virtual IsoString Id() const;
};

extern GradientsMergeMosaicTargetFrames* TheGradientsMergeMosaicTargetFramesParameter;

// ----------------------------------------------------------------------------

class GradientsMergeMosaicTargetFrameEnabled : public MetaBoolean
{
public:

   GradientsMergeMosaicTargetFrameEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern GradientsMergeMosaicTargetFrameEnabled* TheGradientsMergeMosaicTargetFrameEnabledParameter;

// ----------------------------------------------------------------------------

class GradientsMergeMosaicTargetFramePath : public MetaString
{
public:

   GradientsMergeMosaicTargetFramePath( MetaTable* );

   virtual IsoString Id() const;
};

extern GradientsMergeMosaicTargetFramePath* TheGradientsMergeMosaicTargetFramePathParameter;

// ----------------------------------------------------------------------------


class GradientsMergeMosaicType : public MetaEnumeration
{
public:

   typedef enum { Overlay,
          Average,
          NumberOfItems,
          Default = Average } eType_t;

   GradientsMergeMosaicType( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern GradientsMergeMosaicType* TheGradientsMergeMosaicTypeParameter;

// ----------------------------------------------------------------------------

/// this is the radius of an erosion mask that reduces the size of the actual image region
class GradientsMergeMosaicShrinkCount : public MetaInt32
{
public:

   GradientsMergeMosaicShrinkCount( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GradientsMergeMosaicShrinkCount* TheGradientsMergeMosaicShrinkCountParameter;


// ----------------------------------------------------------------------------

/// this is the radius of convolution that feathers the borders of an image
class GradientsMergeMosaicFeatherRadius : public MetaInt32
{
public:

   GradientsMergeMosaicFeatherRadius( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GradientsMergeMosaicFeatherRadius* TheGradientsMergeMosaicFeatherRadiusParameter;

// ----------------------------------------------------------------------------

class GradientsMergeMosaicBlackPoint : public MetaDouble
{
public:

   GradientsMergeMosaicBlackPoint( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern GradientsMergeMosaicBlackPoint* TheGradientsMergeMosaicBlackPointParameter;

// ----------------------------------------------------------------------------

class GradientsMergeMosaicGenerateMask : public MetaBoolean
{
public:

   GradientsMergeMosaicGenerateMask( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern GradientsMergeMosaicGenerateMask* TheGradientsMergeMosaicGenerateMaskParameter;

// ----------------------------------------------------------------------------


PCL_END_LOCAL

} // pcl

#endif

// ----------------------------------------------------------------------------
// EOF GradientsMergeMosaicParameters.h - Released 2019-01-21T12:06:42Z
