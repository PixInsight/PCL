//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0203
// ----------------------------------------------------------------------------
// INDIMountInterface.h - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2017 Klaus Kretzschmar
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

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Dialog.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NetworkTransfer.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/TextBox.h>
#include <pcl/Timer.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>
#include <pcl/TabBox.h>

#include "INDIClient.h"
#include "INDIMountInstance.h"
#include "DeviceConfigBase.h"

namespace pcl {
	class INDIMountInterface;
}

namespace pcl
{

// ----------------------------------------------------------------------------

class CoordinateSearchDialog : public Dialog
{
public:

   CoordinateSearchDialog();

   const String& ObjectName() const
   {
      return m_objectName;
   }

   const String& ObjectType() const
   {
      return m_objectType;
   }

   const String& SpectralType() const
   {
      return m_spectralType;
   }

   double VMagnitude() const
   {
      return m_vmag;
   }

   // degrees
   double RA() const
   {
      return m_RA;
   }

   // degrees
   double Dec() const
   {
      return m_Dec;
   }

   // mas/year
   double MuRA() const
   {
      return m_muRA;
   }

   // mas/year
   double MuDec() const
   {
      return m_muDec;
   }

   // mas
   double Parallax() const
   {
      return m_parallax;
   }

   // AU/year
   double RadialVelocity() const
   {
      return m_radVel;
   }

   bool HasValidCoordinates() const
   {
      return m_valid;
   }

   bool GoToTarget() const
   {
      return m_goto;
   }

private:

   VerticalSizer       Global_Sizer;
      HorizontalSizer      Search_Sizer;
         Label                ObjectName_Label;
         Edit                 ObjectName_Edit;
         PushButton           Search_Button;
      TextBox              SearchInfo_TextBox;
      HorizontalSizer      Buttons_Sizer;
         PushButton           Get_Button;
         PushButton           GoTo_Button;
         PushButton           Cancel_Button;

   String    m_objectName;
   String    m_objectType;
   String    m_spectralType;
   double    m_vmag;     // flux in the V filter
   double    m_RA;       // in degrees
   double    m_Dec;      // in degrees
   double    m_muRA;     // in mas/year
   double    m_muDec;    // in mas/year
   double    m_parallax; // in mas
   double    m_radVel;   // in AU/year
   bool      m_valid;
   bool      m_goto;
   bool      m_downloading;
   bool      m_abort;
   bool      m_firstTimeShown;
   IsoString m_downloadData;

   void e_Show( Control& sender );
   void e_GetFocus( Control& sender );
   void e_LoseFocus( Control& sender );
   bool e_Download( NetworkTransfer& sender, const void* buffer, fsize_type size );
   bool e_Progress( NetworkTransfer& sender,
                    fsize_type downloadTotal, fsize_type downloadCurrent,
                    fsize_type uploadTotal, fsize_type uploadCurrent );
   void e_Click( Button& sender, bool checked );
};

class SyncDataListDialog : public Dialog {
public:
	SyncDataListDialog(Array<SyncDataPoint>& syncDataArray);

private:
	VerticalSizer       Global_Sizer;
      HorizontalSizer      SyncDataList_Sizer;
		TreeBox             SnycData_TreeBox;
	  HorizontalSizer      SyncDataListButton_Sizer;
	    PushButton           Enable_Button;
	    PushButton           Disable_Button;
	    PushButton           Delete_Button;
	    PushButton           Ok_Button;
	    PushButton           Cancel_Button;


	  void e_Click( Button& sender, bool checked );
	  void e_Show( Control& sender );
	  void e_Close( Control& sender , bool& allowClose);

	  Array<SyncDataPoint>&  m_syncDataList;
	  bool                   m_firstTimeShown;
};

#define LABELED_CHECKBOX(NAME)        \
	HorizontalSizer  NAME##_Sizer;    \
	Label            NAME##_Label;    \
    CheckBox         NAME##_CheckBox

class AlignmentConfigDialog : public Dialog {
public:
	AlignmentConfigDialog(INDIMountInterface& w);
private:

	bool m_firstTimeShown = true;

	INDIMountInterface& m_interface;

	VerticalSizer      Global_Sizer;
	  TabBox				AlignmentConfig_TabBox;
		Control             AnalyticalAlignment_ConfigControl;
          VerticalSizer       MountAlignmentConfig_Sizer;

            LABELED_CHECKBOX(ModelPierSide);
            LABELED_CHECKBOX(Offset);
            LABELED_CHECKBOX(Collimation);
            LABELED_CHECKBOX(NonPerpendicular);
            LABELED_CHECKBOX(PolarAxisDisplacement);
            LABELED_CHECKBOX(TubeFlexure);
            LABELED_CHECKBOX(ForkFlexure);
            LABELED_CHECKBOX(DeltaAxisFlexure);
            LABELED_CHECKBOX(Linear);
            LABELED_CHECKBOX(Quadratic);
        //Control             SurfaceSplineAlignment_ConfigControl;
      	  HorizontalSizer      AlignmentConfigButton_Sizer;
      	    PushButton           Ok_Button;
      	    PushButton           Cancel_Button;

     void e_Show( Control& sender );
     void e_Click( Button& sender, bool checked );
     void e_PageSelected(TabBox& sender, int tabIndex);
};


class MountConfigDialog : public ConfigDialogBase {
public:
	MountConfigDialog(const String& deviceName, double geoLat, double geoLong, double telescopeAperture, double teslescopeFocalLenght );
private:

	String m_device;

	HorizontalSizer   Latitude_Sizer;
	   Label             Latitude_Label;
	   SpinBox           Latitude_H_SpinBox;
	   SpinBox           Latitude_M_SpinBox;
	   NumericEdit       Latitude_S_NumericEdit;
	   CheckBox          LatitudeIsSouth_CheckBox;

	HorizontalSizer   Longitude_Sizer;
	   Label             Longitude_Label;
	   SpinBox           Longitude_H_SpinBox;
	   SpinBox           Longitude_M_SpinBox;
	   NumericEdit       Longitude_S_NumericEdit;
	   CheckBox          LongitudeIsWest_CheckBox;

	HorizontalSizer   TelescopeAperture_Sizer;
	   NumericEdit		TelescopeAperture_NumericEdit;

	HorizontalSizer   TelescopeFocalLength_Sizer;
	  NumericEdit		TelescopeFocalLength_NumericEdit;


	void sendUpdatedProperties();


};

// ----------------------------------------------------------------------------

class INDIMountInterfaceExecution;

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

   const String& CurrentDeviceName() const
   {
      return m_device;
   }

   double getGeographicLatitude() const {
	   return m_geoLatitude;
   }
   double getGeographicLongitude() const {
	   return m_geoLongitude;
   }
   int getTelescopeAperture() const {
	   return m_telescopeAperture;
   }
   int getTelescopeFocalLength() const {
	   return m_telescopeFocalLength;
   }
 private:

   String                       m_device;

   INDIMountInterfaceExecution* m_execution              = nullptr;
   CoordinateSearchDialog*      m_searchDialog           = nullptr;
   AlignmentConfigDialog*       m_alignmentConfigDialog  = nullptr;


   struct GUIData
   {
      GUIData(INDIMountInterface& w);

      Timer UpdateDeviceList_Timer;
      Timer UpdateDeviceProperties_Timer;

      VerticalSizer        Global_Sizer;

      SectionBar        ServerParameters_SectionBar;
      Control           ServerParameters_Control;
      VerticalSizer     ServerParameters_Sizer;
         HorizontalSizer   MountDevice_Sizer;
            Label             MountDevice_Label;
            ComboBox          MountDevice_Combo;
            ToolButton        MountDeviceConfig_ToolButton;
         Control           MountProperties_Control;
         VerticalSizer     MountProperties_Sizer;
            HorizontalSizer   MountLST_Sizer;
               Label             LST_Label;
               Label             LST_Value_Label;
            HorizontalSizer   MountEQRA_Sizer;
               Label             RA_Label;
               Label             RA_Value_Label;
            HorizontalSizer   MountEQDec_Sizer;
               Label             Dec_Label;
               Label             Dec_Value_Label;
            HorizontalSizer   MountHZAZ_Sizer;
               Label             AZ_Label;
               Label             AZ_Value_Label;
            HorizontalSizer   MountHZALT_Sizer;
               Label             ALT_Label;
               Label             ALT_Value_Label;
      SectionBar        MountAlignment_SectionBar;
      Control           MountAlignment_Control;
      VerticalSizer     MountAlignment_Sizer;
      	  HorizontalSizer   MountAlignmentFile_Sizer;
              Label             AlignmentFile_Label;
              Edit              AlignmentFile_Edit;
              ToolButton        AlignmentFile_ToolButton;
          HorizontalSizer   MountAlignmentConfig_Sizer;
          	  PushButton       MountAligmentModelConfig_Button;
          	  PushButton       MountAligmentModelFit_Button;
          	  CheckBox         MountAlignmentPlotResiduals_CheckBox;
          	  PushButton       SyncDataList_Button;
      SectionBar        MountGoTo_SectionBar;
      Control           MountGoTo_Control;
      VerticalSizer     MountGoTo_Sizer;
         HorizontalSizer   MountTargetRA_Sizer;
            Label             TargetRA_Label;
            SpinBox           TargetRA_H_SpinBox;
            SpinBox           TargetRA_M_SpinBox;
            NumericEdit       TargetRA_S_NumericEdit;
         HorizontalSizer   MountTargetDec_Sizer;
            Label             TargetDec_Label;
            SpinBox           TargetDec_H_SpinBox;
            SpinBox           TargetDec_M_SpinBox;
            NumericEdit       TargetDec_S_NumericEdit;
            CheckBox          MountTargetDECIsSouth_CheckBox;
         HorizontalSizer   MountComputeApparentPosition_Sizer;
            CheckBox         MountComputeApparentPosition_CheckBox;
         HorizontalSizer   MountSearch_Sizer;
            PushButton        MountSearch_Button;
         HorizontalSizer   MountAlignmentCorrection_Sizer;
         	 CheckBox         MountAlignmentCorrection_CheckBox;
         HorizontalSizer   MountGoToStart_Sizer;
            PushButton        MountGoToStart_Button;
            PushButton        MountSync_Button;
            PushButton        MountPark_Button;
         HorizontalSizer   MountGoToCancel_Sizer;
            PushButton        MountGoToCancel_Button;
         Label             MountGoToInfo_Label;

      SectionBar        Slew_SectionBar;
      Control           Slew_Control;
      HorizontalSizer   Slew_Sizer;
         Control           SlewLeft_Control;
         VerticalSizer     SlewLeft_Sizer;
            HorizontalSizer   SlewTopRow_Sizer;
               ToolButton        SlewTopLeft_Button;
               ToolButton        SlewTop_Button;
               ToolButton        SlewTopRight_Button;
            HorizontalSizer   SlewMiddleRow_Sizer;
               ToolButton        SlewLeft_Button;
               ToolButton        SlewStop_Button;
               ToolButton        SlewRight_Button;
            HorizontalSizer   SlewBottomRow_Sizer;
               ToolButton        SlewBottomLeft_Button;
               ToolButton        SlewBottom_Button;
               ToolButton        SlewBottomRight_Button;
         VerticalSizer     SlewRight_Sizer;
            Label             SlewSpeed_Label;
            ComboBox          SlewSpeed_ComboBox;


      bool m_modelBothPierSides               = true;
      bool m_alignmentConfigOffset            = true;
      bool m_alignmentConfigCollimation       = true;
      bool m_alignmentConfigNonPerpendicular  = true;
      bool m_alignmentConfigPolarAxisDisp     = true;
      bool m_alignmentConfigTubeFlexure       = true;
      bool m_alignmentConfigForkFlexure       = false;
      bool m_alignmentConfigDecAxisFlexure    = true;
      bool m_alignmentLinear                  = true;
      bool m_alignmentQuadratic               = true;

      int  m_alignmentModelIndex              = 0;

      void getAlignmentConfigParamter(int32& configParam);
   };

   GUIData* GUI = nullptr;

   double m_geoLatitude  = 0;
   double m_geoLongitude = 0;
   int    m_telescopeAperture    = 0;
   int    m_telescopeFocalLength = 0;

   void UpdateControls();

   void plotAlignemtResiduals(AlignmentModel* model);


   void e_Timer( Timer& sender );
   void e_Edit( Edit& sender );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Click( Button& sender, bool checked );
   void e_Press( Button& sender );
   void e_Release( Button& sender );

   friend class AlignmentConfigDialog;
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
// EOF INDIMountInterface.h - Released 2017-05-02T09:43:01Z
