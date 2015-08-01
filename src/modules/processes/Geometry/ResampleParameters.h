//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.01.00.0266
// ----------------------------------------------------------------------------
// ResampleParameters.h - Released 2015/07/31 11:49:48 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __ResampleParameters_h
#define __ResampleParameters_h

#include "CommonParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class XSize : public MetaDouble
{
public:

   XSize( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;

   virtual double DefaultValue() const;
};

extern XSize* TheXSizeParameter;

// ----------------------------------------------------------------------------

class YSize : public MetaDouble
{
public:

   YSize( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;

   virtual double DefaultValue() const;
};

extern YSize* TheYSizeParameter;

// ----------------------------------------------------------------------------

class ResampleMode : public MetaEnumeration
{
public:

   enum { RelativeDimensions,    // relative to current image dimensions
          AbsolutePixels,        // to absolute dimensions in pixels
          AbsoluteCentimeters,   // to absolute dimensions in centimeters
          AbsoluteInches,        // to absolute dimensions in inches
          ForceArea,             // force number of pixels, keep aspect ratio
          NumberOfResampleModes,
          Default = RelativeDimensions };

   ResampleMode( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern ResampleMode* TheResampleModeParameter;

// ----------------------------------------------------------------------------

class AbsoluteResampleMode : public MetaEnumeration
{
public:

   enum { ForceWidthAndHeight,   // force both dimensions
          ForceWidth,            // force width, preserve aspect ratio
          ForceHeight,           // force height, preserve aspect ratio
          NumberOfAbsoluteResampleModes,
          Default = ForceWidthAndHeight };

   AbsoluteResampleMode( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern AbsoluteResampleMode* TheAbsoluteResampleModeParameter;

// ----------------------------------------------------------------------------

extern XResolution* TheXResolutionResampleParameter;
extern YResolution* TheYResolutionResampleParameter;
extern MetricResolution* TheMetricResolutionResampleParameter;
extern ForceResolution* TheForceResolutionResampleParameter;

extern InterpolationAlgorithm* TheInterpolationAlgorithmResampleParameter;
extern ClampingThreshold* TheClampingThresholdResampleParameter;
extern Smoothness* TheSmoothnessResampleParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ResampleParameters_h

// ----------------------------------------------------------------------------
// EOF ResampleParameters.h - Released 2015/07/31 11:49:48 UTC
