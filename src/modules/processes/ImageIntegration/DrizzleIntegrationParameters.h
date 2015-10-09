//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.09.04.0282
// ----------------------------------------------------------------------------
// DrizzleIntegrationParameters.h - Released 2015/10/08 11:24:40 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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

#ifndef __DrizzleIntegrationParameters_h
#define __DrizzleIntegrationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class DZInputData : public MetaTable
{
public:

   DZInputData( MetaProcess* );

   virtual IsoString Id() const;
};

extern DZInputData* TheDZInputDataParameter;

// ----------------------------------------------------------------------------

class DZItemEnabled : public MetaBoolean
{
public:

   DZItemEnabled( MetaTable* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DZItemEnabled* TheDZItemEnabledParameter;

// ----------------------------------------------------------------------------

class DZItemPath : public MetaString
{
public:

   DZItemPath( MetaTable* );

   virtual IsoString Id() const;
};

extern DZItemPath* TheDZItemPathParameter;

// ----------------------------------------------------------------------------

class DZInputHints : public MetaString
{
public:

   DZInputHints( MetaProcess* );

   virtual IsoString Id() const;
};

extern DZInputHints* TheDZInputHintsParameter;

// ----------------------------------------------------------------------------

class DZInputDirectory : public MetaString
{
public:

   DZInputDirectory( MetaProcess* );

   virtual IsoString Id() const;
};

extern DZInputDirectory* TheDZInputDirectoryParameter;

// ----------------------------------------------------------------------------

class DZScale : public MetaFloat
{
public:

   DZScale( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DZScale* TheDZScaleParameter;

// ----------------------------------------------------------------------------

class DZDropShrink : public MetaFloat
{
public:

   DZDropShrink( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DZDropShrink* TheDZDropShrinkParameter;

// ----------------------------------------------------------------------------

class DZEnableRejection : public MetaBoolean
{
public:

   DZEnableRejection( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DZEnableRejection* TheDZEnableRejectionParameter;

// ----------------------------------------------------------------------------

class DZEnableImageWeighting : public MetaBoolean
{
public:

   DZEnableImageWeighting( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DZEnableImageWeighting* TheDZEnableImageWeightingParameter;

// ----------------------------------------------------------------------------

class DZEnableSurfaceSplines : public MetaBoolean
{
public:

   DZEnableSurfaceSplines( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DZEnableSurfaceSplines* TheDZEnableSurfaceSplinesParameter;

// ----------------------------------------------------------------------------

class DZUseROI : public MetaBoolean
{
public:

   DZUseROI( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DZUseROI* TheDZUseROIParameter;

// ----------------------------------------------------------------------------

class DZROIX0 : public MetaInt32
{
public:

   DZROIX0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DZROIX0* TheDZROIX0Parameter;

// ----------------------------------------------------------------------------

class DZROIY0 : public MetaInt32
{
public:

   DZROIY0( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DZROIY0* TheDZROIY0Parameter;

// ----------------------------------------------------------------------------

class DZROIX1 : public MetaInt32
{
public:

   DZROIX1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DZROIX1* TheDZROIX1Parameter;

// ----------------------------------------------------------------------------

class DZROIY1 : public MetaInt32
{
public:

   DZROIY1( MetaProcess* );

   virtual IsoString Id() const;
   virtual double DefaultValue() const;
   virtual double MinimumValue() const;
   virtual double MaximumValue() const;
};

extern DZROIY1* TheDZROIY1Parameter;

// ----------------------------------------------------------------------------

class DZClosePreviousImages : public MetaBoolean
{
public:

   DZClosePreviousImages( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DZClosePreviousImages* TheDZClosePreviousImagesParameter;

// ----------------------------------------------------------------------------

class DZNoGUIMessages : public MetaBoolean
{
public:

   DZNoGUIMessages( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool DefaultValue() const;
};

extern DZNoGUIMessages* TheDZNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

class DZOnError : public MetaEnumeration
{
public:

   enum { Continue,
          Abort,
          AskUser,
          NumberOfErrorPolicies,
          Default = Continue };

   DZOnError( MetaProcess* );

   virtual IsoString Id() const;
   virtual size_type NumberOfElements() const;
   virtual IsoString ElementId( size_type ) const;
   virtual int ElementValue( size_type ) const;
   virtual size_type DefaultValueIndex() const;
};

extern DZOnError* TheDZOnErrorParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Output properties
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DZIntegrationImageId : public MetaString
{
public:

   DZIntegrationImageId( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZIntegrationImageId* TheDZIntegrationImageIdParameter;

// ----------------------------------------------------------------------------

class DZWeightImageId : public MetaString
{
public:

   DZWeightImageId( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZWeightImageId* TheDZWeightImageIdParameter;

// ----------------------------------------------------------------------------

class DZNumberOfChannels : public MetaInt32
{
public:

   DZNumberOfChannels( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZNumberOfChannels* TheDZNumberOfChannelsParameter;

// ----------------------------------------------------------------------------

class DZOutputPixels : public MetaUInt64
{
public:

   DZOutputPixels( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZOutputPixels* TheDZOutputPixelsParameter;

// ----------------------------------------------------------------------------

class DZIntegratedPixels : public MetaUInt64
{
public:

   DZIntegratedPixels( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZIntegratedPixels* TheDZIntegratedPixelsParameter;

// ----------------------------------------------------------------------------

class DZTotalRejectedLowRK : public MetaUInt64
{
public:

   DZTotalRejectedLowRK( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZTotalRejectedLowRK* TheDZTotalRejectedLowRKParameter;

class DZTotalRejectedLowG : public MetaUInt64
{
public:

   DZTotalRejectedLowG( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZTotalRejectedLowG* TheDZTotalRejectedLowGParameter;

class DZTotalRejectedLowB : public MetaUInt64
{
public:

   DZTotalRejectedLowB( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZTotalRejectedLowB* TheDZTotalRejectedLowBParameter;

// ----------------------------------------------------------------------------

class DZTotalRejectedHighRK : public MetaUInt64
{
public:

   DZTotalRejectedHighRK( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZTotalRejectedHighRK* TheDZTotalRejectedHighRKParameter;

class DZTotalRejectedHighG : public MetaUInt64
{
public:

   DZTotalRejectedHighG( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZTotalRejectedHighG* TheDZTotalRejectedHighGParameter;

class DZTotalRejectedHighB : public MetaUInt64
{
public:

   DZTotalRejectedHighB( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZTotalRejectedHighB* TheDZTotalRejectedHighBParameter;

// ----------------------------------------------------------------------------

class DZOutputData : public MetaFloat
{
public:

   DZOutputData( MetaProcess* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DZOutputData* TheDZOutputDataParameter;

// ----------------------------------------------------------------------------

class DZImageData : public MetaTable
{
public:

   DZImageData( MetaProcess* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZImageData* TheDZImageDataParameter;

// ----------------------------------------------------------------------------

class DZImageFilePath : public MetaString
{
public:

   DZImageFilePath( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZImageFilePath* TheDZImageFilePathParameter;

// ----------------------------------------------------------------------------

class DZImageWeightRK : public MetaFloat
{
public:

   DZImageWeightRK( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DZImageWeightRK* TheDZImageWeightRKParameter;

class DZImageWeightG : public MetaFloat
{
public:

   DZImageWeightG( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DZImageWeightG* TheDZImageWeightGParameter;

class DZImageWeightB : public MetaFloat
{
public:

   DZImageWeightB( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DZImageWeightB* TheDZImageWeightBParameter;

// ----------------------------------------------------------------------------

class DZImageLocationRK : public MetaDouble
{
public:

   DZImageLocationRK( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DZImageLocationRK* TheDZImageLocationRKParameter;

class DZImageLocationG : public MetaDouble
{
public:

   DZImageLocationG( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DZImageLocationG* TheDZImageLocationGParameter;

class DZImageLocationB : public MetaDouble
{
public:

   DZImageLocationB( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DZImageLocationB* TheDZImageLocationBParameter;

// ----------------------------------------------------------------------------

class DZImageReferenceLocationRK : public MetaDouble
{
public:

   DZImageReferenceLocationRK( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DZImageReferenceLocationRK* TheDZImageReferenceLocationRKParameter;

class DZImageReferenceLocationG : public MetaDouble
{
public:

   DZImageReferenceLocationG( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DZImageReferenceLocationG* TheDZImageReferenceLocationGParameter;

class DZImageReferenceLocationB : public MetaDouble
{
public:

   DZImageReferenceLocationB( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DZImageReferenceLocationB* TheDZImageReferenceLocationBParameter;

// ----------------------------------------------------------------------------

class DZImageScaleRK : public MetaDouble
{
public:

   DZImageScaleRK( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DZImageScaleRK* TheDZImageScaleRKParameter;

class DZImageScaleG : public MetaDouble
{
public:

   DZImageScaleG( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DZImageScaleG* TheDZImageScaleGParameter;

class DZImageScaleB : public MetaDouble
{
public:

   DZImageScaleB( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool ScientificNotation() const;
   virtual bool IsReadOnly() const;
};

extern DZImageScaleB* TheDZImageScaleBParameter;

// ----------------------------------------------------------------------------

class DZImageRejectedLowRK : public MetaUInt64
{
public:

   DZImageRejectedLowRK( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZImageRejectedLowRK* TheDZImageRejectedLowRKParameter;

class DZImageRejectedLowG : public MetaUInt64
{
public:

   DZImageRejectedLowG( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZImageRejectedLowG* TheDZImageRejectedLowGParameter;

class DZImageRejectedLowB : public MetaUInt64
{
public:

   DZImageRejectedLowB( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZImageRejectedLowB* TheDZImageRejectedLowBParameter;

// ----------------------------------------------------------------------------

class DZImageRejectedHighRK : public MetaUInt64
{
public:

   DZImageRejectedHighRK( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZImageRejectedHighRK* TheDZImageRejectedHighRKParameter;

class DZImageRejectedHighG : public MetaUInt64
{
public:

   DZImageRejectedHighG( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZImageRejectedHighG* TheDZImageRejectedHighGParameter;

class DZImageRejectedHighB : public MetaUInt64
{
public:

   DZImageRejectedHighB( MetaTable* );

   virtual IsoString Id() const;
   virtual bool IsReadOnly() const;
};

extern DZImageRejectedHighB* TheDZImageRejectedHighBParameter;

// ----------------------------------------------------------------------------

class DZImageOutputData : public MetaFloat
{
public:

   DZImageOutputData( MetaTable* );

   virtual IsoString Id() const;
   virtual int Precision() const;
   virtual bool IsReadOnly() const;
};

extern DZImageOutputData* TheDZImageOutputDataParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __DrizzleIntegrationParameters_h

// ----------------------------------------------------------------------------
// EOF DrizzleIntegrationParameters.h - Released 2015/10/08 11:24:40 UTC
