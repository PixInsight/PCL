//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0199
// ----------------------------------------------------------------------------
// INDICCDFrameInterface.h - Released 2016/06/20 17:47:31 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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

#ifndef __INDICCDFrameInterface_h
#define __INDICCDFrameInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Timer.h>
#include <pcl/ToolButton.h>

#include "INDIClient.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class INDICCDFrameInterfaceExecution;

class INDICCDFrameInterface : public ProcessInterface
{
public:

   INDICCDFrameInterface();
   virtual ~INDICCDFrameInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;
   virtual InterfaceFeatures Features() const;
   virtual void ResetInstance();
   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );
   virtual ProcessImplementation* NewProcess() const;
   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;
   virtual bool ImportProcess( const ProcessImplementation& );

private:

   String                          m_device;
   INDICCDFrameInterfaceExecution* m_execution;

   struct GUIData
   {
      GUIData( INDICCDFrameInterface& );

      Timer UpdateDeviceList_Timer;
      Timer UpdateDeviceProperties_Timer;

      VerticalSizer     Global_Sizer;

      SectionBar        ServerParameters_SectionBar;
      Control           ServerParameters_Control;
      VerticalSizer     ServerParameters_Sizer;
         HorizontalSizer   CCDDevice_Sizer;
            Label             CCDDevice_Label;
            ComboBox          CCDDevice_Combo;
         Control           CCDProperties_Control;
         VerticalSizer     CCDProperties_Sizer;
            HorizontalSizer   CCDTemp_HSizer;
               NumericEdit       CCDTemp_NumericEdit;
               NumericControl    CCDTargetTemp_NumericEdit;
               ToolButton        CCDTargetTemp_ToolButton;
            HorizontalSizer   CCDBinX_HSizer;
               Label             CCDBinX_Label;
               ComboBox          CCDBinX_Combo;
            HorizontalSizer   CCDBinY_HSizer;
               Label             CCDBinY_Label;
               ComboBox          CCDBinY_Combo;
            HorizontalSizer   CCDFilter_HSizer;
               Label             CCDFilter_Label;
               ComboBox          CCDFilter_Combo;
            HorizontalSizer   CCDFrameType_HSizer;
               Label             CCDFrameType_Label;
               ComboBox          CCDFrameType_Combo;
            HorizontalSizer   UploadMode_HSizer;
               Label             UploadMode_Label;
               ComboBox          UploadMode_Combo;
            HorizontalSizer   ServerUploadDir_HSizer;
               Label             ServerUploadDir_Label;
               Edit              ServerUploadDir_Edit;
               ToolButton        ServerUploadDir_ToolButton;
            HorizontalSizer   ServerFileNameTemplate_HSizer;
               Label             ServerFileNameTemplate_Label;
               Edit              ServerFileNameTemplate_Edit;
               PushButton        ServerFileNameTemplate_PushButton;

      SectionBar        ClientParameters_SectionBar;
      Control           ClientParameters_Control;
      VerticalSizer     ClientParameters_Sizer;
         HorizontalSizer   OpenClientFrames_Sizer;
            CheckBox          OpenClientFrames_CheckBox;
         HorizontalSizer   ReuseImageWindow_Sizer;
            CheckBox          ReuseImageWindow_CheckBox;
         HorizontalSizer   AutoStretch_Sizer;
            CheckBox          AutoStretch_CheckBox;
         HorizontalSizer   LinkedAutoStretch_Sizer;
            CheckBox          LinkedAutoStretch_CheckBox;
         HorizontalSizer   SaveClientFrames_Sizer;
            CheckBox          SaveClientFrames_CheckBox;
         HorizontalSizer   OverwriteClientFrames_Sizer;
            CheckBox          OverwriteClientFrames_CheckBox;
         HorizontalSizer   ClientDownloadDir_HSizer;
            Label             ClientDownloadDir_Label;
            Edit              ClientDownloadDir_Edit;
            ToolButton        ClientDownloadDir_ToolButton;
         HorizontalSizer   ClientFileNameTemplate_HSizer;
            Label             ClientFileNameTemplate_Label;
            Edit              ClientFileNameTemplate_Edit;
         HorizontalSizer   ClientOutputFormatHints_HSizer;
            Label             ClientOutputFormatHints_Label;
            Edit              ClientOutputFormatHints_Edit;

      SectionBar        FrameAcquisition_SectionBar;
      Control           FrameAcquisition_Control;
      VerticalSizer     FrameAcquisition_Sizer;
         HorizontalSizer   ExposureParameters_Sizer;
            VerticalSizer     ExposureParametersLeft_Sizer;
               NumericEdit       ExposureTime_NumericEdit;
               NumericEdit       ExposureDelay_NumericEdit;
               HorizontalSizer   ExposureCount_Sizer;
                  Label             ExposureCount_Label;
                  SpinBox           ExposureCount_SpinBox;
            VerticalSizer     ExposureParametersRight_Sizer;
               HorizontalSizer   StartExposure_Sizer;
                  PushButton        StartExposure_PushButton;
               HorizontalSizer   CancelExposure_Sizer;
                  PushButton        CancelExposure_PushButton;
               Label             ExposureInfo_Label;
         HorizontalSizer   ObjectName_Sizer;
            Label             ObjectName_Label;
            Edit              ObjectName_Edit;
         HorizontalSizer   TelescopeDevice_Sizer;
            Label             TelescopeDevice_Label;
            ComboBox          TelescopeDevice_Combo;
   };

   GUIData* GUI;

   void UpdateControls();

   void e_Show( Control& );
   void e_Hide( Control& );
   void e_Timer( Timer& sender );
   void e_Click( Button& sender, bool checked );
   void e_ItemSelected( ComboBox& sender, int itemIndex );

   friend class INDICCDFrameInterfaceExecution;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern INDICCDFrameInterface* TheINDICCDFrameInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __INDICCDFrameInterface_h

// ----------------------------------------------------------------------------
// EOF INDICCDFrameInterface.h - Released 2016/06/20 17:47:31 UTC
