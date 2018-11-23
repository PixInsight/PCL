//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.03.00.0427
// ----------------------------------------------------------------------------
// ExtractAlphaChannelsProcess.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "ExtractAlphaChannelsProcess.h"
#include "ExtractAlphaChannelsParameters.h"
#include "ExtractAlphaChannelsInstance.h"
#include "ExtractAlphaChannelsInterface.h"

#include <pcl/Arguments.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ExtractAlphaChannelsProcess* TheExtractAlphaChannelsProcess = nullptr;

// ----------------------------------------------------------------------------

#include "ExtractAlphaChannelsIcon.xpm"

// ----------------------------------------------------------------------------

ExtractAlphaChannelsProcess::ExtractAlphaChannelsProcess() : MetaProcess()
{
   TheExtractAlphaChannelsProcess = this;

   // Instantiate process parameter
   new EAChannels( this );
   new EAChannelList( this );
   new EAExtract( this );
   new EADelete( this );
}

// ----------------------------------------------------------------------------

IsoString ExtractAlphaChannelsProcess::Id() const
{
   return "ExtractAlphaChannels";
}

// ----------------------------------------------------------------------------

IsoString ExtractAlphaChannelsProcess::Category() const
{
   return "Image,ChannelManagement";
}

// ----------------------------------------------------------------------------

uint32 ExtractAlphaChannelsProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ExtractAlphaChannelsProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** ExtractAlphaChannelsProcess::IconImageXPM() const
{
   return ExtractAlphaChannelsIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* ExtractAlphaChannelsProcess::DefaultInterface() const
{
   return TheExtractAlphaChannelsInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ExtractAlphaChannelsProcess::Create() const
{
   return new ExtractAlphaChannelsInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ExtractAlphaChannelsProcess::Clone( const ProcessImplementation& p ) const
{
   const ExtractAlphaChannelsInstance* instPtr = dynamic_cast<const ExtractAlphaChannelsInstance*>( &p );
   return (instPtr != nullptr) ? new ExtractAlphaChannelsInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool ExtractAlphaChannelsProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ExtractAlphaChannelsProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: ExtractAlphaChannels [<arg_list>] [<view_list>]"
"\n"
"\n-all"
"\n"
"\n      Remove all existing alpha channels from the target images. (default)"
"\n"
"\n-active | -first"
"\n"
"\n      Remove the active (first) alpha channel only."
"\n"
"\n-channels=<ch_list>"
"\n"
"\n      <ch_list> is a comma-separated list of alpha channel indexes to remove."
"\n      Alpha channels are numbered starting from 0."
"\n"
"\n      For example, -channels=0,3,4 would remove the first, fourth and fifth"
"\n      alpha channels."
"\n"
"\n-extract[+|-]"
"\n"
"\n      Enables/disables alpha channel extraction. When extraction is enabled,"
"\n      alpha channels are removed from the target image and extracted as new"
"\n      images. When extraction is disabled, alpha channels are destroyed and"
"\n      not extracted (default=enabled)."
"\n"
"\n-delete[+|-]"
"\n"
"\n      Enables/disables alpha channel deletion. Note that this option"
"\n                can be specified along with -extract. (default=disabled)."
"\n"
"\n--interface     Launch interface."
"\n"
"\n--help          Shows this help text and exits."
"</raw>" );
}

int ExtractAlphaChannelsProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
      ExtractArguments( argv, ArgumentItemMode::AsViews,
                        ArgumentOption::AllowWildcards|ArgumentOption::NoPreviews );

   ExtractAlphaChannelsInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "channels" )
         {
            if ( arg.NumericValue() < 0 )
               throw Error( "Invalid alpha channel index: " + arg.Token() );
            instance.channelList = String( int( arg.NumericValue() ) );
            instance.channels = EAChannels::ChannelList;
         }
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "channels" )
         {
            String chlist = arg.StringValue();
            chlist.Trim();
            SortedArray<int> channels;
            ExtractAlphaChannelsInstance::ParseChannelList( channels, chlist ); // throws exception on error
            instance.channelList = chlist;
            instance.channels = EAChannels::ChannelList;
         }
         else
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         if ( arg.Id() == "extract" )
            instance.extractChannels = arg.SwitchState();
         if ( arg.Id() == "delete" )
            instance.deleteChannels = arg.SwitchState();
         else
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "all" )
            instance.channels = EAChannels::AllAlphaChannels;
         else if ( arg.Id() == "active" || arg.Id() == "first" )
            instance.channels = EAChannels::ActiveAlphaChannel;
         else if ( arg.Id() == "extract" )
            instance.extractChannels = true;
         else if ( arg.Id() == "delete" )
            instance.deleteChannels = true;
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
// EOF ExtractAlphaChannelsProcess.cpp - Released 2018-11-23T18:45:58Z
