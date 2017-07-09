//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.00.0319
// ----------------------------------------------------------------------------
// LocalNormalizationParameters.h - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#ifndef __LocalNormalizationParameters_h
#define __LocalNormalizationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class LNScale : public MetaInt32
{
public:

   LNScale( MetaProcess* );

   virtual IsoString Id() const;

   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
   virtual double DefaultValue() const;
};

extern LNScale* TheLNScaleParameter;

// ----------------------------------------------------------------------------

class LNRejection : public MetaBoolean
{
public:

   LNRejection( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern LNRejection* TheLNRejectionParameter;

// ----------------------------------------------------------------------------

class LNBackgroundRejectionLimit : public MetaFloat
{
public:

   LNBackgroundRejectionLimit( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern LNBackgroundRejectionLimit* TheLNBackgroundRejectionLimitParameter;

// ----------------------------------------------------------------------------

class LNReferenceRejectionThreshold : public MetaFloat
{
public:

   LNReferenceRejectionThreshold( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern LNReferenceRejectionThreshold* TheLNReferenceRejectionThresholdParameter;

// ----------------------------------------------------------------------------

class LNTargetRejectionThreshold : public MetaFloat
{
public:

   LNTargetRejectionThreshold( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern LNTargetRejectionThreshold* TheLNTargetRejectionThresholdParameter;

// ----------------------------------------------------------------------------

class LNReferencePathOrViewId : public MetaString
{
public:

   LNReferencePathOrViewId( MetaProcess* );

   virtual IsoString Id() const;
};

extern LNReferencePathOrViewId* TheLNReferencePathOrViewIdParameter;

// ----------------------------------------------------------------------------

class LNReferenceIsView : public MetaBoolean
{
public:

   LNReferenceIsView( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern LNReferenceIsView* TheLNReferenceIsViewParameter;

// ----------------------------------------------------------------------------

class LNTargetItems : public MetaTable
{
public:

   LNTargetItems( MetaProcess* );

   virtual IsoString Id() const;
};

extern LNTargetItems* TheLNTargetItemsParameter;

// ----------------------------------------------------------------------------

class LNTargetEnabled : public MetaBoolean
{
public:

   LNTargetEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern LNTargetEnabled* TheLNTargetEnabledParameter;

// ----------------------------------------------------------------------------

class LNTargetImage : public MetaString
{
public:

   LNTargetImage( MetaTable* );

   virtual IsoString Id() const;
};

extern LNTargetImage* TheLNTargetImageParameter;

// ----------------------------------------------------------------------------

class LNInputHints : public MetaString
{
public:

   LNInputHints( MetaProcess* );

   virtual IsoString Id() const;
};

extern LNInputHints* TheLNInputHintsParameter;

// ----------------------------------------------------------------------------

class LNOutputHints : public MetaString
{
public:

   LNOutputHints( MetaProcess* );

   virtual IsoString Id() const;
};

extern LNOutputHints* TheLNOutputHintsParameter;

// ----------------------------------------------------------------------------

class LNGenerateNormalizedImages : public MetaBoolean
{
public:

   LNGenerateNormalizedImages( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern LNGenerateNormalizedImages* TheLNGenerateNormalizedImagesParameter;

// ----------------------------------------------------------------------------

class LNGenerateNormalizationData : public MetaBoolean
{
public:

   LNGenerateNormalizationData( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern LNGenerateNormalizationData* TheLNGenerateNormalizationDataParameter;

// ----------------------------------------------------------------------------

class LNShowBackgroundModels : public MetaBoolean
{
public:

   LNShowBackgroundModels( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern LNShowBackgroundModels* TheLNShowBackgroundModelsParameter;

// ----------------------------------------------------------------------------

class LNShowRejectionMaps : public MetaBoolean
{
public:

   LNShowRejectionMaps( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern LNShowRejectionMaps* TheLNShowRejectionMapsParameter;

// ----------------------------------------------------------------------------

class LNShowNormalizationFunctions : public MetaBoolean
{
public:

   LNShowNormalizationFunctions( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern LNShowNormalizationFunctions* TheLNShowNormalizationFunctionsParameter;

// ----------------------------------------------------------------------------

class LNNoGUIMessages : public MetaBoolean
{
public:

   LNNoGUIMessages( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern LNNoGUIMessages* TheLNNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

class LNOutputDirectory : public MetaString
{
public:

   LNOutputDirectory( MetaProcess* );

   virtual IsoString Id() const;
};

extern LNOutputDirectory* TheLNOutputDirectoryParameter;

// ----------------------------------------------------------------------------

class LNOutputExtension : public MetaString
{
public:

   LNOutputExtension( MetaProcess* );

   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

extern LNOutputExtension* TheLNOutputExtensionParameter;

// ----------------------------------------------------------------------------

class LNOutputPrefix : public MetaString
{
public:

   LNOutputPrefix( MetaProcess* );

   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

extern LNOutputPrefix* TheLNOutputPrefixParameter;

// ----------------------------------------------------------------------------

class LNOutputPostfix : public MetaString
{
public:

   LNOutputPostfix( MetaProcess* );

   virtual IsoString Id() const;
   virtual String DefaultValue() const;
};

extern LNOutputPostfix* TheLNOutputPostfixParameter;

// ----------------------------------------------------------------------------

class LNOverwriteExistingFiles : public MetaBoolean
{
public:

   LNOverwriteExistingFiles( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern LNOverwriteExistingFiles* TheLNOverwriteExistingFilesParameter;

// ----------------------------------------------------------------------------

class LNOnError : public MetaEnumeration
{
public:

   enum { Continue,
          Abort,
          AskUser,
          NumberOfErrorPolicies,
          Default = Continue };

   LNOnError( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern LNOnError* TheLNOnErrorParameter;

// ----------------------------------------------------------------------------

class LNUseFileThreads : public MetaBoolean
{
public:

   LNUseFileThreads( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern LNUseFileThreads* TheLNUseFileThreadsParameter;

// ----------------------------------------------------------------------------

class LNFileThreadOverload : public MetaFloat
{
public:

   LNFileThreadOverload( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern LNFileThreadOverload* TheLNFileThreadOverloadParameter;

// ----------------------------------------------------------------------------

class LNMaxFileReadThreads : public MetaInt32
{
public:

   LNMaxFileReadThreads( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern LNMaxFileReadThreads* TheLNMaxFileReadThreadsParameter;

// ----------------------------------------------------------------------------

class LNMaxFileWriteThreads : public MetaInt32
{
public:

   LNMaxFileWriteThreads( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern LNMaxFileWriteThreads* TheLNMaxFileWriteThreadsParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __LocalNormalizationParameters_h

// ----------------------------------------------------------------------------
// EOF LocalNormalizationParameters.h - Released 2017-07-09T18:07:33Z
