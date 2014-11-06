// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard NoiseGeneration Process Module Version 01.00.02.0206
// ****************************************************************************
// SimplexNoiseParameters.cpp - Released 2014/11/06 17:11:45 UTC
// ****************************************************************************
// This file is part of the standard NoiseGeneration PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include "SimplexNoiseParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SNAmount*   TheSNAmountParameter = 0;
SNScale*    TheSNScaleParameter = 0;
SNOffsetX*  TheSNOffsetXParameter = 0;
SNOffsetY*  TheSNOffsetYParameter = 0;
SNOperator* TheSNOperatorParameter = 0;

// ----------------------------------------------------------------------------

SNAmount::SNAmount( MetaProcess* p ) : MetaFloat( p )
{
   TheSNAmountParameter = this;
}

IsoString SNAmount::Id() const
{
   return "amount";
}

int SNAmount::Precision() const
{
   return 2;
}

double SNAmount::MinimumValue() const
{
   return 0;
}

double SNAmount::MaximumValue() const
{
   return 1;
}

double SNAmount::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

SNScale::SNScale( MetaProcess* p ) : MetaInt32( p )
{
   TheSNScaleParameter = this;
}

IsoString SNScale::Id() const
{
   return "scale";
}

double SNScale::MinimumValue() const
{
   return 1;
}

double SNScale::MaximumValue() const
{
   return 65536;
}

double SNScale::DefaultValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

SNOffsetX::SNOffsetX( MetaProcess* p ) : MetaInt32( p )
{
   TheSNOffsetXParameter = this;
}

IsoString SNOffsetX::Id() const
{
   return "offsetX";
}

double SNOffsetX::MinimumValue() const
{
   return -32768;
}

double SNOffsetX::MaximumValue() const
{
   return +32767;
}

double SNOffsetX::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

SNOffsetY::SNOffsetY( MetaProcess* p ) : MetaInt32( p )
{
   TheSNOffsetYParameter = this;
}

IsoString SNOffsetY::Id() const
{
   return "offsetY";
}

double SNOffsetY::MinimumValue() const
{
   return -32768;
}

double SNOffsetY::MaximumValue() const
{
   return +32767;
}

double SNOffsetY::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

SNOperator::SNOperator( MetaProcess* p ) : MetaEnumeration( p )
{
   TheSNOperatorParameter = this;
}

IsoString SNOperator::Id() const
{
   return "operator";
}

size_type SNOperator::NumberOfElements() const
{
   return NumberOfOperators;
}

IsoString SNOperator::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Copy:   return "Copy";
   case Add:    return "Add";
   case Sub:    return "Sub";
   case Mul:    return "Mul";
   case Div:    return "Div";
   case Pow:    return "Pow";
   case Dif:    return "Dif";
   case Screen: return "Screen";
   case Or:     return "Or";
   case And:    return "And";
   case Xor:    return "Xor";
   case Nor:    return "Nor";
   case Nand:   return "Nand";
   case Xnor:   return "Xnor";
   }
}

int SNOperator::ElementValue( size_type i ) const
{
   return int( i );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF SimplexNoiseParameters.cpp - Released 2014/11/06 17:11:45 UTC
