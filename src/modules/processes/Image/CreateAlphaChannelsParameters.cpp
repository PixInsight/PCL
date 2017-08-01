//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0402
// ----------------------------------------------------------------------------
// CreateAlphaChannelsParameters.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "CreateAlphaChannelsParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CAFromImage*               TheCAFromImageParameter = 0;
CASourceImageIdentifier*   TheCASourceImageIdentifierParameter = 0;
CAInvertSourceImage*       TheCAInvertSourceImageParameter = 0;
CACloseSourceImage*        TheCACloseSourceImageParameter = 0;
CATransparency*            TheCATransparencyParameter = 0;
CAReplaceExistingChannels* TheCAReplaceExistingChannelsParameter = 0;
CAChannelCount*            TheCAChannelCountParameter = 0;

// ----------------------------------------------------------------------------

CAFromImage::CAFromImage( MetaProcess* P ) : MetaBoolean( P )
{
   TheCAFromImageParameter = this;
}

IsoString CAFromImage::Id() const
{
   return "fromImage";
}

bool CAFromImage::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CASourceImageIdentifier::CASourceImageIdentifier( MetaProcess* P ) : MetaString( P )
{
   TheCASourceImageIdentifierParameter = this;
}

IsoString CASourceImageIdentifier::Id() const
{
   return "sourceId";
}

size_type CASourceImageIdentifier::MinLength() const
{
   return 0; // can be void
}

String CASourceImageIdentifier::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

CAInvertSourceImage::CAInvertSourceImage( MetaProcess* P ) : MetaBoolean( P )
{
   TheCAInvertSourceImageParameter = this;
}

IsoString CAInvertSourceImage::Id() const
{
   return "invertSource";
}

bool CAInvertSourceImage::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CACloseSourceImage::CACloseSourceImage( MetaProcess* P ) : MetaBoolean( P )
{
   TheCACloseSourceImageParameter = this;
}

IsoString CACloseSourceImage::Id() const
{
   return "closeSource";
}

bool CACloseSourceImage::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CATransparency::CATransparency( MetaProcess* P ) : MetaDouble( P )
{
   TheCATransparencyParameter = this;
}

IsoString CATransparency::Id() const
{
   return "transparency";
}

int CATransparency::Precision() const
{
   return 8;
}

double CATransparency::DefaultValue() const
{
   return 1;
}

double CATransparency::MinimumValue() const
{
   return 0;
}

double CATransparency::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CAReplaceExistingChannels::CAReplaceExistingChannels( MetaProcess* P ) : MetaBoolean( P )
{
   TheCAReplaceExistingChannelsParameter = this;
}

IsoString CAReplaceExistingChannels::Id() const
{
   return "replace";
}

bool CAReplaceExistingChannels::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CAChannelCount::CAChannelCount( MetaProcess* P ) : MetaInt32( P )
{
   TheCAChannelCountParameter = this;
}

IsoString CAChannelCount::Id() const
{
   return "count";
}

double CAChannelCount::DefaultValue() const
{
   return 1;
}

double CAChannelCount::MinimumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CreateAlphaChannelsParameters.cpp - Released 2017-08-01T14:26:58Z
