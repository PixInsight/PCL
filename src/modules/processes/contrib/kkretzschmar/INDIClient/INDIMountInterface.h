//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.07.0141
// ----------------------------------------------------------------------------
// INDIMountInterface.h - Released 2016/04/28 15:13:36 UTC
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

#ifndef __INDIMountInterface_h
#define __INDIMountInterface_h

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
#include <pcl/RadioButton.h>
//#include <pcl/TreeBox.h>
#include <pcl/TabBox.h>
#include <pcl/ErrorHandler.h>
#include <pcl/Timer.h>
#include <pcl/NetworkTransfer.h>
#include <map>

#include "INDIClient.h"
#include "INDIMountInstance.h"


#if defined(__PCL_LINUX)
#include <memory>
#endif

namespace pcl
{


#define GUI_LABELED_CONTROL(NAME,CONTROL) \
	Label    NAME ## _Label; \
	CONTROL  NAME ## _ ## CONTROL;

// ----------------------------------------------------------------------------

class INDIMountInterfaceExecution;


class CoordSearchDialog : public Dialog {
private:
	VerticalSizer       Global_Sizer;
		HorizontalSizer      Search_Sizer;
		 GUI_LABELED_CONTROL(Search,Edit);
		 PushButton           Search_PushButton;
		HorizontalSizer      SearchResult_Sizer;
		 GUI_LABELED_CONTROL(TRA,Edit);
		 GUI_LABELED_CONTROL(TDEC,Edit);
		HorizontalSizer      SearchControl_Sizer;
		 PushButton           SearchOK_PushButton;
		 PushButton           SearchCancel_PushButton;


	String m_targetObj;
	double m_RA_TargetCoord;
	double m_DEC_TargetCoord;

	IsoString m_downloadedFile;

	bool DownloadObjectCoordinates(NetworkTransfer &sender, const void *buffer, fsize_type size);
	void Button_Click(Button& sender, bool checked);
	void EditCompleted( Edit& sender);
public:

	CoordSearchDialog();
	virtual ~CoordSearchDialog(){}
	double getTargetRaCoord() const {return m_RA_TargetCoord;}
	double getTargetDECCoord() const {return m_DEC_TargetCoord;}



};

class INDIMountInterface : public ProcessInterface
{
public:

   INDIMountInterface();
   virtual ~INDIMountInterface();

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

   struct GUIData
   {
	   GUIData(INDIMountInterface& w);

	   Timer			  UpdateDeviceList_Timer;
	   Timer              UpdateDeviceProperties_Timer;

	   VerticalSizer      Global_Sizer;
	   	   SectionBar         ServerParameters_SectionBar;
	   	   Control			  ServerParameters_Control;
	   	   VerticalSizer      ServerParameters_Sizer;
	   	   	   HorizontalSizer	   MountDevice_Sizer;
	   	   	   	   Label			   MountDevice_Label;
	   	   	   	   ComboBox            MountDevice_Combo;
	   	   	   	Control			   MountProperties_Control;
	   	   	   	VerticalSizer	   MountProperties_Sizer;
	   	   	   		HorizontalSizer      MountLST_Sizer;
	   	   	   			Label            	LST_Label;
	   	   	   			NumericEdit			LST_H_Edit;
	   	   	   			NumericEdit			LST_M_Edit;
	   	   	   			NumericEdit			LST_S_Edit;
	   	   	   		HorizontalSizer    MountEQRA_Sizer;
	   	   	   			Label 				RA_Label;
	   	   	   			NumericEdit			RA_H_Edit;
	   	   	   			NumericEdit			RA_M_Edit;
	   	   	   			NumericEdit			RA_S_Edit;
	   	  		   	HorizontalSizer    MountEQDEC_Sizer;
	   	  		   		Label 				DEC_Label;
	   	  		   		NumericEdit			DEC_D_Edit;
	   	  		   		NumericEdit			DEC_M_Edit;
	   	  		   		NumericEdit			DEC_S_Edit;
	   	  		   	HorizontalSizer    MountHZAZ_Sizer;
	   	  		   		Label 				AZ_Label;
	   	  		   		NumericEdit			AZ_D_Edit;
	   	  		   		NumericEdit			AZ_M_Edit;
	   	  		   		NumericEdit			AZ_S_Edit;
	   	  		    HorizontalSizer    MountHZALT_Sizer;
	   	  		    	Label 				ALT_Label;
	   	  		   		NumericEdit			ALT_D_Edit;
	   	  		   		NumericEdit			ALT_M_Edit;
	   	  		   		NumericEdit			ALT_S_Edit;
	   	  	SectionBar        MountGoto_SectionBar;
	   	  	Control			  MountGoto_Control;
	   	  	VerticalSizer     MountGoto_Sizer;
	   	        HorizontalSizer MountGotoCoord_HSizer;
	   	        	VerticalSizer     MountGotoLeft_Sizer;
	   	  				HorizontalSizer	  MountTargetRA_Sizer;
	   	  					Label             TargetRA_Label;
	   	  					SpinBox			  TargetRA_H_SpinBox;
	   	  					SpinBox			  TargetRA_M_SpinBox;
	   	  					NumericEdit		  TargetRA_S_NumericEdit;
	   	  				HorizontalSizer	  MountTargetDEC_Sizer;
	   	  					Label             TargetDEC_Label;
	   	  					SpinBox			  TargetDEC_H_SpinBox;
	   	  					SpinBox			  TargetDEC_M_SpinBox;
	   	  					NumericEdit		  TargetDEC_S_NumericEdit;
	   	  					CheckBox      	  MountTargetDECIsSouth_CheckBox;
	   	  			VerticalSizer     MountGotoRight_Sizer;
	   	  				PushButton		    MountSearch_PushButton;
	   	  		HorizontalSizer	  MountParking_Sizer;
	   	  			Label              MountParking_Label;
	   	  			CheckBox 		   MountParking_CheckBox;
	   	  		HorizontalSizer	  MountGotoStart_Sizer;
	   	  			Label              MountGotoCommand_Label;
	   	  			PushButton		   MountGotoStart_PushButton;
	   	  			PushButton		   MountGotoCancel_PushButton;
	   	  			Label              MountGotoInfo_Label;
	   	  		HorizontalSizer	  MountSynch_Sizer;
	   	  			Label              MountSynchCommand_Label;
	   	  			PushButton		   MountSynch_PushButton;
   };

   INDIMountInstance instance;

   GUIData* GUI;
   void UpdateControls();

   IsoString m_serverMessage;

   String                       m_Device;
   INDIMountInterfaceExecution* m_execution;



   IsoString               m_downloadedFile;


   void UpdateDeviceList_Timer( Timer& sender );
   void UpdateMount_Timer( Timer &sender );
   void e_Edit( Edit& sender );
   void e_ItemSelected(ComboBox& sender, int itemIndex);
   void e_Click(Button& sender, bool checked);
   bool e_DownloadDataAvailable(NetworkTransfer &sender, const void *buffer, fsize_type size);

   friend class INDIMountInterfaceExecution;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern INDIMountInterface* TheINDIMountInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __INDIMountInterface_h

// ----------------------------------------------------------------------------
// EOF INDIMountInterface.h - Released 2016/04/28 15:13:36 UTC
