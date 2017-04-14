//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0256
// ----------------------------------------------------------------------------
// StarGeneratorProcess.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarGenerator PixInsight module.
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

#include "StarGeneratorProcess.h"
#include "StarGeneratorParameters.h"
#include "StarGeneratorInstance.h"
#include "StarGeneratorInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "StarGeneratorIcon.xpm"

// ----------------------------------------------------------------------------

StarGeneratorProcess* TheStarGeneratorProcess = 0;

// ----------------------------------------------------------------------------

StarGeneratorProcess::StarGeneratorProcess() : MetaProcess()
{
   TheStarGeneratorProcess = this;

   // Instantiate process parameters
   new SGStarDatabasePath( this );
   new SGCenterRA( this );
   new SGCenterDec( this );
   new SGEpoch( this );
   new SGProjectionSystem( this );
   new SGFocalLength( this );
   new SGPixelSize( this );
   new SGSensorWidth( this );
   new SGSensorHeight( this );
   new SGLimitMagnitude( this );
   new SGStarFWHM( this );
   new SGNonlinear( this );
   new SGTargetMinimumValue( this );
   new SGOutputMode( this );
   new SGOutputFilePath( this );
}

// ----------------------------------------------------------------------------

IsoString StarGeneratorProcess::Id() const
{
   return "StarGenerator";
}

// ----------------------------------------------------------------------------

IsoString StarGeneratorProcess::Category() const
{
   return "Render";
}

// ----------------------------------------------------------------------------

uint32 StarGeneratorProcess::Version() const
{
   return 0x100; // required
}

// ----------------------------------------------------------------------------

String StarGeneratorProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** StarGeneratorProcess::IconImageXPM() const
{
   return StarGeneratorIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* StarGeneratorProcess::DefaultInterface() const
{
   return TheStarGeneratorInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* StarGeneratorProcess::Create() const
{
   return new StarGeneratorInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* StarGeneratorProcess::Clone( const ProcessImplementation& p ) const
{
   const StarGeneratorInstance* instPtr = dynamic_cast<const StarGeneratorInstance*>( &p );
   return (instPtr != 0) ? new StarGeneratorInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool StarGeneratorProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: StarGenerator [<arg_list>] [<view_list>]"
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

int StarGeneratorProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::NoItems );

   StarGeneratorInstance instance( this );

   bool launchInterface = false;

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
   }

   if ( launchInterface )
      instance.LaunchInterface();
   else
      instance.LaunchGlobal();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF StarGeneratorProcess.cpp - Released 2017-04-14T23:07:12Z
