//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.03.0102
// ----------------------------------------------------------------------------
// INDIMountInterface.h - Released 2016/03/18 13:15:37 UTC
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


class SkyMap;

class EditEqCoordPropertyDialog : public Dialog {
private:
	VerticalSizer       Global_Sizer;
		HorizontalSizer		RA_Property_Sizer;
			Label               RA_Property_Label;
			Edit				RA_Hour_Edit;
			Label               RA_Colon1_Label;
			Edit				RA_Minute_Edit;
			Label               RA_Colon2_Label;
			Edit				RA_Second_Edit;
			PushButton          Search_PushButton;
		HorizontalSizer		DEC_Property_Sizer;
			Label               DEC_Property_Label;
			Edit				DEC_Hour_Edit;
			Label               DEC_Colon1_Label;
			Edit				DEC_Minute_Edit;
			Label               DEC_Colon2_Label;
			Edit				DEC_Second_Edit;
		HorizontalSizer	Buttons_Sizer;
			PushButton			OK_PushButton;
			PushButton			Cancel_PushButton;

	double m_ra_hour;
	double m_ra_minute;
	double m_ra_second;
	double m_dec_deg;
	double m_dec_arcsecond;
	double m_dec_arcminute;

	String m_RA_TargetCoord;
	String m_DEC_TargetCoord;

public:

	EditEqCoordPropertyDialog(String raCoord, String decCoord);
	virtual ~EditEqCoordPropertyDialog(){}

	virtual void setRACoords(String value);
	virtual void setDECCoords(String value);
	void EditCompleted( Edit& sender);
	String getTargetRaCoord() const {return m_RA_TargetCoord;}
	String getTargetDECCoord() const {return m_DEC_TargetCoord;}

	double getRaHour() {return m_ra_hour;}
	double getRaMinute() {return m_ra_minute;}
	double getRaSecond() {return m_ra_second;}

	double getDecDeg() {return m_dec_deg;}
	double getDecArcMinute() {return m_dec_arcminute;}
	double getDecArcSecond() {return m_dec_arcsecond;}

	void Ok_Button_Click( Button& sender, bool checked );
	void Cancel_Button_Click( Button& sender, bool checked );
};


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

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual InterfaceFeatures Features() const;

   void UpdateControls();



   struct GUIData
   {
	   GUIData(INDIMountInterface& w);

	   Timer			  UpdateDeviceList_Timer;
	   Timer              UpdateMount_Timer;

	   VerticalSizer      Global_Sizer;
	   SectionBar         MountDevice_SectionBar;
	   Control			  MountDevice_Control;
		HorizontalSizer		MountDevice_Sizer;
		 Label				 MountDevice_Label;
		 ComboBox            MountDevice_Combo;
	   SectionBar         SkyChart_SectionBar;
	   Control            SkyChart_Control;
	    HorizontalSizer		SkyChart_HSizer;
	     TabBox				SkyChart_TabBox;
	     Control             SkyChart_AllSkyGraphicsControl;
	     Control             SkyChart_FoVGraphicsControl;
	   SectionBar         MountCoordAndTime_SectionBar;
	   Control			  MountCoordAndTime_Control;
	    HorizontalSizer		MountCoordAndTime_HSizer;
	   	 VerticalSizer		MountCAT_Label_VSizer;
	   	   Label                MountTime_Label;
	   	   Label                MountEQC_Label;
	   	   Label                MountHZC_Label;
		 VerticalSizer        MountCAT_First_VSizer;
		   HorizontalSizer      MountTime_Sizer;
		    GUI_LABELED_CONTROL(UTC,Edit);
		   HorizontalSizer    MountEQC_Sizer;
		   	GUI_LABELED_CONTROL(RA,Edit);
	       HorizontalSizer    MountHZC_Sizer;
		   	GUI_LABELED_CONTROL(AZ,Edit);
		 VerticalSizer        MountCAT_Second_VSizer;
		    HorizontalSizer      MountTime_2_Sizer;
		   	  GUI_LABELED_CONTROL(LST,Edit);
		   	HorizontalSizer    MountEQC_2_Sizer;
		   	  GUI_LABELED_CONTROL(DEC,Edit);
		    HorizontalSizer    MountHZC_2_Sizer;
		   	  GUI_LABELED_CONTROL(ALT,Edit);
	   SectionBar         MountGoto_SectionBar;
	   Control			  MountGoto_Control;
	    VerticalSizer        MountGoto_VSizer;
	     HorizontalSizer	  MountGoto_HSizer;
	      Label                MountGoto_Label;
	      GUI_LABELED_CONTROL(TRA,Edit);
	      GUI_LABELED_CONTROL(TDEC,Edit);
	      PushButton		    MountSet_PushButton;
	      PushButton		    MountSearch_PushButton;
	     HorizontalSizer	  MountGoto_Second_HSizer;
	      PushButton		   MountGoto_PushButton;
	      PushButton		   MountSynch_PushButton;
   };

   private:

   INDIMountInstance instance;

   GUIData* GUI;

   IsoString m_serverMessage;

   //INDINewPropertyListItem m_newPropertyListItem;
   String                  m_Device;
   String                  m_TargetRA;
   String                  m_TargetDEC;
   IsoString               m_downloadedFile;
   SkyMap*                 m_skymap;


   double                  m_geoLat;
   double                  m_lst;
   double				   m_scopeRA;
   double				   m_scopeDEC;
   double				   m_alignedRA;
   double			       m_alignedDEC;
   double                  m_limitStarMag;
   bool                    m_isAllSkyView;

   void UpdateDeviceList_Timer( Timer& sender );
   void UpdateMount_Timer( Timer &sender );
   void EditCompleted( Edit& sender );
   void ComboItemSelected(ComboBox& sender, int itemIndex);
   void Button_Click(Button& sender, bool checked);
   void SkyChart_Paint( Control& sender, const Rect& updateRect );
   void UpdateDeviceList();
   bool DownloadObjectCoordinates(NetworkTransfer &sender, const void *buffer, fsize_type size);
   void TabPageSelected( TabBox& sender, int pageIndex );
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern INDIMountInterface* TheINDIMountInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __INDIMountInterface_h

// ----------------------------------------------------------------------------
// EOF INDIMountInterface.h - Released 2016/03/18 13:15:37 UTC
