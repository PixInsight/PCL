// ****************************************************************************
// PixInsight Class Library - PCL 02.01.00.0779
// Standard StarMonitor Process Module Version 01.00.05.0050
// ****************************************************************************
// StarMonitorProcess.cpp - Released 2016/01/14 19:32:59 UTC
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

#include "StarMonitorProcess.h"
#include "StarMonitorParameters.h"
#include "StarMonitorInstance.h"
#include "StarMonitorInterface.h"
#include "StarMonitorModule.h" // for StarMonitorModule::ReadableVersion();

namespace pcl
{

   // ----------------------------------------------------------------------------

   StarMonitorProcess* TheStarMonitorProcess = 0;

   // ----------------------------------------------------------------------------

   //#include "StarMonitorIcon.xpm"

   // ----------------------------------------------------------------------------

   StarMonitorProcess::StarMonitorProcess() : MetaProcess()
   {
      TheStarMonitorProcess = this;

      // Instantiate process parameters
      new SMStructureLayers(this);
      new SMNoiseLayers(this);
      new SMHotPixelFilterRadius(this);
      new SMSensitivity(this);
      new SMPeakResponse(this);
      new SMMaxStarDistortion(this);
      new SMInvert(this);

      new SMMonitoredFolder(this);

      new SMTargetItems(this);
      new SMTargetImage(TheSMTargetItemsParameter);
      new SMStarQuantity(TheSMTargetItemsParameter);
      new SMBackground(TheSMTargetItemsParameter);
      new SMAmplitude(TheSMTargetItemsParameter);
      new SMRadiusX(TheSMTargetItemsParameter);
      new SMRadiusY(TheSMTargetItemsParameter);
      new SMFWHMx(TheSMTargetItemsParameter);
      new SMFWHMy(TheSMTargetItemsParameter);
      new SMRotationAngle(TheSMTargetItemsParameter);
      new SMMAD(TheSMTargetItemsParameter);
      new SMxScale(TheSMTargetItemsParameter);
      new SMyScale(TheSMTargetItemsParameter);

      new SMInputHints(this);
      new SMScaleMode(this);
      new SMScaleValue(this);

      new SMAlertCheck(this);
      new SMAlertExecute(this);
      new SMAlertArguments(this);
      new SMAlertFWHMxCheck(this);
      new SMAlertFWHMx(this);
      new SMAlertRoundnessCheck(this);
      new SMAlertRoundness(this);
      new SMAlertBackgroundCheck(this);
      new SMAlertBackground(this);
      new SMAlertStarQuantityCheck(this);
      new SMAlertStarQuantity(this);

      //StandardAllocator::EnableFastGrowth();
   }

   // ----------------------------------------------------------------------------

   IsoString StarMonitorProcess::Id() const
   {
      return "StarMonitor";
   }

   // ----------------------------------------------------------------------------

   IsoString StarMonitorProcess::Category() const
   {
      return "ImageInspection";
   }

   // ----------------------------------------------------------------------------

   uint32 StarMonitorProcess::Version() const
   {
      return 0x100;
   }

   // ----------------------------------------------------------------------------

   String StarMonitorProcess::Description() const
   {
      return StarMonitorModule::ReadableVersion() +
              "<html><p>For more detailed information, please refer to the Release Information board on PixInsight Forum:<br/>"
              "http://pixinsight.com/forum/index.php?topic=3646.0</html>";
   }

   // ----------------------------------------------------------------------------
   /*
   const char** StarMonitorProcess::IconImageXPM() const
   {
      return StarMonitorIcon_XPM;
   }
    */
   // ----------------------------------------------------------------------------

   ProcessInterface* StarMonitorProcess::DefaultInterface() const
   {
      return TheStarMonitorInterface;
   }

   // -------------------------------------------------------------------------

   ProcessImplementation* StarMonitorProcess::Create() const
   {
      return new StarMonitorInstance(this);
   }

   // ----------------------------------------------------------------------------

   ProcessImplementation* StarMonitorProcess::Clone(const ProcessImplementation& p) const
   {
      const StarMonitorInstance* instPtr = dynamic_cast<const StarMonitorInstance*> (&p);
      return (instPtr != 0) ? new StarMonitorInstance(*instPtr) : 0;
   }

   // ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF StarMonitorProcess.cpp - Released 2016/01/14 19:32:59 UTC
