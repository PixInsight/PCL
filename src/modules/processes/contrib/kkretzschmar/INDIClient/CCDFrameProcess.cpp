//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.02.0096
// ----------------------------------------------------------------------------
// CCDFrameProcess.cpp - Released 2015/10/13 15:55:45 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2015 Klaus Kretzschmar
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

#include "CCDFrameProcess.h"
#include "CCDFrameInterface.h"
#include "CCDFrameInstance.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

//#include "CCDFrameProcess.xpm" // ### TODO

// ----------------------------------------------------------------------------

CCDFrameProcess* TheCCDFrameProcess = nullptr;

// ----------------------------------------------------------------------------

CCDFrameProcess::CCDFrameProcess() : MetaProcess()
{
   TheCCDFrameProcess = this;
}

// ----------------------------------------------------------------------------

IsoString CCDFrameProcess::Id() const
{
   return "INDICCDFrame";
}

// ----------------------------------------------------------------------------

IsoString CCDFrameProcess::Category() const
{
   return "INDI,Instrumentation";
}

// ----------------------------------------------------------------------------

uint32 CCDFrameProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String CCDFrameProcess::Description() const
{
   return
   "<html>"
   "<p>Acquire frames from INDI CCD devices.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** CCDFrameProcess::IconImageXPM() const
{
   return nullptr; // CCDFrameProcess_XPM; // ### TODO
}

// ----------------------------------------------------------------------------

bool CCDFrameProcess::PrefersGlobalExecution() const
{
	return true;
}
// ----------------------------------------------------------------------------

ProcessInterface* CCDFrameProcess::DefaultInterface() const
{
   return TheCCDFrameInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* CCDFrameProcess::Create() const
{
   return new INDIDeviceControllerInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* CCDFrameProcess::Clone( const ProcessImplementation& p ) const
{
   const CCDFrameInstance* instPtr = dynamic_cast<const CCDFrameInstance*>( &p );
   return (instPtr != nullptr) ? new CCDFrameInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool CCDFrameProcess::CanProcessCommandLines() const
{
   return false;
}

// ----------------------------------------------------------------------------

#if 0
static void ShowHelp()
{
   Console().Write(
"<raw>"
"Nothing to show."
"</raw>" );
}
#endif

int CCDFrameProcess::ProcessCommandLine( const StringList& argv ) const
{
   // ### TODO
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CCDFrameProcess.cpp - Released 2015/10/13 15:55:45 UTC
