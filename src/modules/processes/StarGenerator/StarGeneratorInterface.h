//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0285
// ----------------------------------------------------------------------------
// StarGeneratorInterface.h - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarGenerator PixInsight module.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __StarGeneratorInterface_h
#define __StarGeneratorInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/SpinBox.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/ToolButton.h>

#include "StarGeneratorInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// StarGeneratorInterface
// ----------------------------------------------------------------------------

class StarGeneratorInterface : public ProcessInterface
{
public:

   StarGeneratorInterface();
   virtual ~StarGeneratorInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   InterfaceFeatures Features() const;

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual void SaveSettings() const;
   virtual void LoadSettings();

   // -------------------------------------------------------------------------

private:

   StarGeneratorInstance instance;

   struct GUIData
   {
      GUIData( StarGeneratorInterface& );

      VerticalSizer     Global_Sizer;

         Label             StarDatabase_Label;
         HorizontalSizer   StarDatabase_Sizer;
            Edit              StarDatabase_Edit;
            ToolButton        StarDatabase_ToolButton;

         HorizontalSizer   RA_Sizer;
            Label             RA_Label;
            SpinBox           RAHours_SpinBox;
            SpinBox           RAMins_SpinBox;
            NumericEdit       RASecs_NumericEdit;

         HorizontalSizer   Dec_Sizer;
            Label             Dec_Label;
            SpinBox           DecDegs_SpinBox;
            SpinBox           DecMins_SpinBox;
            NumericEdit       DecSecs_NumericEdit;
            CheckBox          DecSouth_CheckBox;

         HorizontalSizer   Epoch_Sizer;
            Label             Epoch_Label;
            SpinBox           EpochYear_SpinBox;
            SpinBox           EpochMonth_SpinBox;
            SpinBox           EpochDay_SpinBox;

         HorizontalSizer   ProjectionSystem_Sizer;
            Label             ProjectionSystem_Label;
            ComboBox          ProjectionSystem_ComboBox;

         NumericEdit       FocalLength_NumericEdit;
         NumericEdit       PixelSize_NumericEdit;
         NumericEdit       SensorWidth_NumericEdit;
         NumericEdit       SensorHeight_NumericEdit;
         NumericEdit       LimitMagnitude_NumericEdit;

         HorizontalSizer   OutputMode_Sizer;
            Label             OutputMode_Label;
            ComboBox          OutputMode_ComboBox;

         Label             OutputFile_Label;
         HorizontalSizer   OutputFile_Sizer;
            Edit              OutputFile_Edit;
            ToolButton        OutputFile_ToolButton;

         NumericEdit       StarFWHM_NumericEdit;

         HorizontalSizer   Nonlinear_Sizer;
            CheckBox          Nonlinear_CheckBox;
         NumericEdit       TargetMinimumValue_NumericEdit;
   };

   GUIData* GUI;

   void UpdateControls();
   void UpdateRAControls();
   void UpdateDecControls();
   void UpdateEpochControls();

   void GetRA();
   void GetDec();
   void GetEpoch();

   // Event Handlers

   void __EditCompleted( Edit& sender );
   void __RealValueUpdated( NumericEdit& sender, double value );
   void __IntegerValueUpdated( SpinBox& sender, int value );
   void __Button_Clicked( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern StarGeneratorInterface* TheStarGeneratorInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StarGeneratorInterface_h

// ----------------------------------------------------------------------------
// EOF StarGeneratorInterface.h - Released 2017-07-09T18:07:33Z
