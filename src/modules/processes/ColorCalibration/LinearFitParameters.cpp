//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.02.00.0238
// ----------------------------------------------------------------------------
// LinearFitParameters.cpp - Released 2016/02/21 20:22:42 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "LinearFitParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

LFReferenceViewId* TheLFReferenceViewIdParameter = 0;
LFRejectLow*       TheLFRejectLowParameter = 0;
LFRejectHigh*      TheLFRejectHighParameter = 0;

// ----------------------------------------------------------------------------

LFReferenceViewId::LFReferenceViewId( MetaProcess* P ) : MetaString( P )
{
   TheLFReferenceViewIdParameter = this;
}

IsoString LFReferenceViewId::Id() const
{
   return "referenceViewId";
}

// ----------------------------------------------------------------------------

LFRejectLow::LFRejectLow( MetaProcess* P ) : MetaFloat( P )
{
   TheLFRejectLowParameter = this;
}

IsoString LFRejectLow::Id() const
{
   return "rejectLow";
}

int LFRejectLow::Precision() const
{
   return 6;
}

double LFRejectLow::MinimumValue() const
{
   return 0;
}

double LFRejectLow::MaximumValue() const
{
   return 1;
}

double LFRejectLow::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

LFRejectHigh::LFRejectHigh( MetaProcess* P ) : MetaFloat( P )
{
   TheLFRejectHighParameter = this;
}

IsoString LFRejectHigh::Id() const
{
   return "rejectHigh";
}

int LFRejectHigh::Precision() const
{
   return 6;
}

double LFRejectHigh::MinimumValue() const
{
   return 0;
}

double LFRejectHigh::MaximumValue() const
{
   return 1;
}

double LFRejectHigh::DefaultValue() const
{
   return 0.92;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LinearFitParameters.cpp - Released 2016/02/21 20:22:42 UTC
