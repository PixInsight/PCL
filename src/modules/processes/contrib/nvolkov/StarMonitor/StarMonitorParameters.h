// ****************************************************************************
// PixInsight Class Library - PCL 02.01.00.0779
// Standard StarMonitor Process Module Version 01.00.05.0050
// ****************************************************************************
// StarMonitorParameters.h - Released 2016/01/14 19:32:59 UTC
// ****************************************************************************
// This file is part of the standard StarMonitor PixInsight module.
//
// Copyright (c) 2003-2016, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef __StarMonitorParameters_h
#define __StarMonitorParameters_h

#include <pcl/MetaParameter.h>

namespace pcl {

   PCL_BEGIN_LOCAL

   // ----------------------------------------------------------------------------

   class SMStructureLayers : public MetaInt32 {
   public:

      SMStructureLayers(MetaProcess*);

      virtual IsoString Id() const;
      virtual double DefaultValue() const;
      virtual double MinimumValue() const;
      virtual double MaximumValue() const;
   };

   extern SMStructureLayers* TheSMStructureLayersParameter;

   // ----------------------------------------------------------------------------

   class SMNoiseLayers : public MetaInt32 {
   public:

      SMNoiseLayers(MetaProcess*);

      virtual IsoString Id() const;
      virtual double DefaultValue() const;
      virtual double MinimumValue() const;
      virtual double MaximumValue() const;
   };

   extern SMNoiseLayers* TheSMNoiseLayersParameter;

   // ----------------------------------------------------------------------------

   class SMHotPixelFilterRadius : public MetaInt32 {
   public:

      SMHotPixelFilterRadius(MetaProcess*);

      virtual IsoString Id() const;
      virtual double DefaultValue() const;
      virtual double MinimumValue() const;
      virtual double MaximumValue() const;
   };

   extern SMHotPixelFilterRadius* TheSMHotPixelFilterRadiusParameter;

   // ----------------------------------------------------------------------------

   class SMSensitivity : public MetaFloat {
   public:

      SMSensitivity(MetaProcess*);

      virtual IsoString Id() const;
      virtual int Precision() const;
      virtual double DefaultValue() const;
      virtual double MinimumValue() const;
      virtual double MaximumValue() const;
   };

   extern SMSensitivity* TheSMSensitivityParameter;

   // ----------------------------------------------------------------------------

   class SMPeakResponse : public MetaFloat {
   public:

      SMPeakResponse(MetaProcess*);

      virtual IsoString Id() const;
      virtual int Precision() const;
      virtual double DefaultValue() const;
      virtual double MinimumValue() const;
      virtual double MaximumValue() const;
   };

   extern SMPeakResponse* TheSMPeakResponseParameter;

   // ----------------------------------------------------------------------------

   class SMMaxStarDistortion : public MetaFloat {
   public:

      SMMaxStarDistortion(MetaProcess*);

      virtual IsoString Id() const;
      virtual int Precision() const;
      virtual double DefaultValue() const;
      virtual double MinimumValue() const;
      virtual double MaximumValue() const;
   };

   extern SMMaxStarDistortion* TheSMMaxStarDistortionParameter;

   // ----------------------------------------------------------------------------

   class SMInvert : public MetaBoolean {
   public:
      SMInvert(MetaProcess*);
      virtual IsoString Id() const;
      virtual bool DefaultValue() const;
   };
   extern SMInvert* TheSMInvertParameter;

   // ----------------------------------------------------------------------------

   class SMMonitoredFolder : public MetaString {
   public:
      SMMonitoredFolder(MetaProcess*);
      virtual IsoString Id() const;
   };
   extern SMMonitoredFolder* TheSMMonitoredFolderParameter;

   // ----------------------------------------------------------------------------

   class SMTargetItems : public MetaTable {
   public:
      SMTargetItems(MetaProcess*);
      virtual IsoString Id() const;
   };
   extern SMTargetItems* TheSMTargetItemsParameter;

   // ----------------------------------------------------------------------------

   class SMTargetImage : public MetaString {
   public:
      SMTargetImage(MetaTable*);
      virtual IsoString Id() const;
   };
   extern SMTargetImage* TheSMTargetImageParameter;

   // ----------------------------------------------------------------------------

   class SMStarQuantity : public MetaUInt32 {
   public:
      SMStarQuantity(MetaTable*);
      virtual IsoString Id() const;
   };
   extern SMStarQuantity* TheSMStarQuantityParameter;

   // ----------------------------------------------------------------------------

   class SMBackground : public MetaDouble {
   public:
      SMBackground(MetaTable*);
      virtual IsoString Id() const;
      virtual int Precision() const;
   };
   extern SMBackground* TheSMBackgroundParameter;

   // ----------------------------------------------------------------------------

   class SMAmplitude : public MetaDouble {
   public:
      SMAmplitude(MetaTable*);
      virtual IsoString Id() const;
      virtual int Precision() const;
   };
   extern SMAmplitude* TheSMAmplitudeParameter;

   // ----------------------------------------------------------------------------

   class SMRadiusX : public MetaDouble {
   public:
      SMRadiusX(MetaTable*);
      virtual IsoString Id() const;
      virtual int Precision() const;
   };
   extern SMRadiusX* TheSMRadiusXParameter;

   // ----------------------------------------------------------------------------

   class SMRadiusY : public MetaDouble {
   public:
      SMRadiusY(MetaTable*);
      virtual IsoString Id() const;
      virtual int Precision() const;
   };
   extern SMRadiusY* TheSMRadiusYParameter;

   // ----------------------------------------------------------------------------

   class SMFWHMx : public MetaDouble {
   public:
      SMFWHMx(MetaTable*);
      virtual IsoString Id() const;
      virtual int Precision() const;
   };
   extern SMFWHMx* TheSMFWHMxParameter;

   // ----------------------------------------------------------------------------

   class SMFWHMy : public MetaDouble {
   public:
      SMFWHMy(MetaTable*);
      virtual IsoString Id() const;
      virtual int Precision() const;
   };
   extern SMFWHMy* TheSMFWHMyParameter;

   // ----------------------------------------------------------------------------

   class SMRotationAngle : public MetaDouble {
   public:
      SMRotationAngle(MetaTable*);
      virtual IsoString Id() const;
      virtual int Precision() const;
   };
   extern SMRotationAngle* TheSMRotationAngleParameter;

   // ----------------------------------------------------------------------------

   class SMMAD : public MetaDouble {
   public:
      SMMAD(MetaTable*);
      virtual IsoString Id() const;
      virtual int Precision() const;
   };
   extern SMMAD* TheSMMADParameter;

   // ----------------------------------------------------------------------------

   class SMxScale : public MetaFloat {
   public:
      SMxScale(MetaTable*);
      virtual IsoString Id() const;
      virtual int Precision() const;
   };
   extern SMxScale* TheSMxScaleParameter;

   // ----------------------------------------------------------------------------

   class SMyScale : public MetaFloat {
   public:
      SMyScale(MetaTable*);
      virtual IsoString Id() const;
      virtual int Precision() const;
   };
   extern SMyScale* TheSMyScaleParameter;

   // ----------------------------------------------------------------------------

   class SMInputHints : public MetaString {
   public:
      SMInputHints(MetaProcess*);
      virtual IsoString Id() const;
   };
   extern SMInputHints* TheSMInputHintsParameter;

   // ----------------------------------------------------------------------------

   class SMScaleMode : public MetaEnumeration {
   public:

      enum {
         Pixels,
         StandardKeywords,
         LiteralValue,
         NumberOfItems,
         Default = Pixels
      };

      SMScaleMode(MetaProcess*);

      virtual IsoString Id() const;

      virtual size_type NumberOfElements() const;
      virtual IsoString ElementId(size_type) const;
      virtual int ElementValue(size_type) const;
      virtual size_type DefaultValueIndex() const;
   };

   extern SMScaleMode* TheSMScaleModeParameter;

   // ----------------------------------------------------------------------------

   class SMScaleValue : public MetaFloat {
   public:

      SMScaleValue(MetaProcess*);

      virtual IsoString Id() const;
      virtual int Precision() const;
      virtual double DefaultValue() const;
      virtual double MinimumValue() const;
      virtual double MaximumValue() const;
   };

   extern SMScaleValue* TheSMScaleValueParameter;

   // ----------------------------------------------------------------------------

   class SMAlertCheck : public MetaBoolean {
   public:
      SMAlertCheck(MetaProcess*);
      virtual IsoString Id() const;
      virtual bool DefaultValue() const;
   };
   extern SMAlertCheck* TheSMAlertCheckParameter;

   // ----------------------------------------------------------------------------

   class SMAlertExecute : public MetaString {
   public:
      SMAlertExecute(MetaProcess*);
      virtual IsoString Id() const;
      virtual String DefaultValue() const;
   };
   extern SMAlertExecute* TheSMAlertExecuteParameter;

   // ----------------------------------------------------------------------------

   class SMAlertArguments : public MetaString {
   public:
      SMAlertArguments(MetaProcess*);
      virtual IsoString Id() const;
      virtual String DefaultValue() const;
   };
   extern SMAlertArguments* TheSMAlertArgumentsParameter;

   // ----------------------------------------------------------------------------

   class SMAlertFWHMxCheck : public MetaBoolean {
   public:
      SMAlertFWHMxCheck(MetaProcess*);
      virtual IsoString Id() const;
      virtual bool DefaultValue() const;
   };
   extern SMAlertFWHMxCheck* TheSMAlertFWHMxCheckParameter;

   // ----------------------------------------------------------------------------

   class SMAlertFWHMx : public MetaFloat {
   public:
      SMAlertFWHMx(MetaProcess*);
      virtual IsoString Id() const;
      virtual int Precision() const;
      virtual double DefaultValue() const;
      virtual double MinimumValue() const;
      virtual double MaximumValue() const;
   };
   extern SMAlertFWHMx* TheSMAlertFWHMxParameter;

   // ----------------------------------------------------------------------------

   class SMAlertRoundnessCheck : public MetaBoolean {
   public:
      SMAlertRoundnessCheck(MetaProcess*);
      virtual IsoString Id() const;
      virtual bool DefaultValue() const;
   };
   extern SMAlertRoundnessCheck* TheSMAlertRoundnessCheckParameter;

   // ----------------------------------------------------------------------------

   class SMAlertRoundness : public MetaFloat {
   public:
      SMAlertRoundness(MetaProcess*);
      virtual IsoString Id() const;
      virtual int Precision() const;
      virtual double DefaultValue() const;
      virtual double MinimumValue() const;
      virtual double MaximumValue() const;
   };
   extern SMAlertRoundness* TheSMAlertRoundnessParameter;

   // ----------------------------------------------------------------------------

   class SMAlertBackgroundCheck : public MetaBoolean {
   public:
      SMAlertBackgroundCheck(MetaProcess*);
      virtual IsoString Id() const;
      virtual bool DefaultValue() const;
   };
   extern SMAlertBackgroundCheck* TheSMAlertBackgroundCheckParameter;

   // ----------------------------------------------------------------------------

   class SMAlertBackground : public MetaDouble {
   public:
      SMAlertBackground(MetaProcess*);
      virtual IsoString Id() const;
      virtual int Precision() const;
      virtual double DefaultValue() const;
      virtual double MinimumValue() const;
      virtual double MaximumValue() const;
   };
   extern SMAlertBackground* TheSMAlertBackgroundParameter;

   // ----------------------------------------------------------------------------

   class SMAlertStarQuantityCheck : public MetaBoolean {
   public:
      SMAlertStarQuantityCheck(MetaProcess*);
      virtual IsoString Id() const;
      virtual bool DefaultValue() const;
   };
   extern SMAlertStarQuantityCheck* TheSMAlertStarQuantityCheckParameter;

   // ----------------------------------------------------------------------------

   class SMAlertStarQuantity : public MetaUInt32 {
   public:
      SMAlertStarQuantity(MetaProcess*);
      virtual IsoString Id() const;
      virtual double DefaultValue() const;
      virtual double MinimumValue() const;
      virtual double MaximumValue() const;
   };
   extern SMAlertStarQuantity* TheSMAlertStarQuantityParameter;

   // ----------------------------------------------------------------------------

   PCL_END_LOCAL

} // pcl

#endif   // __StarMonitorParameters_h

// ****************************************************************************
// EOF StarMonitorParameters.h - Released 2016/01/14 19:32:59 UTC
