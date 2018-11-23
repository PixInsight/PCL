//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.07.0393
// ----------------------------------------------------------------------------
// ColorManagementSetupInstance.cpp - Released 2018-11-23T18:45:58Z
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
#include "ColorManagementSetupParameters.h"

#include <pcl/GlobalSettings.h>
#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ColorManagementSetupInstance::ColorManagementSetupInstance( const MetaProcess* p ) :
   ProcessImplementation( p ),
   enabled( TheCMSEnabledParameter->DefaultValue() ),
   defaultRenderingIntent( CMSRenderingIntent::DefaultForScreen ),
   onProfileMismatch( CMSOnProfileMismatch::Default ),
   onMissingProfile( CMSOnMissingProfile::Default ),
   defaultEmbedProfilesInRGBImages( TheCMSDefaultEmbedProfilesInRGBImagesParameter->DefaultValue() ),
   defaultEmbedProfilesInGrayscaleImages( TheCMSDefaultEmbedProfilesInGrayscaleImagesParameter->DefaultValue() ),
   useLowResolutionCLUTs( TheCMSUseLowResolutionCLUTsParameter->DefaultValue() ),
   proofingIntent( CMSRenderingIntent::DefaultForProofing ),
   useProofingBPC( TheCMSUseProofingBPCParameter->DefaultValue() ),
   defaultProofingEnabled( TheCMSDefaultProofingEnabledParameter->DefaultValue() ),
   defaultGamutCheckEnabled( TheCMSDefaultGamutCheckEnabledParameter->DefaultValue() ),
   gamutWarningColor( TheCMSGamutWarningColorParameter->DefaultValue() )
{
   /*
    * N.B.: We cannot call PixInsightSettings::GlobalString() if the module has
    * not been installed, because it requires communication with the core
    * application. The interface class will have to initialize its instance
    * member in its reimplementation of Initialize().
    */
   if ( Module->IsInstalled() )
   {
      String profilePath = PixInsightSettings::GlobalString( "ColorManagement/MonitorProfilePath" );
      if ( !profilePath.IsEmpty() )
      {
         ICCProfile icc( profilePath );
         if ( icc.IsProfile() )
            defaultRGBProfile = defaultGrayProfile = icc.Description();
      }

      profilePath = PixInsightSettings::GlobalString( "ColorManagement/ProofingProfilePath" );
      if ( !profilePath.IsEmpty() )
      {
         ICCProfile icc( profilePath );
         if ( icc.IsProfile() )
            proofingProfile = icc.Description();
      }
   }
}

// ----------------------------------------------------------------------------

ColorManagementSetupInstance::ColorManagementSetupInstance( const ColorManagementSetupInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void ColorManagementSetupInstance::Assign( const ProcessImplementation& p )
{
   const ColorManagementSetupInstance* x = dynamic_cast<const ColorManagementSetupInstance*>( &p );
   if ( x != nullptr )
   {
      enabled                               = x->enabled;
      updateMonitorProfile                  = x->updateMonitorProfile;
      defaultRGBProfile                     = x->defaultRGBProfile;
      defaultGrayProfile                    = x->defaultGrayProfile;
      defaultRenderingIntent                = x->defaultRenderingIntent;
      onProfileMismatch                     = x->onProfileMismatch;
      onMissingProfile                      = x->onMissingProfile;
      defaultEmbedProfilesInRGBImages       = x->defaultEmbedProfilesInRGBImages;
      defaultEmbedProfilesInGrayscaleImages = x->defaultEmbedProfilesInGrayscaleImages;
      useLowResolutionCLUTs                 = x->useLowResolutionCLUTs;
      proofingProfile                       = x->proofingProfile;
      proofingIntent                        = x->proofingIntent;
      useProofingBPC                        = x->useProofingBPC;
      defaultProofingEnabled                = x->defaultProofingEnabled;
      defaultGamutCheckEnabled              = x->defaultGamutCheckEnabled;
      gamutWarningColor                     = x->gamutWarningColor;
   }
}

// ----------------------------------------------------------------------------

bool ColorManagementSetupInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "ColorManagementSetup can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool ColorManagementSetupInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ColorManagementSetupInstance::ExecuteGlobal()
{
   /*
    * Find all installed ICC profiles
    */
   StringList all = ICCProfile::FindProfiles();

   /*
    * Find the default RGB profile
    */
   StringList descriptions;
   StringList paths;
   ICCProfile::ExtractProfileList( descriptions,
                                   paths,
                                   all,
                                   ICCColorSpace::RGB );

   StringList::const_iterator i = descriptions.Search( defaultRGBProfile );
   if ( i == descriptions.End() )
      throw Error( "Couldn't find the '" + defaultRGBProfile + "' profile.\n"
                   "Either it has not been installed, it is not a valid RGB profile,\n"
                   "or the corresponding disk file has been removed." );

   String rgbPath = paths[i - descriptions.Begin()];

   /*
    * Find the default grayscale profile
    */
   descriptions.Clear();
   paths.Clear();
   ICCProfile::ExtractProfileList( descriptions,
                                   paths,
                                   all,
                                   ICCColorSpace::RGB|ICCColorSpace::Gray );

   i = descriptions.Search( defaultGrayProfile );
   if ( i == descriptions.End() )
      throw Error( "Couldn't find the '" + defaultGrayProfile + "' profile.\n"
                   "Either it has not been installed, or the corresponding disk file has been removed." );

   String grayPath = paths[i - descriptions.Begin()];

   /*
    * Find the proofing profile
    */
   descriptions.Clear();
   paths.Clear();
   ICCProfile::ExtractProfileList( descriptions,
                                   paths,
                                   all ); // all color spaces are valid for proofing

   i = descriptions.Search( proofingProfile );
   if ( i == descriptions.End() )
      throw Error( "Couldn't find the '" + proofingProfile + "' profile.\n"
                   "Either it has not been installed, or the corresponding disk file has been removed." );

   String proofingPath = paths[i - descriptions.Begin()];

   /*
    * Perform global settings update
    */
   PixInsightSettings::BeginUpdate();

   try
   {
      PixInsightSettings::SetGlobalFlag( "ColorManagement/IsEnabled", enabled );
      if ( !updateMonitorProfile.IsEmpty() )
         PixInsightSettings::SetGlobalString( "ColorManagement/UpdateMonitorProfile", updateMonitorProfile );
      PixInsightSettings::SetGlobalString( "ColorManagement/DefaultRGBProfilePath", rgbPath );
      PixInsightSettings::SetGlobalString( "ColorManagement/DefaultGrayscaleProfilePath", grayPath );
      PixInsightSettings::SetGlobalInteger( "ColorManagement/DefaultRenderingIntent", defaultRenderingIntent );
      PixInsightSettings::SetGlobalInteger( "ColorManagement/OnProfileMismatch", onProfileMismatch );
      PixInsightSettings::SetGlobalInteger( "ColorManagement/OnMissingProfile", onMissingProfile );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/DefaultEmbedProfilesInRGBImages", defaultEmbedProfilesInRGBImages );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/DefaultEmbedProfilesInGrayscaleImages", defaultEmbedProfilesInGrayscaleImages );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/UseLowResolutionCLUTs", useLowResolutionCLUTs );
      PixInsightSettings::SetGlobalString( "ColorManagement/ProofingProfilePath", proofingPath );
      PixInsightSettings::SetGlobalInteger( "ColorManagement/ProofingIntent", proofingIntent );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/UseProofingBPC", useProofingBPC );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/DefaultProofingEnabled", defaultProofingEnabled );
      PixInsightSettings::SetGlobalFlag( "ColorManagement/DefaultGamutCheckEnabled", defaultGamutCheckEnabled );
      PixInsightSettings::SetGlobalColor( "ColorManagement/GamutWarningColor", gamutWarningColor );

      PixInsightSettings::EndUpdate();
      return true;
   }
   catch ( ... )
   {
      // ### Warning: Don't forget to do this, or the core will bite you!
      PixInsightSettings::CancelUpdate();
      throw;
   }
}

// ----------------------------------------------------------------------------

void* ColorManagementSetupInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheCMSEnabledParameter )
      return &enabled;
   if ( p == TheCMSUpdateMonitorProfileParameter )
      return updateMonitorProfile.Begin();
   if ( p == TheCMSDefaultRGBProfileParameter )
      return defaultRGBProfile.Begin();
   if ( p == TheCMSDefaultGrayProfileParameter )
      return defaultGrayProfile.Begin();
   if ( p == TheCMSDefaultRenderingIntentParameter )
      return &defaultRenderingIntent;
   if ( p == TheCMSOnProfileMismatchParameter )
      return &onProfileMismatch;
   if ( p == TheCMSOnMissingProfileParameter )
      return &onMissingProfile;
   if ( p == TheCMSDefaultEmbedProfilesInRGBImagesParameter )
      return &defaultEmbedProfilesInRGBImages;
   if ( p == TheCMSDefaultEmbedProfilesInGrayscaleImagesParameter )
      return &defaultEmbedProfilesInGrayscaleImages;
   if ( p == TheCMSUseLowResolutionCLUTsParameter )
      return &useLowResolutionCLUTs;
   if ( p == TheCMSProofingProfileParameter )
      return proofingProfile.Begin();
   if ( p == TheCMSProofingIntentParameter )
      return &proofingIntent;
   if ( p == TheCMSUseProofingBPCParameter )
      return &useProofingBPC;
   if ( p == TheCMSDefaultProofingEnabledParameter )
      return &defaultProofingEnabled;
   if ( p == TheCMSDefaultGamutCheckEnabledParameter )
      return &defaultGamutCheckEnabled;
   if ( p == TheCMSGamutWarningColorParameter )
      return &gamutWarningColor;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool ColorManagementSetupInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheCMSDefaultRGBProfileParameter )
   {
      defaultRGBProfile.Clear();
      if ( sizeOrLength != 0 )
         defaultRGBProfile.SetLength( sizeOrLength );
   }
   else if ( p == TheCMSDefaultGrayProfileParameter )
   {
      defaultGrayProfile.Clear();
      if ( sizeOrLength != 0 )
         defaultGrayProfile.SetLength( sizeOrLength );
   }
   else if ( p == TheCMSProofingProfileParameter )
   {
      proofingProfile.Clear();
      if ( sizeOrLength != 0 )
         proofingProfile.SetLength( sizeOrLength );
   }
   else if ( p == TheCMSUpdateMonitorProfileParameter )
   {
      updateMonitorProfile.Clear();
      if ( sizeOrLength != 0 )
         updateMonitorProfile.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type ColorManagementSetupInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheCMSDefaultRGBProfileParameter )
      return defaultRGBProfile.Length();
   if ( p == TheCMSDefaultGrayProfileParameter )
      return defaultGrayProfile.Length();
   if ( p == TheCMSProofingProfileParameter )
      return proofingProfile.Length();
   if ( p == TheCMSUpdateMonitorProfileParameter )
      return updateMonitorProfile.Length();

   return 0;
}

// ----------------------------------------------------------------------------

void ColorManagementSetupInstance::LoadCurrentSettings()
{
   enabled                  = PixInsightSettings::GlobalFlag( "ColorManagement/IsEnabled" );
   defaultRGBProfile        = ICCProfile( PixInsightSettings::GlobalString( "ColorManagement/DefaultRGBProfilePath" ) ).Description();
   defaultGrayProfile       = ICCProfile( PixInsightSettings::GlobalString( "ColorManagement/DefaultGrayscaleProfilePath" ) ).Description();
   defaultRenderingIntent   = PixInsightSettings::GlobalInteger( "ColorManagement/DefaultRenderingIntent" );
   onProfileMismatch        = PixInsightSettings::GlobalInteger( "ColorManagement/OnProfileMismatch" );
   onMissingProfile         = PixInsightSettings::GlobalInteger( "ColorManagement/OnMissingProfile" );
   defaultEmbedProfilesInRGBImages
                            = PixInsightSettings::GlobalFlag( "ColorManagement/DefaultEmbedProfilesInRGBImages" );
   defaultEmbedProfilesInGrayscaleImages
                            = PixInsightSettings::GlobalFlag( "ColorManagement/DefaultEmbedProfilesInGrayscaleImages" );
   useLowResolutionCLUTs    = PixInsightSettings::GlobalFlag( "ColorManagement/UseLowResolutionCLUTs" );
   proofingProfile          = ICCProfile( PixInsightSettings::GlobalString( "ColorManagement/ProofingProfilePath" ) ).Description();
   proofingIntent           = PixInsightSettings::GlobalInteger( "ColorManagement/ProofingIntent" );
   useProofingBPC           = PixInsightSettings::GlobalFlag( "ColorManagement/UseProofingBPC" );
   defaultProofingEnabled   = PixInsightSettings::GlobalFlag( "ColorManagement/DefaultProofingEnabled" );
   defaultGamutCheckEnabled = PixInsightSettings::GlobalFlag( "ColorManagement/DefaultGamutCheckEnabled" );
   gamutWarningColor        = PixInsightSettings::GlobalColor( "ColorManagement/GamutWarningColor" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorManagementSetupInstance.cpp - Released 2018-11-23T18:45:58Z
