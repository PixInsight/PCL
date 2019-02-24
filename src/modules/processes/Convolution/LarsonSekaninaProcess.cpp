//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0282
// ----------------------------------------------------------------------------
// LarsonSekaninaProcess.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "LarsonSekaninaProcess.h"
#include "LarsonSekaninaParameters.h"
#include "LarsonSekaninaInstance.h"
#include "LarsonSekaninaInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "LarsonSekaninaIcon.xpm"

// ----------------------------------------------------------------------------

LarsonSekaninaProcess* TheLarsonSekaninaProcess = 0;

// ----------------------------------------------------------------------------

LarsonSekaninaProcess::LarsonSekaninaProcess() : MetaProcess()
{
   TheLarsonSekaninaProcess = this;

   // Instantiate process parameters
   new LSInterpolation( this );
   new LSRadiusDiff( this );
   new LSAngleDiff( this );
   new LSCenterX( this );
   new LSCenterY( this );
   new LSAmount( this );
   new LSThreshold( this );
   new LSDeringing( this );
   new LSRangeLow( this );
   new LSRangeHigh( this );
   new LSUseLuminance( this );
   new LSHighPass( this );
   new LSDisableExtension( this );
}

// ----------------------------------------------------------------------------

IsoString LarsonSekaninaProcess::Id() const
{
   return "LarsonSekanina";
}

// ----------------------------------------------------------------------------

IsoString LarsonSekaninaProcess::Category() const
{
   return "Convolution";
}

// ----------------------------------------------------------------------------

uint32 LarsonSekaninaProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String LarsonSekaninaProcess::Description() const
{
   return

   "<html>"
   "<p>Implementation of the Larson-Sekanina rotational gradient. "
   "This process may be used either as a sharpening filter or as "
   "a pure image processing algorithm to obtain the raw gradient map.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** LarsonSekaninaProcess::IconImageXPM() const
{
   return LarsonSekaninaIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* LarsonSekaninaProcess::DefaultInterface() const
{
   return TheLarsonSekaninaInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* LarsonSekaninaProcess::Create() const
{
   return new LarsonSekaninaInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* LarsonSekaninaProcess::Clone( const ProcessImplementation& p ) const
{
   const LarsonSekaninaInstance* instPtr = dynamic_cast<const LarsonSekaninaInstance*>( &p );
   return (instPtr != 0) ? new LarsonSekaninaInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool LarsonSekaninaProcess::NeedsValidation() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool LarsonSekaninaProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: LarsonSekanina [<arg_list>] [<view_list>]"
"\nNote: This process is currently deprecated because it needs a complete"
"\n      refactoring/redesign."
"\n"
"\n-interpolation=<n>"
"\n"
"\n      <n> selects an interpolation algorithm, and can be one of:"
"\n"
"\n      0 Bilinear"
"\n      1 Bicubic spline"
"\n      2 Same as 1, for compatibility with previous versions."
"\n      3 Bicubic B-spline"
"\n"
"\n-r=<n> | -radius=<n> "
"\n"
"\n      <n> is the radial increment in pixels."
"\n"
"\n-thetha=<n>"
"\n"
"\n      <n> is the angular increment in degrees."
"\n"
"\n-x=<n> | -center-x=<n> "
"\n"
"\n      <n> is the x-coordinate of the center of polar coordinates."
"\n"
"\n-y=<n> | -center-y=<n> "
"\n"
"\n      <n> is the y-coordinate of the center of polar coordinates."
"\n"
"\n-a=<n> | -amount=<n>"
"\n"
"\n      <n> is the filtering strength in the [0,1] range. (default=0.8)"
"\n"
"\n-t=<n> | -threshold=<n>"
"\n"
"\n      <n> is a threshold level in the normalized [0,1] range. This threshold"
"\n      is used to protect low-contrast image features. (default=0)"
"\n"
"\n-dr=<n> | -deringing=<n>"
"\n"
"\n      <n> is the deringing protection value in the [0,1] range. The"
"\n      deringing feature reduces or suppresses the \'dark rings\' problem."
"\n      (default=0)"
"\n"
"\n-L[+|-] | -use-luminance[+|-]"
"\n"
"\n      Enables or disables luminance processing. When this option is enabled,"
"\n      the Larson-Sekaninaing filter is applied to the luminance of a color"
"\n      image exclusively. When this option is disabled, Larson-Sekanina is"
"\n      applied to all nominal channels of the target image. (default=enabled)."
"\n"
"\n-r0=<n> | low-range=<n>"
"\n"
"\n      <n> is the shadows dynamic range extension in the normalized [0,1]"
"\n      range. (default=0)"
"\n"
"\n-r1=<n> | high-range=<n>"
"\n"
"\n      <n> is the highlights dynamic range extension in the normalized [0,1]"
"\n      range. (default=0)"
"\n"
"\n-extension[+|-]"
"\n"
"\n      Enables or disables the dynamic range extension feature (only for"
"\n      floating point images). When this option is disabled, Larson-Sekanina"
"\n      returns the pixel values as calculated, without clipping or rescaling."
"\n      This is useful to calculate pure Laplacian filters. (default=enabled)"
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

int LarsonSekaninaProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   LarsonSekaninaInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "r" || arg.Id() == "radius" )
         {
            if ( arg.NumericValue() < TheLSRadiusDiffParameter->MinimumValue() || arg.NumericValue() > TheLSRadiusDiffParameter->MaximumValue() )
               OUT_OF_RANGE;
            instance.radiusDiff = arg.NumericValue();
         }
         else if ( arg.Id() == "theta" )
         {
            if ( arg.NumericValue() < TheLSAngleDiffParameter->MinimumValue() || arg.NumericValue() > TheLSAngleDiffParameter->MaximumValue() )
               OUT_OF_RANGE;
            instance.angleDiff = arg.NumericValue();
         }
         else if ( arg.Id() == "x" || arg.Id() == "center-x" )
         {
            instance.center.x = arg.NumericValue();
         }
         else if ( arg.Id() == "y" || arg.Id() == "center-y" )
         {
            instance.center.y = arg.NumericValue();
         }
         else if ( arg.Id() == "interpolation" )
         {
            if ( arg.NumericValue() < 0 || arg.NumericValue() > 3 )
               OUT_OF_RANGE;
            instance.interpolation = RoundI(arg.NumericValue());
         }
         else if ( arg.Id() == "a" || arg.Id() == "amount" )
         {
            if ( arg.NumericValue() < TheLSAmountParameter->MinimumValue() || arg.NumericValue() > TheLSAmountParameter->MaximumValue() )
               OUT_OF_RANGE;
            instance.amount = arg.NumericValue();
         }
         else if ( arg.Id() == "t" || arg.Id() == "threshold" )
         {
            if ( arg.NumericValue() < TheLSThresholdParameter->MinimumValue() || arg.NumericValue() > TheLSThresholdParameter->MaximumValue() )
               OUT_OF_RANGE;
            instance.threshold = arg.NumericValue();
         }
         else if ( arg.Id() == "dr" || arg.Id() == "deringing" )
         {
            if ( arg.NumericValue() < TheLSDeringingParameter->MinimumValue() || arg.NumericValue() > TheLSDeringingParameter->MaximumValue() )
               OUT_OF_RANGE;
            instance.deringing = arg.NumericValue();
         }
         else if ( arg.Id() == "r0" || arg.Id() == "low-range" )
         {
            if ( arg.NumericValue() < TheLSRangeLowParameter->MinimumValue() || arg.NumericValue() > TheLSRangeLowParameter->MaximumValue() )
               OUT_OF_RANGE;
            instance.rangeLow = arg.NumericValue();
         }
         else if ( arg.Id() == "r1" || arg.Id() == "high-range" )
         {
            if ( arg.NumericValue() < TheLSRangeHighParameter->MinimumValue() || arg.NumericValue() > TheLSRangeHighParameter->MaximumValue() )
               OUT_OF_RANGE;
            instance.rangeHigh = arg.NumericValue();
         }
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
         throw Error( "Unknown string argument: " + arg.Token() );
      else if ( arg.IsSwitch() )
      {
         if ( arg.Id() == "L" || arg.Id() == "use-luminance" )
            instance.useLuminance = arg.SwitchState();
         else if ( arg.Id() == "extension"  )
            instance.disableExtension = arg.SwitchState();
         else
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "L" || arg.Id() == "use-luminance" )
            instance.useLuminance = true;
         if ( arg.Id() == "extension" )
            instance.useLuminance = false;
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
// EOF LarsonSekaninaProcess.cpp - Released 2019-01-21T12:06:41Z
