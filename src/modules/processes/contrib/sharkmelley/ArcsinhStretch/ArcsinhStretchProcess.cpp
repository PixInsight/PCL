//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 01.00.00.0136
// ----------------------------------------------------------------------------
// ArcsinhStretchProcess.cpp - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017-2018 Mark Shelley
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

#include "ArcsinhStretchInstance.h"
#include "ArcsinhStretchInterface.h"
#include "ArcsinhStretchParameters.h"
#include "ArcsinhStretchProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

//#include "ArcsinhStretchIcon.xpm"

// ----------------------------------------------------------------------------

ArcsinhStretchProcess* TheArcsinhStretchProcess = nullptr;

// ----------------------------------------------------------------------------

ArcsinhStretchProcess::ArcsinhStretchProcess() : MetaProcess()
{
   TheArcsinhStretchProcess = this;

   // Instantiate process parameters
   new ArcsinhStretch( this );
   new ArcsinhStretchBlackPoint( this );
   new ArcsinhStretchProtectHighlights( this );
   new ArcsinhStretchUseRgbws( this );
   new ArcsinhStretchPreviewClipped( this );
}

// ----------------------------------------------------------------------------

IsoString ArcsinhStretchProcess::Id() const
{
   return "ArcsinhStretch";
}

// ----------------------------------------------------------------------------

IsoString ArcsinhStretchProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 ArcsinhStretchProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ArcsinhStretchProcess::Description() const
{
   return
   "<html>"
   "<p>ArcsinhStretch. Apply hyperbolic arcsine stretch to the image intensity while preserving the original color.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** ArcsinhStretchProcess::IconImageXPM() const
{
   return nullptr; // ArcsinhStretchIcon_XPM; ---> put a nice icon here
}
// ----------------------------------------------------------------------------

ProcessInterface* ArcsinhStretchProcess::DefaultInterface() const
{
   return TheArcsinhStretchInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* ArcsinhStretchProcess::Create() const
{
   return new ArcsinhStretchInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ArcsinhStretchProcess::Clone( const ProcessImplementation& p ) const
{
   const ArcsinhStretchInstance* instPtr = dynamic_cast<const ArcsinhStretchInstance*>( &p );
   return (instPtr != nullptr) ? new ArcsinhStretchInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool ArcsinhStretchProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: ArcsinhStretch [<arg_list>] [<view_list>]"
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

int ArcsinhStretchProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
      ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   ArcsinhStretchInstance instance( this );

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

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchProcess.cpp - Released 2018-12-12T09:25:25Z
