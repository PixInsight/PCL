//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.01.0336
// ----------------------------------------------------------------------------
// IntegerResampleParameters.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#include "IntegerResampleParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IRZoomFactor*       TheIRZoomFactorParameter = nullptr;
IRDownsamplingMode* TheIRDownsamplingModeParameter = nullptr;
XResolution*        TheIRXResolutionParameter = nullptr;
YResolution*        TheIRYResolutionParameter = nullptr;
MetricResolution*   TheIRMetricResolutionParameter = nullptr;
ForceResolution*    TheIRForceResolutionParameter = nullptr;
NoGUIMessages*      TheIRNoGUIMessagesParameter = nullptr;

// ----------------------------------------------------------------------------

IRZoomFactor::IRZoomFactor( MetaProcess* P ) : MetaInt32( P )
{
   TheIRZoomFactorParameter = this;
}

IsoString IRZoomFactor::Id() const
{
   return "zoomFactor";
}

double IRZoomFactor::DefaultValue() const
{
   return -2;
}

double IRZoomFactor::MinimumValue() const
{
   return -256;
}

double IRZoomFactor::MaximumValue() const
{
   return +256;
}

// ----------------------------------------------------------------------------

IRDownsamplingMode::IRDownsamplingMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIRDownsamplingModeParameter = this;
}

IsoString IRDownsamplingMode::Id() const
{
   return "downsamplingMode";
}

IsoString IRDownsamplingMode::Aliases() const
{
   return "subsampleMode"; // for compatibility with old versions
}

size_type IRDownsamplingMode::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString IRDownsamplingMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Average:  return "Average";
   case Median:   return "Median";
   case Maximum:  return "Maximum";
   case Minimum:  return "Minimum";
   }
}

int IRDownsamplingMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IRDownsamplingMode::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegerResampleParameters.cpp - Released 2017-04-14T23:07:12Z
