//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0393
// ----------------------------------------------------------------------------
// BinarizeProcess.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "BinarizeProcess.h"
#include "BinarizeParameters.h"
#include "BinarizeInstance.h"
#include "BinarizeInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "BinarizeIcon.xpm"

// ----------------------------------------------------------------------------

BinarizeProcess* TheBinarizeProcess = 0;

// ----------------------------------------------------------------------------

BinarizeProcess::BinarizeProcess() : MetaProcess()
{
   TheBinarizeProcess = this;

   // Instantiate process parameters
   new BinarizeLevelR( this );
   new BinarizeLevelG( this );
   new BinarizeLevelB( this );
   new BinarizeIsGlobal( this );
}

// ----------------------------------------------------------------------------

IsoString BinarizeProcess::Id() const
{
   return "Binarize";
}

// ----------------------------------------------------------------------------

IsoString BinarizeProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 BinarizeProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String BinarizeProcess::Description() const
{
   return

   "<html>"
   "<p>Binarize sets every pixel value below a given threshold to pure black, "
   "and every pixel value above the same threshold to pure white.</p>"

   "<p>The user may set the threshold value either globally (as a unique value "
   "for all nominal channels in the image), or as a set of individual values on "
   "a per-channel basis.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** BinarizeProcess::IconImageXPM() const
{
   return BinarizeIcon_XPM;
}
// ----------------------------------------------------------------------------

ProcessInterface* BinarizeProcess::DefaultInterface() const
{
   return TheBinarizeInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* BinarizeProcess::Create() const
{
   return new BinarizeInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* BinarizeProcess::Clone( const ProcessImplementation& p ) const
{
   const BinarizeInstance* instPtr = dynamic_cast<const BinarizeInstance*>( &p );
   return (instPtr != 0) ? new BinarizeInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool BinarizeProcess::NeedsValidation() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool BinarizeProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"\nUsage: Binarize [<arg_list>] [<view_list>]"
"\n"
"\n-t[<ch>]=<n> | -threshold[<ch>]=<n>"
"\n"
"\n      <n> is the binarization threshold, 0 <= n <= 1."
"\n"
"\n      The optional <ch> suffix selects a channel to apply the corresponding"
"\n      threshold. <ch> can be one of:"
"\n"
"\n      R|K : red/gray channel"
"\n      G   : green channel"
"\n      B   : blue channel"
"\n"
"\n      If no <ch> suffix is specified, values apply equally to the red(gray),"
"\n      green and blue channels of the target views."
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

static double ArgumentThresholdValue( const Argument& arg )
{
   if ( arg.NumericValue() < 0 || arg.NumericValue() > 1 )
      throw Error( "Binarization threshold out of range: " + arg.Token() );
   return arg.NumericValue();
}

int BinarizeProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   BinarizeInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "t" || arg.Id() == "threshold" )
         {
            instance.level[0] = instance.level[1] = instance.level[2] = ArgumentThresholdValue( arg );
            instance.isGlobal = true;
         }
         else if ( arg.Id() == "tR" || arg.Id() == "thresholdR" )
         {
            instance.level[0] = ArgumentThresholdValue( arg );
            instance.isGlobal = false;
         }
         else if ( arg.Id() == "tG" || arg.Id() == "thresholdG" )
         {
            instance.level[1] = ArgumentThresholdValue( arg );
            instance.isGlobal = false;
         }
         else if ( arg.Id() == "tB" || arg.Id() == "thresholdB" )
         {
            instance.level[2] = ArgumentThresholdValue( arg );
            instance.isGlobal = false;
         }
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
         throw Error( "Unknown string argument: " + arg.Token() );
      else if ( arg.IsSwitch() )
         throw Error( "Unknown switch argument: " + arg.Token() );
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
// EOF BinarizeProcess.cpp - Released 2017-07-09T18:07:33Z
