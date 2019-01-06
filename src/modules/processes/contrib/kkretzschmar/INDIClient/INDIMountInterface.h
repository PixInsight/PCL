//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0232
// ----------------------------------------------------------------------------
// INDIMountInterface.h - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2018 Klaus Kretzschmar
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
#include <pcl/EphemerisFile.h>
#include <pcl/Label.h>
#include <pcl/NetworkTransfer.h>
#include <pcl/NumericControl.h>
#include <pcl/Optional.h>
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

#include "INDIMountParameters.h"
#include "INDIClient.h"
#include "INDIMountInstance.h"
#include "DeviceConfigBase.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class INDIMountInterface;

// ----------------------------------------------------------------------------

class CoordinateSearchDialog : public Dialog
{
public:

   CoordinateSearchDialog( INDIMountInterface& );

   // Topocentric apparent right ascension in hours.
   double RA() const
   {
      return m_alpha;
   }

   // Topocentric apparent declination in degrees.
   double Dec() const
   {
      return m_delta;
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

   INDIMountInterface& m_parent;

   double    m_alpha = 0;
   double    m_delta = 0;
   bool      m_valid = false;
   bool      m_goto = false;
   bool      m_downloading = false;
   bool      m_abort = false;
   bool      m_firstTimeShown = true;
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

// ----------------------------------------------------------------------------

class EphemerisSearchDialog : public Dialog
{
public:

   EphemerisSearchDialog( INDIMountInterface& );

   virtual EphemerisObjectList Objects() const = 0;
   virtual const EphemerisFile& Ephemerides() const = 0;

   virtual String ObjectName() const
   {
      return m_objectName;
   }

   // Topocentric apparent right ascension in hours.
   double RA() const
   {
      return m_alpha;
   }

   // Topocentric apparent declination in degrees.
   double Dec() const
   {
      return m_delta;
   }

   bool GoToTarget() const
   {
      return m_goto;
   }

protected:

   VerticalSizer     Global_Sizer;
      HorizontalSizer   Objects_Sizer;
         Label             Object_Label;
         ComboBox          Objects_ComboBox;
         PushButton        Calculate_Button;
      TextBox           ObjectInfo_TextBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        Clear_Button;
         PushButton        Get_Button;
         PushButton        GoTo_Button;
         PushButton        Cancel_Button;

   INDIMountInterface& m_parent;

   IsoString m_objectName;
   double    m_alpha = 0;  // hours
   double    m_delta = 0; // degrees
   bool      m_valid = false;
   bool      m_goto = false;
   bool      m_firstTimeShown = true;

   virtual String ObjectName( const EphemerisObject& ) const;

   void e_Show( Control& sender );
   void e_Click( Button& sender, bool checked );
   virtual void e_ItemSelected( ComboBox& sender, int itemIndex );
};

// ----------------------------------------------------------------------------

class PlanetSearchDialog : public EphemerisSearchDialog
{
public:

   PlanetSearchDialog( INDIMountInterface& );

   EphemerisObjectList Objects() const override;
   const EphemerisFile& Ephemerides() const override;
};

// ----------------------------------------------------------------------------

class AsteroidSearchDialog : public EphemerisSearchDialog
{
public:

   AsteroidSearchDialog( INDIMountInterface& );

   EphemerisObjectList Objects() const override;
   const EphemerisFile& Ephemerides() const override;

   String ObjectName() const override
   {
      return m_objectId + ' ' + m_objectName; // e.g. '1 Ceres'
   }

private:

   IsoString m_objectId;

   String ObjectName( const EphemerisObject& ) const override;

   void e_ItemSelected( ComboBox& sender, int itemIndex ) override;
};

// ----------------------------------------------------------------------------

class SyncDataListDialog : public Dialog
{
public:

   SyncDataListDialog(Array<SyncDataPoint>& syncDataArray);

private:

   VerticalSizer     Global_Sizer;
      HorizontalSizer      SyncDataList_Sizer;
         TreeBox              SnycData_TreeBox;
      HorizontalSizer      SyncDataListButton_Sizer;
         PushButton           Enable_Button;
         PushButton           Disable_Button;
         PushButton           Delete_Button;
         PushButton           Ok_Button;
         PushButton           Cancel_Button;

   void e_Click( Button& sender, bool checked );
   void e_Show( Control& sender );
   void e_Close( Control& sender, bool& allowClose );

   Array<SyncDataPoint>& m_syncDataList;
   bool                  m_firstTimeShown = true;
};

// ----------------------------------------------------------------------------

class AlignmentConfigDialog : public Dialog
{
public:

   AlignmentConfigDialog( INDIMountInterface& );

private:

   bool m_firstTimeShown = true;

   INDIMountInterface& m_interface;

   VerticalSizer        Global_Sizer;
      TabBox               AlignmentConfig_TabBox;
         Control              AnalyticalAlignment_ConfigControl;
            VerticalSizer     MountAlignmentConfig_Sizer;
               CheckBox          ModelPierSide_CheckBox;
               CheckBox          Offset_CheckBox;
               CheckBox          Collimation_CheckBox;
               CheckBox          NonPerpendicular_CheckBox;
               CheckBox          PolarAxisDisplacement_CheckBox;
               CheckBox          TubeFlexure_CheckBox;
               CheckBox          ForkFlexure_CheckBox;
               CheckBox          DeltaAxisFlexure_CheckBox;
               CheckBox          Linear_CheckBox;
               CheckBox          Quadratic_CheckBox;
         //Control             SurfaceSplineAlignment_ConfigControl;
            HorizontalSizer      AlignmentConfigButton_Sizer;
               PushButton           Ok_Button;
               PushButton           Cancel_Button;

   void e_Show( Control& sender );
   void e_Click( Button& sender, bool checked );
   void e_PageSelected( TabBox& sender, int tabIndex );
};

// ----------------------------------------------------------------------------

class MountConfigDialog : public ConfigDialogBase
{
public:

   MountConfigDialog( const String& deviceName,
                      double geoLat, double geoLong, double geoHeight,
                      double telescopeAperture, double teslescopeFocalLenght );
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
   NumericEdit       Height_NumericEdit;
   NumericEdit       TelescopeAperture_NumericEdit;
   NumericEdit       TelescopeFocalLength_NumericEdit;

   void SendUpdatedProperties();
};

// ----------------------------------------------------------------------------

class INDIMountInterfaceExecution;

class INDIMountInterface : public ProcessInterface
{
public:

   INDIMountInterface();
   virtual ~INDIMountInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   const char** IconImageXPM() const override;
   InterfaceFeatures Features() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

   const String& CurrentDeviceName() const
   {
      return m_device;
   }

   double GeographicLatitude() const
   {
      return m_geoLatitude;
   }

   double GeographicLongitude() const
   {
      return m_geoLongitude;
   }

   double GeographicHeight() const
   {
      return m_geoHeight;
   }

   int TelescopeAperture() const
   {
      return m_telescopeAperture;
   }

   int TelescopeFocalLength() const
   {
      return m_telescopeFocalLength;
   }

   bool ShouldApplyAlignmentCorrection() const
   {
      if ( GUI == nullptr )
         return false;
      return GUI->MountAlignmentCorrection_CheckBox.IsChecked();
   }

   String AlignmentFilePath() const
   {
      if ( GUI == nullptr )
         return String();
      return GUI->AlignmentFile_Edit.Text();
   }

   int AlignmentMethod() const;

private:

   String                              m_device;
   INDIMountInterfaceExecution*        m_execution = nullptr;
   AutoPointer<CoordinateSearchDialog> m_searchDialog;
   AutoPointer<PlanetSearchDialog>     m_planetDialog;
   AutoPointer<AsteroidSearchDialog>   m_asteroidDialog;
   AutoPointer<AlignmentConfigDialog>  m_alignmentConfigDialog;

   struct GUIData
   {
      GUIData( INDIMountInterface& );

      Timer UpdateDeviceList_Timer;
      Timer UpdateDeviceProperties_Timer;

      VerticalSizer     Global_Sizer;

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
            PushButton        MountAligmentModelConfig_Button;
            PushButton        MountAligmentModelFit_Button;
            PushButton        SyncDataList_Button;
         HorizontalSizer   MountAlignmentCorrection_Sizer;
            CheckBox          MountAlignmentCorrection_CheckBox;
            CheckBox          MountAlignmentPlotResiduals_CheckBox;
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
            PushButton        MountPlanets_Button;
            PushButton        MountAsteroids_Button;
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

      bool m_modelBothPierSides              = true;
      bool m_alignmentConfigOffset           = true;
      bool m_alignmentConfigCollimation      = true;
      bool m_alignmentConfigNonPerpendicular = true;
      bool m_alignmentConfigPolarAxisDisp    = true;
      bool m_alignmentConfigTubeFlexure      = true;
      bool m_alignmentConfigForkFlexure      = false;
      bool m_alignmentConfigDecAxisFlexure   = true;
      bool m_alignmentLinear                 = true;
      bool m_alignmentQuadratic              = true;
      int  m_alignmentModelIndex             = 0;

      uint32 AlignmentConfigParameter() const;
   };

   GUIData* GUI = nullptr;

   double   m_geoLatitude          = 0;
   double   m_geoLongitude         = 0;
   double   m_geoHeight            = 0;
   int      m_telescopeAperture    = 0;
   int      m_telescopeFocalLength = 0;
   pcl_enum m_pierSide             = IMCPierSide::Default;

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
// EOF INDIMountInterface.h - Released 2018-12-12T09:25:25Z
