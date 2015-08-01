//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.02.00.0189
// ----------------------------------------------------------------------------
// LinearFitProcess.cpp - Released 2015/07/31 11:49:48 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "LinearFitProcess.h"
#include "LinearFitParameters.h"
#include "LinearFitInstance.h"
#include "LinearFitInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "LinearFitIcon.xpm"

// ----------------------------------------------------------------------------

LinearFitProcess* TheLinearFitProcess = 0;

// ----------------------------------------------------------------------------

LinearFitProcess::LinearFitProcess() : MetaProcess()
{
   TheLinearFitProcess = this;

   // Instantiate process parameters
   new LFReferenceViewId( this );
   new LFRejectLow( this );
   new LFRejectHigh( this );
}

// ----------------------------------------------------------------------------

IsoString LinearFitProcess::Id() const
{
   return "LinearFit";
}

// ----------------------------------------------------------------------------

IsoString LinearFitProcess::Category() const
{
   return "ColorCalibration";
}

// ----------------------------------------------------------------------------

uint32 LinearFitProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String LinearFitProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** LinearFitProcess::IconImageXPM() const
{
   return LinearFitIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* LinearFitProcess::DefaultInterface() const
{
   return TheLinearFitInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* LinearFitProcess::Create() const
{
   return new LinearFitInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* LinearFitProcess::Clone( const ProcessImplementation& p ) const
{
   const LinearFitInstance* instPtr = dynamic_cast<const LinearFitInstance*>( &p );
   return (instPtr != 0) ? new LinearFitInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool LinearFitProcess::NeedsValidation() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool LinearFitProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: LinearFit [<arg_list>] [<view_list>]"
"\n"
"\n-v=<view_id> | -reference-view=<view_id>"
"\n"
"\n      Specifies the identifier of an existing view that will be used as the"
"\n      reference image. Can be either a main view or a complete preview"
"\n      (partial previews are not allowed). This parameter is mandatory."
"\n"
"\n-r0=<n> | -reject-low=<n>"
"\n"
"\n      <n> is a low rejection limit in the normalized [0,1] range."
"\n      (default=0)"
"\n"
"\n-r1=<n> | -reject-high=<n>"
"\n"
"\n      <n> is a high rejection limit in the normalized [0,1] range."
"\n      (default=0.92)"
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

#define CHECK_IN_NORM_RANGE \
   if ( arg.NumericValue() < 0 || arg.NumericValue() > 1 ) \
      throw Error( "Argument not in the normalized [0,1] range: " + arg.Token() )

int LinearFitProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   LinearFitInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "r0" || arg.Id() == "reject-low" )
         {
            CHECK_IN_NORM_RANGE;
            instance.rejectLow = arg.NumericValue();
         }
         else if ( arg.Id() == "r1" || arg.Id() == "reject-high" )
         {
            CHECK_IN_NORM_RANGE;
            instance.rejectHigh = arg.NumericValue();
         }
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "v" || arg.Id() == "reference-view" )
            instance.referenceViewId = arg.StringValue();
         else
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
               throw Error( "No such view: " + *j );
            if ( v.FullId() != IsoString( instance.referenceViewId ).Trimmed() )
               instance.LaunchOn( v );
            else
               Console().WarningLn( "<end><cbr>** Skipping reference view: " + v.FullId() );
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
// EOF LinearFitProcess.cpp - Released 2015/07/31 11:49:48 UTC
