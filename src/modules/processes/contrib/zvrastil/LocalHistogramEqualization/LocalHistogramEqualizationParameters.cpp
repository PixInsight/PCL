//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard LocalHistogramEqualization Process Module Version 01.00.00.0211
// ----------------------------------------------------------------------------
// LocalHistogramEqualizationParameters.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard LocalHistogramEqualization PixInsight module.
//
// Copyright (c) 2011-2017 Zbynek Vrastil
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L.
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

#include "LocalHistogramEqualizationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

LHERadius*         TheLHERadiusParameter = 0;
LHEHistogramBins*  TheLHEHistogramBinsParameter = 0;
LHESlopeLimit*     TheLHESlopeLimitParameter = 0;
LHEAmount*         TheLHEAmountParameter = 0;
LHECircularKernel* TheLHECircularKernelParameter = 0;

// ----------------------------------------------------------------------------

LHERadius::LHERadius( MetaProcess* P ) : MetaInt32( P )
{
   TheLHERadiusParameter = this;
}

IsoString LHERadius::Id() const
{
   return "radius";
}

double LHERadius::DefaultValue() const
{
   return 64;
}

double LHERadius::MinimumValue() const
{
   return 16;
}

double LHERadius::MaximumValue() const
{
   return 512;
}

// ----------------------------------------------------------------------------

LHEHistogramBins::LHEHistogramBins( MetaProcess* P ) : MetaEnumeration( P )
{
   TheLHEHistogramBinsParameter = this;
}

IsoString LHEHistogramBins::Id() const
{
   return "histogramBins";
}

size_type LHEHistogramBins::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString LHEHistogramBins::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Bit8:  return "Bit8";
   case Bit10: return "Bit10";
   case Bit12: return "Bit12";
   //case Bit14: return "Bit14";
   //case Bit16: return "Bit16";
   }
}

int LHEHistogramBins::ElementValue( size_type i ) const
{
   return int( i );
}

size_type LHEHistogramBins::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

LHESlopeLimit::LHESlopeLimit( MetaProcess* P ) : MetaDouble( P )
{
   TheLHESlopeLimitParameter = this;
}

IsoString LHESlopeLimit::Id() const
{
   return "slopeLimit";
}

double LHESlopeLimit::DefaultValue() const
{
   return 2.0;
}

double LHESlopeLimit::MinimumValue() const
{
   return 1.0;
}

double LHESlopeLimit::MaximumValue() const
{
   return 64.0;
}

int LHESlopeLimit::Precision() const
{
   return 1;
}

// ----------------------------------------------------------------------------

LHEAmount::LHEAmount( MetaProcess* P ) : MetaDouble( P )
{
   TheLHEAmountParameter = this;
}

IsoString LHEAmount::Id() const
{
   return "amount";
}

double LHEAmount::DefaultValue() const
{
   return 1.0;
}

double LHEAmount::MinimumValue() const
{
   return 0.0;
}

double LHEAmount::MaximumValue() const
{
   return 1.0;
}

int LHEAmount::Precision() const
{
   return 3;
}

// ----------------------------------------------------------------------------

LHECircularKernel::LHECircularKernel( MetaProcess* P ) : MetaBoolean( P )
{
   TheLHECircularKernelParameter = this;
}

IsoString LHECircularKernel::Id() const
{
   return "circularKernel";
}

bool LHECircularKernel::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LocalHistogramEqualizationParameters.cpp - Released 2017-08-01T14:26:58Z
