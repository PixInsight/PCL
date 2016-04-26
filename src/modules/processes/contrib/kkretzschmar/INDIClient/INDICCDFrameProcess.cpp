//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.04.0108
// ----------------------------------------------------------------------------
// INDICCDFrameProcess.cpp - Released 2016/04/15 15:37:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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

#include "INDICCDFrameProcess.h"
#include "INDICCDFrameInterface.h"
#include "INDICCDFrameInstance.h"
#include "INDICCDFrameParameters.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

//#include "INDICCDFrameProcess.xpm" // ### TODO

// ----------------------------------------------------------------------------

INDICCDFrameProcess* TheINDICCDFrameProcess = nullptr;

// ----------------------------------------------------------------------------

INDICCDFrameProcess::INDICCDFrameProcess() : MetaProcess()
{
   TheINDICCDFrameProcess = this;

   new ICFDeviceName( this );
   new ICFUploadMode( this );
   new ICFServerUploadDirectory( this );
   new ICFServerFileNameTemplate( this );
   new ICFFrameType( this );
   new ICFBinningX( this );
   new ICFBinningY( this );
   new ICFExposureTime( this );
   new ICFExposureDelay( this );
   new ICFExposureCount( this );
   new ICFNewImageIdTemplate( this );
   new ICFReuseImageWindow( this );
   new ICFAutoStretch( this );
   new ICFLinkedAutoStretch( this );
}

// ----------------------------------------------------------------------------

IsoString INDICCDFrameProcess::Id() const
{
   return "INDICCDFrame";
}

// ----------------------------------------------------------------------------

IsoString INDICCDFrameProcess::Category() const
{
   return "INDI,Instrumentation";
}

// ----------------------------------------------------------------------------

uint32 INDICCDFrameProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String INDICCDFrameProcess::Description() const
{
   return
   "<html>"
   "<p>Acquire frames from INDI CCD devices.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** INDICCDFrameProcess::IconImageXPM() const
{
   return nullptr; // INDICCDFrameProcess_XPM; // ### TODO
}

// ----------------------------------------------------------------------------

bool INDICCDFrameProcess::PrefersGlobalExecution() const
{
	return true;
}
// ----------------------------------------------------------------------------

ProcessInterface* INDICCDFrameProcess::DefaultInterface() const
{
   return TheINDICCDFrameInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* INDICCDFrameProcess::Create() const
{
   return new INDICCDFrameInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* INDICCDFrameProcess::Clone( const ProcessImplementation& p ) const
{
   const INDICCDFrameInstance* r = dynamic_cast<const INDICCDFrameInstance*>( &p );
   return (r != nullptr) ? new INDICCDFrameInstance( *r ) : nullptr;
}

// ----------------------------------------------------------------------------

bool INDICCDFrameProcess::CanProcessCommandLines() const
{
   // ### TODO
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

int INDICCDFrameProcess::ProcessCommandLine( const StringList& argv ) const
{
   // ### TODO
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDICCDFrameProcess.cpp - Released 2016/04/15 15:37:39 UTC
