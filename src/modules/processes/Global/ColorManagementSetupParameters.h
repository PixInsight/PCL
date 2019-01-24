//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.08.0405
// ----------------------------------------------------------------------------
// ColorManagementSetupParameters.h - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __ColorManagementSetupParameters_h
#define __ColorManagementSetupParameters_h

#include <pcl/ICCProfileTransformation.h>
#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CMSRenderingIntent : public MetaEnumeration
{
public:

   enum { Perceptual = ICCRenderingIntent::Perceptual,
          Saturation = ICCRenderingIntent::Saturation,
          RelativeColorimetric = ICCRenderingIntent::RelativeColorimetric,
          AbsoluteColorimetric = ICCRenderingIntent::AbsoluteColorimetric,
          NumberOfRenderingIntents,
          DefaultForScreen = Perceptual,
          DefaultForProofing = RelativeColorimetric };

   CMSRenderingIntent( MetaProcess* );

   virtual IsoString Id() const = 0;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
};

// ----------------------------------------------------------------------------

class CMSEnabled : public MetaBoolean
{
public:

   CMSEnabled( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern CMSEnabled* TheCMSEnabledParameter;

// ----------------------------------------------------------------------------

class CMSUpdateMonitorProfile : public MetaString
{
public:

   CMSUpdateMonitorProfile( MetaProcess* );

   virtual IsoString Id() const;
};

extern CMSUpdateMonitorProfile* TheCMSUpdateMonitorProfileParameter;

// ----------------------------------------------------------------------------

class CMSDefaultRGBProfile : public MetaString
{
public:

   CMSDefaultRGBProfile( MetaProcess* );

   virtual IsoString Id() const;
};

extern CMSDefaultRGBProfile* TheCMSDefaultRGBProfileParameter;

// ----------------------------------------------------------------------------

class CMSDefaultGrayProfile : public MetaString
{
public:

   CMSDefaultGrayProfile( MetaProcess* );

   virtual IsoString Id() const;
};

extern CMSDefaultGrayProfile* TheCMSDefaultGrayProfileParameter;

// ----------------------------------------------------------------------------

class CMSDefaultRenderingIntent : public CMSRenderingIntent
{
public:

   CMSDefaultRenderingIntent( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type DefaultValueIndex() const;
};

extern CMSDefaultRenderingIntent* TheCMSDefaultRenderingIntentParameter;

// ----------------------------------------------------------------------------

class CMSOnProfileMismatch : public MetaEnumeration
{
public:

   enum { AskUser,
          KeepEmbedded,
          ConvertToDefault,
          DiscardEmbedded,
          DisableCM,
          NumberOfOnProfileMismatchPolicies,
          Default = KeepEmbedded };

   CMSOnProfileMismatch( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern CMSOnProfileMismatch* TheCMSOnProfileMismatchParameter;

// ----------------------------------------------------------------------------

class CMSOnMissingProfile : public MetaEnumeration
{
public:

   enum { AskUser,
          AssignDefault,
          LeaveUntagged,
          DisableCM,
          NumberOfOnMissingProfilePolicies,
          Default = LeaveUntagged };

   CMSOnMissingProfile( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern CMSOnMissingProfile* TheCMSOnMissingProfileParameter;

// ----------------------------------------------------------------------------

class CMSDefaultEmbedProfilesInRGBImages : public MetaBoolean
{
public:

   CMSDefaultEmbedProfilesInRGBImages( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern CMSDefaultEmbedProfilesInRGBImages* TheCMSDefaultEmbedProfilesInRGBImagesParameter;

// ----------------------------------------------------------------------------

class CMSDefaultEmbedProfilesInGrayscaleImages : public MetaBoolean
{
public:

   CMSDefaultEmbedProfilesInGrayscaleImages( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern CMSDefaultEmbedProfilesInGrayscaleImages* TheCMSDefaultEmbedProfilesInGrayscaleImagesParameter;

// ----------------------------------------------------------------------------

class CMSUseLowResolutionCLUTs : public MetaBoolean
{
public:

   CMSUseLowResolutionCLUTs( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern CMSUseLowResolutionCLUTs* TheCMSUseLowResolutionCLUTsParameter;

// ----------------------------------------------------------------------------

class CMSProofingProfile : public MetaString
{
public:

   CMSProofingProfile( MetaProcess* );

   virtual IsoString Id() const;
};

extern CMSProofingProfile* TheCMSProofingProfileParameter;

// ----------------------------------------------------------------------------

class CMSProofingIntent : public CMSRenderingIntent
{
public:

   CMSProofingIntent( MetaProcess* );

   virtual IsoString Id() const;

   virtual size_type DefaultValueIndex() const;
};

extern CMSProofingIntent* TheCMSProofingIntentParameter;

// ----------------------------------------------------------------------------

class CMSUseProofingBPC : public MetaBoolean
{
public:

   CMSUseProofingBPC( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern CMSUseProofingBPC* TheCMSUseProofingBPCParameter;

// ----------------------------------------------------------------------------

class CMSDefaultProofingEnabled : public MetaBoolean
{
public:

   CMSDefaultProofingEnabled( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern CMSDefaultProofingEnabled* TheCMSDefaultProofingEnabledParameter;

// ----------------------------------------------------------------------------

class CMSDefaultGamutCheckEnabled : public MetaBoolean
{
public:

   CMSDefaultGamutCheckEnabled( MetaProcess* );

   virtual IsoString Id() const;

   virtual bool DefaultValue() const;
};

extern CMSDefaultGamutCheckEnabled* TheCMSDefaultGamutCheckEnabledParameter;

// ----------------------------------------------------------------------------

class CMSGamutWarningColor : public MetaUInt32
{
public:

   CMSGamutWarningColor( MetaProcess* );

   virtual IsoString Id() const;

   virtual double DefaultValue() const;
};

extern CMSGamutWarningColor* TheCMSGamutWarningColorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ColorManagementSetupParameters_h

// ----------------------------------------------------------------------------
// EOF ColorManagementSetupParameters.h - Released 2019-01-21T12:06:41Z
