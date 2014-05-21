// ****************************************************************************
// PixInsight Class Library - PCL 02.00.02.0584
// Standard PixInsightINDI Process Module Version 01.00.02.0092
// ****************************************************************************
// PixInsightINDIInterface.h - Released 2013/03/24 18:42:27 UTC
// ****************************************************************************
// This file is part of the standard PixInsightINDI PixInsight module.
//
// Copyright (c) 2003-2013, Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __CCDFrameInterface_h
#define __CCDFrameInterface_h

#include <pcl/Dialog.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/SpinBox.h>
#include <pcl/PushButton.h>
#include <pcl/TreeBox.h>
#include <pcl/ErrorHandler.h>
#include <pcl/Timer.h>
#include <map>

#include "PixInsightINDIclient.h"
#include "CCDFrameInstance.h"

#if defined(__PCL_LINUX)
#include <memory>
#endif

namespace pcl
{




// ----------------------------------------------------------------------------

class CCDFrameInterface : public ProcessInterface
{
public:

   CCDFrameInterface();
   virtual ~CCDFrameInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual void ApplyInstance() const;
   virtual void ApplyInstanceGlobal() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual InterfaceFeatures Features() const;

   void UpdateControls();
   
   struct GUIData
   {
	   GUIData(CCDFrameInterface& w);

	   Timer			  UpdateDeviceList_Timer;
	   Timer              ExposureDuration_Timer;
	   Timer              Temperature_Timer;


	   VerticalSizer      Global_Sizer;
	   SectionBar         CCDDevice_SectionBar;
	   Control			  CCDDevice_Control;
		HorizontalSizer		CCDDevice_Sizer;
		 Label				 CCDDevice_Label;
		 ComboBox            CCDDevice_Combo;
	   SectionBar         CCDParam_SectionBar;
	   Control			  CCDParam_Control;
	   	VerticalSizer		CCDParam_Sizer;
	   	 HorizontalSizer      CCDTemp_Sizer;
	   	  Label                CCDTemp_Label;
	   	  Edit                 CCDTemp_Edit;
	   	 HorizontalSizer      CCDBinning_Sizer;
	   	  Label                CCDBin_Label;
	      Label                CCDBinX_Label;
	  	  Edit                 CCDBinX_Edit;
	  	  Label                CCDBinY_Label;
	      Edit                 CCDBinY_Edit;
	   SectionBar         FrameExposure_SectionBar;
	   Control			  FrameExposure_Control;
		VerticalSizer		FrameExposure_Sizer;
		 HorizontalSizer	 ExpTime_Sizer;
		  Label		  		  ExpTime_Label;
		  Edit                ExpTime_Edit;
		HorizontalSizer	     ExpNum_Sizer;
		  Label				  ExpNum_Label;
		  Edit                ExpNum_Edit;
		HorizontalSizer	     ExpDur_Sizer;
		  Label				  ExpDur_Label;
		  Edit                ExpDur_Edit;
		  Label 			  ExpFrame_Label;
		  Edit                ExpFrame_Edit;
		HorizontalSizer	     Image_Sizer;
		  Label				  SaveImage_CheckBoxLabel;
		  CheckBox            SaveImage_CheckBox;
		  Label				  ImagePath_Label;
		  Edit                ImagePath_Edit;
		  Label 			  ImagePrefix_Label;
		  Edit                ImagePrefix_Edit;
		HorizontalSizer	     ExpButton_Sizer;
		  PushButton		  StartExposure_PushButton;
		  PushButton		  CancelExposure_PushButton;
   };

   private:

   CCDFrameInstance instance;

   GUIData* GUI;

   IsoString m_serverMessage;

   //INDINewPropertyListItem m_newPropertyListItem;
   String                  m_Device;
   double                  m_ExposureDuration;
   int                     m_NumOfExposures;
   double                  m_Temperature;
   bool                    m_saveFrame;
   String                  m_FrameFolder;
   String                  m_FramePrefix;
   
   void UpdateDeviceList_Timer( Timer& sender );
   void ExposureDuration_Timer( Timer &sender );
   void Temperature_Timer( Timer &sender );
   void EditCompleted( Edit& sender );
   void ComboItemSelected(ComboBox& sender, int itemIndex);
   void StartExposureButton_Click(Button& sender, bool checked);
   void CancelButton_Click(Button& sender, bool checked);
   void CheckBoxChecked(Button& sender, Button::check_state state);

   void UpdateDeviceList();
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern CCDFrameInterface* TheCCDFrameInterface; 
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CCDFrameInterface_h

// ****************************************************************************
// EOF CCDFrameInterface.h - Released 2013/03/24 18:42:27 UTC
