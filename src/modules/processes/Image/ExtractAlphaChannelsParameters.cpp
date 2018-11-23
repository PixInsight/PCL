//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.03.00.0427
// ----------------------------------------------------------------------------
// ExtractAlphaChannelsParameters.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "ExtractAlphaChannelsParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

EAChannels*    TheEAChannelsParameter = 0;
EAChannelList* TheEAChannelListParameter = 0;
EAExtract*     TheEAExtractParameter = 0;
EADelete*      TheEADeleteParameter = 0;

// ----------------------------------------------------------------------------

EAChannels::EAChannels( MetaProcess* P ) : MetaEnumeration( P )
{
   TheEAChannelsParameter = this;
}

IsoString EAChannels::Id() const
{
   return "channels";
}

size_type EAChannels::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString EAChannels::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case AllAlphaChannels:     return "AllAlphaChannels";
   case ActiveAlphaChannel:   return "ActiveAlphaChannel";
   case ChannelList:          return "ChannelList";
   }
}

int EAChannels::ElementValue( size_type i ) const
{
   return int( i );
}

size_type EAChannels::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

EAChannelList::EAChannelList( MetaProcess* P ) : MetaString( P )
{
   TheEAChannelListParameter = this;
}

IsoString EAChannelList::Id() const
{
   return "channelList";
}

String EAChannelList::DefaultValue() const
{
   return "0";
}

String EAChannelList::AllowedCharacters() const
{
   return "0123456789, ";
}

// ----------------------------------------------------------------------------

EAExtract::EAExtract( MetaProcess* P ) : MetaBoolean( P )
{
   TheEAExtractParameter = this;
}

IsoString EAExtract::Id() const
{
   return "extract";
}

bool EAExtract::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

EADelete::EADelete( MetaProcess* P ) : MetaBoolean( P )
{
   TheEADeleteParameter = this;
}

IsoString EADelete::Id() const
{
   return "delete";
}

bool EADelete::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExtractAlphaChannelsParameters.cpp - Released 2018-11-23T18:45:58Z
