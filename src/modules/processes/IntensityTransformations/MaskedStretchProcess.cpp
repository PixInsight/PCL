//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.00.0314
// ----------------------------------------------------------------------------
// MaskedStretchProcess.cpp - Released 2015/10/08 11:24:40 UTC
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

#include "MaskedStretchInstance.h"
#include "MaskedStretchInterface.h"
#include "MaskedStretchParameters.h"
#include "MaskedStretchProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "MaskedStretchIcon.xpm"

// ----------------------------------------------------------------------------

MaskedStretchProcess* TheMaskedStretchProcess = 0;

// ----------------------------------------------------------------------------

MaskedStretchProcess::MaskedStretchProcess() : MetaProcess()
{
   TheMaskedStretchProcess = this;

   // Instantiate process parameters
   new MSTargetBackground( this );
   new MSNumberOfIterations( this );
   new MSClippingFraction( this );
   new MSBackgroundReferenceViewId( this );
   new MSBackgroundLow( this );
   new MSBackgroundHigh( this );
   new MSUseROI( this );
   new MSROIX0( this );
   new MSROIY0( this );
   new MSROIX1( this );
   new MSROIY1( this );
   new MSMaskType( this );
}

IsoString MaskedStretchProcess::Id() const
{
   return "MaskedStretch";
}

IsoString MaskedStretchProcess::Category() const
{
   return "IntensityTransformations";
}

uint32 MaskedStretchProcess::Version() const
{
   return 0x100;
}

const char** MaskedStretchProcess::IconImageXPM() const
{
   return MaskedStretchIcon_XPM;
}

ProcessInterface* MaskedStretchProcess::DefaultInterface() const
{
   return TheMaskedStretchInterface;
}

ProcessImplementation* MaskedStretchProcess::Create() const
{
   return new MaskedStretchInstance( this );
}

ProcessImplementation* MaskedStretchProcess::Clone( const ProcessImplementation& p ) const
{
   const MaskedStretchInstance* instPtr = dynamic_cast<const MaskedStretchInstance*>( &p );
   return (instPtr != 0) ? new MaskedStretchInstance( *instPtr ) : 0;
}

bool MaskedStretchProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Iteratively performs a masked nonlinear intensity transformation for highlight detail preservation."
"\nUsage: MaskedStretch [<arg_list>] [<view_list>]"
"\n"
"\n-b=<n> | --background=<n>"
"\n"
"\n      <n> is the target mean background value, 0 <= n <= 1 (0.12)"
"\n"
"\n-n=<n> | --iterations=<n>"
"\n"
"\n      <n> is the number of iterations to be used. 0 <= n <= 1000 (50)."
"\n"
"\n-m=<mask-type> | --mask-type=<mask-type>"
"\n"
"\n      Specifies the type of mask to be used for color images. <mask-type>"
"\n      can be one of:"
"\n"
"\n      V           Use the V component of HSV"
"\n      value       Same meaning as V"
"\n      I           Use the I component of HSI"
"\n      intensity   Same meaning as I"
"\n"
"\n      The default mask type is intensity."
"\n"
"\n--interface"
"\n"
"\n      Launches the interface of this process."
"\n"
"\n--help"
"\n"
"\n      Displays this help and exits."
"</raw>"
   );
}

int MaskedStretchProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   MaskedStretchInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "b" || arg.Id() == "-background" )
         {
            double b = arg.NumericValue();
            if ( b < TheMSTargetBackgroundParameter->MinimumValue() ||
                 b > TheMSTargetBackgroundParameter->MaximumValue() )
               throw Error( "Target background parameter out of range" );
            instance.p_targetBackground = b;
         }
         else if ( arg.Id() == "n" || arg.Id() == "-iterations" )
         {
            int n = TruncInt( arg.NumericValue() );
            if ( n < int( TheMSNumberOfIterationsParameter->MinimumValue() ) ||
                 n > int( TheMSNumberOfIterationsParameter->MaximumValue() ) )
               throw Error( "Number of iterations parameter out of range" );
            instance.p_numberOfIterations = n;
         }
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "m" || arg.Id() == "-mask-type" )
         {
            if ( arg.StringValue() == "I" || arg.StringValue() == "intensity" )
               instance.p_maskType = MSMaskType::Intensity;
            else if ( arg.StringValue() == "V" || arg.StringValue() == "value" )
               instance.p_maskType = MSMaskType::Value;
         }
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
// EOF MaskedStretchProcess.cpp - Released 2015/10/08 11:24:40 UTC
