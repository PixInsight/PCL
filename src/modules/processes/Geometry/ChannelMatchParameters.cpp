//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.01.0365
// ----------------------------------------------------------------------------
// ChannelMatchParameters.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#include "ChannelMatchParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CMChannels* TheCMChannelsParameter = nullptr;
CMEnabled*  TheCMEnabledParameter = nullptr;
CMXOffset*  TheCMXOffsetParameter = nullptr;
CMYOffset*  TheCMYOffsetParameter = nullptr;
CMFactor*   TheCMFactorParameter = nullptr;

// ----------------------------------------------------------------------------

CMChannels::CMChannels( MetaProcess* P ) : MetaTable( P )
{
   TheCMChannelsParameter = this;
}

IsoString CMChannels::Id() const
{
   return "channels";
}

size_type CMChannels::MinLength() const
{
   return 3;
}

size_type CMChannels::MaxLength() const
{
   return 3;
}

// ----------------------------------------------------------------------------

CMEnabled::CMEnabled( MetaTable* T ) : MetaBoolean( T )
{
   TheCMEnabledParameter = this;
}

IsoString CMEnabled::Id() const
{
   return "enabled";
}

bool CMEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CMXOffset::CMXOffset( MetaTable* T ) : MetaFloat( T )
{
   TheCMXOffsetParameter = this;
}

IsoString CMXOffset::Id() const
{
   return "dx";
}

int CMXOffset::Precision() const
{
   return 2;
}

double CMXOffset::MinimumValue() const
{
   return -100;
}

double CMXOffset::MaximumValue() const
{
   return +100;
}

// ----------------------------------------------------------------------------

CMYOffset::CMYOffset( MetaTable* T ) : MetaFloat( T )
{
   TheCMYOffsetParameter = this;
}

IsoString CMYOffset::Id() const
{
   return "dy";
}

int CMYOffset::Precision() const
{
   return 2;
}

double CMYOffset::MinimumValue() const
{
   return -100;
}

double CMYOffset::MaximumValue() const
{
   return +100;
}

// ----------------------------------------------------------------------------

CMFactor::CMFactor( MetaTable* T ) : MetaDouble( T )
{
   TheCMFactorParameter = this;
}

IsoString CMFactor::Id() const
{
   return "k";
}

int CMFactor::Precision() const
{
   return 8;
}

double CMFactor::MinimumValue() const
{
   return 0;
}

double CMFactor::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelMatchParameters.cpp - Released 2017-07-09T18:07:33Z
