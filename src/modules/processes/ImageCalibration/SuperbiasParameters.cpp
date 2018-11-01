//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0345
// ----------------------------------------------------------------------------
// SuperbiasParameters.cpp - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#include "SuperbiasParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SBColumns*           TheSBColumnsParameter = 0;
SBRows*              TheSBRowsParameter = 0;
SBMedianTransform*   TheSBMedianTransformParameter = 0;
SBExcludeLargeScale* TheSBExcludeLargeScaleParameter = 0;
SBMultiscaleLayers*  TheSBMultiscaleLayersParameter = 0;
SBTrimmingFactor*    TheSBTrimmingFactorParameter = 0;
SBSuperbiasViewId*   TheSBSuperbiasViewIdParameter = 0;
SBSuperbiasViewId1*  TheSBSuperbiasViewId1Parameter = 0;

// ----------------------------------------------------------------------------

SBColumns::SBColumns( MetaProcess* P ) : MetaBoolean( P )
{
   TheSBColumnsParameter = this;
}

IsoString SBColumns::Id() const
{
   return "columns";
}

bool SBColumns::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SBRows::SBRows( MetaProcess* P ) : MetaBoolean( P )
{
   TheSBRowsParameter = this;
}

IsoString SBRows::Id() const
{
   return "rows";
}

bool SBRows::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SBMedianTransform::SBMedianTransform( MetaProcess* P ) : MetaBoolean( P )
{
   TheSBMedianTransformParameter = this;
}

IsoString SBMedianTransform::Id() const
{
   return "medianTransform";
}

bool SBMedianTransform::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SBExcludeLargeScale::SBExcludeLargeScale( MetaProcess* P ) : MetaBoolean( P )
{
   TheSBExcludeLargeScaleParameter = this;
}

IsoString SBExcludeLargeScale::Id() const
{
   return "excludeLargeScale";
}

bool SBExcludeLargeScale::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SBMultiscaleLayers::SBMultiscaleLayers( MetaProcess* P ) : MetaInt32( P )
{
   TheSBMultiscaleLayersParameter = this;
}

IsoString SBMultiscaleLayers::Id() const
{
   return "multiscaleLayers";
}

double SBMultiscaleLayers::DefaultValue() const
{
   return 7;
}

double SBMultiscaleLayers::MinimumValue() const
{
   return 1;
}

double SBMultiscaleLayers::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

SBTrimmingFactor::SBTrimmingFactor( MetaProcess* P ) : MetaFloat( P )
{
   TheSBTrimmingFactorParameter = this;
}

IsoString SBTrimmingFactor::Id() const
{
   return "trimmingFactor";
}

int SBTrimmingFactor::Precision() const
{
   return 3;
}

double SBTrimmingFactor::MinimumValue() const
{
   return 0;
}

double SBTrimmingFactor::MaximumValue() const
{
   return 0.4;
}

double SBTrimmingFactor::DefaultValue() const
{
   return 0.2;
}

// ----------------------------------------------------------------------------

SBSuperbiasViewId::SBSuperbiasViewId( MetaProcess* P ) : MetaString( P )
{
   TheSBSuperbiasViewIdParameter = this;
}

IsoString SBSuperbiasViewId::Id() const
{
   return "superbiasViewId";
}

bool SBSuperbiasViewId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SBSuperbiasViewId1::SBSuperbiasViewId1( MetaProcess* P ) : MetaString( P )
{
   TheSBSuperbiasViewId1Parameter = this;
}

IsoString SBSuperbiasViewId1::Id() const
{
   return "superbiasViewId1";
}

bool SBSuperbiasViewId1::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SuperbiasParameters.cpp - Released 2018-11-01T11:07:21Z
