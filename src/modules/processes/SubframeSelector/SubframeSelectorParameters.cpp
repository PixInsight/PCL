//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorParameters.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
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

#include "SubframeSelectorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SSTargetFrames*              TheSSTargetFramesParameter = 0;
SSTargetFrameEnabled*        TheSSTargetFrameEnabledParameter = 0;
SSTargetFramePath*           TheSSTargetFramePathParameter = 0;

SubframeSelectorParameterOne*   TheSubframeSelectorParameterOneParameter = nullptr;
SubframeSelectorParameterTwo*   TheSubframeSelectorParameterTwoParameter = nullptr;
SubframeSelectorParameterThree* TheSubframeSelectorParameterThreeParameter = nullptr;
SubframeSelectorParameterFour*  TheSubframeSelectorParameterFourParameter = nullptr;
SubframeSelectorParameterFive*  TheSubframeSelectorParameterFiveParameter = nullptr;

// ----------------------------------------------------------------------------

SSTargetFrames::SSTargetFrames( MetaProcess* P ) : MetaTable( P )
{
   TheSSTargetFramesParameter = this;
}

IsoString SSTargetFrames::Id() const
{
   return "targetFrames";
}

// ----------------------------------------------------------------------------

SSTargetFrameEnabled::SSTargetFrameEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheSSTargetFrameEnabledParameter = this;
}

IsoString SSTargetFrameEnabled::Id() const
{
   return "enabled";
}

bool SSTargetFrameEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SSTargetFramePath::SSTargetFramePath( MetaTable* T ) : MetaString( T )
{
   TheSSTargetFramePathParameter = this;
}

IsoString SSTargetFramePath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------

SubframeSelectorParameterOne::SubframeSelectorParameterOne( MetaProcess* P ) : MetaFloat( P )
{
   TheSubframeSelectorParameterOneParameter = this;
}

IsoString SubframeSelectorParameterOne::Id() const
{
   return "sampleOne";
}

int SubframeSelectorParameterOne::Precision() const
{
   return 3;
}

double SubframeSelectorParameterOne::DefaultValue() const
{
   return 0;
}

double SubframeSelectorParameterOne::MinimumValue() const
{
   return 0;
}

double SubframeSelectorParameterOne::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

SubframeSelectorParameterTwo::SubframeSelectorParameterTwo( MetaProcess* P ) : MetaInt32( P )
{
   TheSubframeSelectorParameterTwoParameter = this;
}

IsoString SubframeSelectorParameterTwo::Id() const
{
   return "sampleTwo";
}

double SubframeSelectorParameterTwo::DefaultValue() const
{
   return 1;
}

double SubframeSelectorParameterTwo::MinimumValue() const
{
   return 1;
}

double SubframeSelectorParameterTwo::MaximumValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

SubframeSelectorParameterThree::SubframeSelectorParameterThree( MetaProcess* P ) : MetaBoolean( P )
{
   TheSubframeSelectorParameterThreeParameter = this;
}

IsoString SubframeSelectorParameterThree::Id() const
{
   return "sampleThree";
}

bool SubframeSelectorParameterThree::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SubframeSelectorParameterFour::SubframeSelectorParameterFour( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSubframeSelectorParameterFourParameter = this;
}

IsoString SubframeSelectorParameterFour::Id() const
{
   return "sampleFour";
}

size_type SubframeSelectorParameterFour::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SubframeSelectorParameterFour::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case FirstItem:  return "FirstItem";
   case SecondItem: return "SecondItem";
   case ThirdItem:  return "ThirdItem";
   }
}

int SubframeSelectorParameterFour::ElementValue( size_type i ) const
{
   return int( i );
}

size_type SubframeSelectorParameterFour::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

SubframeSelectorParameterFive::SubframeSelectorParameterFive( MetaProcess* P ) : MetaString( P )
{
   TheSubframeSelectorParameterFiveParameter = this;
}

IsoString SubframeSelectorParameterFive::Id() const
{
   return "sampleFive";
}

size_type SubframeSelectorParameterFive::MinLength() const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorParameters.cpp - Released 2017-08-01T14:26:58Z
