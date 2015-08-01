//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.00.0306
// ----------------------------------------------------------------------------
// ScreenTransferFunctionProcess.cpp - Released 2015/07/31 11:49:48 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "ScreenTransferFunctionProcess.h"
#include "ScreenTransferFunctionParameters.h"
#include "ScreenTransferFunctionInstance.h"
#include "ScreenTransferFunctionInterface.h"

#include <pcl/Arguments.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ScreenTransferFunctionProcess* TheScreenTransferFunctionProcess = 0;

// ----------------------------------------------------------------------------

#include "ScreenTransferFunctionIcon.xpm"

// ----------------------------------------------------------------------------

ScreenTransferFunctionProcess::ScreenTransferFunctionProcess() : MetaProcess()
{
   TheScreenTransferFunctionProcess = this;

   // Instantiate process parameters
   new STFSet( this );
   new STFShadowsClipping( TheSTFSetParameter );
   new STFHighlightsClipping( TheSTFSetParameter );
   new STFMidtonesBalance( TheSTFSetParameter );
   new STFLowRange( TheSTFSetParameter );
   new STFHighRange( TheSTFSetParameter );
   new STFInteraction( this );
}

// ----------------------------------------------------------------------------

IsoString ScreenTransferFunctionProcess::Id() const
{
   return "ScreenTransferFunction";
}

// ----------------------------------------------------------------------------

IsoString ScreenTransferFunctionProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 ScreenTransferFunctionProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ScreenTransferFunctionProcess::Description() const
{
   return

   "<html>"
   "<p>A Screen Transfer Function (STF) is used in PixInsight to improve the visibility "
   "of an image <i>as it is represented on the screen</i>, but <i>without altering its actual "
   "pixel values</i> in any way. This is extremely useful to work with processes applied to "
   "linear, <i>unstretched</i> data. Such processes may include deconvolution, wavelet transforms, "
   "color calibration and some noise reduction procedures, among many others.</p>"

   "<p>Unstretched raw images are often poorly represented on the screen. This is because most of "
   "their data are concentrated in small portions of the available dynamic range. This can be "
   "easily identified as a narrow peak near the left side of the histogram.</p>"

   "<p>A STF is a histogram transform that PixInsight applies to the bitmap generated to render "
   "an image on the screen. STF parameters include per-channel midtones balance, shadows and "
   "highlights clipping values, just as any histogram transform does. STF includes also dynamic "
   "range expansion parameters, but these are only available through the command-line interface "
   "of the ScreenTransferFunction process, not on its GUI interface.</p>"

   "<p>In PixInsight, each view can have its own STF, which can be adjusted independently for the "
   "red/gray, green, blue and lightness/luminance channels. This allows for extremely precise and "
   "fine-tuned adaptations of screen renditions to raw data.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** ScreenTransferFunctionProcess::IconImageXPM() const
{
   return ScreenTransferFunctionIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* ScreenTransferFunctionProcess::DefaultInterface() const
{
   return TheScreenTransferFunctionInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ScreenTransferFunctionProcess::Create() const
{
   return new ScreenTransferFunctionInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ScreenTransferFunctionProcess::Clone( const ProcessImplementation& p ) const
{
   const ScreenTransferFunctionInstance* instPtr = dynamic_cast<const ScreenTransferFunctionInstance*>( &p );
   return (instPtr != 0) ? new ScreenTransferFunctionInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool ScreenTransferFunctionProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: ScreenTransferFunction [<arg_list>] [<view_list>]"
"\n"
"\nIn the following parameters, the optional <ch> suffix selects a channel to"
"\napply argument values. <ch> can be one of:"
"\n"
"\n   R|K : red/gray channel"
"\n   G   : green channel"
"\n   B   : blue channel"
"\n   L   : lightness (CIE L*) and relative luminance (CIE Y)"
"\n"
"\nIf no <ch> suffix is specified, values apply equally to the red(gray), green"
"\nand blue channels of the target views."
"\n"
"\n-m[<ch>]=<n>"
"\n"
"\n      Set midtones balance to n, 0 <= n <= 1. (default=0.5)"
"\n"
"\n-c0[<ch>]=<n>"
"\n"
"\n      Set shadows clipping point to n, 0 <= n <= h, where h is the current"
"\n      highlights clipping point. (default=0)"
"\n"
"\n-c1[<ch>]=<n>"
"\n"
"\n      Set highlights clipping point to n, s <= n <= 1, where s is the current"
"\n      shadows clipping point. (default=1)"
"\n"
"\n-r0[<ch>]=<n>"
"\n"
"\n      Set shadows range expansion to n, n <= 0. (default=0)"
"\n"
"\n-r1[<ch>]=<n>   Highlights range expansion."
"\n"
"\n      Set highlights range expansion to n, 1 <= n. (default=1)"
"\n"
"\n-r | -reset"
"\n"
"\n      Reset the screen transfer functions of the target images. This is"
"\n      equivalent to: -m=0.5 -c0=0 -c1=1 -r0=0 -r1=1. Note that these are"
"\n      also the default values of these parameters."
"\n"
"\n-a[=<view_id>] | -acquire[=<view_id>]"
"\n"
"\n      Acquires the parameters of the current screen transfer functions in"
"\n      the specified view. If no <view_id> is specified, the parameters of"
"\n      the active view are acquired. This argument is useful to copy STF"
"\n      parameters from one view to a set of views."
"\n"
"\n--interface"
"\n"
"\n      Launches the interface of this process."
"\n"
"\n--help"
"\n"
"\n      Displays this help and exits."
"\n"
"\nExamples:"
"\n"
"\n   -c0R=.001"
"\n"
"\n   Applies a 0.001 shadows clipping value to the red channels of color"
"\n   images, or to the gray channel of grayscale images."
"\n"
"\n   -m=0.25 -c0G=0.02 -c1B=0.98"
"\n"
"\n   Applies a 0.25 midtones balance to red, green and blue channels of color"
"\n   images, and to the gray channel of grayscale images. Applies a 0.02"
"\n   shadows clipping point and a 0.98 highlights clipping point to the green"
"\n   and blue channels of color images, respectively."
"</raw>" );
}

static double ArgumentClippingPointValue( const Argument& arg )
{
   if ( arg.NumericValue() < 0 || arg.NumericValue() > 1 )
      throw ParseError( "Clipping point out of range: " + arg.Token() );
   return arg.NumericValue();
}

static double ArgumentMidtonesBalanceValue( const Argument& arg )
{
   if ( arg.NumericValue() < 0 || arg.NumericValue() > 1 )
      throw ParseError( "Midtones balance out of range: " + arg.Token() );
   return arg.NumericValue();
}

static double ArgumentShadowsRangeExpansionValue( const Argument& arg )
{
   if ( arg.NumericValue() > 0 )
      throw ParseError( "Shadows range expansion out of range: " + arg.Token() );
   return arg.NumericValue();
}

static double ArgumentHighlightsRangeExpansionValue( const Argument& arg )
{
   if ( arg.NumericValue() < 1 )
      throw ParseError( "Highlights range expansion out of range: " + arg.Token() );
   return arg.NumericValue();
}

int ScreenTransferFunctionProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
      ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   ScreenTransferFunctionInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         //
         // RGB channels
         //
         if ( arg.Id() == "c0" )
            instance[0].c0 = instance[1].c0 = instance[2].c0 = ArgumentClippingPointValue( arg );
         else if ( arg.Id() == "m" )
            instance[0].m = instance[1].m = instance[2].m = ArgumentMidtonesBalanceValue( arg );
         else if ( arg.Id() == "c1" )
            instance[0].c1 = instance[1].c1 = instance[2].c1 = ArgumentClippingPointValue( arg );
         else if ( arg.Id() == "r0" )
            instance[0].r0 = instance[1].r0 = instance[2].r0 = ArgumentShadowsRangeExpansionValue( arg );
         else if ( arg.Id() == "r1" )
            instance[0].r1 = instance[1].r1 = instance[2].r1 = ArgumentHighlightsRangeExpansionValue( arg );
         //
         // Luminance
         //
         else if ( arg.Id() == "c0L" )
            instance[3].c0 = ArgumentClippingPointValue( arg );
         else if ( arg.Id() == "mL" )
            instance[3].m = ArgumentMidtonesBalanceValue( arg );
         else if ( arg.Id() == "c1L" )
            instance[3].c1 = ArgumentClippingPointValue( arg );
         else if ( arg.Id() == "r0L" )
            instance[3].r0 = ArgumentShadowsRangeExpansionValue( arg );
         else if ( arg.Id() == "r1L" )
            instance[3].r1 = ArgumentHighlightsRangeExpansionValue( arg );
         //
         // Red/Gray channel
         //
         else if ( arg.Id() == "c0R" )
            instance[0].c0 = ArgumentClippingPointValue( arg );
         else if ( arg.Id() == "mR" )
            instance[0].m = ArgumentMidtonesBalanceValue( arg );
         else if ( arg.Id() == "c1R" )
            instance[0].c1 = ArgumentClippingPointValue( arg );
         else if ( arg.Id() == "r0R" )
            instance[0].r0 = ArgumentShadowsRangeExpansionValue( arg );
         else if ( arg.Id() == "r1R" )
            instance[0].r1 = ArgumentHighlightsRangeExpansionValue( arg );
         //
         // Green channel
         //
         else if ( arg.Id() == "c0G" )
            instance[1].c0 = ArgumentClippingPointValue( arg );
         else if ( arg.Id() == "mG" )
            instance[1].m = ArgumentMidtonesBalanceValue( arg );
         else if ( arg.Id() == "c1G" )
            instance[1].c1 = ArgumentClippingPointValue( arg );
         else if ( arg.Id() == "r0G" )
            instance[1].r0 = ArgumentShadowsRangeExpansionValue( arg );
         else if ( arg.Id() == "r1G" )
            instance[1].r1 = ArgumentHighlightsRangeExpansionValue( arg );
         //
         // Blue channel
         //
         else if ( arg.Id() == "c0B" )
            instance[2].c0 = ArgumentClippingPointValue( arg );
         else if ( arg.Id() == "mB" )
            instance[2].m = ArgumentMidtonesBalanceValue( arg );
         else if ( arg.Id() == "c1B" )
            instance[2].c1 = ArgumentClippingPointValue( arg );
         else if ( arg.Id() == "r0B" )
            instance[2].r0 = ArgumentShadowsRangeExpansionValue( arg );
         else if ( arg.Id() == "r1B" )
            instance[2].r1 = ArgumentHighlightsRangeExpansionValue( arg );
         //
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "a" || arg.Id() == "acquire" )
         {
            View v = View::ViewById( arg.StringValue() );
            if ( v.IsNull() )
               throw Error( "No such view: " + arg.StringValue() );
            instance.GetViewSTF( v );
         }
         else
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
         throw Error( "Unknown switch argument: " + arg.Token() );
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "a" || arg.Id() == "acquire" )
         {
            ImageWindow w = ImageWindow::ActiveWindow();
            if ( w.IsNull() )
               throw Error( "No active image window to acquire STF." );
            instance.GetViewSTF( w.CurrentView() );
         }
         else if ( arg.Id() == "r" || arg.Id() == "reset" )
            instance.Assign( ScreenTransferFunctionInstance( this ) );
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

// ----------------------------------------------------------------------------
// EOF ScreenTransferFunctionProcess.cpp - Released 2015/07/31 11:49:48 UTC
