//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0203
// ----------------------------------------------------------------------------
// INDIDeviceControllerProcess.cpp - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2017 Klaus Kretzschmar
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

#include "INDIDeviceControllerInstance.h"
#include "INDIDeviceControllerInterface.h"
#include "INDIDeviceControllerParameters.h"
#include "INDIDeviceControllerProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

//#include "PixInsightINDIIcon.xpm" // ### TODO

// ----------------------------------------------------------------------------

INDIDeviceControllerProcess* TheINDIDeviceControllerProcess = nullptr;

// ----------------------------------------------------------------------------

INDIDeviceControllerProcess::INDIDeviceControllerProcess() : MetaProcess()
{
   TheINDIDeviceControllerProcess = this;

   new IDCServerHostName( TheINDIDeviceControllerProcess );
   new IDCServerPort( TheINDIDeviceControllerProcess );
   new IDCServerConnect( TheINDIDeviceControllerProcess );
   new IDCServerCommand( TheINDIDeviceControllerProcess );
   new IDCAbort( TheINDIDeviceControllerProcess );
   new IDCVerbosity( TheINDIDeviceControllerProcess );

   new IDCNewProperties( TheINDIDeviceControllerProcess );
   new IDCNewPropertyKey( TheIDCNewPropertiesParameter );
   new IDCNewPropertyType( TheIDCNewPropertiesParameter );
   new IDCNewPropertyValue( TheIDCNewPropertiesParameter );

   new IDCProperties( TheINDIDeviceControllerProcess );
   new IDCPropertyName( TheIDCPropertiesParameter );
   new IDCPropertyValue( TheIDCPropertiesParameter );
   new IDCPropertyState( TheIDCPropertiesParameter );
   new IDCPropertyType( TheIDCPropertiesParameter );

   new IDCDevices( TheINDIDeviceControllerProcess );
   new IDCDeviceName( TheIDCDevicesParameter );
   new IDCDeviceLabel( TheIDCDevicesParameter );

   new IDCGetCommandResult( TheINDIDeviceControllerProcess );
   new IDCGetCommandParameters( TheINDIDeviceControllerProcess );
}

// ----------------------------------------------------------------------------

IsoString INDIDeviceControllerProcess::Id() const
{
   return "INDIDeviceController";
}

// ----------------------------------------------------------------------------

IsoString INDIDeviceControllerProcess::Category() const
{
   return "INDI,Instrumentation";
}

// ----------------------------------------------------------------------------

uint32 INDIDeviceControllerProcess::Version() const
{
   return 0x100; // required
}

// ----------------------------------------------------------------------------

String INDIDeviceControllerProcess::Description() const
{
   return
   "<html>"
   "<p>An INDI client process for controlling INDI devices.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** INDIDeviceControllerProcess::IconImageXPM() const
{
   return nullptr; // PixInsightINDIIcon_XPM; // ### TODO
}

// ----------------------------------------------------------------------------

bool INDIDeviceControllerProcess::PrefersGlobalExecution() const
{
   return true;
}
// ----------------------------------------------------------------------------

ProcessInterface* INDIDeviceControllerProcess::DefaultInterface() const
{
   return TheINDIDeviceControllerInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* INDIDeviceControllerProcess::Create() const
{
   return new INDIDeviceControllerInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* INDIDeviceControllerProcess::Clone( const ProcessImplementation& p ) const
{
   const INDIDeviceControllerInstance* instPtr = dynamic_cast<const INDIDeviceControllerInstance*>( &p );
   return (instPtr != nullptr) ? new INDIDeviceControllerInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool INDIDeviceControllerProcess::CanProcessCommandLines() const
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

int INDIDeviceControllerProcess::ProcessCommandLine( const StringList& argv ) const
{
   // ### TODO
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerProcess.cpp - Released 2017-05-02T09:43:01Z
