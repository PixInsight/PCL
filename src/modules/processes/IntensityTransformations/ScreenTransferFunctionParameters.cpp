//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0424
// ----------------------------------------------------------------------------
// ScreenTransferFunctionParameters.cpp - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "ScreenTransferFunctionParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

STFSet*                 TheSTFSetParameter = 0;
STFShadowsClipping*     TheSTFShadowsClippingParameter = 0;
STFHighlightsClipping*  TheSTFHighlightsClippingParameter = 0;
STFMidtonesBalance*     TheSTFMidtonesBalanceParameter = 0;
STFLowRange*            TheSTFLowRangeParameter = 0;
STFHighRange*           TheSTFHighRangeParameter = 0;
STFInteraction*         TheSTFInteractionParameter = 0;

// ----------------------------------------------------------------------------

STFSet::STFSet( MetaProcess* p ) : MetaTable( p )
{
   TheSTFSetParameter = this;
}

IsoString STFSet::Id() const
{
   return "STF";
}

size_type STFSet::MinLength() const
{
   return 4;
}

size_type STFSet::MaxLength() const
{
   return 4;
}

// ----------------------------------------------------------------------------

STFShadowsClipping::STFShadowsClipping( MetaTable* T ) : MetaFloat( T )
{
   TheSTFShadowsClippingParameter = this;
}

IsoString STFShadowsClipping::Id() const
{
   return "c0";
}

int STFShadowsClipping::Precision() const
{
   return 5;
}

double STFShadowsClipping::DefaultValue() const
{
   return 0.0;
}

double STFShadowsClipping::MinimumValue() const
{
   return 0.0;
}

double STFShadowsClipping::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

STFHighlightsClipping::STFHighlightsClipping( MetaTable* T ) : MetaFloat( T )
{
   TheSTFHighlightsClippingParameter = this;
}

IsoString STFHighlightsClipping::Id() const
{
   return "c1";
}

int STFHighlightsClipping::Precision() const
{
   return 5;
}

double STFHighlightsClipping::DefaultValue() const
{
   return 0.0;
}

double STFHighlightsClipping::MinimumValue() const
{
   return 0.0;
}

double STFHighlightsClipping::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

STFMidtonesBalance::STFMidtonesBalance( MetaTable* T ) : MetaFloat( T )
{
   TheSTFMidtonesBalanceParameter = this;
}

IsoString STFMidtonesBalance::Id() const
{
   return "m";
}

int STFMidtonesBalance::Precision() const
{
   return 5;
}

double STFMidtonesBalance::DefaultValue() const
{
   return 0.5;
}

double STFMidtonesBalance::MinimumValue() const
{
   return 0.0;
}

double STFMidtonesBalance::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

STFLowRange::STFLowRange( MetaTable* T ) : MetaFloat( T )
{
   TheSTFLowRangeParameter = this;
}

IsoString STFLowRange::Id() const
{
   return "r0";
}

int STFLowRange::Precision() const
{
   return 5;
}

double STFLowRange::DefaultValue() const
{
   return 0.0;
}

double STFLowRange::MinimumValue() const
{
   return -10.0;
}

double STFLowRange::MaximumValue() const
{
   return 0.0;
}

// ----------------------------------------------------------------------------

STFHighRange::STFHighRange( MetaTable* T ) : MetaFloat( T )
{
   TheSTFHighRangeParameter = this;
}

IsoString STFHighRange::Id() const
{
   return "r1";
}

int STFHighRange::Precision() const
{
   return 5;
}

double STFHighRange::DefaultValue() const
{
   return 1.0;
}

double STFHighRange::MinimumValue() const
{
   return 1.0;
}

double STFHighRange::MaximumValue() const
{
   return +11.0;
}

// ----------------------------------------------------------------------------

STFInteraction::STFInteraction( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSTFInteractionParameter = this;
}

IsoString STFInteraction::Id() const
{
   return "interaction";
}

size_type STFInteraction::NumberOfElements() const
{
   return NumberOfInteractionModes;
}

IsoString STFInteraction::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case SeparateChannels: return "SeparateChannels";
   case Grayscale:        return "Grayscale";
   }
}

int STFInteraction::ElementValue( size_type i ) const
{
   return int( i );
}

size_type STFInteraction::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ScreenTransferFunctionParameters.cpp - Released 2018-12-12T09:25:25Z
