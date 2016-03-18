//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0355
// ----------------------------------------------------------------------------
// BinarizeParameters.cpp - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "BinarizeParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

BinarizeLevelR*   TheBinarizeLevelRParameter = 0;
BinarizeLevelG*   TheBinarizeLevelGParameter = 0;
BinarizeLevelB*   TheBinarizeLevelBParameter = 0;
BinarizeIsGlobal* TheBinarizeIsGlobalParameter = 0;

// ----------------------------------------------------------------------------

BinarizeLevelR::BinarizeLevelR( MetaProcess* P ) : MetaDouble( P )
{
   TheBinarizeLevelRParameter = this;
}

IsoString BinarizeLevelR::Id() const
{
   return "thresholdRK";
}

int BinarizeLevelR::Precision() const
{
   return 8;
}

double BinarizeLevelR::MinimumValue() const
{
   return 0;
}

double BinarizeLevelR::MaximumValue() const
{
   return 1;
}

double BinarizeLevelR::DefaultValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

BinarizeLevelG::BinarizeLevelG( MetaProcess* P ) : MetaDouble( P )
{
   TheBinarizeLevelGParameter = this;
}

IsoString BinarizeLevelG::Id() const
{
   return "thresholdG";
}

int BinarizeLevelG::Precision() const
{
   return 8;
}

double BinarizeLevelG::MinimumValue() const
{
   return 0;
}

double BinarizeLevelG::MaximumValue() const
{
   return 1;
}

double BinarizeLevelG::DefaultValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

BinarizeLevelB::BinarizeLevelB( MetaProcess* P ) : MetaDouble( P )
{
   TheBinarizeLevelBParameter = this;
}

IsoString BinarizeLevelB::Id() const
{
   return "thresholdB";
}

int BinarizeLevelB::Precision() const
{
   return 8;
}

double BinarizeLevelB::MinimumValue() const
{
   return 0;
}

double BinarizeLevelB::MaximumValue() const
{
   return 1;
}

double BinarizeLevelB::DefaultValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

BinarizeIsGlobal::BinarizeIsGlobal( MetaProcess* P ) : MetaBoolean( P )
{
   TheBinarizeIsGlobalParameter = this;
}

IsoString BinarizeIsGlobal::Id() const
{
   return "isGlobal";
}

bool BinarizeIsGlobal::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BinarizeParameters.cpp - Released 2016/02/21 20:22:43 UTC
