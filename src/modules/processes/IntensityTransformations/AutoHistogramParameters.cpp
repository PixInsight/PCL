//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0413
// ----------------------------------------------------------------------------
// AutoHistogramParameters.cpp - Released 2018-11-01T11:07:21Z
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

#include "AutoHistogramParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

AHClip*            TheAHClipParameter = 0;
AHClipTogether*    TheAHClipTogetherParameter = 0;
AHClipLowR*        TheAHClipLowRParameter = 0;
AHClipLowG*        TheAHClipLowGParameter = 0;
AHClipLowB*        TheAHClipLowBParameter = 0;
AHClipHighR*       TheAHClipHighRParameter = 0;
AHClipHighG*       TheAHClipHighGParameter = 0;
AHClipHighB*       TheAHClipHighBParameter = 0;

AHStretch*         TheAHStretchParameter = 0;
AHStretchTogether* TheAHStretchTogetherParameter = 0;
AHStretchMethod*   TheAHStretchMethodParameter = 0;
AHTargetMedianR*   TheAHTargetMedianRParameter = 0;
AHTargetMedianG*   TheAHTargetMedianGParameter = 0;
AHTargetMedianB*   TheAHTargetMedianBParameter = 0;

// ----------------------------------------------------------------------------

AHClip::AHClip( MetaProcess* P ) : MetaBoolean( P )
{
   TheAHClipParameter = this;
}

IsoString AHClip::Id() const
{
   return "clip";
}

IsoString AHClip::Aliases() const
{
   return "isHistogramClipEnabled";
}

bool AHClip::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

AHClipTogether::AHClipTogether( MetaProcess* P ) : MetaBoolean( P )
{
   TheAHClipTogetherParameter = this;
}

IsoString AHClipTogether::Id() const
{
   return "clipTogether";
}

IsoString AHClipTogether::Aliases() const
{
   return "isGlobalHistogramClip";
}

bool AHClipTogether::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

AHClipLowR::AHClipLowR( MetaProcess* P ) : MetaFloat( P )
{
   TheAHClipLowRParameter = this;
}

IsoString AHClipLowR::Id() const
{
   return "clipLowR";
}

int AHClipLowR::Precision() const
{
   return 3;
}

double AHClipLowR::MinimumValue() const
{
   return 0;
}

double AHClipLowR::MaximumValue() const
{
   return 100;
}

double AHClipLowR::DefaultValue() const
{
   return 0.01;
}

// ----------------------------------------------------------------------------

AHClipLowG::AHClipLowG( MetaProcess* P ) : MetaFloat( P )
{
   TheAHClipLowGParameter = this;
}

IsoString AHClipLowG::Id() const
{
   return "clipLowG";
}

int AHClipLowG::Precision() const
{
   return 3;
}

double AHClipLowG::MinimumValue() const
{
   return 0;
}

double AHClipLowG::MaximumValue() const
{
   return 100;
}

double AHClipLowG::DefaultValue() const
{
   return 0.01;
}

// ----------------------------------------------------------------------------

AHClipLowB::AHClipLowB( MetaProcess* P ) : MetaFloat( P )
{
   TheAHClipLowBParameter = this;
}

IsoString AHClipLowB::Id() const
{
   return "clipLowB";
}

int AHClipLowB::Precision() const
{
   return 3;
}

double AHClipLowB::MinimumValue() const
{
   return 0;
}

double AHClipLowB::MaximumValue() const
{
   return 100;
}

double AHClipLowB::DefaultValue() const
{
   return 0.01;
}

// ----------------------------------------------------------------------------

AHClipHighR::AHClipHighR( MetaProcess* P ) : MetaFloat( P )
{
   TheAHClipHighRParameter = this;
}

IsoString AHClipHighR::Id() const
{
   return "clipHighR";
}

int AHClipHighR::Precision() const
{
   return 3;
}

double AHClipHighR::MinimumValue() const
{
   return 0;
}

double AHClipHighR::MaximumValue() const
{
   return 100;
}

double AHClipHighR::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

AHClipHighG::AHClipHighG( MetaProcess* P ) : MetaFloat( P )
{
   TheAHClipHighGParameter = this;
}

IsoString AHClipHighG::Id() const
{
   return "clipHighG";
}

int AHClipHighG::Precision() const
{
   return 3;
}

double AHClipHighG::MinimumValue() const
{
   return 0;
}

double AHClipHighG::MaximumValue() const
{
   return 100;
}

double AHClipHighG::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

AHClipHighB::AHClipHighB( MetaProcess* P ) : MetaFloat( P )
{
   TheAHClipHighBParameter = this;
}

IsoString AHClipHighB::Id() const
{
   return "clipHighB";
}

int AHClipHighB::Precision() const
{
   return 3;
}

double AHClipHighB::MinimumValue() const
{
   return 0;
}

double AHClipHighB::MaximumValue() const
{
   return 100;
}

double AHClipHighB::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

AHStretch::AHStretch( MetaProcess* P ) : MetaBoolean( P ) // ### MetaDouble ???
{
   TheAHStretchParameter = this;
}

IsoString AHStretch::Id() const
{
   return "stretch";
}

IsoString AHStretch::Aliases() const
{
   return "isGammaEnabled";
}

bool AHStretch::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

AHStretchTogether::AHStretchTogether( MetaProcess* P ) : MetaBoolean( P )
{
   TheAHStretchTogetherParameter = this;
}

IsoString AHStretchTogether::Id() const
{
   return "stretchTogether";
}

IsoString AHStretchTogether::Aliases() const
{
   return "isGlobalGamma";
}

bool AHStretchTogether::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

AHStretchMethod::AHStretchMethod( MetaProcess* P ) : MetaEnumeration( P )
{
   TheAHStretchMethodParameter = this;
}

IsoString AHStretchMethod::Id() const
{
   return "stretchMethod";
}

size_type AHStretchMethod::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString AHStretchMethod::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Gamma:     return "Gamma";
   case Logarithm: return "Logarithm";
   case MTF:       return "MTF";
   }
}

int AHStretchMethod::ElementValue( size_type i ) const
{
   return int( i );
}

size_type AHStretchMethod::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

AHTargetMedianR::AHTargetMedianR( MetaProcess* P ) : MetaDouble( P )
{
   TheAHTargetMedianRParameter = this;
}

IsoString AHTargetMedianR::Id() const
{
   return "targetMedianR";
}

IsoString AHTargetMedianR::Aliases() const
{
   return "medianR";
}

int AHTargetMedianR::Precision() const
{
   return 8;
}

double AHTargetMedianR::MinimumValue() const
{
   return 0;
}

double AHTargetMedianR::MaximumValue() const
{
   return 1;
}

double AHTargetMedianR::DefaultValue() const
{
   return 0.12;
}

// ----------------------------------------------------------------------------

AHTargetMedianG::AHTargetMedianG( MetaProcess* P ) : MetaDouble( P )
{
   TheAHTargetMedianGParameter = this;
}

IsoString AHTargetMedianG::Id() const
{
   return "targetMedianG";
}

IsoString AHTargetMedianG::Aliases() const
{
   return "medianG";
}

int AHTargetMedianG::Precision() const
{
   return 8;
}

double AHTargetMedianG::MinimumValue() const
{
   return 0;
}

double AHTargetMedianG::MaximumValue() const
{
   return 1;
}

double AHTargetMedianG::DefaultValue() const
{
   return 0.12;
}

// ----------------------------------------------------------------------------

AHTargetMedianB::AHTargetMedianB( MetaProcess* P ) : MetaDouble( P )
{
   TheAHTargetMedianBParameter = this;
}

IsoString AHTargetMedianB::Id() const
{
   return "targetMedianB";
}

IsoString AHTargetMedianB::Aliases() const
{
   return "medianB";
}

int AHTargetMedianB::Precision() const
{
   return 8;
}

double AHTargetMedianB::MinimumValue() const
{
   return 0;
}

double AHTargetMedianB::MaximumValue() const
{
   return 1;
}

double AHTargetMedianB::DefaultValue() const
{
   return 0.12;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AutoHistogramParameters.cpp - Released 2018-11-01T11:07:21Z
