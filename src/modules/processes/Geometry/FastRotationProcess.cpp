//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.01.0346
// ----------------------------------------------------------------------------
// FastRotationProcess.cpp - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#include "FastRotationProcess.h"
#include "FastRotationParameters.h"
#include "FastRotationInstance.h"
#include "FastRotationInterface.h"

#include <pcl/Arguments.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FastRotationProcess* TheFastRotationProcess = nullptr;

// ----------------------------------------------------------------------------

#include "FastRotationIcon.xpm"

// ----------------------------------------------------------------------------

FastRotationProcess::FastRotationProcess() : MetaProcess()
{
   TheFastRotationProcess = this;
   new FRMode( this );
   TheFRNoGUIMessagesParameter = new NoGUIMessages( this );
}

IsoString FastRotationProcess::Id() const
{
   return "FastRotation";
}

IsoString FastRotationProcess::Category() const
{
   return "Geometry";
}

uint32 FastRotationProcess::Version() const
{
   return 0x100;
}

String FastRotationProcess::Description() const
{
   return

   "<html>"
   "<p>The FastRotation process implements simple orthogonal and specular geometric "
   "transformations: Rotate 180 degrees, rotate 90 degrees clockwise, rotate 90 "
   "degrees counter-clockwise, horizontal and vertical mirror.</p>"

   "<p>Notably, these transforms don't interpolate pixel values; they operate by "
   "copying and swapping pixels exclusively.</p>"

   "<p>As most geometric processes, FastRotation cannot be executed on previews.</p>"
   "</html>";
}

const char** FastRotationProcess::IconImageXPM() const
{
   return FastRotationIcon_XPM;
}

ProcessInterface* FastRotationProcess::DefaultInterface() const
{
   return TheFastRotationInterface;
}

ProcessImplementation* FastRotationProcess::Create() const
{
   return new FastRotationInstance( this );
}

ProcessImplementation* FastRotationProcess::Clone( const ProcessImplementation& p ) const
{
   const FastRotationInstance* instPtr = dynamic_cast<const FastRotationInstance*>( &p );
   return (instPtr != nullptr) ? new FastRotationInstance( *instPtr ) : nullptr;
}

bool FastRotationProcess::CanProcessCommandLines() const
{
   return true;
}

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: FastRotation [<arg_list>] [<img_list>]"
"\n"
"\n-r180"
"\n"
"\n      Rotate by 180 degrees."
"\n"
"\n-r90 | -r90ccw"
"\n"
"\n      Rotate +90 degrees (90 degrees counter-clockwise)."
"\n"
"\n-r90cw"
"\n"
"\n      Rotate -90 degrees (90 degrees clockwise)."
"\n"
"\n-mh"
"\n"
"\n      Perform a horizontal mirroring operation."
"\n"
"\n-mv"
"\n"
"\n      Perform a vertical mirroring operation."
"\n"
"\n-a=<n> | -angle=<n>"
"\n"
"\n      <n> is a rotation angle in degrees, and must be one of: 180,+90,-90."
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

int FastRotationProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv,
                                              ArgumentItemMode::AsViews,
                                              ArgumentOption::AllowWildcards|ArgumentOption::NoPreviews );

   FastRotationInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "a" || arg.Id() == "angle" )
         {
            if ( arg.NumericValue() == +180 || arg.NumericValue() == -180 )
               instance.p_mode = FRMode::Rotate180;
            else if ( arg.NumericValue() == 90 )
               instance.p_mode = FRMode::Rotate90CCW;
            else if ( arg.NumericValue() == -90 )
               instance.p_mode = FRMode::Rotate90CW;
            else
               throw Error( "Invalid fast rotation angle - must be 180 or +/-90 degrees: " + arg.Token() );
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
         if ( arg.Id() == "r180" )
            instance.p_mode = FRMode::Rotate180;
         else if ( arg.Id() == "r90" || arg.Id() == "r90ccw" )
            instance.p_mode = FRMode::Rotate90CCW;
         else if ( arg.Id() == "r90cw" )
            instance.p_mode = FRMode::Rotate90CW;
         else if ( arg.Id() == "mh" )
            instance.p_mode = FRMode::HorizontalMirror;
         else if ( arg.Id() == "mv" )
            instance.p_mode = FRMode::VerticalMirror;
         else if ( arg.Id() == "-interface" )
            launchInterface = false;
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
      instance.LaunchOnCurrentWindow();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FastRotationProcess.cpp - Released 2017-05-02T09:43:00Z
