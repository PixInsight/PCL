//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.01.00.0304
// ----------------------------------------------------------------------------
// ChannelMatchParameters.cpp - Released 2015/11/26 16:00:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#include "ChannelMatchParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelTable*           TheChannelTableParameter = 0;
ChannelEnabled*         TheChannelEnabledParameter = 0;
ChannelXOffset*         TheChannelXOffsetParameter = 0;
ChannelYOffset*         TheChannelYOffsetParameter = 0;
ChannelFactor*          TheChannelFactorParameter = 0;

// ----------------------------------------------------------------------------

ChannelTable::ChannelTable( MetaProcess* P ) : MetaTable( P )
{
   TheChannelTableParameter = this;
}

IsoString ChannelTable::Id() const
{
   return "channels";
}

size_type ChannelTable::MinLength() const
{
   return 3;
}

size_type ChannelTable::MaxLength() const
{
   return 3;
}

// ----------------------------------------------------------------------------

ChannelEnabled::ChannelEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheChannelEnabledParameter = this;
}

IsoString ChannelEnabled::Id() const
{
   return "enabled";
}

bool ChannelEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ChannelXOffset::ChannelXOffset( MetaTable* T ) : MetaFloat( T )
{
   TheChannelXOffsetParameter = this;
}

IsoString ChannelXOffset::Id() const
{
   return "dx";
}

int ChannelXOffset::Precision() const
{
   return 2;
}

double ChannelXOffset::MinimumValue() const
{
   return -100;
}

double ChannelXOffset::MaximumValue() const
{
   return +100;
}

// ----------------------------------------------------------------------------

ChannelYOffset::ChannelYOffset( MetaTable* T ) : MetaFloat( T )
{
   TheChannelYOffsetParameter = this;
}

IsoString ChannelYOffset::Id() const
{
   return "dy";
}

int ChannelYOffset::Precision() const
{
   return 2;
}

double ChannelYOffset::MinimumValue() const
{
   return -100;
}

double ChannelYOffset::MaximumValue() const
{
   return +100;
}

// ----------------------------------------------------------------------------

ChannelFactor::ChannelFactor( MetaTable* T ) : MetaDouble( T )
{
   TheChannelFactorParameter = this;
}

IsoString ChannelFactor::Id() const
{
   return "k";
}

int ChannelFactor::Precision() const
{
   return 8;
}

double ChannelFactor::MinimumValue() const
{
   return 0;
}

double ChannelFactor::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelMatchParameters.cpp - Released 2015/11/26 16:00:12 UTC
