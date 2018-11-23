//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard LocalHistogramEqualization Process Module Version 01.00.00.0226
// ----------------------------------------------------------------------------
// LocalHistogramEqualizationProcess.cpp - Released 2018-11-23T18:45:59Z
// ----------------------------------------------------------------------------
// This file is part of the standard LocalHistogramEqualization PixInsight module.
//
// Copyright (c) 2011-2018 Zbynek Vrastil
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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

#include "LocalHistogramEqualizationProcess.h"
#include "LocalHistogramEqualizationParameters.h"
#include "LocalHistogramEqualizationInstance.h"
#include "LocalHistogramEqualizationInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "LocalHistogramEqualizationIcon.xpm"

// ----------------------------------------------------------------------------

LocalHistogramEqualizationProcess* TheLocalHistogramEqualizationProcess = 0;

// ----------------------------------------------------------------------------

LocalHistogramEqualizationProcess::LocalHistogramEqualizationProcess() : MetaProcess()
{
   TheLocalHistogramEqualizationProcess = this;

   // Instantiate process parameters
   new LHERadius( this );
   new LHEHistogramBins( this );
   new LHESlopeLimit( this );
   new LHEAmount( this );
   new LHECircularKernel( this );
}

IsoString LocalHistogramEqualizationProcess::Id() const
{
   return "LocalHistogramEqualization";
}

IsoString LocalHistogramEqualizationProcess::Category() const
{
   return "IntensityTransformations";
}

String LocalHistogramEqualizationProcess::Description() const
{
   return
   "<html>"
   "<p>LocalHistogramEqualization is PixInsight's implementation of the CLAHE method (Contrast-Limited "
   "Adaptive Histogram Equalization). It is designed to increase local contrast in the image to "
   "improve visibility of structures. It has two main parameters: Kernel Radius, which defines a "
   "pixel neighborhood in which local contrast is evaluated; and Contrast Limit, which limits the "
   "contrast increase for bigger uniform areas (like the background) to prevent noise promotion.</p>"
   "</html>";
}

const char** LocalHistogramEqualizationProcess::IconImageXPM() const
{
   return LocalHistogramEqualizationIcon_XPM;
}

ProcessInterface* LocalHistogramEqualizationProcess::DefaultInterface() const
{
   return TheLocalHistogramEqualizationInterface;
}

ProcessImplementation* LocalHistogramEqualizationProcess::Create() const
{
   return new LocalHistogramEqualizationInstance( this );
}

ProcessImplementation* LocalHistogramEqualizationProcess::Clone( const ProcessImplementation& p ) const
{
   const LocalHistogramEqualizationInstance* instPtr = dynamic_cast<const LocalHistogramEqualizationInstance*>( &p );
   return (instPtr != 0) ? new LocalHistogramEqualizationInstance( *instPtr ) : 0;
}

bool LocalHistogramEqualizationProcess::CanProcessCommandLines() const
{
   return true;
}

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: LocalHistogramEqualization [<arg_list>] [<view_list>]"
"\n"
"\n-r=<n> | --radius=<n>"
"\n"
"\n      Sets the kernel radius parameter in pixels to <n> in the range [8,512]"
"\n      (default = 64 pixels)"
"\n"
"\n-l=<f> | --limit=<f>"
"\n"
"\n      Sets the contrast limit parameter to real <f> in the range [1,64]"
"\n      (default = 2)"
"\n"
"\n-a=<f> | --amount=<f>"
"\n"
"\n      Sets the amount parameter to real <f> in the range [0,1]"
"\n      (default = 1)"
"\n"
"\n-h=[8|10|12] | --hist=[8|10|12]"
"\n"
"\n      Sets the histogram resolution to 8, 10 or 12 bits, respectively."
"\n      (default = 8 bits)"
"\n"
"\n-c[+|-] | --circular[+|-]"
"\n"
"\n      Enables or disables the circular kernel. When disabled, a square"
"\n      kernel is used. (default = circular)"
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

int LocalHistogramEqualizationProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
      ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   LocalHistogramEqualizationInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if (arg.Id() == "r" || arg.Id() == "-radius") instance.radius = (int)arg.NumericValue();
         else if (arg.Id() == "l" || arg.Id() == "-limit") instance.slopeLimit = arg.NumericValue();
         else if (arg.Id() == "a" || arg.Id() == "-amount") instance.amount = arg.NumericValue();
         else if (arg.Id() == "h" || arg.Id() == "-hist")
         {
            int bins = (int)arg.NumericValue();
            if (bins == 8) instance.histogramBins = LHEHistogramBins::Bit8;
            else if (bins == 10) instance.histogramBins = LHEHistogramBins::Bit10;
            else if (bins == 12) instance.histogramBins = LHEHistogramBins::Bit12;
            else throw Error( "Invalid value for '-h' argument. Allowed values are 8, 10 or 12" );
         }
         else throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         if (arg.Id() == "c" || arg.Id() == "-circular") instance.circularKernel = arg.SwitchState();
         else throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if (arg.Id() == "c" || arg.Id() == "-circular") instance.circularKernel = true;
         // These are standard parameters that all processes should provide.
         else if ( arg.Id() == "-interface" )
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
// EOF LocalHistogramEqualizationProcess.cpp - Released 2018-11-23T18:45:59Z
