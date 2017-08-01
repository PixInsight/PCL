//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0348
// ----------------------------------------------------------------------------
// LRGBCombinationParameters.cpp - Released 2017-08-01T14:26:57Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#include "LRGBCombinationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

LRGBChannelTable*    TheLRGBChannelTableParameter = 0;
LRGBChannelEnabled*  TheLRGBChannelEnabledParameter = 0;
LRGBChannelId*       TheLRGBChannelIdParameter = 0;
LRGBChannelWeight*   TheLRGBChannelWeightParameter = 0;
LRGBLuminanceMTF*    TheLRGBLuminanceMTFParameter = 0;
LRGBSaturationMTF*   TheLRGBSaturationMTFParameter = 0;
LRGBClipHighlights*  TheLRGBClipHighlightsParameter = 0;
LRGBNoiseReduction*  TheLRGBNoiseReductionParameter = 0;
LRGBLayersRemoved*   TheLRGBLayersRemovedParameter = 0;
LRGBLayersProtected* TheLRGBLayersProtectedParameter = 0;

// ----------------------------------------------------------------------------

LRGBChannelTable::LRGBChannelTable( MetaProcess* P ) : MetaTable( P )
{
   TheLRGBChannelTableParameter = this;
}

IsoString LRGBChannelTable::Id() const
{
   return "channels";
}

size_type LRGBChannelTable::MinLength() const
{
   return 4;
}

size_type LRGBChannelTable::MaxLength() const
{
   return 4;
}

// ----------------------------------------------------------------------------

LRGBChannelEnabled::LRGBChannelEnabled( MetaTable* T ) : ChannelEnabled( T )
{
   TheLRGBChannelEnabledParameter = this;
}

// ----------------------------------------------------------------------------

LRGBChannelId::LRGBChannelId( MetaTable* T ) : ChannelId( T )
{
   TheLRGBChannelIdParameter = this;
}

// ----------------------------------------------------------------------------

LRGBChannelWeight::LRGBChannelWeight( MetaTable* T ) : MetaDouble( T )
{
   TheLRGBChannelWeightParameter = this;
}

IsoString LRGBChannelWeight::Id() const
{
   return "k";
}

int LRGBChannelWeight::Precision() const
{
   return 5;
}

double LRGBChannelWeight::MinimumValue() const
{
   return 0;
}

double LRGBChannelWeight::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

LRGBLuminanceMTF::LRGBLuminanceMTF( MetaProcess* P ) : MetaFloat( P )
{
   TheLRGBLuminanceMTFParameter = this;
}

IsoString LRGBLuminanceMTF::Id() const
{
   return "mL";
}

int LRGBLuminanceMTF::Precision() const
{
   return 3;
}

double LRGBLuminanceMTF::DefaultValue() const
{
   return 0.5;
}

double LRGBLuminanceMTF::MinimumValue() const
{
   return 0.001;
}

double LRGBLuminanceMTF::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

LRGBSaturationMTF::LRGBSaturationMTF( MetaProcess* P ) : MetaFloat( P )
{
   TheLRGBSaturationMTFParameter = this;
}

IsoString LRGBSaturationMTF::Id() const
{
   return "mc";
}

int LRGBSaturationMTF::Precision() const
{
   return 3;
}

double LRGBSaturationMTF::DefaultValue() const
{
   return 0.5;
}

double LRGBSaturationMTF::MinimumValue() const
{
   return 0.001;
}

double LRGBSaturationMTF::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

LRGBClipHighlights::LRGBClipHighlights( MetaProcess* P ) : MetaBoolean( P )
{
   TheLRGBClipHighlightsParameter = this;
}

IsoString LRGBClipHighlights::Id() const
{
   return "clipHighlights";
}

bool LRGBClipHighlights::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LRGBNoiseReduction::LRGBNoiseReduction( MetaProcess* P ) : MetaBoolean( P )
{
   TheLRGBNoiseReductionParameter = this;
}

IsoString LRGBNoiseReduction::Id() const
{
   return "noiseReduction";
}

bool LRGBNoiseReduction::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LRGBLayersRemoved::LRGBLayersRemoved( MetaProcess* P ) : MetaUInt32( P )
{
   TheLRGBLayersRemovedParameter = this;
}

IsoString LRGBLayersRemoved::Id() const
{
   return "layersRemoved";
}

double LRGBLayersRemoved::DefaultValue() const
{
   return 4;
}

double LRGBLayersRemoved::MinimumValue() const
{
   return 1;
}

double LRGBLayersRemoved::MaximumValue() const
{
   return 6;
}

// ----------------------------------------------------------------------------

LRGBLayersProtected::LRGBLayersProtected( MetaProcess* P ) : MetaUInt32( P )
{
   TheLRGBLayersProtectedParameter = this;
}

IsoString LRGBLayersProtected::Id() const
{
   return "layersProtected";
}

double LRGBLayersProtected::DefaultValue() const
{
   return 2;
}

double LRGBLayersProtected::MinimumValue() const
{
   return 0;
}

double LRGBLayersProtected::MaximumValue() const
{
   return 5;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LRGBCombinationParameters.cpp - Released 2017-08-01T14:26:57Z
