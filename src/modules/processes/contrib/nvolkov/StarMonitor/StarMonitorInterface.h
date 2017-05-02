// ****************************************************************************
// PixInsight Class Library - PCL 02.01.00.0779
// Standard StarMonitor Process Module Version 01.00.05.0050
// ****************************************************************************
// StarMonitorInterface.h - Released 2016/01/14 19:32:59 UTC
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

#ifndef __StarMonitorInterface_h
#define __StarMonitorInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/ToolButton.h>
//#include <pcl/PushButton.h>
#include <pcl/NumericControl.h>
#include <pcl/TreeBox.h>
#include <pcl/SpinBox.h>
#include <pcl/ComboBox.h>
#include <pcl/CheckBox.h>
#include <pcl/Timer.h>
#include <pcl/ExternalProcess.h> 
#include <pcl/TextBox.h>

#include "StarMonitorInstance.h"

//#define debug 1
namespace pcl {

// ----------------------------------------------------------------------------

class StarMonitorInterface : public ProcessInterface {
public:

   StarMonitorInterface();
   virtual ~StarMonitorInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   //virtual const char** IconImageXPM() const;

   InterfaceFeatures Features() const;

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch(const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/);

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess(const ProcessImplementation&, pcl::String& whyNot) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess(const ProcessImplementation&);

   virtual void SaveSettings() const;
   virtual void LoadSettings();

   // -------------------------------------------------------------------------

private:

   StarMonitorInstance instance;

   // -------------------------------------------------------------------------

   struct GUIData {
      GUIData(StarMonitorInterface&);

      VerticalSizer Global_Sizer;

      TreeBox TargetImages_TreeBox;

      VerticalSizer GeneralParameters_Sizer;
         HorizontalSizer MonitoredFolder_Sizer;
            Label MonitoredFolder_Label;
            Edit MonitoredFolder_Edit;
            ToolButton MonitoredFolder_ToolButton;
         HorizontalSizer MonitoringButton_Sizer;
            Label MonitoringButton_Label;
            ToolButton MonitoringStartStop_ToolButton;
            ToolButton MonitoringAnimation;
            ToolButton fileAdd_Button;
            ToolButton fileClose_Button;
            ToolButton fileCloseAll_Button;
            ToolButton fileCopyTo_Button;

      SectionBar FormatHints_SectionBar;
      Control FormatHints_Control;
      VerticalSizer FormatHints_Sizer;
         HorizontalSizer InputHints_Sizer;
            Label InputHints_Label;
            Edit InputHints_Edit;

      SectionBar StarDetection_SectionBar;
      Control StarDetection_Control;
      VerticalSizer StarDetection_Sizer;
         HorizontalSizer StructureLayers_Sizer;
            Label StructureLayers_Label;
            SpinBox StructureLayers_SpinBox;
         HorizontalSizer NoiseLayers_Sizer;
            Label NoiseLayers_Label;
            SpinBox NoiseLayers_SpinBox;
         HorizontalSizer HotPixelFilterRadius_Sizer;
            Label HotPixelFilterRadius_Label;
            SpinBox HotPixelFilterRadius_SpinBox;
            NumericControl Sensitivity_NumericControl;
            NumericControl PeakResponse_NumericControl;
            NumericControl MaxStarDistortion_NumericControl;
         HorizontalSizer Invert_Sizer;
            CheckBox Invert_CheckBox;

      SectionBar Scale_SectionBar;
      Control Scale_Control;
      VerticalSizer Scale_Sizer;
         HorizontalSizer ScaleMode_Sizer;
            Label ScaleMode_Label;
            ComboBox ScaleMode_ComboBox;
            NumericEdit ScaleValue_NumericEdit;

      SectionBar Alert_SectionBar;
      Control Alert_Control;
      VerticalSizer Alert_Sizer;
         TextBox Log_TextBox;
         Edit StdIn_Edit;
         HorizontalSizer AlertExecute_Sizer;
            Label AlertExecute_Label;
            Edit AlertExecute_Edit;
         HorizontalSizer AlertArguments_Sizer;
            Label AlertArguments_Label;
            Edit AlertArguments_Edit;
            ToolButton AlertArgumentsTest_ToolButton;
            CheckBox AlertFWHMx_CheckBox;
            NumericControl AlertFWHMx_NumericControl;
            CheckBox AlertRoundness_CheckBox;
            NumericControl AlertRoundness_NumericControl;
            CheckBox AlertBackground_CheckBox;
            NumericControl AlertBackground_NumericControl;
            CheckBox AlertStarQuantity_CheckBox;
            NumericControl AlertStarQuantity_NumericControl;

      SectionBar Show3D_SectionBar;
      Control Show3D_Control;
      VerticalSizer Show3D_Sizer;
         ToolButton Show3D_Button;
         CheckBox ShowVoronoi_CheckBox;
         CheckBox ShowDelaunay_CheckBox;
         CheckBox Flat_CheckBox;
         CheckBox MoveAll_CheckBox;
         CheckBox CameraFixZ_CheckBox;
         CheckBox CameraFixXY_CheckBox;

      Timer UpdateRealTime_Timer;
      ExternalProcess my_AlertProcess;

   };

   GUIData* GUI;

   StringList files;
   


   void CheckSkyCondition(); //Check StarStatistics for last image
   void Alert(); //Execute alert command

   void ProcessFile(const String& path);
   void FindNewFile();
   void InitFindFile();
   void Stop();

   void CheckPath(bool tryStart = true);
   inline void FileAdd();
   inline void FileCopyTo();
   inline void InitGL(int index);
   inline void Show3D(int index);

   // Interface Updates
   void UpdateControls();
   void UpdateTargetImagesList();
   void AdjustDataTreeColumns();

   void AddLog(String);

   // Event Handlers
   void __EditCompleted(Edit& sender);
   void __Button_Click(Button& sender, bool checked);
   void __StarDetection_SpinValueUpdated(SpinBox& sender, int value);
   void __StarDetection_EditValueUpdated(NumericEdit& sender, double value);
   void __ItemSelected(ComboBox& sender, int itemIndex);
   void __NumericEdit_ValueUpdated(NumericEdit& sender, double value);
   void __CheckSection(SectionBar& sender, bool checked);
   //void __ToggleSection( SectionBar& sender, Control& section, bool start );

   inline void __Timer(Timer&);

   void __ExternalProcessOnStarted(ExternalProcess&);
   void __ExternalProcessOnFinished(ExternalProcess&, int exitCode, bool exitOk);
   void __ExternalProcessOnStandardOutputDataAvailable(ExternalProcess&);
   void __ExternalProcessOnError(ExternalProcess&, ExternalProcess::error_code error);

   static String SelectDirectory(const String& caption, const String& initialPath = String());
   static String UniqueFilePath(const String& fileName, const String& dir);

   friend struct GUIData;
   friend class StarMonitorInstance;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
        extern StarMonitorInterface* TheStarMonitorInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StarMonitorInterface_h

// ****************************************************************************
// EOF StarMonitorInterface.h - Released 2016/01/14 19:32:59 UTC
