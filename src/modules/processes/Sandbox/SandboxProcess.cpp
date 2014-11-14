// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard Sandbox Process Module Version 01.00.02.0144
// ****************************************************************************
// SandboxProcess.cpp - Released 2014/11/14 17:19:24 UTC
// ****************************************************************************
// This file is part of the standard Sandbox PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include "SandboxInstance.h"
#include "SandboxInterface.h"
#include "SandboxParameters.h"
#include "SandboxProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

//#include "SandboxIcon.xpm"

// ----------------------------------------------------------------------------

SandboxProcess* TheSandboxProcess = 0;

// ----------------------------------------------------------------------------

SandboxProcess::SandboxProcess() : MetaProcess()
{
   TheSandboxProcess = this;

   // Instantiate process parameters
   new SandboxParameterOne( this );
   new SandboxParameterTwo( this );
   new SandboxParameterThree( this );
   new SandboxParameterFour( this );
   new SandboxParameterFive( this );
}

// ----------------------------------------------------------------------------

IsoString SandboxProcess::Id() const
{
   return "Sandbox";
}

// ----------------------------------------------------------------------------

IsoString SandboxProcess::Category() const
{
   return IsoString(); // No category
}

// ----------------------------------------------------------------------------

uint32 SandboxProcess::Version() const
{
   return 0x100; // required
}

// ----------------------------------------------------------------------------

String SandboxProcess::Description() const
{
   return
   "<html>"
   "<p>Sandbox is just a starting point for development of PixInsight modules. It is an empty module that "
   "does nothing but to provide the basic structure of a module with a process and a process interface.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** SandboxProcess::IconImageXPM() const
{
   return 0; // SandboxIcon_XPM; ---> put a nice icon here
}
// ----------------------------------------------------------------------------

ProcessInterface* SandboxProcess::DefaultInterface() const
{
   return TheSandboxInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* SandboxProcess::Create() const
{
   return new SandboxInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SandboxProcess::Clone( const ProcessImplementation& p ) const
{
   const SandboxInstance* instPtr = dynamic_cast<const SandboxInstance*>( &p );
   return (instPtr != 0) ? new SandboxInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool SandboxProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: Sandbox [<arg_list>] [<view_list>]"
"\n"
"\n--interface"
"\n"
"\n      Launches the interface of this process."
"\n"
"\n--help"
"\n"
"\n      Displays this help and exits."
"</raw>" );
}

int SandboxProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
      ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   SandboxInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         // These are standard parameters that all processes should provide.
         if ( arg.Id() == "-interface" )
            launchInterface = true;
         else if ( arg.Id() == "-help" )
         {
            ShowHelp();
            return 0;
         }
         else
            throw Error( "Unknown argument: " + arg.Token() );
      }
      else if ( arg.IsItemList() )
      {
         ++count;

         if ( arg.Items().IsEmpty() )
            throw Error( "No view(s) found: " + arg.Token() );

         for ( StringList::const_iterator j = arg.Items().Begin(); j != arg.Items().End(); ++j )
         {
            View v = View::ViewById( *j );
            if ( v.IsNull() )
               throw Error( "No such view: " + *j );
            instance.LaunchOn( v );
         }
      }
   }

   if ( launchInterface )
      instance.LaunchInterface();
   else if ( count == 0 )
   {
      if ( ImageWindow::ActiveWindow().IsNull() )
         throw Error( "There is no active image window." );
      instance.LaunchOnCurrentView();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF SandboxProcess.cpp - Released 2014/11/14 17:19:24 UTC
