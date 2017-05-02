//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.07.0141
// ----------------------------------------------------------------------------
// INDIMountProcess.cpp - Released 2016/04/28 15:13:36 UTC
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

#include "INDIMountProcess.h"
#include "INDIMountInterface.h"
#include "INDIMountInstance.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

//#include "PixInsightINDIIcon.xpm"

// ----------------------------------------------------------------------------

INDIMountProcess* TheINDIMountProcess = 0;

// ----------------------------------------------------------------------------

INDIMountProcess::INDIMountProcess() : MetaProcess()
{
   TheINDIMountProcess = this;
}

// ----------------------------------------------------------------------------

IsoString INDIMountProcess::Id() const
{
   return "INDIMount";
}

// ----------------------------------------------------------------------------

IsoString INDIMountProcess::Category() const
{
   return IsoString("INDI"); // No category
}

// ----------------------------------------------------------------------------

uint32 INDIMountProcess::Version() const
{
   return 0x100; // required
}

// ----------------------------------------------------------------------------

String INDIMountProcess::Description() const
{
   return
   "<html>"
   "<p>Control INDI Mount devices./p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** INDIMountProcess::IconImageXPM() const
{
   return 0; // PixInsightINDIIcon_XPM; ---> put a nice icon here
}

// ----------------------------------------------------------------------------

bool INDIMountProcess::PrefersGlobalExecution() const
{
	return true;
}
// ----------------------------------------------------------------------------

ProcessInterface* INDIMountProcess::DefaultInterface() const
{
   return TheINDIMountInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* INDIMountProcess::Create() const
{
   return new INDIDeviceControllerInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* INDIMountProcess::Clone( const ProcessImplementation& p ) const
{
   const INDIMountInstance* instPtr = dynamic_cast<const INDIMountInstance*>( &p );
   return (instPtr != 0) ? new INDIMountInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool INDIMountProcess::CanProcessCommandLines() const
{
   return false;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Nothing to show."
"</raw>" );
}

int INDIMountProcess::ProcessCommandLine( const StringList& argv ) const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIMountProcess.cpp - Released 2016/04/28 15:13:36 UTC
