//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0159
// ----------------------------------------------------------------------------
// UnsharpMaskParameters.cpp - Released 2015/07/31 11:49:48 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "UnsharpMaskParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

USMSigma*               TheUSMSigmaParameter = 0;
USMAmount*              TheUSMAmountParameter = 0;
USMUseLuminance*        TheUSMUseLuminanceParameter = 0;
USMLinear*              TheUSMLinearParameter = 0;
USMDeringing*           TheUSMDeringingParameter = 0;
USMDeringingDark*       TheUSMDeringingDarkParameter = 0;
USMDeringingBright*     TheUSMDeringingBrightParameter = 0;
USMOutputDeringingMaps* TheUSMOutputDeringingMapsParameter = 0;
USMRangeLow*            TheUSMRangeLowParameter = 0;
USMRangeHigh*           TheUSMRangeHighParameter = 0;

// ----------------------------------------------------------------------------

USMSigma::USMSigma( MetaProcess* P ) : MetaFloat( P )
{
   TheUSMSigmaParameter = this;
}

IsoString USMSigma::Id() const
{
   return "sigma";
}

int USMSigma::Precision() const
{
   return 2;
}

double USMSigma::MinimumValue() const
{
   return 0.10;
}

double USMSigma::MaximumValue() const
{
   return 250;
}

double USMSigma::DefaultValue() const
{
   return 2.00;
}

// ----------------------------------------------------------------------------

USMAmount::USMAmount( MetaProcess* P ) : MetaFloat( P )
{
   TheUSMAmountParameter = this;
}

IsoString USMAmount::Id() const
{
   return "amount";
}

int USMAmount::Precision() const
{
   return 2;
}

double USMAmount::MinimumValue() const
{
   return 0.10;
}

double USMAmount::MaximumValue() const
{
   return 1;
}

double USMAmount::DefaultValue() const
{
   return 0.80;
}

// ----------------------------------------------------------------------------

USMUseLuminance::USMUseLuminance( MetaProcess* P ) : MetaBoolean( P )
{
   TheUSMUseLuminanceParameter = this;
}

IsoString USMUseLuminance::Id() const
{
   return "useLuminance";
}

bool USMUseLuminance::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

USMLinear::USMLinear( MetaProcess* P ) : MetaBoolean( P )
{
   TheUSMLinearParameter = this;
}

IsoString USMLinear::Id() const
{
   return "linear";
}

bool USMLinear::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

USMDeringing::USMDeringing( MetaProcess* P ) : MetaBoolean( P )
{
   TheUSMDeringingParameter = this;
}

IsoString USMDeringing::Id() const
{
   return "deringing";
}

bool USMDeringing::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

USMDeringingDark::USMDeringingDark( MetaProcess* P ) : MetaFloat( P )
{
   TheUSMDeringingDarkParameter = this;
}

IsoString USMDeringingDark::Id() const
{
   return "deringingDark";
}

int USMDeringingDark::Precision() const
{
   return 4;
}

double USMDeringingDark::DefaultValue() const
{
   return 0.1000;
}

double USMDeringingDark::MinimumValue() const
{
   return 0;
}

double USMDeringingDark::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

USMDeringingBright::USMDeringingBright( MetaProcess* P ) : MetaFloat( P )
{
   TheUSMDeringingBrightParameter = this;
}

IsoString USMDeringingBright::Id() const
{
   return "deringingBright";
}

int USMDeringingBright::Precision() const
{
   return 4;
}

double USMDeringingBright::DefaultValue() const
{
   return 0;
}

double USMDeringingBright::MinimumValue() const
{
   return 0;
}

double USMDeringingBright::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

USMOutputDeringingMaps::USMOutputDeringingMaps( MetaProcess* P ) : MetaBoolean( P )
{
   TheUSMOutputDeringingMapsParameter = this;
}

IsoString USMOutputDeringingMaps::Id() const
{
   return "outputDeringingMaps";
}

bool USMOutputDeringingMaps::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

USMRangeLow::USMRangeLow( MetaProcess* P ) : MetaFloat( P )
{
   TheUSMRangeLowParameter = this;
}

IsoString USMRangeLow::Id() const
{
   return "rangeLow";
}

int USMRangeLow::Precision() const
{
   return 7;
}

double USMRangeLow::MinimumValue() const
{
   return 0;
}

double USMRangeLow::MaximumValue() const
{
   return 1;
}

double USMRangeLow::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

USMRangeHigh::USMRangeHigh( MetaProcess* P ) : MetaFloat( P )
{
   TheUSMRangeHighParameter = this;
}

IsoString USMRangeHigh::Id() const
{
   return "rangeHigh";
}

int USMRangeHigh::Precision() const
{
   return 7;
}

double USMRangeHigh::MinimumValue() const
{
   return 0;
}

double USMRangeHigh::MaximumValue() const
{
   return 1;
}

double USMRangeHigh::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF UnsharpMaskParameters.cpp - Released 2015/07/31 11:49:48 UTC
