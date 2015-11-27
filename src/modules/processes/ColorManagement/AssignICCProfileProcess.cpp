//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 01.00.00.0274
// ----------------------------------------------------------------------------
// AssignICCProfileProcess.cpp - Released 2015/11/26 16:00:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
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

#include "AssignICCProfileProcess.h"
#include "AssignICCProfileParameters.h"
#include "AssignICCProfileInstance.h"
#include "AssignICCProfileInterface.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/ICCProfile.h>
#include <pcl/File.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AssignICCProfileProcess* TheAssignICCProfileProcess = 0;

// ----------------------------------------------------------------------------

#include "AssignICCProfileIcon.xpm"

// ----------------------------------------------------------------------------

AssignICCProfileProcess::AssignICCProfileProcess() : MetaProcess()
{
   TheAssignICCProfileProcess = this;

   // Instantiate process parameters
   new TargetProfile( this );
   new AssignMode( this );
}

// ----------------------------------------------------------------------------

IsoString AssignICCProfileProcess::Id() const
{
   return "AssignICCProfile";
}

// ----------------------------------------------------------------------------

IsoString AssignICCProfileProcess::Category() const
{
   return "ColorManagement";
}

// ----------------------------------------------------------------------------

uint32 AssignICCProfileProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String AssignICCProfileProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** AssignICCProfileProcess::IconImageXPM() const
{
   return AssignICCProfileIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* AssignICCProfileProcess::DefaultInterface() const
{
   return TheAssignICCProfileInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* AssignICCProfileProcess::Create() const
{
   return new AssignICCProfileInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* AssignICCProfileProcess::Clone( const ProcessImplementation& p ) const
{
   const AssignICCProfileInstance* instPtr = dynamic_cast<const AssignICCProfileInstance*>( &p );
   return (instPtr != 0) ? new AssignICCProfileInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool AssignICCProfileProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: AssignICCProfile [<arg_list>] [<img_list>]"
"\n"
"\n-profile=<profile_id>"
"\n"
"\n      <profile_id> identifies the target ICC profile. The specified"
"\n      identifier must correspond to an installed color profile."
"\n"
"\n-filename=<file_name>"
"\n"
"\n      Specifies the target ICC profile by its file name. The specified file"
"\n      name must correspond to an installed ICC profile, and it must not"
"\n      include a directory. The system profile directories will always be"
"\n      used and cannot be changed."
"\n"
"\n-default[+|-]"
"\n"
"\n      When enabled, this option causes target image(s) to forget their"
"\n      current ICC profiles and assigns them the default RGB or grayscale"
"\n      profile, as per global color management settings."
"\n"
"\n-untag[+|-]"
"\n"
"\n      When enabled, this option causes target image(s) to forget their"
"\n      current ICC profiles, and leaves them untagged. The default profile"
"\n      (as per global color management settings) will be used to render the"
"\n      target image(s) on the screen."
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

int AssignICCProfileProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv,
                     ArgumentItemMode::AsViews,
                     ArgumentOption::AllowWildcards|ArgumentOption::NoPreviews );

   AssignICCProfileInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
         throw Error( "Unknown numeric argument: " + arg.Token() );
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "profile" )
         {
            instance.targetProfile = arg.StringValue();
            instance.targetProfile.Trim();
            if ( instance.targetProfile.IsEmpty() )
               throw Error( "Empty profile identifier: " + arg.Token() );
         }
         else if ( arg.Id() == "filename" )
         {
            String filename = arg.StringValue();
            filename.Trim();
            if ( filename.IsEmpty() )
               throw Error( "Empty file name: " + arg.Token() );

            instance.targetProfile.Clear();

            StringList dirs = ICCProfile::ProfileDirectories();
            for ( StringList::const_iterator i = dirs.Begin(); i != dirs.End(); ++i )
            {
               String path = *i + '/' + filename;
               if ( File::Exists( path ) )
               {
                  ICCProfile icc( path );
                  if ( icc.IsProfile() )
                  {
                     instance.targetProfile = icc.Description();
                     break;
                  }
               }
            }

            if ( instance.targetProfile.IsEmpty() )
               throw Error( "The specified file name does not correspond to a valid ICC profile: " + filename );
         }
         else
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         if ( arg.Id() == "default" )
            instance.mode = arg.SwitchState() ? AssignMode::AssignDefaultProfile : AssignMode::AssignNewProfile;
         else if ( arg.Id() == "untag" )
            instance.mode = arg.SwitchState() ? AssignMode::LeaveUntagged : AssignMode::AssignNewProfile;
         else
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "default" )
            instance.mode = AssignMode::AssignDefaultProfile;
         else if ( arg.Id() == "untag" )
            instance.mode = AssignMode::LeaveUntagged;
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
      instance.LaunchOnCurrentWindow();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AssignICCProfileProcess.cpp - Released 2015/11/26 16:00:12 UTC
