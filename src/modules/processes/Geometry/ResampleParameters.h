//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.01.0327
// ----------------------------------------------------------------------------
// ResampleParameters.h - Released 2016/12/20 17:43:21 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

class RSXSize : public MetaDouble
{
public:

   RSXSize( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern RSXSize* TheRSXSizeParameter;

// ----------------------------------------------------------------------------

class RSYSize : public MetaDouble
{
public:

   RSYSize( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
};

extern RSYSize* TheRSYSizeParameter;

// ----------------------------------------------------------------------------

class RSMode : public MetaEnumeration
{
public:

   enum { RelativeDimensions,    // relative to current image dimensions
          AbsolutePixels,        // to absolute dimensions in pixels
          AbsoluteCentimeters,   // to absolute dimensions in centimeters
          AbsoluteInches,        // to absolute dimensions in inches
          ForceArea,             // force number of pixels, keep aspect ratio
          NumberOfModes,
          Default = RelativeDimensions };

   RSMode( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern RSMode* TheRSModeParameter;

// ----------------------------------------------------------------------------

class RSAbsoluteMode : public MetaEnumeration
{
public:

   enum { ForceWidthAndHeight,   // force both dimensions
          ForceWidth,            // force width, preserve aspect ratio
          ForceHeight,           // force height, preserve aspect ratio
          NumberOfModes,
          Default = ForceWidthAndHeight };

   RSAbsoluteMode( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern RSAbsoluteMode* TheRSAbsoluteModeParameter;

// ----------------------------------------------------------------------------

extern XResolution* TheRSXResolutionParameter;
extern YResolution* TheRSYResolutionParameter;
extern MetricResolution* TheRSMetricResolutionParameter;
extern ForceResolution* TheRSForceResolutionParameter;

extern InterpolationAlgorithm* TheRSInterpolationAlgorithmParameter;
extern ClampingThreshold* TheRSClampingThresholdParameter;
extern Smoothness* TheRSSmoothnessParameter;

extern NoGUIMessages* TheRSNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ResampleParameters_h

// ----------------------------------------------------------------------------
// EOF ResampleParameters.h - Released 2016/12/20 17:43:21 UTC
