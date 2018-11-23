//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.03.0326
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationInterface.h - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PhotometricColorCalibrationInterface_h
#define __PhotometricColorCalibrationInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/GroupBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>

#include "PhotometricColorCalibrationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CoordinateSearchDialog;

class PhotometricColorCalibrationInterface : public ProcessInterface
{
public:

   PhotometricColorCalibrationInterface();
   virtual ~PhotometricColorCalibrationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;
   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

private:

   PhotometricColorCalibrationInstance m_instance;
   CoordinateSearchDialog*             m_searchDialog = nullptr;
   bool                                m_showComplexAngles = true;
   bool                                m_raInTimeUnits = true;

   struct GUIData
   {
      GUIData( PhotometricColorCalibrationInterface& );

      VerticalSizer     Global_Sizer;

         HorizontalSizer   WhiteReference_Sizer;
            Label             WhiteReference_Label;
            ComboBox          WhiteReference_ComboBox;

         HorizontalSizer   Server_Sizer;
            Label             Server_Label;
            ComboBox          Server_ComboBox;

         HorizontalSizer   ApplyColorCalibration_Sizer;
            CheckBox          ApplyColorCalibration_CheckBox;

         SectionBar        ImageParameters_SectionBar;
         Control           ImageParameters_Control;
         HorizontalSizer   ImageParameters_Sizer;
            VerticalSizer     ImageParametersLeft_Sizer;
               HorizontalSizer   RA_Sizer;
                  Label             RA_Label;
                  Edit              RA_Edit;
               HorizontalSizer   Dec_Sizer;
                  Label             Dec_Label;
                  Edit              Dec_Edit;
               HorizontalSizer   ShowComplexAngles_Sizer;
                  CheckBox          ShowComplexAngles_CheckBox;
               HorizontalSizer   RAInTimeUnits_Sizer;
                  CheckBox          RAInTimeUnits_CheckBox;
               HorizontalSizer   Epoch_Sizer;
                  Label             Epoch_Label;
                  Edit              Epoch_Edit;
               NumericEdit       FocalLength_NumericEdit;
               NumericEdit       PixelSize_NumericEdit;
            VerticalSizer     ImageParametersRight_Sizer;
               PushButton        SearchCoordinates_Button;
               PushButton        AcquireFromImage_Button;

         SectionBar        PlateSolverParameters_SectionBar;
         Control           PlateSolverParameters_Control;
         VerticalSizer     PlateSolverParameters_Sizer;
            HorizontalSizer   SolverAutoCatalog_Sizer;
               CheckBox          SolverAutoCatalog_CheckBox;
            HorizontalSizer   SolverCatalog_Sizer;
               Label             SolverCatalog_Label;
               ComboBox          SolverCatalog_ComboBox;
            HorizontalSizer   SolverAutoLimitMagnitude_Sizer;
               CheckBox          SolverAutoLimitMagnitude_CheckBox;
            HorizontalSizer   SolverLimitMagnitude_Sizer;
               Label             SolverLimitMagnitude_Label;
               SpinBox           SolverLimitMagnitude_SpinBox;
            HorizontalSizer   DistortionCorrection_Sizer;
               CheckBox          DistortionCorrection_CheckBox;
            HorizontalSizer   ForcePlateSolve_Sizer;
               CheckBox          ForcePlateSolve_CheckBox;
            HorizontalSizer   IgnorePositionAndScale_Sizer;
               CheckBox          IgnorePositionAndScale_CheckBox;
               ToolButton        ResetSolverConfiguration_ToolButton;

         SectionBar        AdvancedPlateSolverParameters_SectionBar;
         Control           AdvancedPlateSolverParameters_Control;
         VerticalSizer     AdvancedPlateSolverParameters_Sizer;
            HorizontalSizer   ProjectionSystem_Sizer;
               Label             ProjectionSystem_Label;
               ComboBox          ProjectionSystem_ComboBox;
            NumericControl    StarSensitivity_NumericControl;
            HorizontalSizer   NoiseReduction_Sizer;
               Label             NoiseReduction_Label;
               SpinBox           NoiseReduction_SpinBox;
            HorizontalSizer   AlignmentDevice_Sizer;
               Label             AlignmentDevice_Label;
               ComboBox          AlignmentDevice_ComboBox;
            NumericControl    SplineSmoothing_NumericControl;

         SectionBar        PhotometryParameters_SectionBar;
         Control           PhotometryParameters_Control;
         VerticalSizer     PhotometryParameters_Sizer;
            HorizontalSizer   PhotometryCatalog_Sizer;
               Label             PhotometryCatalog_Label;
               ComboBox          PhotometryCatalog_ComboBox;
            HorizontalSizer   PhotometryAutoLimitMagnitude_Sizer;
               CheckBox          PhotometryAutoLimitMagnitude_CheckBox;
            HorizontalSizer   PhotometryLimitMagnitude_Sizer;
               Label             PhotometryLimitMagnitude_Label;
               SpinBox           PhotometryLimitMagnitude_SpinBox;
            HorizontalSizer   AutoAperture_Sizer;
               CheckBox          AutoAperture_CheckBox;
            HorizontalSizer   Aperture_Sizer;
               Label             Aperture_Label;
               SpinBox           Aperture_SpinBox;
            NumericControl    SaturationThreshold_NumericControl;
            HorizontalSizer   UsePSFPhotometry_Sizer;
               CheckBox          UsePSFPhotometry_CheckBox;
            HorizontalSizer   ShowDetectedStars_Sizer;
               CheckBox          ShowDetectedStars_CheckBox;
            HorizontalSizer   ShowBackgroundModels_Sizer;
               CheckBox          ShowBackgroundModels_CheckBox;
            HorizontalSizer   GenerateGraphs_Sizer;
               CheckBox          GenerateGraphs_CheckBox;
               ToolButton        ResetPhotometryConfiguration_ToolButton;

         SectionBar        BackgroundReference_SectionBar;
         Control           BackgroundReference_Control;
         VerticalSizer     BackgroundReference_Sizer;
            HorizontalSizer   BackgroundReferenceView_Sizer;
               Label             BackgroundReferenceView_Label;
               Edit              BackgroundReferenceView_Edit;
               ToolButton        BackgroundReferenceView_ToolButton;
            NumericControl    BackgroundLow_NumericControl;
            NumericControl    BackgroundHigh_NumericControl;
            GroupBox          BackgroundROI_GroupBox;
            VerticalSizer     BackgroundROI_Sizer;
               HorizontalSizer   BackgroundROIRow1_Sizer;
                  Label             BackgroundROIX0_Label;
                  SpinBox           BackgroundROIX0_SpinBox;
                  Label             BackgroundROIY0_Label;
                  SpinBox           BackgroundROIY0_SpinBox;
               HorizontalSizer   BackgroundROIRow2_Sizer;
                  Label             BackgroundROIWidth_Label;
                  SpinBox           BackgroundROIWidth_SpinBox;
                  Label             BackgroundROIHeight_Label;
                  SpinBox           BackgroundROIHeight_SpinBox;
                  PushButton        BackgroundROISelectPreview_Button;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void AcquireMetadata( const ImageWindow& );

   void e_GetFocus( Control& sender );
   void e_EditCompleted( Edit& sender );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Click( Button& sender, bool checked );
   void e_SectionCheck( SectionBar& sender, bool checked );
   void e_GroupCheck( GroupBox& sender, bool checked );
   void e_EditValueUpdated( NumericEdit& sender, double value );
   void e_SpinValueUpdated( SpinBox& sender, int value );
   void e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   String RAToString( double ) const;
   String DecToString( double ) const;
   String EpochToString( double ) const;

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern PhotometricColorCalibrationInterface* ThePhotometricColorCalibrationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PhotometricColorCalibrationInterface_h

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationInterface.h - Released 2018-11-23T18:45:58Z
