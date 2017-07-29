//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 01.00.02.0328
// ----------------------------------------------------------------------------
// CloneStampParameters.cpp - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
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

#include "CloneStampParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CSClonerTable*      TheCSClonerTableParameter = 0;
CSActionIndex*      TheCSActionIndexParameter = 0;
CSTargetX*          TheCSTargetXParameter = 0;
CSTargetY*          TheCSTargetYParameter = 0;
CSActionTable*      TheCSActionTableParameter = 0;
CSSourceIdentifier* TheCSSourceIdentifierParameter = 0;
CSSourceWidth*      TheCSSourceWidthParameter = 0;
CSSourceHeight*     TheCSSourceHeightParameter = 0;
CSSourceX*          TheCSSourceXParameter = 0;
CSSourceY*          TheCSSourceYParameter = 0;
CSRadius*           TheCSRadiusParameter = 0;
CSSoftness*         TheCSSoftnessParameter = 0;
CSOpacity*          TheCSOpacityParameter = 0;
CSWidth*            TheCSWidthParameter = 0;
CSHeight*           TheCSHeightParameter = 0;
CSColor*            TheCSColorParameter = 0;
CSBoundsColor*      TheCSBoundsColorParameter = 0;

// ----------------------------------------------------------------------------

CSClonerTable::CSClonerTable( MetaProcess* P ) : MetaTable( P )
{
   TheCSClonerTableParameter = this;
}

IsoString CSClonerTable::Id() const
{
   return "cloner";
}

// ----------------------------------------------------------------------------

CSActionIndex::CSActionIndex( MetaTable* T ) : MetaUInt32( T )
{
   TheCSActionIndexParameter = this;
}

IsoString CSActionIndex::Id() const
{
   return "actionIndex";
}

// ----------------------------------------------------------------------------

CSTargetX::CSTargetX( MetaTable* T ) : MetaInt32( T )
{
   TheCSTargetXParameter = this;
}

IsoString CSTargetX::Id() const
{
   return "targetX";
}

// ----------------------------------------------------------------------------

CSTargetY::CSTargetY( MetaTable* T ) : MetaInt32( T )
{
   TheCSTargetYParameter = this;
}

IsoString CSTargetY::Id() const
{
   return "targetY";
}

// ----------------------------------------------------------------------------

CSActionTable::CSActionTable( MetaProcess* P ) : MetaTable( P )
{
   TheCSActionTableParameter = this;
}

IsoString CSActionTable::Id() const
{
   return "actions";
}

// ----------------------------------------------------------------------------

CSSourceIdentifier::CSSourceIdentifier( MetaTable* T ) : MetaString( T )
{
   TheCSSourceIdentifierParameter = this;
}

IsoString CSSourceIdentifier::Id() const
{
   return "sourceId";
}

size_type CSSourceIdentifier::MinLength() const
{
   return 0; // can be void
}

String CSSourceIdentifier::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

CSSourceWidth::CSSourceWidth( MetaTable* T ) : MetaInt32( T )
{
   TheCSSourceWidthParameter = this;
}

IsoString CSSourceWidth::Id() const
{
   return "sourceWidth";
}

double CSSourceWidth::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

CSSourceHeight::CSSourceHeight( MetaTable* T ) : MetaInt32( T )
{
   TheCSSourceHeightParameter = this;
}

IsoString CSSourceHeight::Id() const
{
   return "sourceHeight";
}

double CSSourceHeight::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

CSSourceX::CSSourceX( MetaTable* T ) : MetaInt32( T )
{
   TheCSSourceXParameter = this;
}

IsoString CSSourceX::Id() const
{
   return "sourceX";
}

// ----------------------------------------------------------------------------

CSSourceY::CSSourceY( MetaTable* T ) : MetaInt32( T )
{
   TheCSSourceYParameter = this;
}

IsoString CSSourceY::Id() const
{
   return "sourceY";
}

// ----------------------------------------------------------------------------

CSRadius::CSRadius( MetaTable* T ) : MetaInt32( T )
{
   TheCSRadiusParameter = this;
}

IsoString CSRadius::Id() const
{
   return "radius";
}

double CSRadius::DefaultValue() const
{
   return 5;
}

double CSRadius::MinimumValue() const
{
   return 0;
}

double CSRadius::MaximumValue() const
{
   return 255;
}

// ----------------------------------------------------------------------------

CSSoftness::CSSoftness( MetaTable* T ) : MetaFloat( T )
{
   TheCSSoftnessParameter = this;
}

IsoString CSSoftness::Id() const
{
   return "softness";
}

double CSSoftness::DefaultValue() const
{
   return 0.5;
}

double CSSoftness::MinimumValue() const
{
   return 0;
}

double CSSoftness::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CSOpacity::CSOpacity( MetaTable* T ) : MetaFloat( T )
{
   TheCSOpacityParameter = this;
}

IsoString CSOpacity::Id() const
{
   return "opacity";
}

double CSOpacity::DefaultValue() const
{
   return 1;
}

double CSOpacity::MinimumValue() const
{
   return 0;
}

double CSOpacity::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CSWidth::CSWidth( MetaProcess* P ) : MetaInt32( P )
{
   TheCSWidthParameter = this;
}

IsoString CSWidth::Id() const
{
   return "width";
}

// ----------------------------------------------------------------------------

CSHeight::CSHeight( MetaProcess* P ) : MetaInt32( P )
{
   TheCSHeightParameter = this;
}

IsoString CSHeight::Id() const
{
   return "height";
}

// ----------------------------------------------------------------------------

CSColor::CSColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheCSColorParameter = this;
}

IsoString CSColor::Id() const
{
   return "clonerColor";
}

double CSColor::DefaultValue() const
{
   return 0xffe0e0e0;
}

// ----------------------------------------------------------------------------

CSBoundsColor::CSBoundsColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheCSBoundsColorParameter = this;
}

IsoString CSBoundsColor::Id() const
{
   return "boundsColor";
}

double CSBoundsColor::DefaultValue() const
{
   return 0xffe0e0e0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CloneStampParameters.cpp - Released 2017-07-18T16:14:18Z
