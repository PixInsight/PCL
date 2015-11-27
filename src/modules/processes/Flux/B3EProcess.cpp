//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 01.00.00.0123
// ----------------------------------------------------------------------------
// B3EProcess.cpp - Released 2015/11/26 16:00:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
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

#include "B3EProcess.h"
#include "B3EParameters.h"
#include "B3EInstance.h"
#include "B3EInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "B3EIcon.xpm"

// ----------------------------------------------------------------------------

B3EProcess* TheB3EProcess = 0;

// ----------------------------------------------------------------------------

B3EProcess::B3EProcess() : MetaProcess()
{
   TheB3EProcess = this;

   new B3EInputViewId1( this );
   new B3EInputViewId2( this );
   new B3EInputCenter1( this );
   new B3EInputCenter2( this );
   new B3EOutputCenter( this );
   new B3EIntensityUnits( this );
   new B3ESyntheticImage( this );
   new B3EThermalMap( this );
   new B3EOutOfRangeMask( this );
   new B3ESyntheticImageViewId( this );
   new B3EThermalMapViewId( this );
   new B3EOutOfRangeMaskViewId( this );

   // Background Calibration 1
   new B3ESubstractBackground1( this );
   new B3EBackgroundReferenceViewId1( this );
   new B3EBackgroundLow1( this );
   new B3EBackgroundHigh1( this );
   new B3EBackgroundUseROI1( this );
   new B3EBackgroundROIX01( this );
   new B3EBackgroundROIY01( this );
   new B3EBackgroundROIX11( this );
   new B3EBackgroundROIY11( this );
   new B3EOutputBackgroundReferenceMask1( this );

   // Background Calibration 2
   new B3ESubstractBackground2( this );
   new B3EBackgroundReferenceViewId2( this );
   new B3EBackgroundLow2( this );
   new B3EBackgroundHigh2( this );
   new B3EBackgroundUseROI2( this );
   new B3EBackgroundROIX02( this );
   new B3EBackgroundROIY02( this );
   new B3EBackgroundROIX12( this );
   new B3EBackgroundROIY12( this );
   new B3EOutputBackgroundReferenceMask2( this );
}

// ----------------------------------------------------------------------------

IsoString B3EProcess::Id() const
{
   return "B3Estimator";
}

// ----------------------------------------------------------------------------

IsoString B3EProcess::Category() const
{
   return "Flux";
}

// ----------------------------------------------------------------------------

uint32 B3EProcess::Version() const
{
   return 0x100; // required
}

// ----------------------------------------------------------------------------

String B3EProcess::Description() const
{
   return

   "";
}

// ----------------------------------------------------------------------------

const char** B3EProcess::IconImageXPM() const
{
   return B3EIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* B3EProcess::DefaultInterface() const
{
   return TheB3EInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* B3EProcess::Create() const
{
   return new B3EInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* B3EProcess::Clone( const ProcessImplementation& p ) const
{
   const B3EInstance* instPtr = dynamic_cast<const B3EInstance*>( &p );
   return (instPtr != 0) ? new B3EInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool B3EProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool B3EProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: B3Estimator [<arg_list>] [<view_list>]"
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

int B3EProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
      ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   B3EInstance instance( this );

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
         {
            Console().WriteLn( "No view(s) found: " + arg.Token() );
            throw;
         }

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

// ----------------------------------------------------------------------------
// EOF B3EProcess.cpp - Released 2015/11/26 16:00:12 UTC
