//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.07.0386
// ----------------------------------------------------------------------------
// ColorManagementSetupProcess.cpp - Released 2018-11-01T11:07:20Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#include "ColorManagementSetupInstance.h"
#include "ColorManagementSetupInterface.h"
#include "ColorManagementSetupParameters.h"
#include "ColorManagementSetupProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Color.h> // for RGBAColor()
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ColorManagementSetupProcess* TheColorManagementSetupProcess = 0;

// ----------------------------------------------------------------------------

#include "ColorManagementSetupIcon.xpm"

// ----------------------------------------------------------------------------

ColorManagementSetupProcess::ColorManagementSetupProcess() : MetaProcess()
{
   TheColorManagementSetupProcess = this;

   // Instantiate process parameters
   new CMSEnabled( this );
   new CMSUpdateMonitorProfile( this );
   new CMSDefaultRGBProfile( this );
   new CMSDefaultGrayProfile( this );
   new CMSDefaultRenderingIntent( this );
   new CMSOnProfileMismatch( this );
   new CMSOnMissingProfile( this );
   new CMSDefaultEmbedProfilesInRGBImages( this );
   new CMSDefaultEmbedProfilesInGrayscaleImages( this );
   new CMSUseLowResolutionCLUTs( this );
   new CMSProofingProfile( this );
   new CMSProofingIntent( this );
   new CMSUseProofingBPC( this );
   new CMSDefaultProofingEnabled( this );
   new CMSDefaultGamutCheckEnabled( this );
   new CMSGamutWarningColor( this );
}

// ----------------------------------------------------------------------------

IsoString ColorManagementSetupProcess::Id() const
{
   return "ColorManagementSetup";
}

// ----------------------------------------------------------------------------

IsoString ColorManagementSetupProcess::Category() const
{
   return "Global,ColorManagement";
}

// ----------------------------------------------------------------------------

uint32 ColorManagementSetupProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ColorManagementSetupProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** ColorManagementSetupProcess::IconImageXPM() const
{
   return ColorManagementSetupIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* ColorManagementSetupProcess::DefaultInterface() const
{
   return TheColorManagementSetupInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ColorManagementSetupProcess::Create() const
{
   return new ColorManagementSetupInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ColorManagementSetupProcess::Clone( const ProcessImplementation& p ) const
{
   const ColorManagementSetupInstance* instPtr = dynamic_cast<const ColorManagementSetupInstance*>( &p );
   return (instPtr != 0) ? new ColorManagementSetupInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool ColorManagementSetupProcess::PrefersGlobalExecution() const
{
   return true; // this is a global process
}

// ----------------------------------------------------------------------------

bool ColorManagementSetupProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: ColorManagementSetup [<arg_list>]"
"\n"
"\n-e | --enable"
"\n"
"\n      Globally enable color management."
"\n"
"\n-d | --disable"
"\n"
"\n      Globally disable color management."
"\n"
"\n-r=<profile_id> | --rgb-profile=<profile_id>"
"\n"
"\n      Sets the default ICC profile that will be used for RGB color images."
"\n      <profile_id> must correspond to an installed RGB profile."
"\n      (default = the current monitor profile)"
"\n"
"\n-g=<profile_id> | --grayscale-profile=<profile_id>"
"\n"
"\n      Sets the default ICC profile that will be used for grayscale images."
"\n      <profile_id> must correspond to an installed RGB or grayscale profile."
"\n      (default = the current monitor profile)"
"\n"
"\n-ri=<intent> | --rendering-intent=<intent>"
"\n"
"\n      <intent> is a rendering intent to use in screen color management"
"\n      transformations. Possible values for <intent> are:"
"\n"
"\n      perceptual, saturation, relative, absolute"
"\n"
"\n      which correspond to the standard perceptual, saturation, relative"
"\n      colorimetric and absolute colorimetric rendering intents, respectively."
"\n      (default=perceptual)"
"\n"
"\n--on-profile-mismatch=<policy>"
"\n"
"\n      <policy> identifies a profile mismatch policy. Valid policies are:"
"\n"
"\n      ask     : Ask the user what to do."
"\n      keep    : Keep mismatching embedded profiles. (default=keep)"
"\n      convert : Convert pixel values to the default profile."
"\n      discard : Ignore mismatching profile and leave image untagged."
"\n      disable : Disable color management for mismatching images."
"\n"
"\n--on-missing-profile=<policy>"
"\n"
"\n      <policy> identifies a missing profile policy. Valid policies are:"
"\n"
"\n      ask     : Ask the user what to do."
"\n      default : Assign the default profile."
"\n      ignore  : Leave the image untagged. (default=ignore)"
"\n      disable : Disable color management for untagged images."
"\n"
"\n--proofing-profile=<profile_id>"
"\n"
"\n      Sets the soft-proofing ICC profile. <profile_id> can be any"
"\n      installed profile. (default = the current monitor profile)"
"\n"
"\n-pi=<intent> | --proofing-intent=<intent>"
"\n"
"\n      <intent> is a rendering intent used for soft-proofing color management"
"\n      transformations. See the -ri argument for possible values of <intent>."
"\n      (default=relative)"
"\n"
"\n--proofing-bpc[+|-]"
"\n"
"\n      Enables or disables black point compensation for proofing color"
"\n      management transformations. (default=enabled)"
"\n"
"\n-gw=<css_color> || --gamut-warning-color=<css_color>"
"\n"
"\n      Specifies a RGB color that will be used as the global gamut warning"
"\n      color (for representation of out-of-gamut pixel values on images with"
"\n      active proofing). <css_color> is a valid CSS color value; it can be"
"\n      either an hex-encoded value in the #RRGGBB or #RRGGBBAA formats, or a"
"\n      valid CSS color name such as \"white\", \"black\", \"red\", and so on."
"\n      (default=#A9A9A9)"
"\n"
"\n--default-proofing[+|-]"
"\n"
"\n      Enables or disables color proofing for newly created and just opened"
"\n      images. (default=disabled)"
"\n"
"\n--default-gamut-check[+|-]"
"\n"
"\n      Enables or disables the gamut check feature for newly created and just"
"\n      opened images. Note that gamut check only works when proofing is"
"\n      enabled for a particular image. (default=disabled)"
"\n"
"\n--embed-rgb[+|-]"
"\n--embed-grayscale[+|-]"
"\n"
"\n      Enables or disables default profile embedding in written image files,"
"\n      for RGB color and grayscale images, respectively. Note that each file"
"\n      format may have its own profile embedding preferences; these are only"
"\n      default settings."
"\n"
"\n--load"
"\n"
"\n      Load the current color management settings."
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

int ColorManagementSetupProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::NoItems );

   ColorManagementSetupInstance instance( this );
   bool launchInterface = false;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "r" | arg.Id() == "-rgb-profile" )
         {
            instance.defaultRGBProfile = arg.StringValue();
            instance.defaultRGBProfile.Trim();
            if ( instance.defaultRGBProfile.IsEmpty() )
               throw Error( "Empty RGB profile: " + arg.Token() );
         }
         else if ( arg.Id() == "g" || arg.Id() == "-grayscale-profile" )
         {
            instance.defaultGrayProfile = arg.StringValue();
            instance.defaultGrayProfile.Trim();
            if ( instance.defaultGrayProfile.IsEmpty() )
               throw Error( "Empty grayscale profile: " + arg.Token() );
         }
         else if ( arg.Id() == "-proofing-profile" )
         {
            instance.proofingProfile = arg.StringValue();
            instance.proofingProfile.Trim();
            if ( instance.proofingProfile.IsEmpty() )
               throw Error( "Empty proofing profile: " + arg.Token() );
         }
         else if ( arg.Id() == "ri" || arg.Id() == "-rendering-intent"
                || arg.Id() == "pi" || arg.Id() == "-proofing-intent" )
         {
            pcl_enum intent;
            if ( arg.StringValue() == "perceptual" )
               intent = CMSDefaultRenderingIntent::Perceptual;
            else if ( arg.StringValue() == "saturation" )
               intent = CMSDefaultRenderingIntent::Saturation;
            else if ( arg.StringValue() == "relative" || arg.StringValue() == "relativeColorimetric" )
               intent = CMSDefaultRenderingIntent::RelativeColorimetric;
            else if ( arg.StringValue() == "absolute" || arg.StringValue() == "absoluteColorimetric" )
               intent = CMSDefaultRenderingIntent::AbsoluteColorimetric;
            else
               throw Error( "Invalid rendering intent: " + arg.Token() );

            if ( arg.Id() == "ri" || arg.Id() == "-rendering-intent" )
               instance.defaultRenderingIntent = intent;
            else
               instance.proofingIntent = intent;
         }
         else if ( arg.Id() == "-on-profile-mismatch" )
         {
            if ( arg.StringValue() == "ask" )
               instance.onProfileMismatch = CMSOnProfileMismatch::AskUser;
            else if ( arg.StringValue() == "keep" )
               instance.onProfileMismatch = CMSOnProfileMismatch::KeepEmbedded;
            else if ( arg.StringValue() == "convert" )
               instance.onProfileMismatch = CMSOnProfileMismatch::ConvertToDefault;
            else if ( arg.StringValue() == "discard" )
               instance.onProfileMismatch = CMSOnProfileMismatch::DiscardEmbedded;
            else if ( arg.StringValue() == "disable" )
               instance.onProfileMismatch = CMSOnProfileMismatch::DisableCM;
            else
               throw Error( "Invalid profile mismatch policy: " + arg.Token() );
         }
         else if ( arg.Id() == "-on-missing-profile" )
         {
            if ( arg.StringValue() == "ask" )
               instance.onMissingProfile = CMSOnMissingProfile::AskUser;
            else if ( arg.StringValue() == "default" )
               instance.onMissingProfile = CMSOnMissingProfile::AssignDefault;
            else if ( arg.StringValue() == "ignore" )
               instance.onMissingProfile = CMSOnMissingProfile::LeaveUntagged;
            else if ( arg.StringValue() == "disable" )
               instance.onMissingProfile = CMSOnMissingProfile::DisableCM;
            else
               throw Error( "Invalid missing profile policy: " + arg.Token() );
         }
         else if ( arg.Id() == "gw" || arg.Id() == "-gamut-warning-color" )
         {
            instance.gamutWarningColor = RGBAColor( arg.StringValue() );
         }
         else
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         if ( arg.Id() == "-embed-rgb" )
            instance.defaultEmbedProfilesInRGBImages = arg.SwitchState();
         else if ( arg.Id() == "-embed-grayscale" )
            instance.defaultEmbedProfilesInGrayscaleImages = arg.SwitchState();
         else if ( arg.Id() == "-proofing-bpc" )
            instance.useProofingBPC = arg.SwitchState();
         else if ( arg.Id() == "-default-proofing" )
            instance.defaultProofingEnabled = arg.SwitchState();
         else if ( arg.Id() == "-default-gamut-check" )
            instance.defaultGamutCheckEnabled = arg.SwitchState();
         else
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "e" || arg.Id() == "-enable" )
            instance.enabled = true;
         else if ( arg.Id() == "d" || arg.Id() == "-disable" )
            instance.enabled = false;
         if ( arg.Id() == "-embed-rgb" )
            instance.defaultEmbedProfilesInRGBImages = true;
         else if ( arg.Id() == "-embed-grayscale" )
            instance.defaultEmbedProfilesInGrayscaleImages = true;
         else if ( arg.Id() == "-load" )
            instance.LoadCurrentSettings();
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
         throw Error( "Invalid non-parametric argument: " + arg.Token() );
   }

   if ( launchInterface || arguments.IsEmpty() )
      instance.LaunchInterface();
   else
      instance.LaunchGlobal();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorManagementSetupProcess.cpp - Released 2018-11-01T11:07:20Z
