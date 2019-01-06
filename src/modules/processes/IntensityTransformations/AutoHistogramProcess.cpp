//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0424
// ----------------------------------------------------------------------------
// AutoHistogramProcess.cpp - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "AutoHistogramProcess.h"
#include "AutoHistogramParameters.h"
#include "AutoHistogramInstance.h"
#include "AutoHistogramInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "AutoHistogramIcon.xpm"

// ----------------------------------------------------------------------------

AutoHistogramProcess* TheAutoHistogramProcess = nullptr;

// ----------------------------------------------------------------------------

AutoHistogramProcess::AutoHistogramProcess()
{
   TheAutoHistogramProcess = this;

   // Instantiate process parameters
   new AHClip( this );
   new AHClipTogether( this );
   new AHClipLowR( this );
   new AHClipLowG( this );
   new AHClipLowB( this );
   new AHClipHighR( this );
   new AHClipHighG( this );
   new AHClipHighB( this );
   new AHStretch( this );
   new AHStretchTogether( this );
   new AHStretchMethod( this );
   new AHTargetMedianR( this );
   new AHTargetMedianG( this );
   new AHTargetMedianB( this );
}

// ----------------------------------------------------------------------------

IsoString AutoHistogramProcess::Id() const
{
   return "AutoHistogram";
}

// ----------------------------------------------------------------------------

IsoString AutoHistogramProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 AutoHistogramProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

const char** AutoHistogramProcess::IconImageXPM() const
{
   return AutoHistogramIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* AutoHistogramProcess::DefaultInterface() const
{
   return TheAutoHistogramInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* AutoHistogramProcess::Create() const
{
   return new AutoHistogramInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* AutoHistogramProcess::Clone( const ProcessImplementation& p ) const
{
   const AutoHistogramInstance* instPtr = dynamic_cast<const AutoHistogramInstance*>( &p );
   return (instPtr != nullptr) ? new AutoHistogramInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool AutoHistogramProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: AutoHistogram [<arg_list>] [<view_list>]"
"\n"
"\nIn the following parameters, the optional <ch> suffix selects a channel to"
"\napply argument values. <ch> can be one of:"
"\n"
"\n   R|K : red/gray channel"
"\n   G   : green channel"
"\n   B   : blue channel"
"\n   L   : CIE L* component (lightness)"
"\n"
"\nIf no <ch> suffix is specified, values apply equally to the red(gray), green"
"\nand blue channels of the target views."
"\n"
"\n-c0[<ch>]=<n> | p_clipLow[<ch>]=<n>"
"\n"
"\n      <n> is the percentage of clipped pixels at the shadows. (default=0)"
"\n"
"\n-c1[<ch>]=<n> | p_clipHigh[<ch>]=<n>"
"\n"
"\n      <n> is the percentage of clipped pixels at the highlights. (default=0)"
"\n"
"\n-m[<ch>]=<n> | median[<ch>]=<n>"
"\n"
"\n      <n> is the target median pixel value, 0 <= n <= 1."
"\n"
"\n-clip[+|-]"
"\n"
"\n      Enables/disables the histogram clipping function. (default=disabled)."
"\n"
"\n-gamma[+|-]"
"\n"
"\n      Enables/disables the automatic gamma function (default=enabled)."
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

// ----------------------------------------------------------------------------

static double ArgumentPercentageValue( const Argument& arg )
{
   if ( arg.NumericValue() < 0 || arg.NumericValue() > 100 )
      throw Error( "Clipped pixels percentage out of range: " + arg.Token() );
   return arg.NumericValue();
}

// ----------------------------------------------------------------------------

static double ArgumentTargetMedianValue( const Argument& arg )
{
   if ( arg.NumericValue() < 0 || arg.NumericValue() > 1 )
      throw Error( "Target median value out of range: " + arg.Token() );
   return arg.NumericValue();
}

// ----------------------------------------------------------------------------

int AutoHistogramProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   AutoHistogramInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "c0" || arg.Id() == "p_clipLow" )
            instance.p_clipLow[0] = instance.p_clipLow[1] = instance.p_clipLow[2] = ArgumentPercentageValue( arg );
         else if ( arg.Id() == "c1" || arg.Id() == "p_clipHigh" )
            instance.p_clipHigh[0] = instance.p_clipHigh[1] = instance.p_clipHigh[2] = ArgumentPercentageValue( arg );
         else if ( arg.Id() == "m" || arg.Id() == "median" )
            instance.p_targetMedian[0] = instance.p_targetMedian[1] = instance.p_targetMedian[2] = ArgumentTargetMedianValue( arg );
         else if ( arg.Id() == "c0R" || arg.Id() == "clipLowR" || arg.Id() == "c0K" || arg.Id() == "clipLowK" )
            instance.p_clipLow[0] = ArgumentPercentageValue( arg );
         else if ( arg.Id() == "c1R" || arg.Id() == "clipHighR" )
            instance.p_clipHigh[0] = ArgumentPercentageValue( arg );
         else if ( arg.Id() == "mR" || arg.Id() == "medianR" )
            instance.p_targetMedian[0] = ArgumentTargetMedianValue( arg );
         else if ( arg.Id() == "c0G" || arg.Id() == "clipLowG" )
            instance.p_clipLow[1] = ArgumentPercentageValue( arg );
         else if ( arg.Id() == "c1G" || arg.Id() == "clipHighG" )
            instance.p_clipHigh[1] = ArgumentPercentageValue( arg );
         else if ( arg.Id() == "mG" || arg.Id() == "medianG" )
            instance.p_targetMedian[1] = ArgumentTargetMedianValue( arg );
         else if ( arg.Id() == "c0B" || arg.Id() == "clipLowB" )
            instance.p_clipLow[2] = ArgumentPercentageValue( arg );
         else if ( arg.Id() == "c1B" || arg.Id() == "clipHighB" )
            instance.p_clipHigh[2] = ArgumentPercentageValue( arg );
         else if ( arg.Id() == "mB" || arg.Id() == "medianB" )
            instance.p_targetMedian[2] = ArgumentTargetMedianValue( arg );
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
         throw Error( "Unknown string argument: " + arg.Token() );
      else if ( arg.IsSwitch() )
      {
         if ( arg.Id() == "clip" )
            instance.p_clip = arg.SwitchState();
         else if ( arg.Id() == "gamma" )
            instance.p_stretch = arg.SwitchState();
         else
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "clip" )
            instance.p_clip = true;
         else if ( arg.Id() == "gamma" )
            instance.p_stretch = true;
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

// ----------------------------------------------------------------------------
// EOF AutoHistogramProcess.cpp - Released 2018-12-12T09:25:25Z
