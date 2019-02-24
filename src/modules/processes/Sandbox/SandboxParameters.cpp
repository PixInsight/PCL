//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Sandbox Process Module Version 01.00.02.0286
// ----------------------------------------------------------------------------
// SandboxParameters.cpp - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard Sandbox PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "SandboxParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SandboxParameterOne*   TheSandboxParameterOneParameter = nullptr;
SandboxParameterTwo*   TheSandboxParameterTwoParameter = nullptr;
SandboxParameterThree* TheSandboxParameterThreeParameter = nullptr;
SandboxParameterFour*  TheSandboxParameterFourParameter = nullptr;
SandboxParameterFive*  TheSandboxParameterFiveParameter = nullptr;

// ----------------------------------------------------------------------------

SandboxParameterOne::SandboxParameterOne( MetaProcess* P ) : MetaFloat( P )
{
   TheSandboxParameterOneParameter = this;
}

IsoString SandboxParameterOne::Id() const
{
   return "sampleOne";
}

int SandboxParameterOne::Precision() const
{
   return 3;
}

double SandboxParameterOne::DefaultValue() const
{
   return 0;
}

double SandboxParameterOne::MinimumValue() const
{
   return 0;
}

double SandboxParameterOne::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

SandboxParameterTwo::SandboxParameterTwo( MetaProcess* P ) : MetaInt32( P )
{
   TheSandboxParameterTwoParameter = this;
}

IsoString SandboxParameterTwo::Id() const
{
   return "sampleTwo";
}

double SandboxParameterTwo::DefaultValue() const
{
   return 1;
}

double SandboxParameterTwo::MinimumValue() const
{
   return 1;
}

double SandboxParameterTwo::MaximumValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

SandboxParameterThree::SandboxParameterThree( MetaProcess* P ) : MetaBoolean( P )
{
   TheSandboxParameterThreeParameter = this;
}

IsoString SandboxParameterThree::Id() const
{
   return "sampleThree";
}

bool SandboxParameterThree::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SandboxParameterFour::SandboxParameterFour( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSandboxParameterFourParameter = this;
}

IsoString SandboxParameterFour::Id() const
{
   return "sampleFour";
}

size_type SandboxParameterFour::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SandboxParameterFour::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case FirstItem:  return "FirstItem";
   case SecondItem: return "SecondItem";
   case ThirdItem:  return "ThirdItem";
   }
}

int SandboxParameterFour::ElementValue( size_type i ) const
{
   return int( i );
}

size_type SandboxParameterFour::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

SandboxParameterFive::SandboxParameterFive( MetaProcess* P ) : MetaString( P )
{
   TheSandboxParameterFiveParameter = this;
}

IsoString SandboxParameterFive::Id() const
{
   return "sampleFive";
}

size_type SandboxParameterFive::MinLength() const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SandboxParameters.cpp - Released 2019-01-21T12:06:42Z
