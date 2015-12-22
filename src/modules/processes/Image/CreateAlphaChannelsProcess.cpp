//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0348
// ----------------------------------------------------------------------------
// CreateAlphaChannelsProcess.cpp - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "CreateAlphaChannelsProcess.h"
#include "CreateAlphaChannelsParameters.h"
#include "CreateAlphaChannelsInstance.h"
#include "CreateAlphaChannelsInterface.h"

#include <pcl/Arguments.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CreateAlphaChannelsProcess* TheCreateAlphaChannelsProcess = 0;

// ----------------------------------------------------------------------------

#include "CreateAlphaChannelsIcon.xpm"

// ----------------------------------------------------------------------------

CreateAlphaChannelsProcess::CreateAlphaChannelsProcess() : MetaProcess()
{
   TheCreateAlphaChannelsProcess = this;

   // Instantiate process parameter
   new CAFromImage( this );
   new CASourceImageIdentifier( this );
   new CAInvertSourceImage( this );
   new CACloseSourceImage( this );
   new CATransparency( this );
   new CAReplaceExistingChannels( this );
   new CAChannelCount( this );
}

// ----------------------------------------------------------------------------

IsoString CreateAlphaChannelsProcess::Id() const
{
   return "CreateAlphaChannels";
}

// ----------------------------------------------------------------------------

IsoString CreateAlphaChannelsProcess::Category() const
{
   return "Image,ChannelManagement";
}

// ----------------------------------------------------------------------------

uint32 CreateAlphaChannelsProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String CreateAlphaChannelsProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** CreateAlphaChannelsProcess::IconImageXPM() const
{
   return CreateAlphaChannelsIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* CreateAlphaChannelsProcess::DefaultInterface() const
{
   return TheCreateAlphaChannelsInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* CreateAlphaChannelsProcess::Create() const
{
   return new CreateAlphaChannelsInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* CreateAlphaChannelsProcess::Clone( const ProcessImplementation& p ) const
{
   const CreateAlphaChannelsInstance* instPtr = dynamic_cast<const CreateAlphaChannelsInstance*>( &p );
   return (instPtr != 0) ? new CreateAlphaChannelsInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool CreateAlphaChannelsProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: CreateAlphaChannels [<arg_list>] [<view_list>]"
"\n"
"\n-i=<view> | -image=<view>"
"\n"
"\n      Use <view> as the source of newly created alpha channels. If <view>"
"\n      corresponds to a color image, its CIE L* component will be taken as the"
"\n      source of new alpha channels. This option invalidates any constant"
"\n      transparency values previously specified with the -t argument."
"\n      (default = target image)"
"\n"
"\n-c[+|-] | -close-source[+|-]"
"\n"
"\n      Enable/disable closing source images. If this option is enabled, alpha"
"\n      channel source images specified with the -i argument will be closed"
"\n      automatically. (default=disabled)"
"\n"
"\n-t=<n> | -transparency=<n>"
"\n"
"\n      <n> is a constant real value, in the normalized [0,1] range, that will be"
"\n      used to fill newly created alpha channels. This option invalidates any"
"\n      source images previously specified with the -i argument."
"\n      (default = the CIE L* component of the target image is used as the source"
"\n      of new alpha channels)"
"\n"
"\n-n=<n> | -count=<n>"
"\n"
"\n      <n> is a nonzero positive integer indicating the amount of alpha channels"
"\n      that will be created (default=1)."
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

int CreateAlphaChannelsProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   CreateAlphaChannelsInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "t" || arg.Id() == "transparency" )
         {
            if ( arg.NumericValue() < 0 || arg.NumericValue() > 1 )
               throw Error( "Transparency value out of range: " + arg.Token() );
            instance.transparency = arg.NumericValue();
            instance.fromImage = false;
         }
         else if ( arg.Id() == "n" || arg.Id() == "count" )
         {
            if ( arg.NumericValue() <= 0 )
               throw Error( "Invalid channel count: " + arg.Token() );
            instance.count = unsigned( arg.NumericValue() );
         }
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "i" || arg.Id() == "image" )
         {
            String id = arg.StringValue();
            id.Trim();
            if ( !id.IsEmpty() && !id.IsValidIdentifier() )
               throw Error( "Invalid image identifier: " + arg.Token() );
            instance.imageId = id;
            instance.fromImage = true;
         }
         else
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         if ( arg.Id() == "c" || arg.Id() == "close-source" )
            instance.closeSourceImage = arg.SwitchState();
         else
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "c" || arg.Id() == "close-source" )
            instance.closeSourceImage = true;
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
// EOF CreateAlphaChannelsProcess.cpp - Released 2015/12/18 08:55:08 UTC
