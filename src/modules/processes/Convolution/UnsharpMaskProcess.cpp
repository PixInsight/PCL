//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0226
// ----------------------------------------------------------------------------
// UnsharpMaskProcess.cpp - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
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

#include "UnsharpMaskProcess.h"
#include "UnsharpMaskParameters.h"
#include "UnsharpMaskInstance.h"
#include "UnsharpMaskInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "UnsharpMaskIcon.xpm"

// ----------------------------------------------------------------------------

UnsharpMaskProcess* TheUnsharpMaskProcess = 0;

// ----------------------------------------------------------------------------

UnsharpMaskProcess::UnsharpMaskProcess() : MetaProcess()
{
   TheUnsharpMaskProcess = this;

   // Instantiate process parameters
   new USMSigma( this );
   new USMAmount( this );
   new USMUseLuminance( this );
   new USMLinear( this );
   new USMDeringing( this );
   new USMDeringingDark( this );
   new USMDeringingBright( this );
   new USMOutputDeringingMaps( this );
   new USMRangeLow( this );
   new USMRangeHigh( this );
}

// ----------------------------------------------------------------------------

IsoString UnsharpMaskProcess::Id() const
{
   return "UnsharpMask";
}

// ----------------------------------------------------------------------------

IsoString UnsharpMaskProcess::Category() const
{
   return "Convolution";
}

// ----------------------------------------------------------------------------

uint32 UnsharpMaskProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String UnsharpMaskProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** UnsharpMaskProcess::IconImageXPM() const
{
   return UnsharpMaskIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* UnsharpMaskProcess::DefaultInterface() const
{
   return TheUnsharpMaskInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* UnsharpMaskProcess::Create() const
{
   return new UnsharpMaskInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* UnsharpMaskProcess::Clone( const ProcessImplementation& p ) const
{
   const UnsharpMaskInstance* instPtr = dynamic_cast<const UnsharpMaskInstance*>( &p );
   return (instPtr != 0) ? new UnsharpMaskInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool UnsharpMaskProcess::NeedsValidation() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool UnsharpMaskProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: UnsharpMask [<arg_list>] [<view_list>]"
"\n"
"\n-s=<n> | -sigma=<n> | -stddev=<n>"
"\n"
"\n      <n> is the standard deviation of the Gaussian filter in pixels. The"
"\n      valid range is [0.1,250]. (default=2.0)"
"\n"
"\n-a=<n> | -amount=<n>"
"\n"
"\n      <n> is the filtering strenght, or the amount by which the unsharp mask"
"\n      filter will be applied. The valid range of values is [0.1,1]."
"\n      (default=0.8)"
"\n"
"\n-L[+|-] | -CIEL[+|-]"
"\n"
"\n      Enables/disables processing of the CIE L* component (lightness). When"
"\n      this option is enabled, the USM filter is applied to the CIE L* component"
"\n      of a target RGB color image. When this option is disabled, USM is applied"
"\n      to all the nominal channels of the target image. (default=disabled)"
"\n"
"\n-Y[+|-] | -CIEY[+|-]"
"\n"
"\n      Enables/disables processing of the CIE Y component (luminance). When this"
"\n      option is enabled, the USM filter is applied to the CIE L* component of a"
"\n      target RGB color image. When this option is disabled, USM is applied to"
"\n      all the nominal channels of the target image. (default=disabled)"
"\n"
"\n-d[+|-] | -deringing[+|-]"
"\n"
"\n      Enables/disables the deringing algorithm. (default=disabled)"
"\n"
"\n-dd=<n> | -deringing-dark=<n>"
"\n"
"\n      <n> is the deringing regularization threshold for dark ringing"
"\n      artifacts, in the [0,1] range. (default=0.1)"
"\n"
"\n-db=<n> | -deringing-bright=<n>"
"\n"
"\n      <n> is the deringing regularization threshold for bright ringing"
"\n      artifacts, in the [0,1] range. (default=0)"
"\n"
"\n-dm[+|-] | -deringing-maps[+|-]"
"\n"
"\n      Enables/disables generation of deringing map images. (default=disabled)"
"\n"
"\n-r0=<n> | -range-low=<n>"
"\n"
"\n      <n> is the shadows dynamic range extension in [0,1]. (default=0)"
"\n"
"\n-r1=<n> | -range-high=<n>"
"\n"
"\n      <n> is the highlights dynamic range extension in [0,1] (default=0)"
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

int UnsharpMaskProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   UnsharpMaskInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "s" || arg.Id() == "sigma" || arg.Id() == "stddev" )
         {
            if ( arg.NumericValue() < TheUSMSigmaParameter->MinimumValue() || arg.NumericValue() > TheUSMSigmaParameter->MaximumValue() )
               OUT_OF_RANGE;
            instance.sigma = arg.NumericValue();
         }
         else if ( arg.Id() == "a" || arg.Id() == "amount" )
         {
            if ( arg.NumericValue() < TheUSMAmountParameter->MinimumValue() || arg.NumericValue() > TheUSMAmountParameter->MaximumValue() )
               OUT_OF_RANGE;
            instance.amount = arg.NumericValue();
         }
         else if ( arg.Id() == "dd" || arg.Id() == "deringing-dark" )
         {
            if ( arg.NumericValue() < TheUSMDeringingDarkParameter->MinimumValue() || arg.NumericValue() > TheUSMDeringingDarkParameter->MaximumValue() )
               OUT_OF_RANGE;
            instance.deringingDark = arg.NumericValue();
         }
         else if ( arg.Id() == "db" || arg.Id() == "deringing-bright" )
         {
            if ( arg.NumericValue() < TheUSMDeringingBrightParameter->MinimumValue() || arg.NumericValue() > TheUSMDeringingBrightParameter->MaximumValue() )
               OUT_OF_RANGE;
            instance.deringingBright = arg.NumericValue();
         }
         else if ( arg.Id() == "r0" || arg.Id() == "range-low" )
         {
            if ( arg.NumericValue() < TheUSMRangeLowParameter->MinimumValue() || arg.NumericValue() > TheUSMRangeLowParameter->MaximumValue() )
               OUT_OF_RANGE;
            instance.rangeLow = arg.NumericValue();
         }
         else if ( arg.Id() == "r1" || arg.Id() == "range-high" )
         {
            if ( arg.NumericValue() < TheUSMRangeHighParameter->MinimumValue() || arg.NumericValue() > TheUSMRangeHighParameter->MaximumValue() )
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
         if ( arg.Id() == "L" || arg.Id() == "CIEL" )
            instance.useLuminance = arg.SwitchState();
         else if ( arg.Id() == "Y" || arg.Id() == "CIEY" )
            instance.linear = arg.SwitchState();
         else if ( arg.Id() == "d" || arg.Id() == "deringing" )
            instance.deringing = arg.SwitchState();
         else if ( arg.Id() == "dm" || arg.Id() == "deringing-maps" )
            instance.outputDeringingMaps = arg.SwitchState();
         else
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "L" || arg.Id() == "CIEL" )
            instance.useLuminance = true;
         else if ( arg.Id() == "Y" || arg.Id() == "CIEY" )
            instance.linear = true;
         else if ( arg.Id() == "d" || arg.Id() == "deringing" )
            instance.deringing = true;
         else if ( arg.Id() == "dm" || arg.Id() == "deringing-maps" )
            instance.outputDeringingMaps = true;
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
// EOF UnsharpMaskProcess.cpp - Released 2017-05-02T09:43:00Z
