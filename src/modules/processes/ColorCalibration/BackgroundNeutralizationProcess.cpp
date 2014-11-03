// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard ColorCalibration Process Module Version 01.02.00.0168
// ****************************************************************************
// BackgroundNeutralizationProcess.cpp - Released 2014/10/29 07:34:54 UTC
// ****************************************************************************
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "BackgroundNeutralizationProcess.h"
#include "BackgroundNeutralizationParameters.h"
#include "BackgroundNeutralizationInstance.h"
#include "BackgroundNeutralizationInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "BackgroundNeutralizationIcon.xpm"

// ----------------------------------------------------------------------------

BackgroundNeutralizationProcess* TheBackgroundNeutralizationProcess = 0;

// ----------------------------------------------------------------------------

BackgroundNeutralizationProcess::BackgroundNeutralizationProcess() : MetaProcess()
{
   TheBackgroundNeutralizationProcess = this;

   // Instantiate process parameters
   new BNBackgroundReferenceViewId( this );
   new BNBackgroundLow( this );
   new BNBackgroundHigh( this );
   new BNUseROI( this );
   new BNROIX0( this );
   new BNROIY0( this );
   new BNROIX1( this );
   new BNROIY1( this );
   new BNMode( this );
   new BNTargetBackground( this );
}

// ----------------------------------------------------------------------------

IsoString BackgroundNeutralizationProcess::Id() const
{
   return "BackgroundNeutralization";
}

// ----------------------------------------------------------------------------

IsoString BackgroundNeutralizationProcess::Category() const
{
   return "ColorCalibration";
}

// ----------------------------------------------------------------------------

uint32 BackgroundNeutralizationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String BackgroundNeutralizationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** BackgroundNeutralizationProcess::IconImageXPM() const
{
   return BackgroundNeutralizationIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* BackgroundNeutralizationProcess::DefaultInterface() const
{
   return TheBackgroundNeutralizationInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* BackgroundNeutralizationProcess::Create() const
{
   return new BackgroundNeutralizationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* BackgroundNeutralizationProcess::Clone( const ProcessImplementation& p ) const
{
   const BackgroundNeutralizationInstance* instPtr = dynamic_cast<const BackgroundNeutralizationInstance*>( &p );
   return (instPtr != 0) ? new BackgroundNeutralizationInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool BackgroundNeutralizationProcess::NeedsValidation() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool BackgroundNeutralizationProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: BackgroundNeutralization [<arg_list>] [<view_list>]"
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

int BackgroundNeutralizationProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   BackgroundNeutralizationInstance instance( this );

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
         {
            Console().WriteLn( "No view(s) found: " + arg.Token() );
            throw;
         }

         for ( StringList::const_iterator j = arg.Items().Begin(); j != arg.Items().End(); ++j )
         {
            View v = View::ViewById( *j );
            if ( v.IsNull() )
               throw Error( "Invalid view identifier: " + *j );

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
// EOF BackgroundNeutralizationProcess.cpp - Released 2014/10/29 07:34:54 UTC
