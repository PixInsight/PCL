//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 01.00.00.0339
// ----------------------------------------------------------------------------
// MorphologicalTransformationParameters.h - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __MorphologicalTransformationParameters_h
#define __MorphologicalTransformationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class MorphologicalOp : public MetaEnumeration
{
public:

   enum { Erosion, Dilation, Opening, Closing, Median, Selection, Midpoint,
          NumberOfMorphologicalOps, Default = Erosion };

   MorphologicalOp( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern MorphologicalOp* TheMorphologicalOpParameter;

// ----------------------------------------------------------------------------

class InterlacingDistance : public MetaUInt32
{
public:

   InterlacingDistance( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern InterlacingDistance* TheInterlacingDistanceParameter;

// ----------------------------------------------------------------------------

class LowThreshold : public MetaDouble
{
public:

   LowThreshold( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern LowThreshold* TheLowThresholdParameter;

// ----------------------------------------------------------------------------

class HighThreshold : public MetaDouble
{
public:

   HighThreshold( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern HighThreshold* TheHighThresholdParameter;

// ----------------------------------------------------------------------------

class NumberOfIterations : public MetaUInt32
{
public:

   NumberOfIterations( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern NumberOfIterations* TheNumberOfIterationsParameter;

// ----------------------------------------------------------------------------

class Amount : public MetaFloat
{
public:

   Amount( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern Amount* TheAmountParameter;

// ----------------------------------------------------------------------------

class SelectionPoint : public MetaFloat
{
public:

   SelectionPoint( MetaProcess* );

   virtual IsoString Id() const;

   virtual int Precision() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern SelectionPoint* TheSelectionPointParameter;

// ----------------------------------------------------------------------------

class StructureName : public MetaString
{
public:

   StructureName( MetaProcess* );

   virtual IsoString Id() const;
};

extern StructureName* TheStructureNameParameter;

// ----------------------------------------------------------------------------

class StructureSize : public MetaUInt32
{
public:

   StructureSize( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern StructureSize* TheStructureSizeParameter;

// ----------------------------------------------------------------------------

class StructureWayTable : public MetaTable
{
public:

   StructureWayTable( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type MinLength() const;
};

extern StructureWayTable* TheStructureWayTableParameter;

// ----------------------------------------------------------------------------

class StructureWayMask : public MetaBlock
{
public:

   StructureWayMask( MetaTable* );

   virtual IsoString Id() const;
};

extern StructureWayMask* TheStructureWayMaskParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __MorphologicalTransformationParameters_h

// ----------------------------------------------------------------------------
// EOF MorphologicalTransformationParameters.h - Released 2018-11-01T11:07:21Z
