//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0393
// ----------------------------------------------------------------------------
// HistogramTransformationParameters.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "HistogramTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

HistogramTransformationTable* TheHistogramTransformationTableParameter = 0;
ShadowsClipping*              TheShadowsClippingParameter = 0;
HighlightsClipping*           TheHighlightsClippingParameter = 0;
MidtonesBalance*              TheMidtonesBalanceParameter = 0;
LowRange*                     TheLowRangeParameter = 0;
HighRange*                    TheHighRangeParameter = 0;

// ----------------------------------------------------------------------------

HistogramTransformationTable::HistogramTransformationTable( MetaProcess* P ) : MetaTable( P )
{
   TheHistogramTransformationTableParameter = this;
}

IsoString HistogramTransformationTable::Id() const
{
   return "H";
}

size_type HistogramTransformationTable::MinLength() const
{
   return 4; // For compatibility with old (vintage?) versions: allow no alpha channel
}

size_type HistogramTransformationTable::MaxLength() const
{
   return 5;
}

// ----------------------------------------------------------------------------

ShadowsClipping::ShadowsClipping( MetaTable* T ) : MetaDouble( T )
{
   TheShadowsClippingParameter = this;
}

IsoString ShadowsClipping::Id() const
{
   return "c0";
}

int ShadowsClipping::Precision() const
{
   return 8;
}

double ShadowsClipping::DefaultValue() const
{
   return 0;
}

double ShadowsClipping::MinimumValue() const
{
   return 0;
}

double ShadowsClipping::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

HighlightsClipping::HighlightsClipping( MetaTable* T ) : MetaDouble( T )
{
   TheHighlightsClippingParameter = this;
}

IsoString HighlightsClipping::Id() const
{
   return "c1";
}

int HighlightsClipping::Precision() const
{
   return 8;
}

double HighlightsClipping::DefaultValue() const
{
   return 1;
}

double HighlightsClipping::MinimumValue() const
{
   return 0;
}

double HighlightsClipping::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

MidtonesBalance::MidtonesBalance( MetaTable* T ) : MetaDouble( T )
{
   TheMidtonesBalanceParameter = this;
}

IsoString MidtonesBalance::Id() const
{
   return "m";
}

int MidtonesBalance::Precision() const
{
   return 8;
}

double MidtonesBalance::DefaultValue() const
{
   return 0.5;
}

double MidtonesBalance::MinimumValue() const
{
   return 0;
}

double MidtonesBalance::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

LowRange::LowRange( MetaTable* T ) : MetaDouble( T )
{
   TheLowRangeParameter = this;
}

IsoString LowRange::Id() const
{
   return "r0";
}

int LowRange::Precision() const
{
   return 8;
}

double LowRange::DefaultValue() const
{
   return 0;
}

double LowRange::MinimumValue() const
{
   return -10;
}

double LowRange::MaximumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

HighRange::HighRange( MetaTable* T ) : MetaDouble( T )
{
   TheHighRangeParameter = this;
}

IsoString HighRange::Id() const
{
   return "r1";
}

int HighRange::Precision() const
{
   return 8;
}

double HighRange::DefaultValue() const
{
   return 1;
}

double HighRange::MinimumValue() const
{
   return 1;
}

double HighRange::MaximumValue() const
{
   return +11;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF HistogramTransformationParameters.cpp - Released 2017-07-09T18:07:33Z
