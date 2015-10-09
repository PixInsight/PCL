//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0258
// ----------------------------------------------------------------------------
// ChannelExtractionProcess.cpp - Released 2015/10/08 11:24:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#include "ChannelExtractionProcess.h"
#include "ChannelExtractionParameters.h"
#include "ChannelExtractionInstance.h"
#include "ChannelExtractionInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelExtractionProcess*  TheChannelExtractionProcess = 0;

// ----------------------------------------------------------------------------

#include "ChannelExtractionIcon.xpm"

// ----------------------------------------------------------------------------

ChannelExtractionProcess::ChannelExtractionProcess() : MetaProcess()
{
   TheChannelExtractionProcess = this;

   // Instantiate process parameters
   TheColorSpaceIdExtractionParameter = new ColorSpaceId( this );
   TheChannelTableExtractionParameter = new ChannelTable( this );
   TheChannelEnabledExtractionParameter = new ChannelEnabled( TheChannelTableExtractionParameter );
   TheChannelIdExtractionParameter = new ChannelId( TheChannelTableExtractionParameter );
   TheChannelSampleFormatExtractionParameter = new ChannelSampleFormat( this );
}

// ----------------------------------------------------------------------------

IsoString ChannelExtractionProcess::Id() const
{
   return "ChannelExtraction";
}

// ----------------------------------------------------------------------------

IsoString ChannelExtractionProcess::Category() const
{
   return "ColorSpaces,ChannelManagement";
}

// ----------------------------------------------------------------------------

uint32 ChannelExtractionProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ChannelExtractionProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** ChannelExtractionProcess::IconImageXPM() const
{
   return ChannelExtractionIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* ChannelExtractionProcess::DefaultInterface() const
{
   return TheChannelExtractionInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ChannelExtractionProcess::Create() const
{
   return new ChannelExtractionInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ChannelExtractionProcess::Clone( const ProcessImplementation& p ) const
{
   const ChannelExtractionInstance* instPtr = dynamic_cast<const ChannelExtractionInstance*>( &p );
   return (instPtr != 0) ? new ChannelExtractionInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool ChannelExtractionProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelExtractionProcess.cpp - Released 2015/10/08 11:24:39 UTC
