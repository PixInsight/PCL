//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.06.0288
// ----------------------------------------------------------------------------
// ColorManagementSetupParameters.cpp - Released 2015/10/08 11:24:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#include "ColorManagementSetupParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CMSEnabled*                               TheCMSEnabledParameter = 0;
CMSUpdateMonitorProfile*                  TheCMSUpdateMonitorProfileParameter = 0;
CMSDefaultRGBProfile*                     TheCMSDefaultRGBProfileParameter = 0;
CMSDefaultGrayProfile*                    TheCMSDefaultGrayProfileParameter = 0;
CMSDefaultRenderingIntent*                TheCMSDefaultRenderingIntentParameter = 0;
CMSOnProfileMismatch*                     TheCMSOnProfileMismatchParameter = 0;
CMSOnMissingProfile*                      TheCMSOnMissingProfileParameter = 0;
CMSDefaultEmbedProfilesInRGBImages*       TheCMSDefaultEmbedProfilesInRGBImagesParameter = 0;
CMSDefaultEmbedProfilesInGrayscaleImages* TheCMSDefaultEmbedProfilesInGrayscaleImagesParameter = 0;
CMSUseLowResolutionCLUTs*                 TheCMSUseLowResolutionCLUTsParameter = 0;
CMSProofingProfile*                       TheCMSProofingProfileParameter = 0;
CMSProofingIntent*                        TheCMSProofingIntentParameter = 0;
CMSUseProofingBPC*                        TheCMSUseProofingBPCParameter = 0;
CMSDefaultProofingEnabled*                TheCMSDefaultProofingEnabledParameter = 0;
CMSDefaultGamutCheckEnabled*              TheCMSDefaultGamutCheckEnabledParameter = 0;
CMSGamutWarningColor*                     TheCMSGamutWarningColorParameter = 0;

// ----------------------------------------------------------------------------

CMSRenderingIntent::CMSRenderingIntent( MetaProcess* P ) : MetaEnumeration( P )
{
}

size_type CMSRenderingIntent::NumberOfElements() const
{
   return NumberOfRenderingIntents;
}

IsoString CMSRenderingIntent::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Perceptual:           return "Perceptual";
   case Saturation:           return "Saturation";
   case RelativeColorimetric: return "RelativeColorimetric";
   case AbsoluteColorimetric: return "AbsoluteColorimetric";
   }
}

int CMSRenderingIntent::ElementValue( size_type i ) const
{
   return int( i );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CMSEnabled::CMSEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSEnabledParameter = this;
}

IsoString CMSEnabled::Id() const
{
   return "enabled";
}

bool CMSEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CMSUpdateMonitorProfile::CMSUpdateMonitorProfile( MetaProcess* P ) : MetaString( P )
{
   TheCMSUpdateMonitorProfileParameter = this;
}

IsoString CMSUpdateMonitorProfile::Id() const
{
   return "updateMonitorProfile";
}

// ----------------------------------------------------------------------------

CMSDefaultRGBProfile::CMSDefaultRGBProfile( MetaProcess* P ) : MetaString( P )
{
   TheCMSDefaultRGBProfileParameter = this;
}

IsoString CMSDefaultRGBProfile::Id() const
{
   return "defaultRGBProfile";
}

// ----------------------------------------------------------------------------

CMSDefaultGrayProfile::CMSDefaultGrayProfile( MetaProcess* P ) : MetaString( P )
{
   TheCMSDefaultGrayProfileParameter = this;
}

IsoString CMSDefaultGrayProfile::Id() const
{
   return "defaultGrayscaleProfile";
}

// ----------------------------------------------------------------------------

CMSDefaultRenderingIntent::CMSDefaultRenderingIntent( MetaProcess* P ) : CMSRenderingIntent( P )
{
   TheCMSDefaultRenderingIntentParameter = this;
}

IsoString CMSDefaultRenderingIntent::Id() const
{
   return "defaultRenderingIntent";
}

size_type CMSDefaultRenderingIntent::DefaultValueIndex() const
{
   return DefaultForScreen;
}

// ----------------------------------------------------------------------------

CMSOnProfileMismatch::CMSOnProfileMismatch( MetaProcess* P ) : MetaEnumeration( P )
{
   TheCMSOnProfileMismatchParameter = this;
}

IsoString CMSOnProfileMismatch::Id() const
{
   return "onProfileMismatch";
}

size_type CMSOnProfileMismatch::NumberOfElements() const
{
   return NumberOfOnProfileMismatchPolicies;
}

IsoString CMSOnProfileMismatch::ElementId( size_type i ) const
{
   switch ( i )
   {
   case AskUser:           return "AskUser";
   default:
   case KeepEmbedded:      return "KeepEmbedded";
   case ConvertToDefault:  return "ConvertToDefault";
   case DiscardEmbedded:   return "DiscardEmbedded";
   case DisableCM:         return "DisableCM";
   }
}

int CMSOnProfileMismatch::ElementValue( size_type i ) const
{
   return int( i );
}

size_type CMSOnProfileMismatch::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

CMSOnMissingProfile::CMSOnMissingProfile( MetaProcess* P ) : MetaEnumeration( P )
{
   TheCMSOnMissingProfileParameter = this;
}

IsoString CMSOnMissingProfile::Id() const
{
   return "onMissingProfile";
}

size_type CMSOnMissingProfile::NumberOfElements() const
{
   return NumberOfOnMissingProfilePolicies;
}

IsoString CMSOnMissingProfile::ElementId( size_type i ) const
{
   switch ( i )
   {
   case AskUser:        return "AskUser";
   default:
   case AssignDefault:  return "AssignDefault";
   case LeaveUntagged:  return "LeaveUntagged";
   case DisableCM:      return "DisableCM";
   }
}

int CMSOnMissingProfile::ElementValue( size_type i ) const
{
   return int( i );
}

size_type CMSOnMissingProfile::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

CMSDefaultEmbedProfilesInRGBImages::CMSDefaultEmbedProfilesInRGBImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSDefaultEmbedProfilesInRGBImagesParameter = this;
}

IsoString CMSDefaultEmbedProfilesInRGBImages::Id() const
{
   return "defaultEmbedProfilesInRGBImages";
}

bool CMSDefaultEmbedProfilesInRGBImages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CMSDefaultEmbedProfilesInGrayscaleImages::CMSDefaultEmbedProfilesInGrayscaleImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSDefaultEmbedProfilesInGrayscaleImagesParameter = this;
}

IsoString CMSDefaultEmbedProfilesInGrayscaleImages::Id() const
{
   return "defaultEmbedProfilesInGrayscaleImages";
}

bool CMSDefaultEmbedProfilesInGrayscaleImages::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CMSUseLowResolutionCLUTs::CMSUseLowResolutionCLUTs( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSUseLowResolutionCLUTsParameter = this;
}

IsoString CMSUseLowResolutionCLUTs::Id() const
{
   return "useLowResolutionCLUTs";
}

bool CMSUseLowResolutionCLUTs::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CMSProofingProfile::CMSProofingProfile( MetaProcess* P ) : MetaString( P )
{
   TheCMSProofingProfileParameter = this;
}

IsoString CMSProofingProfile::Id() const
{
   return "proofingProfile";
}

// ----------------------------------------------------------------------------

CMSProofingIntent::CMSProofingIntent( MetaProcess* P ) : CMSRenderingIntent( P )
{
   TheCMSProofingIntentParameter = this;
}

IsoString CMSProofingIntent::Id() const
{
   return "proofingIntent";
}

size_type CMSProofingIntent::DefaultValueIndex() const
{
   return DefaultForProofing;
}

// ----------------------------------------------------------------------------

CMSUseProofingBPC::CMSUseProofingBPC( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSUseProofingBPCParameter = this;
}

IsoString CMSUseProofingBPC::Id() const
{
   return "useProofingBPC";
}

bool CMSUseProofingBPC::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CMSDefaultProofingEnabled::CMSDefaultProofingEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSDefaultProofingEnabledParameter = this;
}

IsoString CMSDefaultProofingEnabled::Id() const
{
   return "defaultProofingEnabled";
}

bool CMSDefaultProofingEnabled::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CMSDefaultGamutCheckEnabled::CMSDefaultGamutCheckEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSDefaultGamutCheckEnabledParameter = this;
}

IsoString CMSDefaultGamutCheckEnabled::Id() const
{
   return "defaultGamutCheckEnabled";
}

bool CMSDefaultGamutCheckEnabled::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CMSGamutWarningColor::CMSGamutWarningColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheCMSGamutWarningColorParameter = this;
}

IsoString CMSGamutWarningColor::Id() const
{
   return "gamutWarningColor";
}

double CMSGamutWarningColor::DefaultValue() const
{
   return 0xffa9a9a9; // Dark Gray
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorManagementSetupParameters.cpp - Released 2015/10/08 11:24:39 UTC
