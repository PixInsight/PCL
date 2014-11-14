// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard IntensityTransformations Process Module Version 01.07.00.0287
// ****************************************************************************
// RescaleProcess.cpp - Released 2014/11/14 17:19:22 UTC
// ****************************************************************************
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "RescaleProcess.h"
#include "RescaleParameters.h"
#include "RescaleInstance.h"
#include "RescaleInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

RescaleProcess* TheRescaleProcess = 0;

// ----------------------------------------------------------------------------

#include "RescaleIcon.xpm"

// ----------------------------------------------------------------------------

RescaleProcess::RescaleProcess() : MetaProcess()
{
   TheRescaleProcess = this;

   // Instantiate process parameters
   new RescalingMode( this );
}

// ----------------------------------------------------------------------------

IsoString RescaleProcess::Id() const
{
   return "Rescale";
}

// ----------------------------------------------------------------------------

IsoString RescaleProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 RescaleProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String RescaleProcess::Description() const
{
   return

   "<html>"
   "<p>The Rescale process remaps pixel sample values linearly so that they cover "
   "the entire available data range, from pure black to pure white.</p>"

   "<p>The user may select whether this operation is to be performed on nominal"
   "channels (RGB/K) of the target image(s), or on their lightness (CIE L*) or relative "
   "luminance (CIE Y) components.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** RescaleProcess::IconImageXPM() const
{
   return RescaleIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* RescaleProcess::DefaultInterface() const
{
   return TheRescaleInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* RescaleProcess::Create() const
{
   return new RescaleInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* RescaleProcess::Clone( const ProcessImplementation& p ) const
{
   const RescaleInstance* instPtr = dynamic_cast<const RescaleInstance*>( &p );
   return (instPtr != 0) ? new RescaleInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool RescaleProcess::NeedsValidation() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool RescaleProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: Rescale [<arg_list>] [<view_list>]"
"\n"
"\n-rgb | -rgbk | -RGB | -RGBK"
"\n"
"\n      Applies rescaling to the red, green and blue channels of a RGB color"
"\n      image, or to the gray channel of a grayscale image. For color images,"
"\n      this mode preserves the existing color balance."
"\n"
"\n-i"
"\n"
"\n      Applies rescaling to the nominal channels separately, as if each channel"
"\n      were an independent image. In this mode the existing color balance is"
"\n      not preserved for color images."
"\n"
"\n-L | -CIEL"
"\n"
"\n      Applies rescaling to the CIE L* component (lightness) of a color image, "
"\n      or to the gray nominal channel of a grayscale image."
"\n"
"\n-Y | -CIEY"
"\n"
"\n      Applies rescaling to the CIE Y component (relative luminance) of a color "
"\n      image, or to the gray nominal channel of a grayscale image."
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

#define OUT_OF_RANGE \
   throw pcl::Error( "Numerical argument out of range: " + arg.Token() )

int RescaleProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   RescaleInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
         throw Error( "Unknown numeric argument: " + arg.Token() );
      else if ( arg.IsString() )
         throw Error( "Unknown string argument: " + arg.Token() );
      else if ( arg.IsSwitch() )
         throw Error( "Unknown switch argument: " + arg.Token() );
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "rgb" || arg.Id() == "rgbk" || arg.Id() == "RGB" || arg.Id() == "RGBK" )
            instance.mode = RescalingMode::RGBK;
         else if ( arg.Id() == "i" )
            instance.mode = RescalingMode::RGBK_Individual;
         else if ( arg.Id() == "L" || arg.Id() == "CIEL" )
            instance.mode = RescalingMode::CIEL;
         else if ( arg.Id() == "Y" || arg.Id() == "CIEY" )
            instance.mode = RescalingMode::CIEY;
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
            continue;
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

// ****************************************************************************
// EOF RescaleProcess.cpp - Released 2014/11/14 17:19:22 UTC
