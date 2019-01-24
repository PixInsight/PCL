//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.02.0404
// ----------------------------------------------------------------------------
// ChannelMatchProcess.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "ChannelMatchProcess.h"
#include "ChannelMatchParameters.h"
#include "ChannelMatchInstance.h"
#include "ChannelMatchInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelMatchProcess* TheChannelMatchProcess = nullptr;

// ----------------------------------------------------------------------------

#include "ChannelMatchIcon.xpm"

// ----------------------------------------------------------------------------

ChannelMatchProcess::ChannelMatchProcess() : MetaProcess()
{
   TheChannelMatchProcess = this;

   new CMChannels( this );
   new CMEnabled( TheCMChannelsParameter );
   new CMXOffset( TheCMChannelsParameter );
   new CMYOffset( TheCMChannelsParameter );
   new CMFactor( TheCMChannelsParameter );
}

IsoString ChannelMatchProcess::Id() const
{
   return "ChannelMatch";
}

IsoString ChannelMatchProcess::Category() const
{
   return "Geometry";
}

uint32 ChannelMatchProcess::Version() const
{
   return 0x100;
}

String ChannelMatchProcess::Description() const
{
   return "";
}

const char** ChannelMatchProcess::IconImageXPM() const
{
   return ChannelMatchIcon_XPM;
}

ProcessInterface* ChannelMatchProcess::DefaultInterface() const
{
   return TheChannelMatchInterface;
}

ProcessImplementation* ChannelMatchProcess::Create() const
{
   return new ChannelMatchInstance( this );
}

ProcessImplementation* ChannelMatchProcess::Clone( const ProcessImplementation& p ) const
{
   const ChannelMatchInstance* instPtr = dynamic_cast<const ChannelMatchInstance*>( &p );
   return (instPtr != nullptr) ? new ChannelMatchInstance( *instPtr ) : nullptr;
}

bool ChannelMatchProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelMatchProcess.cpp - Released 2019-01-21T12:06:41Z
